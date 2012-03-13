/*
 * mconf.c -- routines to process a configuration file and to return
 *            a configuration structure
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: mconf.c,v 1.1 2002/01/18 23:34:29 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <meteo.h>
#include <mconf.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <search.h>
#include <stdio.h>

/*
 * create and destroy value types
 */
md_t	*md_new(const char *key) {
	md_t	*result = NULL;
	if (key == NULL) {
		errno = EFAULT;
		return NULL;
	}
	result = (md_t *)malloc(sizeof(md_t));
	if (result) {
		result->key = strdup(key);
		result->type = MC_INTEGER;
		result->data.ival = 0;
	}
	return result;
}
void	md_clear(md_t *x) {
	if (x == NULL) return;
	if (x->key) { free(x->key); x->key = NULL; }
	if (x->type == MC_STRING) {
		if (x->data.string) {
			free(x->data.string);
			x->data.string = NULL;
		}
	}
	x->type = MC_INTEGER;
	x->data.ival = 0;
}
void	md_free(md_t *x) {
	if (x == NULL) return;
	md_clear(x);
	free(x);
}
static void	md_copy_val(md_t *result, const md_t *x) {
	switch (x->type) {
	case MC_STRING:
		result->data.string = strdup(x->data.string);
		break;
	case MC_INTEGER:
		result->data.ival = x->data.ival;
		break;
	case MC_DOUBLE:
		result->data.dval = x->data.dval;
		break;
	case MC_COLOR:
		memcpy(result->data.color, x->data.color, sizeof(color_t));
		break;
	case MC_DATA:
		result->data.data = mc_node_copy(x->data.data);
		break;
	default:
		fprintf(stderr, "%s:%d: unknown type: %d\n", __FILE__, __LINE__,
			result->type);
		return;
	}
	result->type = x->type;
}
md_t	*md_copy(const md_t *x) {
	md_t	*result;
	if (x == NULL) return NULL;
	result = md_new(x->key);
	if (result == NULL) return NULL;
	md_copy_val(result, x);
	return result;
}
md_t	*md_assign(md_t *result, const md_t *x) {
	if ((result == NULL) || (x == NULL)) { errno = EINVAL; return NULL; }
	md_clear(result);
	result->key = strdup(x->key);
	md_copy_val(result, x);
	return result;
}

void	md_set(md_t *result, mc_type_t t, void *data) {
	if (result == NULL) return;
	result->type = t;
	switch (t) {
	case MC_STRING:
		result->data.string = strdup((char *)data);
		break;
	case MC_INTEGER:
		result->data.ival = *(int *)data;
		break;
	case MC_DOUBLE:
		result->data.dval = *(double *)data;
		break;
	case MC_COLOR:
		memcpy(result->data.color, (int *)data, sizeof(color_t));
		break;
	case MC_DATA:
		result->data.data = mc_node_copy(data);
		break;
	}
}
static void	mc_dump_1(FILE *outfile, const mc_node_t *n, int level);
static void	md_display_1(FILE *outfile, const md_t *m, const int level) {
	char	*prefix = "";
	if (m == NULL) return;
	switch (m->type) {
	case MC_STRING:
		fprintf(outfile, "%*.*s%s \"%s\"; \t\t// (md_t *)%p\n",
			8 * level, 8 * level, "", m->key, m->data.string, m);
		break;
	case MC_INTEGER:
		fprintf(outfile, "%*.*s%s %d; \t\t// (md_t *)%p\n",
			8 * level, 8 * level, "", m->key, m->data.ival, m);
		break;
	case MC_DOUBLE:
		fprintf(outfile, "%*.*s%s %f; \t\t// (md_t *)%p\n",
			8 * level, 8 * level, "", m->key, m->data.dval, m);
		break;
	case MC_COLOR:
		fprintf(outfile, "%*.*s%s %d %d %d; \t\t// (md_t *)%p\n",
			8 * level, 8 * level, "", m->key,
			m->data.color[0], m->data.color[1], m->data.color[2],
			m);
		break;
	case MC_DATA:
		if ((0 == strcmp(m->key, "300")) ||
			(0 == strcmp(m->key, "1800")) ||
			(0 == strcmp(m->key, "7200")) ||
			(0 == strcmp(m->key, "86400"))) {
			prefix = "interval ";
		}
		fprintf(outfile, "%*.*s%s%s { \t\t// (md_t *)%p -> "
			"(mc_node_t *)%p\n", 8 * level, 8 * level, "",
			prefix, m->key, m, m->data.data);
		mc_dump_1(outfile, m->data.data, level + 1);
		fprintf(outfile, "%*.*s};\n", 8 *level, 8 * level, "");
		break;
	}
}
void	md_display(FILE *outfile, const md_t *m) {
	md_display_1(outfile, m, 0);
}

