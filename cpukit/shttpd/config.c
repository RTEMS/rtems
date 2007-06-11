/*
 * Copyright (c) 2004-2005 Sergey Lyubka <valenok@gmail.com>
 * All rights reserved
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Sergey Lyubka wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */

#include "defs.h"

/*
 * Configuration parameters setters
 */
static void
set_int(struct shttpd_ctx *ctx, void *ptr, const char *string)
{
	ctx = NULL;	/* Unused */
	* (int *) ptr = atoi(string);
}

static void
set_str(struct shttpd_ctx *ctx, void *ptr, const char *string)
{
	ctx = NULL;	/* Unused */
	* (char **) ptr = my_strdup(string);
}

static void
set_log_file(struct shttpd_ctx *ctx, void *ptr, const char *string)
{
	FILE	**fp = ptr;
	ctx = NULL;

	if ((*fp = fopen(string, "a")) == NULL)
		elog(E_FATAL, NULL, "cannot open log file %s: %s",
		    string, strerror(errno));
}

#ifndef NO_SSL
/*
 * Dynamically load SSL library. Set up ctx->ssl_ctx pointer.
 */
static void
set_ssl(struct shttpd_ctx *ctx, void *arg, const char *pem)
{
	SSL_CTX		*CTX;
	void		*lib;
	struct ssl_func	*fp;

	arg = NULL;	/* Unused */

	/* Load SSL library dynamically */
	if ((lib = dlopen(SSL_LIB, RTLD_LAZY)) == NULL)
		elog(E_FATAL, NULL, "set_ssl: cannot load %s", SSL_LIB);

	for (fp = ssl_sw; fp->name != NULL; fp++)
		if ((fp->ptr.v_void = dlsym(lib, fp->name)) == NULL)
			elog(E_FATAL, NULL,"set_ssl: cannot find %s", fp->name);

	/* Initialize SSL crap */
	SSL_library_init();

	if ((CTX = SSL_CTX_new(SSLv23_server_method())) == NULL)
		elog(E_FATAL, NULL, "SSL_CTX_new error");
	else if (SSL_CTX_use_certificate_file(CTX, pem, SSL_FILETYPE_PEM) == 0)
		elog(E_FATAL, NULL, "cannot open %s", pem);
	else if (SSL_CTX_use_PrivateKey_file(CTX, pem, SSL_FILETYPE_PEM) == 0)
		elog(E_FATAL, NULL, "cannot open %s", pem);
	ctx->ssl_ctx = CTX;
}
#endif /* NO_SSL */

static void
set_mime(struct shttpd_ctx *ctx, void *arg, const char *string)
{
	arg = NULL;
	set_mime_types(ctx, string);
}

