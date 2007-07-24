/*
 * Copyright (c) 2004-2005 Sergey Lyubka <valenok@gmail.com>
 * All rights reserved
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Sergey Lyubka wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */

#ifndef DEFS_HEADER_DEFINED
#define	DEFS_HEADER_DEFINED

#include "std_includes.h"
#include "llist.h"
#include "io.h"
#include "shttpd.h"
#include "md5.h"

#define	VERSION		"1.37"		/* Version			*/

#ifndef CONFIG
#define	CONFIG		"shttpd.conf"	/* Configuration file		*/
#endif /* CONFIG */

#define	HTPASSWD	".htpasswd"	/* Passwords file name		*/
#define	DFLT_IO_SIZ	"16384"		/* Default max request size	*/
#define	HTTP_PORT	"80"		/* Default listening port	*/
#define	INDEX_FILES	"index.html index.htm index.php index.cgi"
#define	CGI_EXT		".cgi .pl .php"	/* Default CGI extensions	*/
#define	REALM		"mydomain.com"	/* Default authentication realm	*/
#define	DELIM_CHARS	" ,"		/* Separators for lists		*/

#define	EXPIRE_TIME	3600		/* Expiration time, seconds	*/
#define	ENV_MAX		4096		/* Size of environment block	*/
#define	CGI_ENV_VARS	64		/* Maximum vars passed to CGI	*/
#define	URI_MAX		32768		/* Maximum URI size		*/
#define	MIN_REQ_LEN	16		/* "GET / HTTP/1.1\n\n"		*/

#define	NELEMS(ar)	(sizeof(ar) / sizeof(ar[0]))

#ifdef _DEBUG
#define	DBG(x)	do { printf x ; putchar('\n'); fflush(stdout); } while (0)
#else
#define	DBG(x)
#endif /* DEBUG */

#ifdef EMBEDDED
#include "shttpd.h"
#endif /* EMBEDDED */

/*
 * Darwin prior to 7.0 and Win32 do not have socklen_t
 */
#ifdef NO_SOCKLEN_T
typedef int socklen_t;
#endif /* NO_SOCKLEN_T */

/*
 * For parsing. This guy represents a substring.
 */
struct vec {
	const char	*ptr;
	int		len;
};

enum {METHOD_GET, METHOD_POST, METHOD_PUT, METHOD_DELETE, METHOD_HEAD};
enum {HDR_DATE, HDR_INT, HDR_STRING};	/* HTTP header types		*/
enum {E_FATAL = 1, E_LOG = 2};		/* Flags for elog() function	*/
typedef unsigned long big_int_t;	/* Type for Content-Length	*/
	
/*
 * Unified socket address
 */
struct usa {
	socklen_t len;
	union {
		struct sockaddr	sa;
		struct sockaddr_in sin;
	} u;
};

/*
 * This thing is aimed to hold values of any type.
 * Used to store parsed headers' values.
 */
union variant {
	char		*v_str;
	int		v_int;
	big_int_t	v_big_int;
	time_t		v_time;
	void		(*v_func)(void);
	void		*v_void;
	struct vec	v_vec;
};

/*
 * This structure is used to hold mime types and associated file extensions.
 */
struct mime_type {
	const char	*ext;
	int		ext_len;
	const char	*mime;
};

struct mime_type_link {
	struct llhead	link;
	char		*ext;
	int		ext_len;
	char		*mime;
};

/*
 * This is used only in embedded configuration. This structure holds a
 * registered URI, associated callback function with callback data.
 * For non-embedded compilation shttpd_callback_t is not defined, so
 * we use union variant to keep the compiler silent.
 */
struct registered_uri {
	struct llhead	link;
	const char	*uri;
	union variant	callback;
	void		*callback_data;
};

/*
 * User may bind a passwords file to any URI. This makes that URI password
 * protected: anybody who accesses that URI will be asked to authorize.
 */
struct uri_auth {
	struct llhead	link;
	const char	*uri;
	const char	*file_name;
	size_t		uri_len;
};

