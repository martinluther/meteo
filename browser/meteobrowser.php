<?php
//
//   meteobrowser.php -- php script implementing a browser function for meteo
//                       data, as stored in the database by the meteo programs
//
//   (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
//
//   $Id: meteobrowser.php,v 1.10 2002/11/24 22:31:06 afm Exp $
//
//   This scripts generates overview pages for meteorological data containing
//   client side image maps (so that a lynx browser can also profit from these
//   pages). Clicking on an image should bring up a new page with the next
//   level of detail. There are also a few buttons that allow direct navigation
//   to the next or previous time interval (day, week, month, year).
//
//   The position is controlled by the following variables:
//
//	$HTTP_GET_VARS[level]		"day", "week", "month", "year"
//	$HTTP_GET_VARS[midtime]	unix time for the middle of the current interval
//	$HTTP_GET_VARS[station]	station name
//

$graphlist = array();
$stationstart = array();

// include the configuration file
include("meteobrowser.inc");

//-------------------------------------------------------------------------
// No configurable code below this line
//-------------------------------------------------------------------------

// handle localization
if ($HTTP_GET_VARS[lang] == "") {
	$HTTP_GET_VARS[lang] = $language_default;
}
if ($HTTP_GET_VARS[lang] == "de") { $HTTP_GET_VARS[lang] = "de_DE"; }
if ($HTTP_GET_VARS[lang] == "en") { $HTTP_GET_VARS[lang] = "en_US"; }
setlocale(LC_ALL, $HTTP_GET_VARS[lang]);
putenv("LANG=$HTTP_GET_VARS[lang]");
bindtextdomain("meteobrowser", "./locale");
textdomain("meteobrowser");

// handle time stamp in various formats
function getctime($filename) {
	if (!file_exists($filename)) {
		return -1;
	}
	$rc = stat($filename);
	if ($rc == FALSE) {
		return -1;
	}
	return $rc[10];
}

// if the variables are not set, we should set them to reasonable defaults
if ($HTTP_GET_VARS[level] == "") {
	$HTTP_GET_VARS[level] = $level_default;
}
if ($HTTP_GET_VARS[midtime] == "") {
	$HTTP_GET_VARS[midtime] = $midtime_default;
}
if ($HTTP_GET_VARS[station] == "") {
	$HTTP_GET_VARS[station] = $station_default;
}

// if the midtime string has exactly 8 characters, then it is a date stamp
// and must first be converted to a Unix timestamp
if (strlen($HTTP_GET_VARS[midtime]) == 8) {
	$year = substr($HTTP_GET_VARS[midtime], 0, 4);
	$month = substr($HTTP_GET_VARS[midtime], 4, 2);
	$day = substr($HTTP_GET_VARS[midtime], 6, 2);
	$HTTP_GET_VARS[midtime] = mktime(12, 0, 0, $month, $day, $year);
}

// compute the local time, as a starting point to compute the canonical
// date
$lt = localtime($HTTP_GET_VARS[midtime], 1);

// shift to the middle of the day
$lt[tm_sec] = 0; $lt[tm_min] = 0; $lt[tm_hour] = 12;