#define	OFS(x)	offsetof(struct shttpd_ctx, x)
#define BOOL_OPT	"0|1"
const struct opt options[] = {
	{'d', "document_root", "Web root directory", set_str,
		OFS(document_root), "directory", NULL, OPT_DIR},
	{'i', "index_files", "Index files", set_str, OFS(index_files),
		"file_list", INDEX_FILES, OPT_ADVANCED},
	{'p', "listen_port", "Listening port", set_int,
		OFS(port), "port", HTTP_PORT, OPT_INT | OPT_ADVANCED},
	{'D', "list_directories", "Directory listing", set_int,
		OFS(dirlist), BOOL_OPT, "1", OPT_BOOL | OPT_ADVANCED},
#ifndef NO_CGI
	{'c', "cgi_extensions", "CGI extensions", set_str,
		OFS(cgi_extensions), "ext_list", CGI_EXT, OPT_ADVANCED},
	{'C', "cgi_interpreter", "CGI interpreter", set_str,
		OFS(cgi_interpreter), "file", NULL, OPT_FILE | OPT_ADVANCED},
	{'V', "cgi_envvar", "CGI envir variables", set_str,
		OFS(cgi_vars), "X=Y,....", NULL, OPT_ADVANCED},
#endif /* NO_CGI */
	{'N', "auth_realm", "Authentication realm", set_str,
		OFS(auth_realm), "auth_realm", REALM, OPT_ADVANCED},
	{'l', "access_log", "Access log file", set_log_file,
		OFS(access_log), "file", NULL, OPT_FILE | OPT_ADVANCED},
	{'e', "error_log", "Error log file", set_log_file,
		OFS(error_log), "file", NULL, OPT_FILE | OPT_ADVANCED},
	{'m', "mime_types", "Mime types file", set_mime,
		OFS(mime_file), "file", NULL, OPT_FILE | OPT_ADVANCED},
	{'P', "global_htpasswd", "Global passwords file", set_str,
		OFS(global_passwd_file), "file", NULL, OPT_FILE | OPT_ADVANCED},
#ifndef NO_SSL
	{'s', "ssl_certificate", "SSL certificate file", set_ssl,
		OFS(ssl_ctx), "pem_file", NULL, OPT_FILE | OPT_ADVANCED},
#endif /* NO_SSL */
	{'U', "put_auth", "PUT,DELETE auth file",set_str,
		OFS(put_auth_file), "file", NULL, OPT_FILE | OPT_ADVANCED},
	{'a', "aliases", "Aliases", set_str,
		OFS(aliases), "X=Y,...", NULL, OPT_ADVANCED},
	{'b', "io_buf_size", "IO buffer size", set_int, OFS(io_buf_size),
		"bytes", DFLT_IO_SIZ, OPT_INT | OPT_ADVANCED},
#ifdef _WIN32
	{'S', "auto_start", "Autostart with Windows", set_int,
		OFS(auto_start), BOOL_OPT, "1", OPT_BOOL},
#else
	{'I', "inetd_mode", "Inetd mode", set_int,
		OFS(inetd_mode), BOOL_OPT, NULL, OPT_BOOL	},
	{'u', "runtime_uid", "Run as user", set_str,
		OFS(uid), "user_name", NULL, 0		},
#endif /* _WIN32 */
	{0,   NULL, NULL, NULL, 0, NULL, NULL, 0	}
};

static const struct opt *
find_option(int sw, const char *name)
{
	const struct opt	*opt;

	for (opt = options; opt->sw != 0; opt++)
		if (sw == opt->sw || (name && strcmp(opt->name, name) == 0))
			return (opt);

	return (NULL);
}

static void
set_option(const struct opt *opt, const char *val, char **tmpvars)
{
	tmpvars += opt - options;

	if (*tmpvars != NULL)
		free(*tmpvars);

	*tmpvars = my_strdup(val);
}

/*
 * Initialize shttpd context
 */