/*
 * User may want to handle certain errors. This structure holds the
 * handlers for corresponding error codes.
 */
struct error_handler {
	struct llhead	link;
	int		code;
	union variant	callback;
	void		*callback_data;
};

struct http_header {
	int		len;		/* Header name length		*/
	int		type;		/* Header type			*/
	size_t		offset;		/* Value placeholder		*/
	const char	*name;		/* Header name			*/
};

/*
 * This guy holds parsed HTTP headers
 */
struct headers {
	union variant	cl;		/* Content-Length:		*/
	union variant	ct;		/* Content-Type:		*/
	union variant	connection;	/* Connection:			*/
	union variant	ims;		/* If-Modified-Since:		*/
	union variant	user;		/* Remote user name		*/
	union variant	auth;		/* Authorization		*/
	union variant	useragent;	/* User-Agent:			*/
	union variant	referer;	/* Referer:			*/
	union variant	cookie;		/* Cookie:			*/
	union variant	location;	/* Location:			*/
	union variant	range;		/* Range:			*/
	union variant	status;		/* Status:			*/
	union variant	transenc;	/* Transfer-Encoding:		*/
};

/* Must go after union variant definition */
#include "ssl.h"

/*
 * The communication channel
 */
union channel {
	int		fd;		/* Regular static file		*/
	int		sock;		/* Connected socket		*/
	struct {
		int		sock;	/* XXX important. must be first	*/
		SSL		*ssl;	/* shttpd_poll() assumes that	*/
	} ssl;				/* SSL-ed socket		*/
	struct {
		DIR	*dirp;
		char	*path;
	} dir;				/* Opened directory		*/
	struct {
		void		*state;	/* For keeping state		*/
		union variant	func;	/* User callback function	*/
		void		*data;	/* User defined parameters	*/
	} emb;				/* Embedded, user callback	*/
};

struct stream;

/*
 * IO class descriptor (file, directory, socket, SSL, CGI, etc)
 * These classes are defined in io_*.c files.
 */
struct io_class {
	const char *name;
	int (*read)(struct stream *, void *buf, size_t len);
	int (*write)(struct stream *, const void *buf, size_t len);
	void (*close)(struct stream *);
};

/*
 * Data exchange stream. It is backed by some communication channel:
 * opened file, socket, etc. The 'read' and 'write' methods are
 * determined by a communication channel.
 */
struct stream {
	struct conn		*conn;
	union channel		chan;		/* Descriptor		*/
	struct io		io;		/* IO buffer		*/
	const struct io_class	*io_class;	/* IO class		*/
	int			nread_last;	/* Bytes last read	*/
	int			headers_len;
	big_int_t		content_len;
	unsigned int		flags;
#define	FLAG_HEADERS_PARSED	1
#define	FLAG_SSL_ACCEPTED	2
#define	FLAG_R			4		/* Can read in general	*/
#define	FLAG_W			8		/* Can write in general	*/
#define	FLAG_CLOSED		16
#define	FLAG_DONT_CLOSE		32
#define	FLAG_ALWAYS_READY	64		/* File, dir, user_func	*/
};

struct conn {
	struct llhead	link;		/* Connections chain		*/
	struct shttpd_ctx *ctx;		/* Context this conn belongs to */
	struct usa	sa;		/* Remote socket address	*/
	time_t		birth_time;	/* Creation time		*/
	time_t		expire_time;	/* Expiration time		*/

	int		status;		/* Reply status code		*/
	int		method;		/* Request method		*/
	char		*uri;		/* Decoded URI			*/
	unsigned long	major_version;	/* Major HTTP version number    */
	unsigned long	minor_version;	/* Minor HTTP version number    */
	char		*request;	/* Request line			*/
	char		*headers;	/* Request headers		*/
	char		*query;		/* QUERY_STRING part of the URI	*/
	char		*path_info;	/* PATH_INFO thing		*/
	const char	*mime_type;	/* Mime type			*/

