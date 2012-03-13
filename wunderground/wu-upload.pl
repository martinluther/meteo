#!/usr/bin/perl

# $Id: wu-upload.pl,v 1.1 2003/06/08 10:42:07 afm Exp $

# -------------------------------------------------------------------
# PROGRAM : wu-upload
# PURPOSE : Upload weather information to Weather Underground
# AUTHOR  : Jeremy Botha
# DATE    : 2001-11-11 (version 1.00)
# -------------------------------------------------------------------
# LICENSE : GNU General Public License - 
#           http://www.gnu.org/copyleft/gpl.html
# -------------------------------------------------------------------

use POSIX;
use LWP::UserAgent;
use HTTP::Request;
use strict;

# ===================================================================
# CONSTANT DECLARATION
# ===================================================================

my $ID = "KMNHOPKI1";
my $PASSWORD = "hcomplex";
my $WU_URL = "http://weatherstation.wunderground.com/weatherstation/updateweatherstation.php";
my $WDATFILE="/home/foo/weather/meteo.out";
my $SOFTWARETYPE="meteo_script";

# ===================================================================
# FUNCTIONS
# ===================================================================

#----------
# read_vpw : Read values from formatted file
#----------
sub read_vpw ($$) {

  my ($WDATFILE, $wthr) = @_;
  my ($key, $data);

  open(WDATFILE,"<$WDATFILE") || die "Unable to open weather data file $WDATFILE!";

  while (<WDATFILE>) {
    chomp;
    ($key, $data) = split(/=/);
    $wthr->{$key} = $data;
  }

  close(WDATFILE);
}

