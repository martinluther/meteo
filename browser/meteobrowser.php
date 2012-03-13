<?php
//
//   meteobrowser.php -- php script implementing a browser function for meteo
//                       data, as stored in the database by the meteo programs
//
//   (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
//
//   $Id: meteobrowser.php,v 1.30 2004/05/08 22:39:43 afm Exp $
//
//   This scripts generates overview pages for meteorological data containing
//   client side image maps (so that a lynx browser can also profit from these
//   pages). Clicking on an image should bring up a new page with the next
//   level of detail. There are also a few buttons that allow direct navigation
//   to the next or previous time interval (day, week, month, year).
//
//   The position is controlled by the following variables:
//
//	$HTTP_GET_VARS[label]	string encoding for the current time and
//				interval type
//	$HTTP_GET_VARS[station]	station name
//
if (!function_exists('gettext')) {
	function gettext($msgid) { return $msgid; }
	function bindtextdomain($domain, $path) { return; }
	function textdomain($domain) { return; }
}
?>
<html>
<head>
<link href="meteo.css" type="text/css" rel="stylesheet">
<?php

$graphlist = array();
$conflist = array();
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
if ($HTTP_GET_VARS[station] != "") {
	$station = $HTTP_GET_VARS[station];
} else {
	$station = $station_default;
}
$baseurl = $pageurl."?lang=$HTTP_GET_VARS[lang]&station=$HTTP_GET_VARS[station]";
setlocale(LC_ALL, $HTTP_GET_VARS[lang]);
putenv("LANG=$HTTP_GET_VARS[lang]");
bindtextdomain("meteobrowser", "./locale");
textdomain("meteobrowser");

// compute a default label
$label_default = "m".date("Ym");

// if the variables are not set, we should set them to reasonable defaults
if ($HTTP_GET_VARS[label] == "") {
	$HTTP_GET_VARS[label] = $label_default;
}
if ($HTTP_GET_VARS[station] == "") {
	$HTTP_GET_VARS[station] = $station_default;
}
$station = $HTTP_GET_VARS[station];

// convert the label into a cleartext string
$title = $HTTP_GET_VARS[label];

// compute the parameters for the new images
$graphs =  split(":", $graphlist[$station]);

// check whether there is something in the conflist array for this station
if ("" != $conflist[$station]) {
	$conffile = $conflist[$station];
} else {
	$conffile = $confpath . "/meteo.xml";
}

// compute the names of images for the images
$basecmd = $meteodraw." -s $station ".
		"-f $conffile ".
		"-p $cachedir -L $HTTP_GET_VARS[label] -u $baseurl";
 
//printf("<pre>%s</pre>\n", $basecmd);

$timestamp = shell_exec($meteolabel." -s ".$HTTP_GET_VARS[label]);

// compute a numeric version of the current level
if (0 == strncmp($HTTP_GET_VARS[label], "d", 1)) {
	$level = 0;
	$title = gettext("weather overview of %B %e, %Y");
	$starttime = $timestamp - 200 * 300;
	$endtime = $timestamp + 200 * 300;
}
if (0 == strncmp($HTTP_GET_VARS[label], "w", 1)) {
	$level = 1;
	$title = gettext("weather overview of week %V, %Y, ");
	$dateformat = gettext("%x");
	$fromto = sprintf(gettext("from %s to %s"),
		strftime("%x", $timestamp - 3 * 86400),
		strftime("%x", $timestamp + 3 * 86400));
	$title .= $fromto;
	$starttime = $timestamp - 200 * 1800;
	$endtime = $timestamp + 200 * 1800;
}
if (0 == strncmp($HTTP_GET_VARS[label], "m", 1)) {
	$level = 2;
	$title = gettext("weather overview of %B %Y");
	$starttime = $timestamp - 200 * 7200;
	$endtime = $timestamp + 200 * 7200;
}
if (0 == strncmp($HTTP_GET_VARS[label], "y", 1)) {
	$level = 3;
	$title = gettext("weather overview of %Y");
	$starttime = $timestamp - 200 * 86400;
	$endtime = $timestamp + 200 * 86400;
}
$fulltitle = $station." ".strftime($title, $timestamp);

?>

<title><?echo $fulltitle?></title>
<meta name="robots" content="index,nofollow">
</head>
<body bgcolor="#ffffff">

<h1><?echo $fulltitle?></h1>

<table cellpadding="0" cellspacing="0" border="0">

<?php
// compute the map for the current label
$cmd = $basecmd." -g $graphs[0] -I -m";
//printf("map command: %s", $cmd);
system(escapeshellcmd($cmd));

?>

<tr>
<td colspan="6">
<?= gettext("To see more details, click into the region you like inside ".
	"the graphs below, to zoom out or move forward or backward ".
	"in time, use the navigation bar.")?>
