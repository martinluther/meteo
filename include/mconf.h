/*
 * mconf.h -- configuration file processing
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: mconf.h,v 1.2 2002/03/03 22:09:38 afm Exp $
 */
#ifndef _MCONF_H
#define _MCONF_H

#include <meteograph.h>
#include <sys/types.h>
#include <stdio.h>

typedef enum mc_type_e {
	MC_INTEGER = 0, MC_DOUBLE = 1, MC_STRING = 2, MC_COLOR = 3, MC_DATA = 4
} mc_type_t;

typedef int	color_t[3];

typedef struct mc_node_s mc_node_t;

typedef union val_u {
	char		*string;
	int		ival;
	double		dval;
	color_t		color;
	mc_node_t	*data;
} val_t;

typedef struct md_s {
	char		*key;
	mc_type_t	type;
	val_t		data;
} md_t;

extern md_t	*md_new(const char *);
extern void	md_clear(md_t *);
extern void	md_free(md_t *);
extern md_t	*md_copy(const md_t *);
extern md_t	*md_assign(md_t *, const md_t *);
extern void	md_set(md_t *, mc_type_t, void *);
extern void	md_display(FILE *, const md_t *);

struct mc_node_s {
	size_t	length;
	md_t	**parms;
};

extern mc_node_t	*mc_node_new(void);
extern void		mc_node_clear(mc_node_t *);
extern void		mc_node_free(mc_node_t *);
extern int		mc_node_put(mc_node_t *, const md_t *);
extern const md_t	*mc_node_get(const mc_node_t *, const char *);
extern int		mc_node_del(mc_node_t *, const char *);
extern mc_node_t	*mc_node_copy(const mc_node_t *);

extern mc_node_t	*mc_parse(FILE *);
extern mc_node_t	*mc_readconf(const char *);
extern void		mc_dump(FILE *, const mc_node_t *);

/*
 * search the parse tree for a certain value, we use a hierarchical name space
 * as in the java properties
 */
extern const md_t	*mc_getvalue(const mc_node_t *, const char *);

#define	M_MC_GET_PROTO(type, funcname)					\
extern const type	funcname(const mc_node_t *, const char *key,	\
				const type def);			\
extern const type	funcname##_f(const mc_node_t *, 		\
				const char *grname,			\
				const char *lr, int interval, 		\
				const char *what, const type def);
M_MC_GET_PROTO(double, mc_get_double)
M_MC_GET_PROTO(int, mc_get_int)
M_MC_GET_PROTO(char *, mc_get_string)
M_MC_GET_PROTO(int *, mc_get_color)
	
#endif /* _MCONF_H */
