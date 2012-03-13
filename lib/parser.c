
/*  A Bison parser, made from ../lib/parser.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse mpparse
#define yylex mplex
#define yyerror mperror
#define yylval mplval
#define yychar mpchar
#define yydebug mpdebug
#define yynerrs mpnerrs
#define	MP_NUMBER	257
#define	MP_INTEGER	258
#define	MP_DBPORT	259
#define	MP_STRING	260
#define	MP_PORT	261
#define	MP_COLORNAME	262
#define	MP_GRAPHNAME	263
#define	MP_STRINGNAME	264
#define	MP_NUMBERNAME	265
#define	MP_DBNAME	266
#define	MP_STATIONNAME	267
#define	MP_LEFT	268
#define	MP_RIGHT	269
#define	MP_LABEL	270
#define	MP_STATION	271
#define	MP_DATABASE	272
#define	MP_SPEED	273
#define	MP_INTERVAL	274

#line 1 "../lib/parser.y"

/*
 * parser.y -- configuration file parser for meteo graph definitions
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: parser.y,v 1.2 2002/01/27 21:01:43 afm Exp $
 */
#include <mconf.h>
#include <pdebug.h>
#include <mdebug.h>

extern int	mplineno;
mc_node_t	*mpconffile;

#line 17 "../lib/parser.y"
typedef union {
	double		dval;
	int		ival;
	char		*string;
	int		color[3];
	md_t		*md;
	mc_node_t	*mc;
} YYSTYPE;
#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		85
#define	YYFLAG		-32768
#define	YYNTBASE	24

#define YYTRANSLATE(x) ((unsigned)(x) <= 274 ? yytranslate[x] : 48)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    23,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    21,     2,    22,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     7,     9,    11,    13,    19,    23,    27,
    29,    35,    37,    39,    42,    44,    46,    52,    58,    60,
    62,    64,    67,    74,    76,    79,    83,    87,    91,    93,
    95,    97,    99,   102,   106,   110,   116,   120,   124,   126
};

static const short yyrhs[] = {    25,
     0,    26,     0,    25,    26,     0,    31,     0,    27,     0,
    45,     0,    18,    21,    43,    22,    23,     0,    30,    29,
    23,     0,     4,     4,     4,     0,     8,     0,    32,    21,
    33,    22,    23,     0,     9,     0,    34,     0,    33,    34,
     0,    40,     0,    35,     0,    14,    21,    37,    22,    23,
     0,    15,    21,    37,    22,    23,     0,    40,     0,    38,
     0,    36,     0,    37,    36,     0,    20,     4,    21,    39,
    22,    23,     0,    40,     0,    39,    40,     0,    41,     6,
    23,     0,    42,     3,    23,     0,    42,     4,    23,     0,
    28,     0,    10,     0,    11,     0,    44,     0,    43,    44,
     0,    12,     6,    23,     0,     5,     4,    23,     0,    17,
    21,    47,    22,    23,     0,    13,     6,    23,     0,    19,
     4,    23,     0,    46,     0,    47,    46,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    46,    52,    57,    63,    64,    65,    68,    75,    82,    87,
    90,    96,    99,   104,   110,   111,   114,   119,   126,   127,
   130,   135,   141,   151,   156,   162,   174,   179,   186,   189,
   197,   200,   205,   211,   216,   223,   229,   234,   243,   248
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","MP_NUMBER",
"MP_INTEGER","MP_DBPORT","MP_STRING","MP_PORT","MP_COLORNAME","MP_GRAPHNAME",
"MP_STRINGNAME","MP_NUMBERNAME","MP_DBNAME","MP_STATIONNAME","MP_LEFT","MP_RIGHT",
"MP_LABEL","MP_STATION","MP_DATABASE","MP_SPEED","MP_INTERVAL","'{'","'}'","';'",
"conffile","confsections","confsection","dbdef","colordef","colorspec","colorname",
"graph","graphname","parameters","parameter","leftrightparameter","scaleparameter",
"scaleparameters","interval","simpleparameters","simpleparameter","stringparameter",
"numberparameter","dbparameters","dbparameter","station","stationparameter",
"stationparameters", NULL
};
#endif