</td>
</tr>

<?php
// Here we start to compute the tags for going up to higher resolution
// levels
$hops = "up";
?>
<tr>

<td align="center" width="96">
<?php
if ($starttime > $stationstart[$station]) {
	$previouslabel = shell_exec($meteolabel." ".$HTTP_GET_VARS[label].
		" previous");
	$url = $baseurl . "&label=".$previouslabel;
?>
<a href="<? echo $url ?>"><img src="previous.png"
	alt="previous <?echo $HTTP_GET_VARS[label]?>"
	border="0" /></a>
<?php
}
?>
</td>

<td align="center" width="96">
<?php
// compute the link to go up to the week, this should only be set if the
// current label begins with a d

if ($level == 0) {
	// compute higher level URL
	$uplabel = shell_exec($meteolabel." ".$HTTP_GET_VARS[label]." up");
	$url = $baseurl."&label=".$uplabel;
?>
<a href="<? echo $url ?>"><img src="<? echo $hops ?>.png"
	border="0" /><img src="week.png" border="0" /><img
	src="<? echo $hops ?>.png" border="0" /></a>
<?php
	$hops = $hops."up";
}
?>
</td>

<td align="center" width="96">
<?php
// compute the link to go up to the week, this should only be set if the
// current label begins with a d
if (($level == 0) || ($level == 1)) {
	switch ($level) {
		case 0:
			$uplabel = shell_exec($meteolabel." ".
				$HTTP_GET_VARS[label]." upup");
			break;
		case 1:
			$uplabel = shell_exec($meteolabel." ".
				$HTTP_GET_VARS[label]." up");
			break;
	}
	// compute higher level URL
	$url = $baseurl."&label=".$uplabel;
?>
<a href="<? echo $url ?>"><img src="<? echo $hops ?>.png"
	border="0" /><img src="month.png" border="0" /><img
	src="<? echo $hops ?>.png" border="0" /></a>
<?php
	$hops = $hops."up";
}
?>
</td>

<td align="center" width="96">
<?php
// compute the link to go up to the week, this should only be set if the
// current label begins with a d
if (($level == 0) || ($level == 1) || ($level == 2)) {
	switch ($level) {
		case 0:
			$uplabel = shell_exec($meteolabel." ".
				$HTTP_GET_VARS[label]." upupup");
			break;
		case 1:
			$uplabel = shell_exec($meteolabel." ".
				$HTTP_GET_VARS[label]." upup");
			break;
		case 2:
			$uplabel = shell_exec($meteolabel." ".
				$HTTP_GET_VARS[label]." up");
			break;
	}
	// compute higher level URL
	$url = $baseurl."&label=".$uplabel;
?>
<a href="<? echo $url ?>"><img src="<? echo $hops ?>.png"
	border="0" /><img src="year.png" border="0" /><img
	src="<? echo $hops ?>.png" border="0" /></a>
<?php
	$hops = $hops."up";
}
?>
</td>

<td align="center" width="96">
<?php
if ($endtime < time()) {
	$nextlabel = shell_exec("/usr/local/bin/meteolabel $HTTP_GET_VARS[label] next");
$url = $baseurl . "&label=".$nextlabel;
?>
<a href="<? echo $url ?>"><img src="next.png"
	alt="next <?echo $HTTP_GET_VARS[label]?>"
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
$explanation[temperature_inside] =
	"The <font color=\"#b40000\">red</font> curve shows inside ".
	"temperature, the <font color=\"#ffb4b4\">light red</font> ".
	"area shows the range between maximum and minimum temperature ".
	"during the sampling interval.";
$explanation[pressure] = 
	"The <font color=\"#7f7fff\">blue</font> graph shows ".
	"average barometric pressure, the range between minimum ".
	"and maximum pressure during the sampling interval is shown ".
	"in <font color=\"#d2d2ff\">light blue</font>.";
$explanation[humidity] =
	"The <font color=\"#7f7fff\">blue</font> graph shows ".
	"relative humidity.";
$explanation[humidity_inside] =
	"The <font color=\"#7f7fff\">blue</font> graph shows ".
	"inside relative humidity.";
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

// create the map by running meteodraw once without graph arguments

// now run the meteograph program once for each graph. This will produce
// the new graph and will return a <img> tag suitable for inclusion in
// the page
foreach ($graphs as $graph) {
?>
	<tr>
	<td colspan="5" valign="top">
<?php
	// call the meteodraw program to rebuild the image and spit
	// out a suitable img link
	$cmd = $basecmd." -g $graph -I -i";
// printf("%s\n", $cmd);
	system(escapeshellcmd($cmd));
?>
	</td>
	<td valign="top"><? echo gettext($explanation[$explanationname[$graph]]) ?></td>
	</tr>
<?php
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
