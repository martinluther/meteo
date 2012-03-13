#!/bin/sh

# $Id: do-wu-update.sh,v 1.1 2003/06/08 10:42:07 afm Exp $

WEATHER_DIR=/home/foo/weather
LOG=$WEATHER_DIR/do-wu-update.log

#
# get some fresh data from the meteo database
# and write it into the input format expected by wu-upload.pl
#
$WEATHER_DIR/meteo-wu-fmt.pl > $WEATHER_DIR/meteo.out

#
# use wu-upload.pl to upload our station's data to Weather Underground
#
$WEATHER_DIR/wu-upload.pl > $LOG