static const short yyr1[] = {     0,
    24,    25,    25,    26,    26,    26,    27,    28,    29,    30,
    31,    32,    33,    33,    34,    34,    35,    35,    36,    36,
    37,    37,    38,    39,    39,    40,    40,    40,    40,    41,
    42,    43,    43,    44,    44,    45,    46,    46,    47,    47
};

static const short yyr2[] = {     0,
     1,     1,     2,     1,     1,     1,     5,     3,     3,     1,
     5,     1,     1,     2,     1,     1,     5,     5,     1,     1,
     1,     2,     6,     1,     2,     3,     3,     3,     1,     1,
     1,     1,     2,     3,     3,     5,     3,     3,     1,     2
};

static const short yydefact[] = {     0,
    12,     0,     0,     1,     2,     5,     4,     0,     6,     0,
     0,     3,     0,     0,     0,    39,     0,     0,     0,     0,
    32,    10,    30,    31,     0,     0,    29,     0,     0,    13,
    16,    15,     0,     0,     0,     0,     0,    40,     0,     0,
     0,    33,     0,     0,     0,     0,     0,    14,     0,     0,
     0,    37,    38,    36,    35,    34,     7,     0,    21,     0,
    20,    19,     0,     0,     8,    11,    26,    27,    28,     0,
     0,    22,     0,     9,     0,    17,    18,     0,    24,     0,
    25,    23,     0,     0,     0
};

static const short yydefgoto[] = {    83,
     4,     5,     6,    27,    46,    28,     7,     8,    29,    30,
    31,    59,    60,    61,    78,    62,    33,    34,    20,    21,
     9,    16,    17
};

static const short yypact[] = {    19,
-32768,   -19,    -9,    19,-32768,-32768,-32768,    -6,-32768,    38,
    44,-32768,    32,    14,    25,-32768,    26,    37,    52,    -4,
-32768,-32768,-32768,-32768,     9,    39,-32768,    55,    -5,-32768,
-32768,-32768,    57,    51,    41,    43,    45,-32768,    46,    47,
    48,-32768,    24,    24,    63,    49,    50,-32768,    53,    54,
    56,-32768,-32768,-32768,-32768,-32768,-32768,    70,-32768,     3,
-32768,-32768,    11,    71,-32768,-32768,-32768,-32768,-32768,    40,
    58,-32768,    59,-32768,    42,-32768,-32768,    16,-32768,    60,
-32768,-32768,    78,    80,-32768
};

static const short yypgoto[] = {-32768,
-32768,    81,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    61,
-32768,   -56,    62,-32768,-32768,   -13,-32768,-32768,-32768,    64,
-32768,    69,-32768
};


#define	YYLAST		106


static const short yytable[] = {    32,
    18,    10,    22,    72,    23,    24,    72,    19,    25,    26,
    22,    11,    23,    24,    13,    32,    47,    41,    22,    35,
    23,    24,    58,    22,    71,    23,    24,     1,    36,    43,
    58,    22,    73,    23,    24,     2,     3,    80,    14,    22,
    39,    23,    24,    58,    15,    25,    26,    37,    18,    22,
    14,    23,    24,    50,    51,    19,    15,    40,    45,    44,
    75,    79,    49,    52,    81,    53,    64,    54,    55,    56,
    57,    65,    66,    70,    74,    67,    68,    84,    69,    85,
    76,    77,    82,    42,    12,    38,     0,     0,     0,    48,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    63
};