static void
initialize_context(struct shttpd_ctx *ctx, const char *config_file,
		int argc, char *argv[], char **tmpvars)
{
	char			line[FILENAME_MAX], root[FILENAME_MAX],
					var[sizeof(line)], val[sizeof(line)];
	const char		*arg;
	size_t			i;
	const struct opt	*opt;
	FILE 			*fp;
	struct tm		*tm;

	current_time = time(NULL);
	tm = localtime(&current_time);
	tz_offset = 0;
#if 0
	tm->tm_gmtoff - 3600 * (tm->tm_isdst > 0 ? 1 : 0);
#endif

	(void) memset(ctx, 0, sizeof(*ctx));

	ctx->start_time = current_time;
	InitializeCriticalSection(&ctx->mutex);

	LL_INIT(&ctx->connections);
	LL_INIT(&ctx->mime_types);
	LL_INIT(&ctx->registered_uris);
	LL_INIT(&ctx->uri_auths);
	LL_INIT(&ctx->error_handlers);

	/* First pass: set the defaults */
	for (opt = options; opt->sw != 0; opt++)
		if (opt->def != NULL)
			tmpvars[opt - options] = my_strdup(opt->def);

	/* Second pass: load config file  */
	if (config_file != NULL && (fp = fopen(config_file, "r")) != NULL) {
		DBG(("init_ctx: config file %s", config_file));

		/* Loop through the lines in config file */
		while (fgets(line, sizeof(line), fp) != NULL) {

			/* Skip comments and empty lines */
			if (line[0] == '#' || line[0] == '\n')
				continue;

			/* Trim trailing newline character */
			line[strlen(line) - 1] = '\0';

			if (sscanf(line, "%s %[^#\n]", var, val) != 2)
				elog(E_FATAL,0,"init_ctx: bad line: [%s]",line);

			if ((opt = find_option(0, var)) == NULL)
				elog(E_FATAL, NULL, 
				    "set_option: unknown variable [%s]", var);
			set_option(opt, val, tmpvars);
		}
		(void) fclose(fp);
	}

	/* Third pass: process command line args */
	for (i = 1; i < (size_t) argc && argv[i][0] == '-'; i++)
		if ((opt = find_option(argv[i][1], NULL)) != NULL) {
			arg = argv[i][2] ? &argv[i][2] : argv[++i];
			
			if (arg == NULL)
				usage(argv[0]);

			set_option(opt, arg, tmpvars);
		} else {
			usage(argv[0]);
		}

	/* Call setters functions now */
	for (i = 0; i < NELEMS(options); i++)
		if (tmpvars[i] != NULL) {
			options[i].setter(ctx,
			    ((char *) ctx) + options[i].ofs, tmpvars[i]);
			free(tmpvars[i]);
		}
	
#ifndef NO_SSL
	/* If SSL is wanted and port is not overridden, set it to 443 */
	if (ctx->port == atoi(HTTP_PORT) && ctx->ssl_ctx != NULL)
		ctx->port = 443;
#endif /* NO_SSL */

	/* If document_root is not set, set it to current directory */
	if (ctx->document_root == NULL) {
		(void) my_getcwd(root, sizeof(root));
		ctx->document_root = my_strdup(root);
	}

#ifdef _WIN32
	{WSADATA data;	WSAStartup(MAKEWORD(2,2), &data);}
#endif /* _WIN32 */

	DBG(("init_ctx: initialized context %p", (void *) ctx));
}

/*
 * Show usage string and exit.
 */
void
usage(const char *prog)
{
	const struct opt	*opt;

	(void) fprintf(stderr,
	    "SHTTPD version %s (c) Sergey Lyubka\n"
	    "usage: %s [OPTIONS] [config_file]\n"
	    "Note: config line keyword for every option is in the "
	    "round brackets\n", VERSION, prog);

#if !defined(NO_AUTH)
	(void) fprintf(stderr, "-A <htpasswd_file> <realm> <user> <passwd>\n");
#endif /* NO_AUTH */

	for (opt = options; opt->name != NULL; opt++)
		(void) fprintf(stderr, "-%c <%s>\t\t%s (%s)\n",
		    opt->sw, opt->arg, opt->desc, opt->name);

	exit(EXIT_FAILURE);
}

struct shttpd_ctx *
init_from_argc_argv(const char *config_file, int argc, char *argv[])
{
	struct shttpd_ctx	*ctx;
	char			*tmpvars[NELEMS(options)];
	size_t			i;

	/* Initialize all temporary holders to NULL */
	for (i = 0; i < NELEMS(tmpvars); i++)
		tmpvars[i] = NULL;

	if ((ctx = malloc(sizeof(*ctx))) != NULL)
		initialize_context(ctx, config_file, argc, argv, tmpvars);
	
	return (ctx);
}

struct shttpd_ctx *
shttpd_init(const char *config_file, ...)
{
	struct shttpd_ctx	*ctx;
	va_list			ap;
	const char		*opt_name, *opt_value;
	char			*tmpvars[NELEMS(options)];
	const struct opt	*opt;
	size_t			i;

	/* Initialize all temporary holders to NULL */
	for (i = 0; i < NELEMS(tmpvars); i++)
		tmpvars[i] = NULL;

	if ((ctx = malloc(sizeof(*ctx))) != NULL) {

		va_start(ap, config_file);
		while ((opt_name = va_arg(ap, const char *)) != NULL) {
			opt_value = va_arg(ap, const char *);
			
			if ((opt = find_option(0, opt_name)) == NULL)
				elog(E_FATAL, NULL, "shttpd_init: "
				    "unknown variable [%s]", opt_name);
			set_option(opt, opt_value, tmpvars);
		}
		va_end(ap);

		initialize_context(ctx, config_file, 0, NULL, tmpvars);
	}

	return (ctx);
}
