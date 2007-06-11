/*
 * Copyright (c) 2004-2005 Sergey Lyubka <valenok@gmail.com>
 * All rights reserved
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Sergey Lyubka wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */

/*
 * Small and portable HTTP server, http://shttpd.sourceforge.net
 * $Id$
 */

#include "defs.h"

time_t		current_time;	/* Current UTC time		*/
int		tz_offset;	/* Time zone offset from UTC	*/

static LL_HEAD(listeners);	/* List of listening sockets	*/

const struct vec known_http_methods[] = {
	{"GET",		3},
	{"POST",	4},
	{"PUT",		3},
	{"DELETE",	6},
	{"HEAD",	4},
	{NULL,		0}
};

/*
 * This structure tells how HTTP headers must be parsed.
 * Used by parse_headers() function.
 */
#define	OFFSET(x)	offsetof(struct headers, x)
static const struct http_header http_headers[] = {
	{16, HDR_INT,	 OFFSET(cl),		"Content-Length: "	},
	{14, HDR_STRING, OFFSET(ct),		"Content-Type: "	},
	{12, HDR_STRING, OFFSET(useragent),	"User-Agent: "		},
	{19, HDR_DATE,	 OFFSET(ims),		"If-Modified-Since: "	},
	{15, HDR_STRING, OFFSET(auth),		"Authorization: "	},
	{9,  HDR_STRING, OFFSET(referer),	"Referer: "		},
	{8,  HDR_STRING, OFFSET(cookie),	"Cookie: "		},
	{10, HDR_STRING, OFFSET(location),	"Location: "		},
	{8,  HDR_INT,	 OFFSET(status),	"Status: "		},
	{7,  HDR_STRING, OFFSET(range),		"Range: "		},
	{12, HDR_STRING, OFFSET(connection),	"Connection: "		},
	{19, HDR_STRING, OFFSET(transenc),	"Transfer-Encoding: "	},
	{0,  HDR_INT,	 0,			NULL			}
};

struct shttpd_ctx *init_ctx(const char *config_file, int argc, char *argv[]);

void
decode_url_encoded_string(const char *src, int src_len, char *dst, int dst_len)
{
	int	i, j, a, b;
#define	HEXTOI(x)  (isdigit(x) ? x - '0' : x - 'W')

	for (i = j = 0; i < src_len && j < dst_len - 1; i++, j++)
		switch (src[i]) {
		case '%':
			if (isxdigit(((unsigned char *) src)[i + 1]) &&
			    isxdigit(((unsigned char *) src)[i + 2])) {
				a = tolower(((unsigned char *)src)[i + 1]);
				b = tolower(((unsigned char *)src)[i + 2]);
				dst[j] = (HEXTOI(a) << 4) | HEXTOI(b);
				i += 2;
			} else {
				dst[j] = '%';
			}
			break;
		case '+':
			dst[j] = ' ';
			break;
		default:
			dst[j] = src[i];
			break;
		}

	dst[j] = '\0';	/* Null-terminate the destination */
}

void
shttpd_add_mime_type(struct shttpd_ctx *ctx, const char *ext, const char *mime)
{
	struct mime_type_link	*e;
	const char		*error_msg = "shttpd_add_mime_type: no memory";

	if ((e = malloc(sizeof(*e))) == NULL) {
		elog(E_FATAL, 0, error_msg);
	} else if ((e->ext= my_strdup(ext)) == NULL) {
		elog(E_FATAL, 0, error_msg);
	} else if ((e->mime = my_strdup(mime)) == NULL) {
		elog(E_FATAL, 0, error_msg);
	} else {
		e->ext_len = strlen(ext);
		LL_TAIL(&ctx->mime_types, &e->link);
	}
}


static const char *
is_alias(struct shttpd_ctx *ctx, const char *uri,
		struct vec *a_uri, struct vec *a_path)
{
	const char	*p, *s = ctx->aliases;
	size_t		len;

	DBG(("is_alias: aliases [%s]", s == NULL ? "" : s));

	FOR_EACH_WORD_IN_LIST(s, len) {
		if ((p = memchr(s, '=', len)) != NULL &&
		    memcmp(uri, s, p - s) == 0) {
			a_uri->ptr = s;
			a_uri->len = p - s;
			a_path->ptr = ++p;
			a_path->len = (s + len) - p;
			return (s);
		}
	}

	return (NULL);
}

void
stop_stream(struct stream *stream)
{
	if (stream->io_class != NULL && stream->io_class->close != NULL)
		stream->io_class->close(stream);

	stream->io_class= NULL;
	stream->flags |= FLAG_CLOSED;
	stream->flags &= ~(FLAG_R | FLAG_W | FLAG_ALWAYS_READY);

	DBG(("%d %s stopped. %lu of content data, %d now in a buffer",
	    stream->conn->rem.chan.sock, 
	    stream->io_class ? stream->io_class->name : "(null)",
	    (unsigned long) stream->io.total, io_data_len(&stream->io)));
}