static const short yycheck[] = {    13,
     5,    21,     8,    60,    10,    11,    63,    12,    14,    15,
     8,    21,    10,    11,    21,    29,    22,    22,     8,     6,
    10,    11,    20,     8,    22,    10,    11,     9,     4,    21,
    20,     8,    22,    10,    11,    17,    18,    22,    13,     8,
     4,    10,    11,    20,    19,    14,    15,    22,     5,     8,
    13,    10,    11,     3,     4,    12,    19,     6,     4,    21,
    21,    75,     6,    23,    78,    23,     4,    23,    23,    23,
    23,    23,    23,     4,     4,    23,    23,     0,    23,     0,
    23,    23,    23,    20,     4,    17,    -1,    -1,    -1,    29,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    44
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYPARSE_RETURN_TYPE
#define YYPARSE_RETURN_TYPE int
#endif


#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 222 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
YYPARSE_RETURN_TYPE
yyparse (void *);
#else
YYPARSE_RETURN_TYPE
yyparse (void);
#endif
#endif

YYPARSE_RETURN_TYPE
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
#ifndef YYSTACK_USE_ALLOCA
  int yyfree_stacks = 0;
#endif

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
#ifndef YYSTACK_USE_ALLOCA
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
#endif	    
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 46 "../lib/parser.y"
{
			yyval.mc = yyvsp[0].mc;
			mpconffile = yyval.mc;
		;
    break;}
case 2:
#line 52 "../lib/parser.y"
{
			mc_node_t	*n = mc_node_new();
			mc_node_put(n, yyvsp[0].md);
			yyval.mc = n;
		;
    break;}
case 3:
#line 57 "../lib/parser.y"
{
			mc_node_put(yyvsp[-1].mc, yyvsp[0].md);
			yyval.mc = yyvsp[-1].mc;
		;
    break;}
case 4:
#line 63 "../lib/parser.y"
{ yyval.md = yyvsp[0].md; ;
    break;}
case 5:
#line 64 "../lib/parser.y"
{ yyval.md = yyvsp[0].md; ;
    break;}
case 6:
#line 65 "../lib/parser.y"
{ yyval.md = yyvsp[0].md; ;
    break;}
case 7:
#line 68 "../lib/parser.y"
{
			md_t *m = md_new("database");
			md_set(m, MC_DATA, yyvsp[-2].mc);
			yyval.md = m;
		;
    break;}
case 8:
#line 75 "../lib/parser.y"
{
			md_t	*x = md_new(yyvsp[-2].string);
			md_set(x, MC_COLOR, yyvsp[-1].color);
			yyval.md = x;
		;
    break;}
case 9:
#line 82 "../lib/parser.y"
{
			yyval.color[0] = yyvsp[-2].ival; yyval.color[1] = yyvsp[-1].ival; yyval.color[2] = yyvsp[0].ival;
		;
    break;}
case 10:
#line 87 "../lib/parser.y"
{ yyval.string = yyvsp[0].string; ;
    break;}
case 11:
#line 90 "../lib/parser.y"
{
			md_t	*n = md_new(yyvsp[-4].string);
			md_set(n, MC_DATA, yyvsp[-2].mc);
			yyval.md = n;
		;
    break;}
case 12:
#line 96 "../lib/parser.y"
{ yyval.string = yyvsp[0].string; ;
    break;}
case 13:
#line 99 "../lib/parser.y"
{
			mc_node_t	*n = mc_node_new();
			mc_node_put(n, yyvsp[0].md);
			yyval.mc = n;
		;
    break;}
case 14:
#line 104 "../lib/parser.y"
{
			mc_node_put(yyvsp[-1].mc, yyvsp[0].md);
			yyval.mc = yyvsp[-1].mc;
		;
    break;}
case 15:
#line 110 "../lib/parser.y"
{ yyval.md = yyvsp[0].md; ;
    break;}
case 16:
#line 111 "../lib/parser.y"
{ yyval.md = yyvsp[0].md; ;
    break;}
case 17:
#line 114 "../lib/parser.y"
{
			md_t	*x = md_new("left");
			md_set(x, MC_DATA, yyvsp[-2].mc);
			yyval.md = x;
		;
    break;}
case 18:
#line 119 "../lib/parser.y"
{
			md_t	*x = md_new("right");
			md_set(x, MC_DATA, yyvsp[-2].mc);
			yyval.md = x;
		;
    break;}
case 19:
#line 126 "../lib/parser.y"
{ yyval.md = yyvsp[0].md; ;
    break;}
case 20:
#line 127 "../lib/parser.y"
{ yyval.md = yyvsp[0].md; ;
    break;}
case 21:
#line 130 "../lib/parser.y"
{
			mc_node_t	*n = mc_node_new();
			mc_node_put(n, yyvsp[0].md);
			yyval.mc = n;
		;
    break;}
case 22:
#line 135 "../lib/parser.y"
{
			mc_node_put(yyvsp[-1].mc, yyvsp[0].md);
			yyval.mc = yyvsp[-1].mc;
		;
    break;}
case 23:
#line 141 "../lib/parser.y"
{
			char	label[32];
			md_t	*m;
			snprintf(label, 32, "%d", yyvsp[-4].ival);
			m = md_new(label);
			md_set(m, MC_DATA, yyvsp[-2].mc);
			yyval.md = m;
		;
    break;}
case 24:
#line 151 "../lib/parser.y"
{
			mc_node_t	*n = mc_node_new();
			mc_node_put(n, yyvsp[0].md);
			yyval.mc = n;
		;
    break;}
case 25:
#line 156 "../lib/parser.y"
{
			mc_node_put(yyvsp[-1].mc, yyvsp[0].md);
			yyval.mc = yyvsp[-1].mc;
		;
    break;}
case 26:
#line 162 "../lib/parser.y"
{
			md_t	*m = md_new(yyvsp[-2].string);
			md_set(m, MC_STRING, yyvsp[-1].string);
			yyval.md = m;
		;
    break;}
case 27:
#line 174 "../lib/parser.y"
{
			md_t	*m = md_new(yyvsp[-2].string);
			md_set(m, MC_DOUBLE, &yyvsp[-1].dval);
			yyval.md = m;
		;
    break;}
case 28:
#line 179 "../lib/parser.y"
{
			double	d;
			md_t	*m = md_new(yyvsp[-2].string);
			d = yyvsp[-1].ival;
			md_set(m, MC_DOUBLE, &d);
			yyval.md = m;
		;
    break;}
case 30:
#line 189 "../lib/parser.y"
{ yyval.string = yyvsp[0].string; ;
    break;}
case 31:
#line 197 "../lib/parser.y"
{ yyval.string = yyvsp[0].string; ;
    break;}
case 32:
#line 200 "../lib/parser.y"
{
			mc_node_t *n = mc_node_new();
			mc_node_put(n, yyvsp[0].md);
			yyval.mc = n;
		;
    break;}
case 33:
#line 205 "../lib/parser.y"
{
			mc_node_put(yyvsp[-1].mc, yyvsp[0].md);
			yyval.mc = yyvsp[-1].mc;
		;
    break;}
case 34:
#line 211 "../lib/parser.y"
{
			md_t *m = md_new(yyvsp[-2].string);
			md_set(m, MC_STRING, yyvsp[-1].string);
			yyval.md = m;
		;
    break;}
case 35:
#line 216 "../lib/parser.y"
{
			md_t *m = md_new("port");
			md_set(m, MC_INTEGER, yyvsp[-1].ival);
			yyval.md = m;
		;
    break;}
case 36:
#line 223 "../lib/parser.y"
{
			md_t	*m = md_new("station");
			md_set(m, MC_DATA, yyvsp[-2].mc);
			yyval.md = m;
		;
    break;}
case 37:
#line 229 "../lib/parser.y"
{
			md_t *m = md_new(yyvsp[-2].string);
			md_set(m, MC_STRING, yyvsp[-1].string);
			yyval.md = m;
		;
    break;}
case 38:
#line 234 "../lib/parser.y"
{
			int	speed;
			md_t	*m = md_new("speed");
			speed = yyvsp[-1].ival;
			md_set(m, MC_INTEGER, &speed);
			yyval.md = m;
		;
    break;}
case 39:
#line 243 "../lib/parser.y"
{
			mc_node_t	*n = mc_node_new();
			mc_node_put(n, yyvsp[0].md);
			yyval.mc = n;
		;
    break;}
case 40:
#line 248 "../lib/parser.y"
{
			mc_node_put(yyvsp[-1].mc, yyvsp[0].md);
			yyval.mc = yyvsp[-1].mc;
		;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 554 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
#ifndef YYSTACK_USE_ALLOCA
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
#endif
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
#ifndef YYSTACK_USE_ALLOCA
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
#endif    
  return 1;
}
#line 255 "../lib/parser.y"


int	mperror(char *s) {
	mdebug(LOG_ERR, __FILE__, mplineno, 0,
		"meteo conf file parse error on line %d", mplineno);
	return 0;
}
