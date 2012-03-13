<?php
//
//   meteobrowser.php -- php script implementing a browser function for meteo
//                       data, as stored in the database by the meteo programs
//
//   (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
//
//   $Id: meteobrowser.php,v 1.4 2002/01/30 09:49:01 afm Exp $
//
//   This scripts generates overview pages for meteorological data containing
//   client side image maps (so that a lynx browser can also profit from these
//   pages). Clicking on an image should bring up a new page with the next
//   level of detail. There are also a few buttons that allow direct navigation
//   to the next or previous time interval (day, week, month, year).
//
//   The position is controlled by the following variables:
//
//	$level		"day", "week", "month", "year"
//	$midtime	unix time for the middle of the current interval
//	$station	station name
//

$graphlist = array();
$stationstart = array();

// include the configuration file
include("meteobrowser.inc");

//-------------------------------------------------------------------------
// No configurable code below this line
//-------------------------------------------------------------------------

// handle localization
if ($lang == "") {
	$lang == $default_language;
}
if ($lang == "de") { $lang = "de_DE"; }
if ($lang == "en") { $lang = "en_US"; }
setlocale(LC_ALL, $lang);
putenv("LANG=$lang");
bindtextdomain("meteobrowser", "./locale");
textdomain("meteobrowser");

// handle time stamp in various formats
function getctime($filename) {
	$rc = stat($filename);
	if ($rc == FALSE) {
		return -1;
	}
	return $rc[10];
}

// if the variables are not set, we should set them to reasonable defaults
if ($level == "") {
	$level = $level_default;
}
if ($midtime == "") {
	$midtime = $midtime_default;
}
if ($station == "") {
	$station = $station_default;
}

// if the midtime string has exactly 8 characters, then it is a date stamp
// and must first be converted to a Unix timestamp
if (strlen($midtime) == 8) {
	$year = substr($midtime, 0, 4);
	$month = substr($midtime, 4, 2);
	$day = substr($midtime, 6, 2);
	$midtime = mktime(12, 0, 0, $month, $day, $year);
}

// compute the local time, as a starting point to compute the canonical
// date
$lt = localtime($midtime, 1);

// shift to the middle of the day
$lt[tm_sec] = 0; $lt[tm_min] = 0; $lt[tm_hour] = 12;

// compute interval from level
switch ($level) {
	case "day":
		$interval = 300;
		break;
	case "week":
		$interval = 1800;
		$lt[tm_mday] += 3 - $lt[tm_wday];
		// shift to the center of the current weak
		break;
	case "month":
		$interval = 7200;
		// shift to the center of the current month
		$lt[tm_mday] = 15;
		break;
	case "year":
		$interval = 86400;
		// shift to the middle of the year
		$lt[tm_mday] = 30; $lt[tm_mon] = 5;
		break;
}
$midoffset = 200 * $interval;	// offset from middle of graph in seconds

// normalize the mid time: compute the localtime from it, and find the
// neareast exact middle of the day
$midtime = mktime($lt[tm_hour], $lt[tm_min], $lt[tm_sec],
		$lt[tm_mon] + 1, $lt[tm_mday], $lt[tm_year]);
$middate = strftime("%D", $midtime);
$endtime = $midtime + $midoffset;
$endtimestamp = strftime("%Y%m%d%H%M%S", $endtime);