/*
 * Setup listening socket on given port, return socket
 */
static int
open_listening_port(int port)
{
	int		sock, on = 1;
	struct usa	sa;

#ifdef _WIN32
	{WSADATA data;	WSAStartup(MAKEWORD(2,2), &data);}
#endif /* _WIN32 */

	sa.len				= sizeof(sa.u.sin);
	sa.u.sin.sin_family		= AF_INET;
	sa.u.sin.sin_port		= htons((uint16_t) port);
	sa.u.sin.sin_addr.s_addr	= htonl(INADDR_ANY);

	if ((sock = socket(PF_INET, SOCK_STREAM, 6)) == -1)
		goto fail;
	if (set_non_blocking_mode(sock) != 0)
		goto fail;
	if (setsockopt(sock, SOL_SOCKET,
	    SO_REUSEADDR,(char *) &on, sizeof(on)) != 0)
		goto fail;
	if (bind(sock, &sa.u.sa, sa.len) < 0)
		goto fail;
	if (listen(sock, 128) != 0)
		goto fail;

#ifndef _WIN32
	(void) fcntl(sock, F_SETFD, FD_CLOEXEC);
#endif /* !_WIN32 */

	return (sock);
fail:
	if (sock != -1)
		(void) closesocket(sock);
	elog(E_LOG, NULL, "open_listening_port(%d): %s", port, strerror(errno));
	return (-1);
}

/*
 * Check whether full request is buffered Return headers length, or 0
 */
int
get_headers_len(const char *buf, size_t buflen)
{
	const char	*s, *e;
	int		len = 0;

	for (s = buf, e = s + buflen - 1; len == 0 && s < e; s++)
		/* Control characters are not allowed but >=128 is. */
		if (!isprint(*(unsigned char *)s) && *s != '\r' && *s != '\n' && *(unsigned char *)s < 128)
			len = -1;
		else if (s[0] == '\n' && s[1] == '\n')
			len = s - buf + 2;
		else if (s[0] == '\n' && &s[1] < e &&
		    s[1] == '\r' && s[2] == '\n')
			len = s - buf + 3;

	return (len);
}

/*
 * Send error message back to a client.
 */
void
send_server_error(struct conn *c, int status, const char *reason)
{
#ifdef EMBEDDED
	struct llhead		*lp;
	struct error_handler	*e;

	LL_FOREACH(&c->ctx->error_handlers, lp) {
		e = LL_ENTRY(lp, struct error_handler, link);

		if (e->code == status) {
			if (c->loc.io_class != NULL &&
			    c->loc.io_class->close != NULL)
				c->loc.io_class->close(&c->loc);
			io_clear(&c->loc.io);
			setup_embedded_stream(c, e->callback, NULL);
			return;
		}
	}
#endif /* EMBEDDED */

	io_clear(&c->loc.io);
	c->loc.headers_len = c->loc.io.head = my_snprintf(c->loc.io.buf,
	    c->loc.io.size, "HTTP/1.1 %d %s\r\n\r\n%d %s",
	    status, reason, status, reason);
	c->status = status;
	stop_stream(&c->loc);
}

/*
 * Convert month to the month number. Return -1 on error, or month number
 */
static int
montoi(const char *s)
{
	static const char *ar[] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	size_t	i;

	for (i = 0; i < sizeof(ar) / sizeof(ar[0]); i++)
		if (!strcmp(s, ar[i]))
			return (i);

	return (-1);
}

/*
 * Parse date-time string, and return the corresponding time_t value
 */
static time_t
date_to_epoch(const char *s)
{
	struct tm	tm, *tmp;
	char		mon[32];
	int		sec, min, hour, mday, month, year;

	(void) memset(&tm, 0, sizeof(tm));
	sec = min = hour = mday = month = year = 0;

	if (((sscanf(s, "%d/%3s/%d %d:%d:%d",
	    &mday, mon, &year, &hour, &min, &sec) == 6) ||
	    (sscanf(s, "%d %3s %d %d:%d:%d",
	    &mday, mon, &year, &hour, &min, &sec) == 6) ||
	    (sscanf(s, "%*3s, %d %3s %d %d:%d:%d",
	    &mday, mon, &year, &hour, &min, &sec) == 6) ||
	    (sscanf(s, "%d-%3s-%d %d:%d:%d",
	    &mday, mon, &year, &hour, &min, &sec) == 6)) &&
	    (month = montoi(mon)) != -1) {
		tm.tm_mday	= mday;
		tm.tm_mon	= month;
		tm.tm_year	= year;
		tm.tm_hour	= hour;
		tm.tm_min	= min;
		tm.tm_sec	= sec;
	}

	if (tm.tm_year > 1900)
		tm.tm_year -= 1900;
	else if (tm.tm_year < 70)
		tm.tm_year += 100;

	/* Set Daylight Saving Time field */
	tmp = localtime(&current_time);
	tm.tm_isdst = tmp->tm_isdst;

	return (mktime(&tm));
}