#---------
# dateutc : Convert current date format to utc
#---------
sub dateutc ($$) {

  my ($stationDate, $stationTime) = @_;
  my ($YYYY, $MO, $DD, $HH, $MM);

  ($MO, $DD, $YYYY) = split(/\//,$stationDate,3);
  ($HH, $MM) = split(/[:ap]/,$stationTime,2);

  # Convert to 24-hour time code if am/pm indicators are found
  if ( $stationTime =~ /([ap])/ ) {
    if ( $1 eq "p" && $HH != 12 ) {
      $HH += 12;
    } elsif ( $1 eq "a" && $HH == 12 ) {
      $HH = 0;
    }
  }

  return strftime("%Y-%m-%d %H:%M:%S",gmtime(mktime(0,$MM,$HH,$DD,$MO-1,$YYYY-1900,0,0,0)));
}

#-----------
# knottomph : Convert from knots to miles per hour
#-----------
sub knottomph ($) {
  my ($knot) = @_;
  return sprintf("%.2f",$knot*1.15077945);
}

#----------
# kmhtomph : Convert from kilometers per hour to miles per hour
#----------
sub kmhtomph ($) {
  my ($kmh) = @_;
  return sprintf("%.2f",$kmh/1.609344);
}

#---------
# mstomph : Convert from meters per second to miles per hour
#---------
sub mstomph ($) {
  my ($ms) = @_;
  return sprintf("%.2f",$ms*2.23693629);
}

#------
# CtoF : Convert from degrees Centigrade to degrees Fahrenheit
#------
sub CtoF ($) {
  my ($C) = @_;
  return sprintf("%.2f",9*$C/5+32);
}

#--------
# mmtoin : Convert from Millimeters to Inches (Hg & Rain)
#--------
sub mmtoin ($) {
  my ($mm) = @_;
  return sprintf("%.3f",$mm/25.4);
}

#--------
# mbtoin : Convert from Millibar to InchesHg/Mercury
#--------
sub mbtoin ($) {
  my ($mb) = @_;
  return sprintf("%.3f",$mb/33.8638864);
}

#--------
# hPatoin : Convert from hectaPascals to InchesHg/Mercury
#--------
sub hPatoin ($) {
  my ($hPa) = @_;
  return sprintf("%.3f",$hPa/33.8638864);
}

#-----------
# build_url : Assemble all the variables required for the url
#-----------
sub  build_url (\%) {
  
  my ($wthr, %wuf, $field, $url);

  ($wthr) = @_;

  $wuf{dateutc} = &dateutc($wthr->{stationDate}, $wthr->{stationTime});

  $wuf{winddir} = $wthr->{windDir};

  if ( $wthr->{windUnit} =~ /mph/ ) {
    $wuf{windspeedmph} = $wthr->{wind10Avg};
    $wuf{windgustmph} = $wthr->{windSpeed};
  } elsif ( $wthr->{windUnit} =~ /knot/ ) {
    $wuf{windspeedmph} = &knottomph($wthr->{wind10Avg});
    $wuf{windgustmph} = &knottomph($wthr->{windSpeed});
  } elsif ( $wthr->{windUnit} =~ /km\/h/ ) {
    $wuf{windspeedmph} = &kmhtomph($wthr->{wind10Avg});
    $wuf{windgustmph} = &kmhtomph($wthr->{windSpeed});
  } elsif ( $wthr->{windUnit} =~ /m\/s/ ) {
    $wuf{windspeedmph} = &mstomph($wthr->{wind10Avg});
    $wuf{windgustmph} = &mstomph($wthr->{windSpeed});
  }

  # Best-guess the gust speed based on available data
  if ( $wuf{windgustmph} < $wuf{windspeedmph} ) {
    $wuf{windgustmph} = $wuf{windspeedmph};
  }

  $wuf{humidity} = $wthr->{outsideHumidity};

  if ( $wthr->{tempUnit} =~ /F/ ) {
    $wuf{tempf} = $wthr->{outsideTemp};
    $wuf{dewptf} = $wthr->{outsideDewPt};
  } elsif ( $wthr->{tempUnit} =~ /C/ ) {
    $wuf{tempf} = &CtoF($wthr->{outsideTemp});
    $wuf{dewptf} = &CtoF($wthr->{outsideDewPt});
  }

  if ( $wuf{tempf} > 140 ) {
	die "wu-upload.pl: temp = $wuf{tempf} is whacked - skipping upload\n";
  }

  if ( $wthr->{rainUnit} =~ /in/ ) {
    $wuf{rainin} = $wthr->{dailyRain};
  } elsif ( $wthr->{rainUnit} =~ /mm/ ) {
    $wuf{rainin} = &mmtoin($wthr->{dailyRain});
  }

  if ( $wthr->{barUnit} =~ /in/ ) {
    $wuf{baromin} = $wthr->{barometer};
  } elsif ( $wthr->{barUnit} =~ /mm/ ) {
    $wuf{baromin} = &mmtoin($wthr->{barometer});
  } elsif ( $wthr->{barUnit} =~ /mb/ ) {
    $wuf{baromin} = &mbtoin($wthr->{barometer});
  } elsif ( $wthr->{barUnit} =~ /hPa/ ) {
    $wuf{baromin} = &hPatoin($wthr->{barometer});
  }

  $wuf{weather} = 'NA';
  $wuf{clouds} = 'NA';

  foreach $field ( 'dateutc', 'winddir', 'windspeedmph', 'windgustmph',
                   'humidity', 'tempf', 'rainin', 'baromin', 'dewptf',
		   'weather', 'clouds' ) {
    $url = $url . $field . '=' . $wuf{$field} . '&';
  }

  return $url;
}

#----------
# post_url : Use http to post the url
#----------
sub  post_url ($) {

  my ($url, $ua, $req, $resp, $resp_data);

  ($url) = @_;

# Comment out the fields below for testing, and uncomment the print statement
$ua = LWP::UserAgent->new;
$req = HTTP::Request->new(GET => $url);
$resp = $ua->simple_request($req);
$resp_data = $resp->content;

# For troubleshooting:
  print $url;
  return $resp_data;
}
  
# ===================================================================
# Main
# ===================================================================

my (%wthr, $url, $response, $var);

# Get Vantage Pro weatherstation data from html file
&read_vpw($WDATFILE, \%wthr);

# Build the "contents" of the url
$url = &build_url(\%wthr);

# Add the prepend and append, remove any [unwanted] return characters
$url = $WU_URL . "?action=updateraw\&" . "ID=$ID\&" . "PASSWORD=$PASSWORD\&" . $url . "softwaretype=$SOFTWARETYPE";
$url =~ s/
//g;

# Use http to post the url to wunderground
$response =  &post_url($url);

# I am only using the response to monitor the exit code on a scheduled job in
# W2k - this can either be ignored or the script extended to log to the W2k
# event log.
if ( $response ne 'success' ) { die "Error posting weather data : $response" }