// compute midtimes of previous and next interval
switch ($level) {
	case "day":
		$previousmidtime = $midtime - 86400;
		$nextmidtime = $midtime + 86400;
		$iconmonth = "upup.png";
		$iconyear = "upupup.png";
		$title = strftime(gettext("%A, %d %B %Y"), $midtime);
		$beginintervaltime = $midtime - 86400/2;
		$endintervaltime = $midtime + 86400/2;
		break;
	case "week":
		$previousmidtime = $midtime - 7 * 86400;
		$nextmidtime = $midtime + 7 * 86400;
		$iconmonth = "up.png";
		$iconyear = "upup.png";
		$fromweek = strftime(gettext("%B %e, %Y"),
				$midtime - 144 * $interval);
		$toweek = strftime(gettext("%B %e, %Y"),
				$midtime + 144 * $interval);
		$title = sprintf(gettext("week from %s to %s"),
			$fromweek, $toweek);
		$beginintervaltime = $midtime - 3.5 * 86400;
		$endintervaltime = $midtime + 3.5 * 86400;
		break;
	case "month":
		$previousmidtime = $midtime - 30 * 86400;
		$nextmidtime = $midtime + 30 * 86400;
		$iconyear = "up.png";
		$title = strftime("%B %Y", $midtime);
		$beginintervaltime = $midtime - 15.5 * 86400;
		$endintervaltime = $midtime + 16.5 * 86400;
		break;
	case "year":
		$previousmidtime = $midtime - 365 * 86400;
		$nextmidtime = $midtime + 365 * 86400;
		$title = sprintf(gettext("year %s"), date("Y", $midtime));
		$beginintervaltime = $midtime - 183 * 86400;
		$endintervaltime = $midtime + 183 * 86400;
		break;
}

// compute the parameters for the new images
$graphs = split(":", $graphlist[$station]);

// compute the names of images for the images
$cmd = $meteograph." -f $confpath/meteo-$station.conf -c $cachedir -a -t ";
$computegraphs = FALSE;
foreach ($graphs as $graph) {
	// compute the file name
	$filename = "cache/$station-$graph-$level-$endtimestamp.png";
	$fctime = getctime($filename);
	if (	// if the file does not exist
		($fctime < 0) ||
		// if the last modified time of the graph is earlier than the
		// endtime, then it cannot possibly be complete. recompute
		// if more than $interval seconds old
		(($fctime < $endtime) && ($fctime < (time() - $interval))) ||
		(($fctime >= $endtime) && ($fctime < (time() - $rintval)))) {
		$cmd .= " -g ".$graph;
		$computegraphs = TRUE;
	}
}
$cmd .= " -e $endtimestamp $interval";
if ($computegraphs) {
	system($cmd);
}

$fulltitle = $station . " " . gettext("weather overview of") . " " . $title;

?>