static void
remove_double_dots(char *s)
{
	char	*p = s;

	while (*s != '\0') {
		*p++ = *s++;
		if (s[-1] == '/')
			while (*s == '.' || *s == '/')
				s++;
	}
	*p = '\0';
}

void
parse_headers(const char *s, int len, struct headers *parsed)
{
	const struct http_header	*h;
	union variant			*v;
	const char			*p, *e = s + len;

	DBG(("parsing headers (len %d): [%.*s]", len, len, s));

	/* Loop through all headers in the request */
	while (s < e) {

		/* Find where this header ends */
		for (p = s; p < e && *p != '\n'; ) p++;

		/* Is this header known to us ? */
		for (h = http_headers; h->len != 0; h++)
			if (e - s > h->len &&
			    !my_strncasecmp(s, h->name, h->len))
				break;

		/* If the header is known to us, store its value */
		if (h->len != 0) {

			/* Shift to where value starts */
			s += h->len;

			/* Find place to store the value */
			v = (union variant *) ((char *) parsed + h->offset);

			/* Fetch header value into the connection structure */
			if (h->type == HDR_STRING) {
				v->v_vec.ptr = s;
				v->v_vec.len = p - s;
				if (p[-1] == '\r' && v->v_vec.len > 0)
					v->v_vec.len--;
			} else if (h->type == HDR_INT) {
				v->v_big_int = strtoul(s, NULL, 10);
			} else if (h->type == HDR_DATE) {
				v->v_time = date_to_epoch(s);
			}
		}

		s = p + 1;	/* Shift to the next header */
	}
}

/*
 * For given directory path, substitute it to valid index file.
 * Return 0 if index file has been found, -1 if not found
 */
static int
find_index_file(struct conn *c, char *path, size_t maxpath, struct stat *stp)
{
	char		buf[FILENAME_MAX];
	const char	*s = c->ctx->index_files;
	size_t		len;

	FOR_EACH_WORD_IN_LIST(s, len) {
		my_snprintf(buf, sizeof(buf), "%s%c%.*s",path, DIRSEP, len, s);
		if (my_stat(buf, stp) == 0) {
			my_strlcpy(path, buf, maxpath);
			c->mime_type = get_mime_type(c->ctx, s, len);
			return (0);
		}
	}

	return (-1);
}

/*
 * Try to open requested file, return 0 if OK, -1 if error.
 * If the file is given arguments using PATH_INFO mechanism,
 * initialize pathinfo pointer.
 */
static int
get_path_info(struct conn *c, char *path, struct stat *stp)
{
	char	*p, *e;

	if (my_stat(path, stp) == 0)
		return (0);

	p = path + strlen(path);
	e = path + strlen(c->ctx->document_root) + 2;
	
	/* Strip directory parts of the path one by one */
	for (; p > e; p--)
		if (*p == '/') {
			*p = '\0';
			if (!my_stat(path, stp) && !S_ISDIR(stp->st_mode)) {
				c->path_info = p + 1;
				return (0);
			} else {
				*p = '/';
			}
		}

	return (-1);
}


