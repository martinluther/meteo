%{
/*
 * scanner.l -- configuration file scanner
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: scanner.l,v 1.5 2001/12/26 22:10:46 afm Exp $
 */
#include <mconf.h>
#include <parser.h>
#include <string.h>
int	mplineno = 0;
%}

comment	\/\/.*
eol	\n
string	\"[^"]*\"
integer	-?[0-9]+
number	-?(([0-9]+)|([0-9]*\.[0-9]+))

%%

{comment}	; /* just forget about comments 			*/
[ \t]*		; /* absorb white space					*/
{string}	{
			mplval.string = strdup(mptext + 1);
			mplval.string[strlen(mplval.string) - 1] = '\0';
			return MP_STRING;
		 }
{eol}		{ mplineno++; } /* increment the line counter 		*/
{integer}	{
			mplval.ival = atoi(mptext);
			return MP_INTEGER;
		}
{number}	{ mplval.dval = atof(mptext); return MP_NUMBER; }

pressure|temperature|temperature_inside|rain|wind|radiation {
		mplval.string = strdup(mptext);
		return MP_GRAPHNAME;
	}

interval	{ return MP_INTERVAL; 		}

min|max|scale|start|end|step	{
		mplval.string = strdup(mptext);
		return MP_NUMBERNAME; 
	}
label|format	{
		mplval.string = strdup(mptext);
		return MP_STRINGNAME;
	}

left	{ return MP_LEFT; }
right	{ return MP_RIGHT; }

color|gustcolor|speedcolor|rangecolor|dewcolor|solarcolor|uvcolor	{
		mplval.string = strdup(mptext);
		return MP_COLORNAME;
	}

database	{ return MP_DATABASE; }
hostname|dbname|prefix|user|password|writer|writerpassword|msgqueue {
		mplval.string = strdup(mptext);
		return MP_DBNAME;
	}
port	{
		mplval.string = strdup(mptext);
		return MP_PORT;
	}

station		{ return MP_STATION; }
speed		{ return MP_SPEED; }
url|type	{
		mplval.string = strdup(mptext);
		return MP_STATIONNAME;
	}
	

.		{ return mptext[0];		}

%%

int	mpwrap(void) {
	/* stop scanning */
	return 1;
}
