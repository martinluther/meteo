<!--

   meteobrowser.php -- php script implementing a browser function for meteo
                       data, as stored in the database by the meteo programs

   (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung

   $Id: meteobrowser.php,v 1.3 2002/01/15 23:44:36 afm Exp $

   This scripts generates overview pages for meteorological data containing
   client side image maps (so that a lynx browser can also profit from these
   pages). Clicking on an image should bring up a new page with the next
   level of detail. There are also a few buttons that allow direct navigation
   to the next or previous time interval (day, week, month, year).

   The position is controlled by the following variables:

	$level		"day", "week", "month", "year"
	$midtime	unix time for the middle of the current interval
	$station	station name

-->
<?php
// include the following only if your PHP installation lacks gettext support
// not really a problem right now, as the i18n has not been done yet.
// include("dummygettext.inc");

// the stationlist contains a colon separated list of graphs to be drawn 
// for each station that can be browsed through the current instance of
// the script, see the sample file for instructions.
include("stationlist.inc");

// the path the the meteograph binary, could be derived from the configure
// run, but as quite a few things have to be configured in the script anyway,
// doing it manually isn't really more complicated.
$meteograph = "/usr/local/bin/meteograph";

// the directory containing the configuration files, which must have names
// of the form $confpath/meteo-$stationname.conf
$confpath = "/home/afm/Projects/meteo";

// a directory writable by the webserver just below the installation location
// of this script, or at least so that a browser sees it just besides 
// meteobrowser.php
$cachedir = "/home/afm/Projects/meteo/cache";

// the interval after after which a graph must be recomputed, even if it
// is unlikely that new data has arrived.
$rintval = 86400;	// recompute graphs older than a day

// defaults for the script, used to display something useful to the user
// if called without any parameters (so that it can be used as an index
// page)
$level_default = "year";
$midtime_default = time();	// not a particularly good value, but the
				// normalization below will fix that
$station_default = "Altendorf";	// could use the environment variables to
				// find out which station to display so that
				// the script could be used with virtual hosts

//-------------------------------------------------------------------------
// No configurable code below this line
//-------------------------------------------------------------------------
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
		break;
	case "week":
		$previousmidtime = $midtime - 7 * 86400;
		$nextmidtime = $midtime + 7 * 86400;
		$iconmonth = "up.png";
		$iconyear = "upup.png";
		break;
	case "month":
		$previousmidtime = $midtime - 30 * 86400;
		$nextmidtime = $midtime + 30 * 86400;
		$iconyear = "up.png";
		break;
	case "year":
		$previousmidtime = $midtime - 365 * 86400;
		$nextmidtime = $midtime + 365 * 86400;
		break;
}

// compute the parameters for the new images
$graphs = split(":", $stationlist[$station]);

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

?>

<html>
<head>
<link href="meteo.css" type="text/css" rel="stylesheet">
<title><?echo $station?> <?echo gettext("weather overview")?></title>
</head>
<body bgcolor="#ffffff">
<map name="yearmap">
	<?php
		$basicurl
		= "meteobrowser.php?station=$station&level=month&midtime=";
	?>

	<area coords="0,0,73,142" alt="Dec"
	href="<?echo $basicurl?><?echo $midtime - 1296000 - 6 * 2592000?>">
	<area coords="74,0,104,142" alt="Jan"
	href="<?echo $basicurl?><?echo $midtime - 1296000 - 5 * 2592000?>">
	<area coords="105,0,133,142" alt="Feb"
	href="<?echo $basicurl?><?echo $midtime - 1296000 - 4 * 2592000?>">
	<area coords="134,0,164,142" alt="Mar"
	href="<?echo $basicurl?><?echo $midtime - 1296000 - 3 * 2592000?>">
	<area coords="165,0,194,142" alt="Apr"
	href="<?echo $basicurl?><?echo $midtime - 1296000 - 2 * 2592000?>">
	<area coords="195,0,225,142" alt="May"
	href="<?echo $basicurl?><?echo $midtime - 1296000 - 1 * 2592000?>">
	<area coords="226,0,255,142" alt="Jun"
	href="<?echo $basicurl?><?echo $midtime - 1296000?>">
	<area coords="256,0,286,142" alt="Jul"
	href="<?echo $basicurl?><?echo $midtime - 1296000 + 1 * 2592000?>">
	<area coords="287,0,317,142" alt="Aug"
	href="<?echo $basicurl?><?echo $midtime - 1296000 + 2 * 2592000?>">
	<area coords="318,0,347,142" alt="Sep"
	href="<?echo $basicurl?><?echo $midtime - 1296000 + 3 * 2592000?>">
	<area coords="348,0,378,142" alt="Oct"
	href="<?echo $basicurl?><?echo $midtime - 1296000 + 4 * 2592000?>">
	<area coords="379,0,408,142" alt="Nov"
	href="<?echo $basicurl?><?echo $midtime - 1296000 + 5 * 2592000?>">
	<area coords="409,0,439,142" alt="Dec"
	href="<?echo $basicurl?><?echo $midtime - 1296000 + 6 * 2592000?>">
	<area coords="440,0,499,142" alt="Jan"
	href="<?echo $basicurl?><?echo $midtime - 1296000 + 7 * 2592000?>">