static void
decide_what_to_do(struct conn *c)
{
	char		path[URI_MAX], buf[1024];
	struct vec	alias_uri, alias_path;
	struct stat	st;
	int		rc;
#ifdef EMBEDDED
	struct registered_uri	*ruri;
#endif /* EMBEDDED */

	DBG(("decide_what_to_do: [%s]", c->uri));

	if ((c->query = strchr(c->uri, '?')) != NULL)
		*c->query++ = '\0';

	decode_url_encoded_string(c->uri, strlen(c->uri),
	    c->uri, strlen(c->uri) + 1);
	remove_double_dots(c->uri);
	
	if (strlen(c->uri) + strlen(c->ctx->document_root) >= sizeof(path)) {
		send_server_error(c, 400, "URI is too long");
		return;
	}

	(void) my_snprintf(path, sizeof(path), "%s%s",
	    c->ctx->document_root, c->uri);

	/* User may use the aliases - check URI for mount point */
	if (is_alias(c->ctx, c->uri, &alias_uri, &alias_path) != NULL) {
		(void) my_snprintf(path, sizeof(path), "%.*s%s",
		    alias_path.len, alias_path.ptr, c->uri + alias_uri.len);
		DBG(("using alias %.*s -> %.*s", alias_uri.len, alias_uri.ptr,
		    alias_path.len, alias_path.ptr));
	}

#if !defined(NO_AUTH)
	if (check_authorization(c, path) != 1) {
		send_authorization_request(c);
	} else
#endif /* NO_AUTH */
#ifdef EMBEDDED
	if ((ruri = is_registered_uri(c->ctx, c->uri)) != NULL) {
		setup_embedded_stream(c, ruri->callback, ruri->callback_data);
	} else
#endif /* EMBEDDED */
	if (strstr(path, HTPASSWD)) {
		/* Do not allow to view passwords files */
		send_server_error(c, 403, "Forbidden");
	} else
#if !defined(NO_AUTH)
	if ((c->method == METHOD_PUT || c->method == METHOD_DELETE) &&
	    (c->ctx->put_auth_file == NULL || !is_authorized_for_put(c))) {
		send_authorization_request(c);
	} else
#endif /* NO_AUTH */
	if (c->method == METHOD_PUT) {
		c->status = my_stat(path, &st) == 0 ? 200 : 201;

		if (c->ch.range.v_vec.len > 0) {
			send_server_error(c, 501, "PUT Range Not Implemented");
		} else if ((rc = put_dir(path)) == 0) {
			send_server_error(c, 200, "OK");
		} else if (rc == -1) {
			send_server_error(c, 500, "PUT Directory Error");
		} else if (c->rem.content_len == 0) {
			send_server_error(c, 411, "Length Required");
		} else if ((c->loc.chan.fd = my_open(path, O_WRONLY | O_BINARY |
		    O_CREAT | O_NONBLOCK | O_TRUNC, 0644)) == -1) {
			send_server_error(c, 500, "PUT Error");
		} else {
			DBG(("PUT file [%s]", c->uri));
			c->loc.io_class = &io_file;
			c->loc.flags |= FLAG_W | FLAG_ALWAYS_READY ;
		}
	} else if (c->method == METHOD_DELETE) {
		DBG(("DELETE [%s]", c->uri));
		if (my_remove(path) == 0)
			send_server_error(c, 200, "OK");
		else
			send_server_error(c, 500, "DELETE Error");
	} else if (get_path_info(c, path, &st) != 0) {
		send_server_error(c, 404, "Not Found");
	} else if (S_ISDIR(st.st_mode) && path[strlen(path) - 1] != '/') {
		(void) my_snprintf(buf, sizeof(buf),
			"Moved Permanently\r\nLocation: %s/", c->uri);
		send_server_error(c, 301, buf);
	} else if (S_ISDIR(st.st_mode) &&
	    find_index_file(c, path, sizeof(path) - 1, &st) == -1 &&
	    c->ctx->dirlist == 0) {
		send_server_error(c, 403, "Directory Listing Denied");
	} else if (S_ISDIR(st.st_mode) && c->ctx->dirlist) {
		if ((c->loc.chan.dir.path = my_strdup(path)) != NULL)
			get_dir(c);
		else
			send_server_error(c, 500, "GET Directory Error");
	} else if (S_ISDIR(st.st_mode) && c->ctx->dirlist == 0) {
		send_server_error(c, 403, "Directory listing denied");
#ifndef NO_CGI
	} else if (is_cgi(c->ctx, path)) {
		if (c->method != METHOD_POST && c->method != METHOD_GET) {
			send_server_error(c, 501, "Bad method ");
		} else if ((run_cgi(c, path)) == -1) {
			send_server_error(c, 500, "Cannot exec CGI");
		} else {
			do_cgi(c);
		}
#endif /* NO_CGI */
	} else if (c->ch.ims.v_time && st.st_mtime <= c->ch.ims.v_time) {
		send_server_error(c, 304, "Not Modified");
	} else if ((c->loc.chan.fd = my_open(path,
	    O_RDONLY | O_BINARY, 0644)) != -1) {
		get_file(c, &st);
	} else {
		send_server_error(c, 500, "Internal Error");
	}
}

static int
set_request_method(struct conn *c)
{
	const struct vec	*v;

	assert(c->rem.io.head >= MIN_REQ_LEN);

	/* Set the request method */
	for (v = known_http_methods; v->ptr != NULL; v++)
		if (!memcmp(c->rem.io.buf, v->ptr, v->len)) {
			c->method = v - known_http_methods;
			break;
		}

	return (v->ptr == NULL);
}