<html>
<head>
<link href="meteo.css" type="text/css" rel="stylesheet">
<title><?echo $fulltitle?></title>
</head>
<body bgcolor="#ffffff">
<?php
if ($level == "year") {
	printf("<map name=\"yearmap\">\n");
	$basicurl
	= "meteobrowser.php?station=$station&level=month&lang=$lang&midtime=";
	$monthcoords = array();
	$monthcoords[-6] ="0,0,73,142";
	$monthcoords[-5] ="74,0,104,142";
	$monthcoords[-4] ="105,0,133,142";
	$monthcoords[-3] ="134,0,164,142";
	$monthcoords[-2] ="165,0,194,142";
	$monthcoords[-1] ="195,0,225,142";
	$monthcoords[0] ="226,0,255,142";
	$monthcoords[1] ="256,0,286,142";
	$monthcoords[2] ="287,0,317,142";
	$monthcoords[3] ="318,0,347,142";
	$monthcoords[4] ="348,0,378,142";
	$monthcoords[5] ="379,0,408,142";
	$monthcoords[6] ="409,0,439,142";
	$monthcoords[7] ="440,0,499,142";
	function montharea($i) {
		global	$basicurl, $midtime, $station, $stationstart,
			$monthcoords;
		$monthtime = $midtime - 1296000 + $i * 2592000;
		$monthstart = $monthtime - 15.5 * 86400;
		$monthend = $monthtime + 16.5 * 86400;
		if (($monthend < $stationstart[$station]) ||
			($monthstart > time())) {
			return;
		}
		printf("<area coords=\"%s\"\n", $monthcoords[$i]);
		printf("alt=\"%s\"\n", date("M Y", $monthtime));
		printf("href=\"%s%d\">\n", $basicurl, $monthtime);
	}
	for ($i = -6; $i <= 7; $i++) {
		montharea($i);
	}
	printf("</map>\n");
}
if ($level == "month") {
	printf("<map name=\"monthmap\">\n");
	function weekarea($i) {
		global	$basicurl, $midweek, $weeks, $midtime, $count,
			$station, $stationstart;
		$weektime = $midtime + ($i - $midweek) * 604800;
		$weekstart = $weektime - 3 * 48 * 1800;
		$weekend = $weektime + 3 * 48 * 1800;
		if (($weekend < $stationstart[$station]) ||
			($weekstart > time())) {
			return;
		}
		printf("<area coords=\"%d,0,%d,142\"\n",
			($i == 0) ? 0 : (255 + $weeks[$i - 1]),
			($i == $count) ? 499 : (255 + $weeks[$i]));
		printf("alt=\"week from %s to %s\"\n",
			date("d M Y", $weekstart),
			date("d M Y", $weekend));
		printf("href=\"%s%d\">\n", $basicurl, $weektime);
	}
	$basicurl
	= "meteobrowser.php?station=$station&level=week&lang=$lang&midtime=";
	$weeks = array();
	$lt = localtime($midtime - 200 * 7200, 1);
	$week = $lt[tm_wday];
	$count = 0;
	for ($i = -199; $i <= 200; $i++) {
		$lt = localtime($midtime + $i * 7200, 1);
		$week2 = $lt[tm_wday];
		if ($i == 0) {
			$midweek = $count;
		}
		if (($week2 == 0) && ($week != 0)) {
			$weeks[$count] = $i;
			$count++;
		}
		$week = $week2;
	}
	for ($i = 1; $i <= $count; $i++) {
		weekarea($i);
	}
	printf("</map>\n");
}
if ($level == "week") {
	printf("<map name=\"weekmap\">\n");
	$basicurl
	= "meteobrowser.php?station=$station&level=day&lang=$lang&midtime=";
	$daycoords = array();
	$daycoords[-4] = "0,0,86,142";
	$daycoords[-3] = "87,0,134,142";
	$daycoords[-2] = "135,0,182,142";
	$daycoords[-1] = "183,0,230,142";
	$daycoords[0] = "231,0,278,142";
	$daycoords[1] = "279,0,326,142";
	$daycoords[2] = "327,0,374,142";
	$daycoords[3] = "374,0,422,142";
	$daycoords[4] = "423,0,499,142";
	function dayarea($i) {
		global	$daycoords, $midtime, $basicurl, $stationstart,
			$station;
		$daytime = $midtime + $i * 86400;
		$daystart = $daytime - 86500/2;
		$dayend = $daytime + 86500/2;
		if (($dayend < $stationstart[$station]) ||
			($daystart > time())) {
			return;
		}
		printf("<area coords=\"%s\"\n", $daycoords[$i]);
		printf("alt=\"%s\"\n",
			date("D d M Y", $daytime));
		printf("href=\"%s%s\">\n", $basicurl, $daytime);
	}
	for ($i = -4; $i <= 4; $i++) {
		dayarea($i);
	}
	printf("</map>\n");
}
?>
<h1><?echo $fulltitle?></h1>

<table cellpadding="0" cellspacing="0" border="0">

<tr>
<td colspan="6">
<?= gettext("To see more details, click into the region you like inside ".
	"the graphs below, to zoom out or move forward or backward ".
	"in time, use the navigation bar.")?>
</td>
</tr>

<tr>

<td align="left" width="96">
<?php
if ($beginintervaltime >= $stationstart[$station]) {
?>
<a href="meteobrowser.php?station=<?= $station?>&level=<?= $level?>&lang=<?echo $lang?>&midtime=<?= $previousmidtime?>"><img src="previous.png"
alt="previous <?= $level?>" border="0" /></a>
<?php
}
?>
</td>

<td align="center" width="96">
<?php
if ($interval < 1800) {
?>
<a href="meteobrowser.php?station=<?= $station?>&level=week&lang=<?echo $lang?>&midtime=<?= $midtime?>"><img src="up.png"
border="0" /><img src="week.png" alt="week" border="0" /><img src="up.png"
border="0" /></a>
<?php } else { ?>
	&nbsp;
<?php
}
?>
</td>

