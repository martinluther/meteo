<?php
//
// dummygettext.inc -- dummy functions for those sites that don't have the
//                     gettext function
//
// (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
//
// $Id: dummygettext.inc,v 1.2 2002/01/30 09:49:01 afm Exp $
//
function gettext($msgid) { return $msgid; }
function bindtextdomain($domain, $path) { return; }
function textdomain($domain) { return; }

?>
