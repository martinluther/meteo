typedef union {
	double		dval;
	int		ival;
	char		*string;
	int		color[3];
	md_t		*md;
	mc_node_t	*mc;
} YYSTYPE;
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


extern YYSTYPE mplval;