static int	md_compare(const md_t **a, const md_t **b) {
	if ((a == NULL) || (b == NULL)) return 0;
	if ((*a == NULL) || (*b == NULL)) return 0;
	return strcmp((*a)->key, (*b)->key);
}

/*
 * create and destroy nodes
 */
mc_node_t	*mc_node_new(void) {
	mc_node_t	*result = (mc_node_t *)malloc(sizeof(mc_node_t));
	if (result) {
		result->parms = NULL;
		result->length = 0;
	}
	return result;
}
void	mc_node_clear(mc_node_t *x) {
	int	i;
	if (x == NULL) return;
	for (i = 0; i < x->length; i++) {
		md_free(x->parms[i]);
	}
}
void	mc_node_free(mc_node_t *x) {
	if (x == NULL) return;
	mc_node_clear(x);
	free(x);
}

/*
 * mc_node_find -- find the index of an md_t* in the parms array matching
 *                 the given key. This is a static method, as nobody should
 *                 play around with the internal structure of the mc_node,
 *                 but to be serious about this, we would have to make the
 *                 type opaque, which is too much of a hassle.
 */
static int	mc_node_find(const mc_node_t *n, const char *key) {
	int	i;
	size_t	l;
	md_t	**mp, *m;
	/* search for the element					*/
	m = md_new(key);
	l = n->length;
	mp = (md_t **)lfind(&m, n->parms, &l, sizeof(md_t *),
		(int (*)(const void *, const void *))md_compare);
	md_free(m);
	if (mp == NULL) { errno = ESRCH; return -1; }
	/* compute the position of the element				*/
	i = mp - n->parms;
	return i;
}
static int	mc_node_del_i(mc_node_t *n, const int i) {
	/* catch common error conditions				*/
	if (n == NULL) { errno = EINVAL; return -1; }
	if ((i < 0) || (i >= n->length)) { errno = EINVAL; return -1; }
	/* free the data structure at the given offset			*/
	md_free(n->parms[i]); n->parms[i] = NULL;
	/* adapt the length, so it becomes easier to compute the length	*/
	/* of the data block we need to copy				*/
	n->length--;
	memcpy(&n->parms[i], &n->parms[i+1], sizeof(md_t *) * (n->length - i));
	return 0;
}

int	mc_node_del(mc_node_t *n, const char *key) {
	int	i;
	/* return an error if the key does not exist			*/
	if ((i = mc_node_find(n, key)) < 0) {
		errno = ESRCH;
		return -1;
	}
	/* delete the data at the position found			*/
	return mc_node_del_i(n, i);
}

int	mc_node_put(mc_node_t *n, const md_t *val) {
	int	i;
	md_t	**newdata;
	if ((n == NULL) || (val == NULL)) { errno = EINVAL; return -1; }
	if ((i = mc_node_find(n, val->key)) >= 0) {
		mc_node_del(n, val->key);
	}
	newdata = (md_t **)realloc(n->parms, (n->length + 1) * sizeof(md_t *));
	if (newdata == NULL) {
		errno = ENOMEM; return -1;
	}
	n->parms = newdata;
	n->parms[n->length] = md_copy(val);
	n->length++;
	return 0;
}

/*
 * mc_node_get -- returns a md_t * which is managed by the mc_node functions,
 *                so if you plan to destroy or modify the structure, make a
 *                copy first.
 */
const md_t	*mc_node_get(const mc_node_t *n, const char *key) {
	int	i;
	if ((i = mc_node_find(n, key)) < 0) {
		errno = ESRCH;
		return NULL;
	}
	return n->parms[i];
}

mc_node_t	*mc_node_copy(const mc_node_t *n) {
	int		i;
	mc_node_t	*result = mc_node_new();
	if (result) {
		for (i = 0; i < n->length; i++)
			mc_node_put(result, n->parms[i]);
	} else
		errno = ENOMEM;
	return result;
}

/*
 * parse a configuration file
 */
extern FILE	*mpin;
extern mc_node_t	*mpconffile;
extern int mpdebug;
extern int mpparse(void);
mc_node_t	*mc_parse(FILE *in) {
	if (debug > 1)
		mpdebug = 1;
	mpin = in;
	mpparse();
	return mpconffile;
}

