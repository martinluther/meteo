#!/usr/bin/perl

# $Id: meteo-wu-fmt.pl,v 1.1 2003/06/08 10:42:07 afm Exp $

#
# set the username and password below
# to the username and password of a mysql user who has
# read access to the meteo tables in the mysql database.
#
my $db_user = "foo";
my $db_passwd = "bird";

#
# TODO: make sure data changed!
# read/save timestamp from database and don't send anything to wu
# un;ess the timestamp on the weather data changed!
#

use strict;
use DBI();

my ($wind10avg);

#
# formulas from http://www.usatoday.com/weather/whumcalc.htm
#
# returns a dewpoint given a temp (celsius) and relative humidity
#
# Logic: First, obtain the saturation vapor pressure(Es) using formula (5)
# from air temperature Tc. 
#
# (5) Es=6.11*10.0**(7.5*Tc/(237.7+Tc)) 
#
# The next step is to use the saturation vapor pressure and the relative humidity
# to compute the actual vapor pressure(E) of the air. This can be done with
# the following formula. 
#
# (9) E=(RH*Es)/100 
#
# RH=relative humidity of air expressed as a percent.(i.e. 80%) 
#
# Now you are ready to use the following formula to obtain the
# dewpoint temperature. 
#
# Note: ln( ) means to take the natural log of the variable in the parentheses 
#
# (10) Tdc=(-430.22+237.7*ln(E))/(-ln(E)+19.08) 
#
sub dewpt ($$) {
  my ($Tc, $RH) = @_;
  my ($Es, $E, $Tdc);

  $Es = 6.11 * 10.0 ** (7.5 * $Tc / (237.7 + $Tc));
  $E = ($RH * $Es) / 100;
  $Tdc = (-430.22 + 237.7 * log($E)) / (-log($E) +19.08);

  return $Tdc;
}


#
# Connect to the database.
#
my $dbh = DBI->connect("DBI:mysql:database=meteo;host=localhost",
                       $db_user, $db_passwd,
                       {'RaiseError' => 1});

#
# first compute 10 minute average wind speed
#
my ($i, $ref);
$i = 10;
$wind10avg = 0;
my $sth = $dbh->prepare("SELECT timekey, windspeed FROM stationdata order by timekey DESC");
$sth->execute();

while (($i > 0) && ($ref = $sth->fetchrow_hashref())) {

  my $windspeed = $ref->{'windspeed'};

  $wind10avg = $wind10avg + $windspeed;

  $i--;
}

$sth->finish();

$wind10avg /= 10;



#
# Now grab the rest of the values we need
#
my $sth = $dbh->prepare("SELECT timekey, year, month, mday, hour, min, winddir, windspeed, humidity, temperature, rain, barometer  FROM stationdata order by timekey desc");

$sth->execute();

if (my $ref = $sth->fetchrow_hashref()) {

  my ($year, $month, $day, $hour, $min);
  my ($winddir, $windspeed);
  my ($humidity, $temp, $rain, $barometer, $dewpt);
  my ($tempUnit, $windUnit, $barUnit, $rainUnit);

  $year		= $ref->{'year'};		# stationDate
  $month	= $ref->{'month'};		# stationDate
  $day		= $ref->{'mday'};		# stationDate
  $hour		= $ref->{'hour'};		# stationTime
  $min		= $ref->{'min'};		# stationTime

  $winddir	= $ref->{'winddir'};		# windDir
  $windspeed	= $ref->{'windspeed'};		# windSpeed

  $humidity	= $ref->{'humidity'};		# outsideHumidity
  $temp		= $ref->{'temperature'};	# outsideTemp
  $rain		= $ref->{'rain'};		# should be raintotal?

  $barometer	= $ref->{'barometer'};		# barometer

  $dewpt	= dewpt($temp, $humidity);	# outsideDewPt

  $tempUnit	= "deg C";
  $windUnit	= "m/s";
  $barUnit	= "hPa";
  $rainUnit	= "mm";


  #
  # stationDate=<!--stationDate-->
  # stationTime=<!--stationTime-->
  #
  printf("stationDate=%s/%s/%s\n", $month, $day, $year);
  printf("stationTime=%s:%s\n", $hour, $min);

  # windDir=<!--windDir-->
  printf("windDir=%s\n", $winddir);

  # wind10Avg=<!--wind10Avg-->
  printf("wind10Avg=%s\n", $wind10avg);

  # windSpeed=<!--windSpeed-->
  printf("windSpeed=%s\n", $windspeed);

  # outsideHumidity=<!--outsideHumidity-->
  printf("outsideHumidity=%s\n", $humidity);

  # outsideTemp=<!--outsideTemp-->
  printf("outsideTemp=%s\n", $temp);

  # dailyRain=<!--dailyRain-->
  printf("dailyRain=%s\n", $rain);

  # barometer=<!--barometer-->
  printf("barometer=%s\n", $barometer);

  # outsideDewPt=<!--outsideDewPt-->
  printf("outsideDewPt=%s\n", $dewpt);

  # tempUnit=<!--tempUnit-->
  printf("tempUnit=%s\n", $tempUnit);

  # windUnit=<!--windUnit-->
  printf("windUnit=%s\n", $windUnit);

  # barUnit=<!--barUnit-->
  printf("barUnit=%s\n", $barUnit);

  # rainUnit=<!--rainUnit-->
  printf("rainUnit=%s\n", $rainUnit);
}

$sth->finish();

# Disconnect from the database.
$dbh->disconnect();