<td align="center" width="96">
<?php
if ($interval < 7200) {
?>
<a href="meteobrowser.php?station=<?= $station?>&level=month&lang=<?echo $lang?>&midtime=<?= $midtime?>"><img src="<?echo $iconmonth?>" border="0" /><img src="month.png"
alt="month" border="0" /><img src="<?echo $iconmonth?>" border="0" /></a>
<?php
}
?>
</td>

<td align="center" width="96">
<?php
if ($interval < 86400) {
?>
<a href="meteobrowser.php?station=<?echo $station?>&level=year&lang=<?echo $lang?>&midtime=<?echo $midtime?>"><img src="<?echo $iconyear?>" border="0" /><img src="year.png"
alt="year" border="0" /><img src="<?echo $iconyear?>" border="0" /></a>
<?php
}
?>
</td>

<td align="right" width="96">
<?php
if ($endintervaltime <= time()) {
?>
<a href="meteobrowser.php?station=<?echo $station?>&level=<?echo $level?>&lang=<?echo $lang?>&midtime=<?echo $nextmidtime?>"><img src="next.png" alt="next <?echo $level?>"
border="0" /></a>
<?php
}
?>
</td>
<td>&nbsp;</td>

</tr>

<?php
// the following explanations of the graphs have to be translated into 
// the various languages the meteo package is supposed to support
$explanation = array();
$explanation[temperature] =
	"The <font color=\"#b40000\">red</font> curve shows the temperature, ".
	"the <font color=\"#ffb4b4\">light red</font> area shows the range ".
	"between maximum and minimum temperature during the sampling ".
	"interval.  The <font color=\"#6464ff\">blue</font> curve shows the ".
	"dew point, the temperature at which dew begins to form.";
$explanation[pressure] = 
	"The <font color=\"#7f7fff\">blue</font> graph shows ".
	"average barometric pressure, the range between minimum ".
	"and maximum pressure during the sampling interval is shown ".
	"in <font color=\"#d2d2ff\">light blue</font>.";
$explanation[rain] = 
	"The <font color=\"#0000ff\">blue</font> histogram shows ".
	"the total rain (or moisture content of the ".
	"precipitation) in <i>mm</i> during the sampling interval. ";
$explanation[wind] =
	"The lower part of the graph shows in <font ".
	"color=\"#00ff00\">light green</font> the average wind speed ".
	"during the sampling interval, the maximum speed is shown ".
	"in <font color=\"#006400\">dark green</font>, scale on the ".
	"left of the graph. The upper part shows the wind azimut ".
	"in <font color=\"#6464ff\">blue</font>, scale on the right. ";
$explanation[radiation] =
	"The <font color=\"#c8b400\">gold</font> histogram shows the ".
	"total solar radiation in W/m^2, scale on the left of the ".
	"graph.  The <font color=\"#6400c8\">pink</font> curve shows ".
	"the UV index, scale on the right.  </p> <p> Note that after ".
	"snow fall the readings of both sensors are not reliably, ".
	"as the sensors are not automatically freed of the snow.";
$explanation[temperature_inside] = "";

foreach ($graphs as $graph) {
	printf("<tr>\n");
	if ("day" == $level) {
		$mapstatement = "";
	} else {
		$mapstatement = sprintf(" usemap=\"#%smap\"", $level);
	}
	printf("<td colspan=\"5\" valign=\"top\">".
		"<img src=\"cache/%s-%s-%s-%s.png\" border=\"0\" ".
		"width=\"500\" height=\"144\"%s /></td>\n",
		$station, $graph, $level, $endtimestamp, $mapstatement);
	printf("<td valign=\"top\">%s</td>\n",
		gettext($explanation[$graph]));
	printf("</tr>\n");
}
?>
</table>

<hr />
<p>
<?= gettext("Graphs produced by the ".
"<a href=\"http://meteo.othello.ch/\">meteo</a> package,")?>
&copy; 2002 <a href="mailto:afm@othello.ch">Dr. Andreas M&uuml;ller</a>,
<a href="http://www.othello.ch/">Beratung und Entwicklung</a>
</p>
</body>
</html>
