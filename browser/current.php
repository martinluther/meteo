<?php
//
// current.php -- sample page to display current meteo data using
//                the meteo map file
//

// pipe the output of the meteomap programm through xsltproc, 
// thereby converting it to HTML
passthru("/usr/local/bin/meteomap /usr/local/etc/meteo-Altendorf.map | /usr/bin/xsltproc /usr/local/lib/meteo-current.xsl -");
?>