static void
parse_http_request(struct conn *c)
{
	char	*s, *e, *p, *start;
	char	*end_number;
	int	uri_len, req_len;

	s = c->rem.io.buf;
	req_len = c->rem.headers_len = get_headers_len(s, c->rem.io.head);

	if (req_len == 0 && io_space_len(&c->rem.io) == 0)
		send_server_error(c, 400, "Request is too big");

	if (req_len == 0)
		return;
	else if (req_len < MIN_REQ_LEN)
		send_server_error(c, 400, "Bad request");
	else if (set_request_method(c))
		send_server_error(c, 501, "Method Not Implemented");
	else if ((c->request = my_strndup(s, req_len)) == NULL)
		send_server_error(c, 500, "Cannot allocate request");

	if (c->loc.flags & FLAG_CLOSED)
		return;

	DBG(("Conn %d: parsing request: [%.*s]", c->rem.chan.sock, req_len, s));
	c->rem.flags |= FLAG_HEADERS_PARSED;

	/* Set headers pointer. Headers follow the request line */
	c->headers = memchr(c->request, '\n', req_len);
	assert(c->headers != NULL);
	assert(c->headers < c->request + req_len);
	if (c->headers > c->request && c->headers[-1] == '\r')
		c->headers[-1] = '\0';
	*c->headers++ = '\0';

	/*
	 * Now make a copy of the URI, because it will be URL-decoded,
	 * and we need a copy of unmodified URI for the access log.
	 * First, we skip the REQUEST_METHOD and shift to the URI.
	 */
	for (p = c->request, e = p + req_len; *p != ' ' && p < e; p++);
	while (p < e && *p == ' ') p++;

	/* Now remember where URI starts, and shift to the end of URI */
	for (start = p; p < e && !isspace((unsigned char)*p); ) p++;
	uri_len = p - start;
	/* Skip space following the URI */
	while (p < e && *p == ' ') p++;

	/* Now comes the HTTP-Version in the form HTTP/<major>.<minor> */
	if (strncmp(p, "HTTP/", 5) != 0) {
		send_server_error(c, 400, "Bad HTTP version");
		return;
	}
	p += 5;
	/* Parse the HTTP major version number */
	c->major_version = strtoul(p, &end_number, 10);
	if (end_number == p || *end_number != '.') {
		send_server_error(c, 400, "Bad HTTP major version");
		return;
	}
	p = end_number + 1;
	/* Parse the minor version number */
	c->minor_version = strtoul(p, &end_number, 10);
	if (end_number == p || *end_number != '\0') {
		send_server_error(c, 400, "Bad HTTP minor version");
		return;
	}
	/* Version must be <=1.1 */
	if (c->major_version > 1 ||
	    (c->major_version == 1 && c->minor_version > 1)) {
		send_server_error(c, 505, "HTTP version not supported");
		return;
	}

	if (uri_len <= 0) {
		send_server_error(c, 400, "Bad URI");
	} else if ((c->uri = malloc(uri_len + 1)) == NULL) {
		send_server_error(c, 500, "Cannot allocate URI");
	} else {
		my_strlcpy(c->uri, (char *) start, uri_len + 1);
		parse_headers(c->headers,
		    (c->request + req_len) - c->headers, &c->ch);

		/* Remove the length of request from total, count only data */
		assert(c->rem.io.total >= (big_int_t) req_len);
		c->rem.io.total -= req_len;

		c->rem.content_len = c->ch.cl.v_big_int;
		io_inc_tail(&c->rem.io, req_len);

		decide_what_to_do(c);
	}
}

void
shttpd_add_socket(struct shttpd_ctx *ctx, int sock)
{
	struct conn	*c;
	struct usa	sa;
	int		l = ctx->inetd_mode ? E_FATAL : E_LOG;
#if !defined(NO_SSL)
	SSL		*ssl = NULL;
#endif /* NO_SSL */

	sa.len = sizeof(sa.u.sin);
	(void) set_non_blocking_mode(sock);

	if (getpeername(sock, &sa.u.sa, &sa.len)) {
		elog(l, NULL, "add_socket: %s", strerror(errno));
#if !defined(NO_SSL)
	} else if (ctx->ssl_ctx && (ssl = SSL_new(ctx->ssl_ctx)) == NULL) {
		elog(l, NULL, "add_socket: SSL_new: %s", strerror(ERRNO));
		(void) closesocket(sock);
	} else if (ctx->ssl_ctx && SSL_set_fd(ssl, sock) == 0) {
		elog(l, NULL, "add_socket: SSL_set_fd: %s", strerror(ERRNO));
		(void) closesocket(sock);
		SSL_free(ssl);
#endif /* NO_SSL */
	} else if ((c = calloc(1, sizeof(*c) + 2 * ctx->io_buf_size)) == NULL) {

#if !defined(NO_SSL)
		if (ssl)
			SSL_free(ssl);
#endif /* NO_SSL */
		(void) closesocket(sock);
		elog(l, NULL, "add_socket: calloc: %s", strerror(ERRNO));
	} else {
		ctx->nrequests++;
		c->rem.conn = c->loc.conn = c;
		c->ctx		= ctx;
		c->sa		= sa;
		c->birth_time	= current_time;
		c->expire_time	= current_time + EXPIRE_TIME;

		set_close_on_exec(sock);

		c->loc.io_class	= NULL;
	
		c->rem.io_class	= &io_socket;
		c->rem.chan.sock = sock;

		/* Set IO buffers */
		c->loc.io.buf	= (char *) (c + 1);
		c->rem.io.buf	= c->loc.io.buf + ctx->io_buf_size;
		c->loc.io.size	= c->rem.io.size = ctx->io_buf_size;

#if !defined(NO_SSL)
		if (ssl) {
			c->rem.io_class	= &io_ssl;
			c->rem.chan.ssl.sock = sock;
			c->rem.chan.ssl.ssl = ssl;
			ssl_handshake(&c->rem);
		}
#endif /* NO_SSL */

		EnterCriticalSection(&ctx->mutex);
		LL_TAIL(&ctx->connections, &c->link);
		ctx->nactive++;
		LeaveCriticalSection(&ctx->mutex);
		
		DBG(("%s:%hu connected (socket %d)",
		    inet_ntoa(* (struct in_addr *) &sa.u.sin.sin_addr.s_addr),
		    ntohs(sa.u.sin.sin_port), sock));
	}
}