	struct headers	ch;		/* Parsed client headers	*/

	struct stream	loc;		/* Local stream			*/
	struct stream	rem;		/* Remote stream		*/
};


/*
 * SHTTPD context
 */
struct shttpd_ctx {
	time_t		start_time;	/* Start time			*/
	int		nactive;	/* # of connections now		*/
	unsigned long	nrequests;	/* Requests made		*/
	uint64_t	in, out;	/* IN/OUT traffic counters	*/
#if !defined(NO_SSL)
	SSL_CTX		*ssl_ctx;	/* SSL context			*/
#endif /* NO_SSL */
	struct llhead	connections;	/* List of connections		*/

	struct llhead	mime_types;	/* Known mime types		*/
	struct llhead	registered_uris;/* User urls			*/
	struct llhead	uri_auths;	/* User auth files		*/
	struct llhead	error_handlers;	/* Embedded error handlers	*/

	FILE	*access_log;		/* Access log stream		*/
	FILE	*error_log;		/* Error log stream		*/
	char	*put_auth_file;		/* PUT auth file		*/
	char	*document_root;		/* Document root		*/
	char	*index_files;		/* Index files			*/
	char	*aliases;		/* Aliases			*/
	char	*mime_file;		/* Mime types file		*/
#if !defined(NO_CGI)
	char	*cgi_vars;		/* CGI environment variables	*/
	char	*cgi_extensions;	/* CGI extensions		*/
	char	*cgi_interpreter;	/* CGI script interpreter	*/
#endif /* NO_CGI */
	char	*auth_realm;		/* Auth realm			*/
	char	*global_passwd_file;	/* Global passwords file	*/
	char	*uid;			/* Run as user			*/
	int	port;			/* Listening port		*/
	int	dirlist;		/* Directory listing		*/
	int	gui;			/* Show GUI flag		*/
	int	auto_start;		/* Start on OS boot		*/
	int	io_buf_size;		/* IO buffer size		*/
	int	inetd_mode;		/* Inetd flag			*/
#if defined(_WIN32)
	CRITICAL_SECTION mutex;		/* For MT case		*/
	HANDLE		ev[2];		/* For thread synchronization */
#elif defined(__rtems__)
	rtems_id         mutex;
#endif /* _WIN32 */
};

struct listener {
	struct llhead	link;
	struct shttpd_ctx *ctx;		/* Context that socket belongs	*/
	int		sock;		/* Listening socket		*/
};

/* Option setter function */
typedef void (*optset_t)(struct shttpd_ctx *, void *ptr, const char *string);
struct opt {
	int		sw;		/* Command line switch		*/
	const char	*name;		/* Option name in config file	*/
	const char	*desc;		/* Description			*/
	optset_t	setter;		/* Option setter function	*/
	size_t		ofs;		/* Value offset in context	*/
	const char	*arg;		/* Argument format		*/
	const char	*def;		/* Default option value		*/
	unsigned int	flags;		/* Flags			*/
#define	OPT_BOOL	1
#define	OPT_INT		2
#define	OPT_FILE	4
#define	OPT_DIR		8
#define	OPT_ADVANCED	16
};

extern const struct opt options[];

/*
 * In SHTTPD, list of values are represented as comma or space separated
 * string. For example, list of CGI extensions can be represented as
 * ".cgi,.php,.pl", or ".cgi .php .pl". The macro that follows allows to
 * loop through the individual values in that list.
 * A "const char *" pointer and size_t variable must be passed to the macro.
 * Spaces or commas can be used as delimiters (macro DELIM_CHARS)
 */
#define	FOR_EACH_WORD_IN_LIST(s,len)	\
	for (; s != NULL && (len = strcspn(s, DELIM_CHARS)) != 0; s += len + 1)

/*
 * shttpd.c
 */
extern time_t		current_time;	/* Current UTC time		*/
extern int		tz_offset;	/* Offset from GMT time zone	*/
extern const struct vec known_http_methods[];

