<!--

   meteobrowser.php -- php script implementing a browser function for meteo
                       data, as stored in the database by the meteo programs

   (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung

   $Id: meteobrowser.php,v 1.2 2002/01/14 23:39:35 afm Exp $

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
// include("dummygettext.inc");
include("stationlist.inc");
$meteograph = "/usr/local/bin/meteograph";
$confpath = "/home/afm/Projects/meteo";
$cachedir = "/home/afm/Projects/meteo/cache";

// if the variables are not set, we should set them to reasonable defaults
if ($level == "") {
	$level = "day";
}
if ($midtime == "") {
	$midtime = time();	// not a particularly good value, but the
				// normalization below will fix that
}
if ($station == "") {
	$station = "Altendorf";
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
		break;
	case "week":
		$previousmidtime = $midtime - 7 * 86400;
		$nextmidtime = $midtime + 7 * 86400;
		break;
	case "month":
		$previousmidtime = $midtime - 30 * 86400;
		$nextmidtime = $midtime + 30 * 86400;
		break;
	case "year":
		$previousmidtime = $midtime - 365 * 86400;
		$nextmidtime = $midtime + 365 * 86400;
		break;
}

// compute the parameters for the new images
$graphs = split(":", $stationlist[$station]);

// compute the names of images for the images
$cmd = $meteograph." -f $confpath/meteo-$station.conf ".
	"-c $cachedir -a -t ";
$computegraphs = FALSE;
foreach ($graphs as $graph) {
	// compute the file name
	$filename = "cache/$station-$graph-$level-$endtimestamp.png";
	if (FALSE == stat($filename)) {
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
<title><?echo $station?> <?echo gettext("weather overview")?></title>
</head>
<body bgcolor="#ffffff">
<map name="yearmap">
	<?php $basicurl = "meteobrowser.php?station=$station&level=month&midtime="; ?>

	<area coords="0,0,73,142"
	href="<?echo $basicurl?><?echo $midtime - 1296000 - 6 * 2592000?>">
	<area coords="74,0,104,142"
	href="<?echo $basicurl?><?echo $midtime - 1296000 - 5 * 2592000?>">
	<area coords="105,0,133,142"
	href="<?echo $basicurl?><?echo $midtime - 1296000 - 4 * 2592000?>">
	<area coords="134,0,164,142"
	href="<?echo $basicurl?><?echo $midtime - 1296000 - 3 * 2592000?>">
	<area coords="165,0,194,142"
	href="<?echo $basicurl?><?echo $midtime - 1296000 - 2 * 2592000?>">
	<area coords="195,0,225,142"
	href="<?echo $basicurl?><?echo $midtime - 1296000 - 1 * 2592000?>">
	<area coords="226,0,255,142"
	href="<?echo $basicurl?><?echo $midtime - 1296000?>">
	<area coords="256,0,286,142"
	href="<?echo $basicurl?><?echo $midtime - 1296000 + 1 * 2592000?>">
	<area coords="287,0,317,142"
	href="<?echo $basicurl?><?echo $midtime - 1296000 + 2 * 2592000?>">
	<area coords="318,0,347,142"
	href="<?echo $basicurl?><?echo $midtime - 1296000 + 3 * 2592000?>">
	<area coords="348,0,378,142"
	href="<?echo $basicurl?><?echo $midtime - 1296000 + 4 * 2592000?>">
	<area coords="379,0,408,142"
	href="<?echo $basicurl?><?echo $midtime - 1296000 + 5 * 2592000?>">
	<area coords="409,0,439,142"
	href="<?echo $basicurl?><?echo $midtime - 1296000 + 6 * 2592000?>">
	<area coords="440,0,499,142"
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
	<area coords="0,0,<?echo 255 + $weeks[0]?>,142"
	href="<?= $basicurl?><?= $midtime - $midweek * 604800?>">
<?php
	for ($i = 1; $i < $count; $i++) {
		printf("<area coords=\"%d,0,%d,142\"\n",
			255 + $weeks[$i - 1], 255 + $weeks[$i]);
		printf("href=\"%s%d\">\n", $basicurl, 
			$midtime + ($i - $midweek) * 604800);
	}
?>
	<area coords="<?echo 255 + $weeks[$count - 1]?>,0,499,142"
	href="<?= $basicurl?><?= $midtime + ($count - $midweek) * 604800?>">
</map>
<map name="weekmap">
	<?php $basicurl = "meteobrowser.php?station=$station&level=day&midtime="; ?>

	<area coords="0,0,86,142"
		href="<?= $basicurl?><?= $midtime - 4 * 86400?>">
	<area coords="87,0,134,142"
		href="<?= $basicurl?><?= $midtime - 3 * 86400?>">
	<area coords="135,0,182,142"
		href="<?= $basicurl?><?= $midtime - 2 * 86400?>">
	<area coords="183,0,230,142"
		href="<?= $basicurl?><?= $midtime - 1 * 86400?>">
	<area coords="231,0,278,142"
		href="<?= $basicurl?><?= $midtime?>">
	<area coords="279,0,326,142"
		href="<?= $basicurl?><?= $midtime + 1 * 86400?>">
	<area coords="327,0,374,142"
		href="<?= $basicurl?><?= $midtime + 2 * 86400?>">
	<area coords="374,0,422,142"
		href="<?= $basicurl?><?= $midtime + 3 * 86400?>">
	<area coords="423,0,499,142"
		href="<?= $basicurl?><?= $midtime + 4 * 86400?>">
</map>
<h1><?echo $station?> <?echo gettext("weather overview")?></h1>
<p>
Overview for
<?echo $level?>
 from
<?echo date("r", $midtime - 200 * $interval)?>
 to
<?echo date("r", $midtime + 200 * $interval)?>
</p>
<table>
<tr>
<td>
<a href="meteobrowser.php?station=<?echo $station?>&level=<?echo $level?>&midtime=<?echo $previousmidtime?>">previous <?echo $level?></a>
</td>

<?php if ($interval < 1800) { ?>
<td>
<a href="meteobrowser.php?station=<?echo $station?>&level=week&midtime=<?echo $midtime?>">week</a>
</td>
<?php } ?>

<?php if ($interval < 7200) { ?>
<td>
<a href="meteobrowser.php?station=<?echo $station?>&level=month&midtime=<?echo $midtime?>">month</a>
</td>
<?php } ?>

<?php if ($interval < 86400) { ?>
<td>
<a href="meteobrowser.php?station=<?echo $station?>&level=year&midtime=<?echo $midtime?>">year</a>
</td>
<?php } ?>
<td>
<a href="meteobrowser.php?station=<?echo $station?>&level=<?echo $level?>&midtime=<?echo $nextmidtime?>">next <?echo $level?></a>
</td>
</tr>
</table>
</p>
<table>
<?php
foreach ($graphs as $graph) {
	printf("<tr>\n");
	printf("<td><img src=\"cache/%s-%s-%s-%s.png\" border=\"0\" usemap=\"#%smap\"></td>\n",
		$station, $graph, $level, $endtimestamp, $level);
	printf("</tr>\n");
}
?>
</table>

</body>
</html>