int
shttpd_active(struct shttpd_ctx *ctx)
{
	return (ctx->nactive);
}

/*
 * Setup a listening socket on given port. Return opened socket or -1
 */
int
shttpd_listen(struct shttpd_ctx *ctx, int port)
{
	struct listener	*l;
	int		sock;

	if ((sock = open_listening_port(port)) == -1) {
	} else if ((l = calloc(1, sizeof(*l))) == NULL) {
		(void) closesocket(sock);
	} else {
		l->sock	= sock;
		l->ctx	= ctx;
		LL_TAIL(&listeners, &l->link);
		DBG(("shttpd_listen: added socket %d", sock));
	}

	return (sock);
}

int
shttpd_accept(int lsn_sock, int milliseconds)
{
	struct timeval	tv;
	struct usa	sa;
	fd_set		read_set;
	int		sock = -1;
	
	tv.tv_sec	= milliseconds / 1000;
	tv.tv_usec	= milliseconds % 1000;
	sa.len		= sizeof(sa.u.sin);
	FD_ZERO(&read_set);
	FD_SET(lsn_sock, &read_set);
	
	if (select(lsn_sock + 1, &read_set, NULL, NULL, &tv) == 1)
		sock = accept(lsn_sock, &sa.u.sa, &sa.len);

	return (sock);
}

static void
read_stream(struct stream *stream)
{
	int	n, len;

	len = io_space_len(&stream->io);
	assert(len > 0);

	/* Do not read more that needed */
	if (stream->content_len > 0 &&
	    stream->io.total + len > stream->content_len)
		len = stream->content_len - stream->io.total;

	/* Read from underlying channel */
	n = stream->nread_last = stream->io_class->read(stream,
	    io_space(&stream->io), len);

	if (n > 0)
		io_inc_head(&stream->io, n);
	else if (n == -1 && (ERRNO == EINTR || ERRNO == EWOULDBLOCK))
		n = n;	/* Ignore EINTR and EAGAIN */
	else if (!(stream->flags & FLAG_DONT_CLOSE))
		stop_stream(stream);

	DBG(("read_stream (%d %s): read %d/%d/%lu bytes (errno %d)",
	    stream->conn->rem.chan.sock,
	    stream->io_class ? stream->io_class->name : "(null)",
	    n, len, (unsigned long) stream->io.total, ERRNO));

	/*
	 * Close the local stream if everything was read
	 * XXX We do not close the remote stream though! It may be
	 * a POST data completed transfer, we do not want the socket
	 * to be closed.
	 */
	if (stream->content_len > 0 && stream == &stream->conn->loc) {
		assert(stream->io.total <= stream->content_len);
		if (stream->io.total == stream->content_len)
			stop_stream(stream);
	}

	stream->conn->expire_time = current_time + EXPIRE_TIME;
}

static void
write_stream(struct stream *from, struct stream *to)
{
	int	n, len;

	len = io_data_len(&from->io);
	assert(len > 0);

	/* TODO: should be assert on CAN_WRITE flag */
	n = to->io_class->write(to, io_data(&from->io), len);
	to->conn->expire_time = current_time + EXPIRE_TIME;
	DBG(("write_stream (%d %s): written %d/%d bytes (errno %d)",
	    to->conn->rem.chan.sock,
	    to->io_class ? to->io_class->name : "(null)", n, len, ERRNO));

	if (n > 0)
		io_inc_tail(&from->io, n);
	else if (n == -1 && (ERRNO == EINTR || ERRNO == EWOULDBLOCK))
		n = n;	/* Ignore EINTR and EAGAIN */
	else if (!(to->flags & FLAG_DONT_CLOSE))
		stop_stream(to);
}