extern void	stop_stream(struct stream *stream);
extern void	decode_url_encoded_string(const char *, int, char *dst, int);
extern void	send_server_error(struct conn *, int code, const char *reason);
extern int	get_headers_len(const char *buf, size_t buflen);
extern void	parse_headers(const char *s, int len, struct headers *parsed);

/*
 * mime_type.c
 */
extern const char *get_mime_type(struct shttpd_ctx *, const char *uri, int len);
extern void	set_mime_types(struct shttpd_ctx *ctx, const char *path);

/*
 * config.c
 */
extern void	usage(const char *prog);
extern struct shttpd_ctx *init_from_argc_argv(const char *, int, char *[]);

/*
 * log.c
 */
extern void	elog(int flags, struct conn *c, const char *fmt, ...);
extern void	log_access(FILE *fp, const struct conn *c);

/*
 * string.c
 */
#ifndef HAVE_STRLCPY
extern void	my_strlcpy(register char *, register const char *, size_t);
#else
#include <string.h>
#define my_strlcpy(d,s,l) strlcpy(d,s,l)
#endif

#ifndef HAVE_STRNCASECMP
extern int	my_strncasecmp(register const char *,
		register const char *, size_t);
#else
#ifdef __rtems__
/* strncasecmp should be in strings.h, but newlib has it in <string.h> */
#include <string.h>
#else
#include <strings.h>
#endif
#define my_strncasecmp(s1,s2,l) strncasecmp(s1,s2,l)
#endif

#ifndef HAVE_STRNDUP
extern char	*my_strndup(const char *ptr, size_t len);
#else
#include <string.h>
#define my_strndup(x,l) strndup((x),(l))
#endif

#ifndef HAVE_STRDUP
extern char	*my_strdup(const char *str);
#else
#include <string.h>
#define my_strdup(x) strdup(x)
#endif
extern int	my_snprintf(char *buf, size_t buflen, const char *fmt, ...);

/*
 * compat_*.c
 */
extern void	set_close_on_exec(int fd);
extern int	set_non_blocking_mode(int fd);
#if __rtems__
#define my_stat	stat
#define my_open open
#define my_remove remove
#define my_rename rename
#define my_mkdir mkdir
#define my_getcwd getcwd
#else
extern int	my_stat(const char *, struct stat *stp);
extern int	my_open(const char *, int flags, int mode);
extern int	my_remove(const char *);
extern int	my_rename(const char *, const char *);
extern int	my_mkdir(const char *, int);
extern char *	my_getcwd(char *, int);
#endif
extern int	spawn_process(struct conn *c, const char *prog,
		char *envblk, char *envp[], int sock, const char *dir);

/*
 * io_*.c
 */
extern const struct io_class	io_file;
extern const struct io_class	io_socket;
extern const struct io_class	io_ssl;
extern const struct io_class	io_cgi;
extern const struct io_class	io_dir;
extern const struct io_class	io_embedded;

extern int	put_dir(const char *path);
extern void	get_dir(struct conn *c);
extern void	get_file(struct conn *c, struct stat *stp);
extern void	ssl_handshake(struct stream *stream);
extern void	setup_embedded_stream(struct conn *, union variant, void *);
extern struct registered_uri *is_registered_uri(struct shttpd_ctx *,
		const char *uri);

/*
 * auth.c
 */
extern int	check_authorization(struct conn *c, const char *path);
extern int	is_authorized_for_put(struct conn *c);
extern void	send_authorization_request(struct conn *c);
extern int	edit_passwords(const char *fname, const char *domain,
		const char *user, const char *pass);

/*
 * cgi.c
 */
extern int	is_cgi(struct shttpd_ctx *ctx, const char *path);
extern int	run_cgi(struct conn *c, const char *prog);
extern void	do_cgi(struct conn *c);

#define CGI_REPLY	"HTTP/1.1     OK\r\n"
#define	CGI_REPLY_LEN	(sizeof(CGI_REPLY) - 1)

#endif /* DEFS_HEADER_DEFINED */