</map>
<map name="monthmap">
<?php
	$basicurl = "meteobrowser.php?station=$station&level=week&midtime=";
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
?>
	<area coords="0,0,<?= 255 + $weeks[0]?>,142"
	href="<?echo $basicurl?><?= $midtime - $midweek * 604800?>">
<?php
	for ($i = 1; $i < $count; $i++) {
		printf("<area coords=\"%d,0,%d,142\"\n",
			255 + $weeks[$i - 1], 255 + $weeks[$i]);
		printf("href=\"%s%d\">\n", $basicurl, 
			$midtime + ($i - $midweek) * 604800);
	}
?>
	<area coords="<?= 255 + $weeks[$count - 1]?>,0,499,142"
	href="<?= $basicurl?><?= $midtime + ($count - $midweek) * 604800?>">
</map>
<map name="weekmap">
	<?php
		$basicurl
		= "meteobrowser.php?station=$station&level=day&midtime=";
	?>

	<area coords="0,0,86,142" alt="Sat"
		href="<?= $basicurl?><?= $midtime - 4 * 86400?>">
	<area coords="87,0,134,142" alt="Sun"
		href="<?= $basicurl?><?= $midtime - 3 * 86400?>">
	<area coords="135,0,182,142" alt="Mon"
		href="<?= $basicurl?><?= $midtime - 2 * 86400?>">
	<area coords="183,0,230,142" alt="Tue"
		href="<?= $basicurl?><?= $midtime - 1 * 86400?>">
	<area coords="231,0,278,142" alt="Wed"
		href="<?= $basicurl?><?= $midtime?>">
	<area coords="279,0,326,142" alt="Thu"
		href="<?= $basicurl?><?= $midtime + 1 * 86400?>">
	<area coords="327,0,374,142" alt="Fri"
		href="<?= $basicurl?><?= $midtime + 2 * 86400?>">
	<area coords="374,0,422,142" alt="Sat"
		href="<?= $basicurl?><?= $midtime + 3 * 86400?>">
	<area coords="423,0,499,142" alt="Sun"
		href="<?= $basicurl?><?= $midtime + 4 * 86400?>">
</map>
<h1><?echo $station?> <?echo gettext("weather overview")?></h1>
<p>
Overview for
<b><?echo $level?></b>
 from
<b><?echo date("r", $midtime - 200 * $interval)?></b>
 to
<b><?echo date("r", $midtime + 200 * $interval)?></b>
</p>
<p>
To see more details, click into the region you like inside the graphs
below, to zoom out or move forward or backward in time, use the navigation
bar.
</p>
<table width="100%">
<tr>

<td align="left">
<a href="meteobrowser.php?station=<?= $station?>&level=<?= $level?>&midtime=<?= $previousmidtime?>"><img src="previous.png"
alt="previous <?= $level?>" border="0" /></a>
</td>

<?php
if ($interval < 1800) {
?>
<td align="center">
<a href="meteobrowser.php?station=<?= $station?>&level=week&midtime=<?= $midtime?>"><img src="up.png"
border="0" /><img src="week.png" alt="week" border="0" /><img src="up.png"
border="0" /></a>
</td>
<?php
}
?>

<?php
if ($interval < 7200) {
?>
<td align="center">
<a href="meteobrowser.php?station=<?= $station?>&level=month&midtime=<?= $midtime?>"><img src="<?echo $iconmonth?>" border="0" /><img src="month.png"
alt="month" border="0" /><img src="<?echo $iconmonth?>" border="0" /></a>
</td>
<?php
}
?>

<?php
if ($interval < 86400) {
?>
<td align="center">
<a href="meteobrowser.php?station=<?echo $station?>&level=year&midtime=<?echo $midtime?>"><img src="<?echo $iconyear?>" border="0" /><img src="year.png"
alt="year" border="0" /><img src="<?echo $iconyear?>" border="0" /></a>
</td>
<?php
}
?>

<td align="right">
<a href="meteobrowser.php?station=<?echo $station?>&level=<?echo $level?>&midtime=<?echo $nextmidtime?>"><img src="next.png" alt="next <?echo $level?>"
border="0" /></a>
</td>


</tr>
</table>
</p>

<table>
<?php
foreach ($graphs as $graph) {
	printf("<tr>\n");
	printf("<td><img src=\"cache/%s-%s-%s-%s.png\" border=\"0\" ".
		"usemap=\"#%smap\"></td>\n",
		$station, $graph, $level, $endtimestamp, $level);
	printf("</tr>\n");
}
?>
</table>

<hr />
<p>
Graphs produced by the <a href="http://meteo.othello.ch/">meteo</a> package,
&copy; 2002 <a href="mailto:afm@othello.ch">Dr. Andreas M&uuml;ller</a>,
<a href="http://www.othello.ch/">Beratung und Entwicklung</a>
</p>
</body>
</html>