mc_node_t	*mc_readconf(const char *conffilename) {
	FILE		*conffile;
	mc_node_t	*result;
	if (strcmp(conffilename, "-")) {
		if (NULL == (conffile = fopen(conffilename, "r"))) {
			fprintf(stderr, "%s:%d: cannot read conf file %s: "
				"%s (%d)\n", __FILE__, __LINE__, conffilename,
				strerror(errno), errno);
			exit(EXIT_FAILURE);
		}
	} else {
		conffile = stdin;
	}
	result = mc_parse(conffile);
	if (conffile != stdin) {
		fclose(conffile);
	}
	return result;
}

/*
 * dump the contents of the parse tree
 */
static void	mc_dump_1(FILE *outfile, const mc_node_t *n, const int level) {
	int	i;
	if (n == NULL) return;
	for (i = 0; i < n->length; i++) {
		md_display_1(outfile, n->parms[i], level);
	}
}
void	mc_dump(FILE *outfile, const mc_node_t *n) {
	mc_dump_1(outfile, n, 0);
}

/*
 * search the parse tree for a value, e.g. for "wind.left.300.min". This
 * should return the last of the values min, wind.min, wind.left.min and
 * wind.left.300.min found.
 */
const md_t	*mc_getvalue(const mc_node_t *n, const char *name) {
	const md_t	*r2;
	md_t		*result = NULL;
	const mc_node_t	*mc = n;
	char		*nm;
	char		**dirnames = 0, **dir;
	char		*basename, *remain;
	int		i, l, k;

	/* create a copy of the name string				*/
	nm = (char *)alloca(strlen(name) + 1);
	strcpy(nm, name);

	/* split the name into components				*/
	remain = nm; i = 0; k = 0;
	while (*remain) {
		l = strcspn(remain, ".");
		dirnames = (char **)realloc(dirnames, sizeof(char *) * ++k);
		dirnames[i++] = remain;
		if (remain[l] == '\0')
			break;
		remain[l] = '\0';
		remain += l + 1;
	}
	k--;
	basename = dirnames[k];
	dirnames[k] = NULL;

	/* try to walk the try, retrieving a value for the last 	*/
	/* component at each step					*/
	dir = dirnames;
	mc = n;
	while (mc != NULL) {
		/* try to find a value in the current node		*/
		r2 = mc_node_get(mc, basename);
		if (r2 != NULL) {
			result = r2;
		}

		/* if there aren't any other path components, give up	*/
		if (*dir == NULL) {
			break;
		}

		/* step to the next node				*/
		r2 = mc_node_get(mc, *dir); dir++;
		if (r2 != NULL) {
			if (r2->type == MC_DATA)
				mc = r2->data.data;
			else {
				mc = NULL;
			}
		} else
			mc = NULL;
	}

	/* dirnames							*/
	free(dirnames);

	/* return the last value found					*/
	return result;
}

/*
 * type save accessor methods
 */
const double	mc_get_double(const mc_node_t *mc, const char *key,
			const double def) {
	const md_t	*m;
	/* retrieve the md_t for this key				*/
	m = mc_getvalue(mc, key);
	if (NULL == m)
		return def;

	/* assure the type is correct					*/
	if (m->type != MC_DOUBLE)
		return def;

	/* return correctly typed value					*/
	return m->data.dval;
}

const int	mc_get_int(const mc_node_t *mc, const char *key,
			const int def) {
	const md_t	*m;
	/* retrieve the md_t for this key				*/
	m = mc_getvalue(mc, key);
	if (NULL == m)
		return def;

	/* assure the type is correct					*/
	if (m->type != MC_INTEGER)
		return def;

	/* return correctly typed value					*/
	return m->data.ival;
}

const char	*mc_get_string(const mc_node_t *mc, const char *key,
			const char *def) {
	const md_t	*m;
	/* retrieve the md_t for this key				*/
	m = mc_getvalue(mc, key);
	if (NULL == m)
		return def;

	/* assure the type is correct					*/
	if (m->type != MC_STRING)
		return def;

	/* return correctly typed value					*/
	return m->data.string;
}

const int	*mc_get_color(const mc_node_t *mc, const char *key,
			const int *def) {
	const md_t	*m;
	/* retrieve the md_t for this key				*/
	m = mc_getvalue(mc, key);
	if (NULL == m)
		return def;

	/* assure the type is correct					*/
	if (m->type != MC_COLOR)
		return def;

	/* return correctly typed value					*/
	return m->data.color;
	
}