static void
disconnect(struct conn *c)
{
	static const struct vec	ka = {"keep-alive", 10};
	int			dont_close;

	DBG(("Disconnecting %d (%.*s)", c->rem.chan.sock,
	    c->ch.connection.v_vec.len, c->ch.connection.v_vec.ptr));

#if !defined(_WIN32) || defined(NO_GUI)
	if (c->ctx->access_log != NULL)
#endif /* _WIN32 */
			log_access(c->ctx->access_log, c);

	/* In inetd mode, exit if request is finished. */
	if (c->ctx->inetd_mode)
		exit(0);

	if (c->loc.io_class != NULL && c->loc.io_class->close != NULL)
		c->loc.io_class->close(&c->loc);

	/*
	 * Check the "Connection: " header before we free c->request
	 * If it its 'keep-alive', then do not close the connection
	 */
	dont_close =  c->ch.connection.v_vec.len >= ka.len &&
	    !my_strncasecmp(ka.ptr, c->ch.connection.v_vec.ptr, ka.len);

	if (c->request)
		free(c->request);
	if (c->uri)
		free(c->uri);

	/* Handle Keep-Alive */
	dont_close = 0;
	if (dont_close) {
		c->loc.io_class = NULL;
		c->loc.flags = c->rem.flags = 0;
		c->query = c->request = c->uri = c->path_info = NULL;
		c->mime_type = NULL;
		(void) memset(&c->ch, 0, sizeof(c->ch));
		io_clear(&c->rem.io);
		io_clear(&c->loc.io);
		c->rem.io.total = c->loc.io.total = 0;
	} else {
		if (c->rem.io_class != NULL)
			c->rem.io_class->close(&c->rem);

		EnterCriticalSection(&c->ctx->mutex);
		LL_DEL(&c->link);
		c->ctx->nactive--;
		assert(c->ctx->nactive >= 0);
		LeaveCriticalSection(&c->ctx->mutex);

		free(c);
	}
}

static void
add_to_set(int fd, fd_set *set, int *max_fd)
{
	FD_SET(fd, set);
	if (fd > *max_fd)
		*max_fd = fd;
}

/*
 * One iteration of server loop. This is the core of the data exchange.
 */
void
shttpd_poll(struct shttpd_ctx *ctx, int milliseconds)
{
	struct llhead	*lp, *tmp;
	struct listener	*l;
	struct conn	*c;
	struct timeval	tv;			/* Timeout for select() */
	fd_set		read_set, write_set;
	int		sock, max_fd = -1, msec = milliseconds;
	struct usa	sa;

	current_time = time(0);
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);

	/* Add listening sockets to the read set */
	LL_FOREACH(&listeners, lp) {
		l = LL_ENTRY(lp, struct listener, link);
		FD_SET(l->sock, &read_set);
		if (l->sock > max_fd)
			max_fd = l->sock;
		DBG(("FD_SET(%d) (listening)", l->sock));
	}

	/* Multiplex streams */
	LL_FOREACH(&ctx->connections, lp) {
		c = LL_ENTRY(lp, struct conn, link);
		
		/* If there is a space in remote IO, check remote socket */
		if (io_space_len(&c->rem.io))
			add_to_set(c->rem.chan.fd, &read_set, &max_fd);

#if !defined(NO_CGI)
		/*
		 * If there is a space in local IO, and local endpoint is
		 * CGI, check local socket for read availability
		 */
		if (io_space_len(&c->loc.io) && (c->loc.flags & FLAG_R) &&
		    c->loc.io_class == &io_cgi)
			add_to_set(c->loc.chan.fd, &read_set, &max_fd);

		/*
		 * If there is some data read from remote socket, and
		 * local endpoint is CGI, check local for write availability
		 */
		if (io_data_len(&c->rem.io) && (c->loc.flags & FLAG_W) &&
		    c->loc.io_class == &io_cgi)
			add_to_set(c->loc.chan.fd, &write_set, &max_fd);
#endif /* NO_CGI */

		/*
		 * If there is some data read from local endpoint, check the
		 * remote socket for write availability
		 */
		if (io_data_len(&c->loc.io))
			add_to_set(c->rem.chan.fd, &write_set, &max_fd);

		if (io_space_len(&c->loc.io) && (c->loc.flags & FLAG_R) &&
		    (c->loc.flags & FLAG_ALWAYS_READY))
			msec = 0;
		
		if (io_data_len(&c->rem.io) && (c->loc.flags & FLAG_W) &&
		    (c->loc.flags & FLAG_ALWAYS_READY))
			msec = 0;
	}

	tv.tv_sec = msec / 1000;
	tv.tv_usec = msec % 1000;

	/* Check IO readiness */
	if (select(max_fd + 1, &read_set, &write_set, NULL, &tv) < 0) {
#ifdef _WIN32
		/*
		 * On windows, if read_set and write_set are empty,
		 * select() returns "Invalid parameter" error
		 * (at least on my Windows XP Pro). So in this case,
		 * we sleep here.
		 */
		Sleep(milliseconds);
#endif /* _WIN32 */
		DBG(("select: %d", ERRNO));
		return;
	}

	/* Check for incoming connections on listener sockets */
	LL_FOREACH(&listeners, lp) {
		l = LL_ENTRY(lp, struct listener, link);
		if (!FD_ISSET(l->sock, &read_set))
			continue;
		do {
			sa.len = sizeof(sa.u.sin);
			if ((sock = accept(l->sock, &sa.u.sa, &sa.len)) != -1) {
#if defined(_WIN32)
				shttpd_add_socket(ctx, sock);
#else
				if (sock < FD_SETSIZE) {
					shttpd_add_socket(ctx, sock);
				} else {
					elog(E_LOG, NULL,
					   "shttpd_poll: ctx %p: disarding "
					   "socket %d, too busy", ctx, sock);
					(void) closesocket(sock);
				}
#endif /* _WIN32 */
			}
		} while (sock != -1);
	}

	/* Process all connections */
	LL_FOREACH_SAFE(&ctx->connections, lp, tmp) {
		c = LL_ENTRY(lp, struct conn, link);

		/* Read from remote end if it is ready */
		if (FD_ISSET(c->rem.chan.fd, &read_set) &&
		    io_space_len(&c->rem.io))
			read_stream(&c->rem);

		/* If the request is not parsed yet, do so */
		if (!(c->rem.flags & FLAG_HEADERS_PARSED))
			parse_http_request(c);

		DBG(("loc: %u [%.*s]", io_data_len(&c->loc.io),
		    io_data_len(&c->loc.io), io_data(&c->loc.io)));
		DBG(("rem: %u [%.*s]", io_data_len(&c->rem.io),
		    io_data_len(&c->rem.io), io_data(&c->rem.io)));

		/* Read from the local end if it is ready */
		if (io_space_len(&c->loc.io) &&
		    ((c->loc.flags & FLAG_ALWAYS_READY)
		    
#if !defined(NO_CGI)
		    ||(c->loc.io_class == &io_cgi &&
		     FD_ISSET(c->loc.chan.fd, &read_set))
#endif /* NO_CGI */
		    ))
			read_stream(&c->loc);

		if (io_data_len(&c->rem.io) > 0 && (c->loc.flags & FLAG_W) &&
		    c->loc.io_class != NULL && c->loc.io_class->write != NULL)
			write_stream(&c->rem, &c->loc);

		if (io_data_len(&c->loc.io) > 0 && c->rem.io_class != NULL)
			write_stream(&c->loc, &c->rem); 

		if (c->rem.nread_last > 0)
			c->ctx->in += c->rem.nread_last;
		if (c->loc.nread_last > 0)
			c->ctx->out += c->loc.nread_last;

		/* Check whether we should close this connection */
		if ((current_time > c->expire_time) ||
		    (c->rem.flags & FLAG_CLOSED) ||
		    ((c->loc.flags & FLAG_CLOSED) && !io_data_len(&c->loc.io)))
			disconnect(c);
	}
}

