%{
/*
 * parser.y -- configuration file parser for meteo graph definitions
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: parser.y,v 1.1 2002/01/18 23:34:30 afm Exp $
 */
#include <mconf.h>

extern int	mplineno;
mc_node_t	*mpconffile;
%}

%union {
	double		dval;
	int		ival;
	char		*string;
	int		color[3];
	md_t		*md;
	mc_node_t	*mc;
}

%token <dval> MP_NUMBER
%token <ival> MP_INTEGER MP_DBPORT
%token <string> MP_STRING MP_PORT

%token <string> MP_COLORNAME MP_GRAPHNAME MP_STRINGNAME MP_NUMBERNAME MP_DBNAME
%token <string> MP_STATIONNAME
%token MP_LEFT MP_RIGHT
%token MP_LABEL MP_STATION MP_DATABASE MP_SPEED
%token MP_INTERVAL

%type <color> colorspec
%type <string> colorname graphname stringparameter numberparameter
%type <md> colordef parameter simpleparameter graph dbparameter station
%type <md> leftrightparameter scaleparameter interval stationparameter
%type <md> dbdef confsection
%type <mc> parameters conffile scaleparameters simpleparameters
%type <mc> dbparameters stationparameters confsections

%%

conffile:	confsections 	{
			$$ = $1;
			mpconffile = $$;
		}
	;

confsections:		confsection		{
			mc_node_t	*n = mc_node_new();
			mc_node_put(n, $1);
			$$ = n;
		}
	|	confsections confsection	{
			mc_node_put($1, $2);
			$$ = $1;
		}
	;

confsection:	graph	{ $$ = $1; }
	|	dbdef	{ $$ = $1; }
	|	station	{ $$ = $1; }
	;

dbdef:		MP_DATABASE '{' dbparameters '}' ';' {
			md_t *m = md_new("database");
			md_set(m, MC_DATA, $3);
			$$ = m;
		}
	;

colordef:	colorname colorspec ';' {
			md_t	*x = md_new($1);
			md_set(x, MC_COLOR, $2);
			$$ = x;
		}
	;

colorspec:	MP_INTEGER MP_INTEGER MP_INTEGER {
			$$[0] = $1; $$[1] = $2; $$[2] = $3;
		}
	;

colorname:	MP_COLORNAME	{ $$ = $1; }
	;

graph:	graphname '{' parameters '}' ';' {
			md_t	*n = md_new($1);
			md_set(n, MC_DATA, $3);
			$$ = n;
		}

graphname:	MP_GRAPHNAME	{ $$ = $1; }
	;

parameters:	parameter 	{
			mc_node_t	*n = mc_node_new();
			mc_node_put(n, $1);
			$$ = n;
		}
	|	parameters parameter	{
			mc_node_put($1, $2);
			$$ = $1;
		}
	;

parameter:	simpleparameter		{ $$ = $1; }
	|	leftrightparameter	{ $$ = $1; }
	;

leftrightparameter:	MP_LEFT '{' scaleparameters '}' ';' {
			md_t	*x = md_new("left");
			md_set(x, MC_DATA, $3);
			$$ = x;
		}
	|	MP_RIGHT '{' scaleparameters '}' ';'	{
			md_t	*x = md_new("right");
			md_set(x, MC_DATA, $3);
			$$ = x;
		}
	;

scaleparameter:	simpleparameter	{ $$ = $1; }
	|	interval	{ $$ = $1; }
	;

scaleparameters:	scaleparameter	{
			mc_node_t	*n = mc_node_new();
			mc_node_put(n, $1);
			$$ = n;
		}
	|	scaleparameters scaleparameter	{
			mc_node_put($1, $2);
			$$ = $1;
		}
	;

interval:	MP_INTERVAL MP_INTEGER '{' simpleparameters '}' ';' {
			char	label[32];
			md_t	*m;
			snprintf(label, 32, "%d", $2);
			m = md_new(label);
			md_set(m, MC_DATA, $4);
			$$ = m;
		}
	;

simpleparameters:	simpleparameter	{
			mc_node_t	*n = mc_node_new();
			mc_node_put(n, $1);
			$$ = n;
		}
	|	simpleparameters simpleparameter {
			mc_node_put($1, $2);
			$$ = $1;
		}
	;
	
simpleparameter:	stringparameter MP_STRING ';' {
			md_t	*m = md_new($1);
			md_set(m, MC_STRING, $2);
			$$ = m;
		}
/*
	|	integerparameter MP_INTEGER ';'	{
			md_t	*m = md_new($1);
			md_set(m, MC_INTEGER, &$2);
			$$ = m;
		}
*/
	|	numberparameter MP_NUMBER ';' {
			md_t	*m = md_new($1);
			md_set(m, MC_DOUBLE, &$2);
			$$ = m;
		}
	|	numberparameter MP_INTEGER ';' {
			double	d;
			md_t	*m = md_new($1);
			d = $2;
			md_set(m, MC_DOUBLE, &d);
			$$ = m;
		}
	|	colordef
	;

stringparameter:	MP_STRINGNAME	{ $$ = $1; }
	;

/*
integerparameter:
	;
*/

numberparameter:	MP_NUMBERNAME	{ $$ = $1; }
	;

dbparameters:	dbparameter {
			mc_node_t *n = mc_node_new();
			mc_node_put(n, $1);
			$$ = n;
		}
	|	dbparameters dbparameter {
			mc_node_put($1, $2);
			$$ = $1;
		}
	;

dbparameter:	MP_DBNAME MP_STRING ';' {
			md_t *m = md_new($1);
			md_set(m, MC_STRING, $2);
			$$ = m;
		}
	|	MP_DBPORT MP_INTEGER ';' {
			md_t *m = md_new("port");
			md_set(m, MC_INTEGER, $2);
			$$ = m;
		}
	;

station:	MP_STATION '{' stationparameters '}' ';' {
			md_t	*m = md_new("station");
			md_set(m, MC_DATA, $3);
			$$ = m;
		}

stationparameter:	MP_STATIONNAME MP_STRING ';' {
			md_t *m = md_new($1);
			md_set(m, MC_STRING, $2);
			$$ = m;
		}
	|	MP_SPEED MP_INTEGER ';' {
			int	speed;
			md_t	*m = md_new("speed");
			speed = $2;
			md_set(m, MC_INTEGER, &speed);
			$$ = m;
		}
	;

stationparameters:	stationparameter	{
			mc_node_t	*n = mc_node_new();
			mc_node_put(n, $1);
			$$ = n;
		}
	|	stationparameters stationparameter	{
			mc_node_put($1, $2);
			$$ = $1;
		}
	;

	
%%

int	mperror(char *s) {
	fprintf(stderr, "%s:%d: meteo conf file parse error on line %d\n",
		__FILE__, __LINE__, mplineno);
	return 0;
}