// compute interval from level
switch ($HTTP_GET_VARS[level]) {
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
$HTTP_GET_VARS[midtime] = mktime($lt[tm_hour], $lt[tm_min], $lt[tm_sec],
		$lt[tm_mon] + 1, $lt[tm_mday], $lt[tm_year]);
$middate = strftime("%D", $HTTP_GET_VARS[midtime]);
$endtime = $HTTP_GET_VARS[midtime] + $midoffset;
$endtimestamp = strftime("%Y%m%d%H%M%S", $endtime);

// compute midtimes of previous and next interval
switch ($HTTP_GET_VARS[level]) {
	case "day":
		$previousmidtime = $HTTP_GET_VARS[midtime] - 86400;
		$nextmidtime = $HTTP_GET_VARS[midtime] + 86400;
		$iconmonth = "upup.png";
		$iconyear = "upupup.png";
		$title = strftime(gettext("%A, %d %B %Y"),
			$HTTP_GET_VARS[midtime]);
		$beginintervaltime = $HTTP_GET_VARS[midtime] - 86400/2;
		$endintervaltime = $HTTP_GET_VARS[midtime] + 86400/2;
		break;
	case "week":
		$previousmidtime = $HTTP_GET_VARS[midtime] - 7 * 86400;
		$nextmidtime = $HTTP_GET_VARS[midtime] + 7 * 86400;
		$iconmonth = "up.png";
		$iconyear = "upup.png";
		$fromweek = strftime(gettext("%B %e, %Y"),
				$HTTP_GET_VARS[midtime] - 144 * $interval);
		$toweek = strftime(gettext("%B %e, %Y"),
				$HTTP_GET_VARS[midtime] + 144 * $interval);
		$title = sprintf(gettext("week from %s to %s"),
			$fromweek, $toweek);
		$beginintervaltime = $HTTP_GET_VARS[midtime] - 3.5 * 86400;
		$endintervaltime = $HTTP_GET_VARS[midtime] + 3.5 * 86400;
		break;
	case "month":
		$previousmidtime = $HTTP_GET_VARS[midtime] - 30 * 86400;
		$nextmidtime = $HTTP_GET_VARS[midtime] + 30 * 86400;
		$iconyear = "up.png";
		$title = strftime("%B %Y", $HTTP_GET_VARS[midtime]);
		$beginintervaltime = $HTTP_GET_VARS[midtime] - 15.5 * 86400;
		$endintervaltime = $HTTP_GET_VARS[midtime] + 16.5 * 86400;
		break;
	case "year":
		$previousmidtime = $HTTP_GET_VARS[midtime] - 365 * 86400;
		$nextmidtime = $HTTP_GET_VARS[midtime] + 365 * 86400;
		$title = sprintf(gettext("year %s"), date("Y", $HTTP_GET_VARS[midtime]));
		$beginintervaltime = $HTTP_GET_VARS[midtime] - 183 * 86400;
		$endintervaltime = $HTTP_GET_VARS[midtime] + 183 * 86400;
		break;
}

// compute the parameters for the new images
$graphs = split(":", $graphlist[$HTTP_GET_VARS[station]]);

// compute the names of images for the images
$cmd = $meteograph." -s $HTTP_GET_VARS[station] -f $confpath/meteo.xml ".
		"-c $cachedir -a -t ";
$computegraphs = FALSE;
foreach ($graphs as $graph) {
	// compute the file name
	$filename = "cache/$HTTP_GET_VARS[station]-$graph-".
		"$HTTP_GET_VARS[level]-$endtimestamp.png";
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

$fulltitle = $HTTP_GET_VARS[station] . " " . gettext("weather overview of") . " " . $title;

?>

<html>
<head>
<link href="meteo.css" type="text/css" rel="stylesheet">
<title><?echo $fulltitle?></title>
</head>
<body bgcolor="#ffffff">
<?php
if ($HTTP_GET_VARS[level] == "year") {
	printf("<map name=\"yearmap\">\n");
	$basicurl
	= "meteobrowser.php?station=$HTTP_GET_VARS[station]&level=month&".
		"lang=$HTTP_GET_VARS[lang]&midtime=";
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
		global	$basicurl, $HTTP_GET_VARS, $stationstart, $monthcoords;
		$monthtime = $HTTP_GET_VARS[midtime] - 1296000 + $i * 2592000;
		$monthstart = $monthtime - 15.5 * 86400;
		$monthend = $monthtime + 16.5 * 86400;
		if (($monthend < $stationstart[$HTTP_GET_VARS[station]]) ||
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
if ($HTTP_GET_VARS[level] == "month") {
	printf("<map name=\"monthmap\">\n");
	function weekarea($i) {
		global	$basicurl, $midweek, $weeks, $HTTP_GET_VARS,
			$count, $stationstart;
		$weektime = $HTTP_GET_VARS[midtime] + ($i - $midweek) * 604800;
		$weekstart = $weektime - 3 * 48 * 1800;
		$weekend = $weektime + 3 * 48 * 1800;
		if (($weekend < $stationstart[$HTTP_GET_VARS[station]]) ||
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
	= "meteobrowser.php?station=$HTTP_GET_VARS[station]&level=week&".
		"lang=$HTTP_GET_VARS[lang]&midtime=";
	$weeks = array();
	$lt = localtime($HTTP_GET_VARS[midtime] - 200 * 7200, 1);
	$week = $lt[tm_wday];
	$count = 0;
	for ($i = -199; $i <= 200; $i++) {
		$lt = localtime($HTTP_GET_VARS[midtime] + $i * 7200, 1);
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
if ($HTTP_GET_VARS[level] == "week") {
	printf("<map name=\"weekmap\">\n");
	$basicurl = "meteobrowser.php?station=$HTTP_GET_VARS[station]&".
		"level=day&lang=$HTTP_GET_VARS[lang]&midtime=";
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
		global	$daycoords, $HTTP_GET_VARS, $basicurl, $stationstart;
		$daytime = $HTTP_GET_VARS[midtime] + $i * 86400;
		$daystart = $daytime - 86500/2;
		$dayend = $daytime + 86500/2;
		if (($dayend < $stationstart[$HTTP_GET_VARS[station]]) ||
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
if ($beginintervaltime >= $stationstart[$HTTP_GET_VARS[station]]) {
?>
<a href="meteobrowser.php?station=<?= $HTTP_GET_VARS[station]?>&level=<?= $HTTP_GET_VARS[level]?>&lang=<?echo $HTTP_GET_VARS[lang]?>&midtime=<?= $previousmidtime?>"><img src="previous.png"
alt="previous <?= $HTTP_GET_VARS[level]?>" border="0" /></a>
<?php
}
?>
</td>

<td align="center" width="96">
<?php
if ($interval < 1800) {
?>
<a href="meteobrowser.php?station=<?= $HTTP_GET_VARS[station]?>&level=week&lang=<?echo $HTTP_GET_VARS[lang]?>&midtime=<?= $HTTP_GET_VARS[midtime]?>"><img src="up.png"
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
<a href="meteobrowser.php?station=<?= $HTTP_GET_VARS[station]?>&level=month&lang=<?echo $HTTP_GET_VARS[lang]?>&midtime=<?= $HTTP_GET_VARS[midtime]?>"><img src="<?echo $iconmonth?>" border="0" /><img src="month.png"
alt="month" border="0" /><img src="<?echo $iconmonth?>" border="0" /></a>
<?php
}
?>
</td>

<td align="center" width="96">
<?php
if ($interval < 86400) {
?>
<a href="meteobrowser.php?station=<?echo $HTTP_GET_VARS[station]?>&level=year&lang=<?echo $HTTP_GET_VARS[lang]?>&midtime=<?echo $HTTP_GET_VARS[midtime]?>"><img src="<?echo $iconyear?>" border="0" /><img src="year.png"
alt="year" border="0" /><img src="<?echo $iconyear?>" border="0" /></a>
<?php
}
?>
</td>

<td align="right" width="96">
<?php
if ($endintervaltime <= time()) {
?>
<a href="meteobrowser.php?station=<?echo $HTTP_GET_VARS[station]?>&level=<?echo $HTTP_GET_VARS[level]?>&lang=<?echo $HTTP_GET_VARS[lang]?>&midtime=<?echo $nextmidtime?>"><img src="next.png" alt="next <?echo $HTTP_GET_VARS[level]?>"
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
	if ("day" == $HTTP_GET_VARS[level]) {
		$mapstatement = "";
	} else {
		$mapstatement = sprintf(" usemap=\"#%smap\"",
			$HTTP_GET_VARS[level]);
	}
	printf("<td colspan=\"5\" valign=\"top\">".
		"<img src=\"cache/%s-%s-%s-%s.png\" border=\"0\" ".
		"width=\"500\" height=\"144\"%s /></td>\n",
		$HTTP_GET_VARS[station], $graph, $HTTP_GET_VARS[level],
			$endtimestamp, $mapstatement);
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