/*
 * Deallocate shttpd object, free up the resources
 */
void
shttpd_fini(struct shttpd_ctx *ctx)
{
	struct llhead		*lp, *tmp;
	struct mime_type_link	*mtl;
	struct conn		*c;
	struct listener		*l;
	struct registered_uri	*ruri;

	/* Free configured mime types */
	LL_FOREACH_SAFE(&ctx->mime_types, lp, tmp) {
		mtl = LL_ENTRY(lp, struct mime_type_link, link);
		free(mtl->mime);
		free(mtl->ext);
		free(mtl);
	}

#if 0
	struct opt		*opt;
	/* Free strdup-ed temporary vars */
	for (opt = options; opt->sw != 0; opt++)
		if (opt->tmp) {
			free(opt->tmp);
			opt->tmp = NULL;
		}
#endif
	
	/* Free all connections */
	LL_FOREACH_SAFE(&ctx->connections, lp, tmp) {
		c = LL_ENTRY(lp, struct conn, link);
		disconnect(c);
	}

	/* Free registered URIs (must be done after disconnect()) */
	LL_FOREACH_SAFE(&ctx->registered_uris, lp, tmp) {
		ruri = LL_ENTRY(lp, struct registered_uri, link);
		free((void *)ruri->uri);
		free(ruri);
	}

	/* Free listener sockets for this context */
	LL_FOREACH_SAFE(&listeners, lp, tmp) {
		l = LL_ENTRY(lp, struct listener, link);
		(void) closesocket(l->sock);
		LL_DEL(&l->link);
		free(l);
	}

	if (ctx->access_log)		(void) fclose(ctx->access_log);
	if (ctx->error_log)		(void) fclose(ctx->error_log);
	if (ctx->put_auth_file)		free(ctx->put_auth_file);
	if (ctx->document_root)		free(ctx->document_root);
	if (ctx->index_files)		free(ctx->index_files);
	if (ctx->aliases)		free(ctx->aliases);
#if !defined(NO_CGI)
	if (ctx->cgi_vars)		free(ctx->cgi_vars);
	if (ctx->cgi_extensions)	free(ctx->cgi_extensions);
	if (ctx->cgi_interpreter)	free(ctx->cgi_interpreter);
#endif /* NO_CGI */
	if (ctx->auth_realm)		free(ctx->auth_realm);
	if (ctx->global_passwd_file)	free(ctx->global_passwd_file);
	if (ctx->uid)			free(ctx->uid);

	/* TODO: free SSL context */

	free(ctx);
}
