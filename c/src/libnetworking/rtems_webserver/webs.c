/*
 * webs.c -- GoAhead Embedded HTTP webs server
 *
 * Copyright (c) Go Ahead Software Inc., 1995-1999. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 */

/******************************** Description *********************************/

/*
 *	This module implements an embedded HTTP/1.1 webs server. It supports
 *	loadable URL handlers that define the nature of URL processing performed.
 */

/********************************* Includes ***********************************/

#include	"wsIntrn.h"

/******************************** Global Data *********************************/

websStatsType	 websStats;				/* Web access stats */
webs_t			*webs;					/* Open connection list head */
sym_fd_t		 websMime;				/* Set of mime types */
int				 websMax;				/* List size */
int				 websPort;				/* Listen port for server */
char_t			 websHost[64];			/* Host name for the server */
char_t			 websIpaddr[64];		/* IP address for the server */
char_t			*websHostUrl = NULL;	/* URL to access server */

/*********************************** Locals ***********************************/
/*
 *	Standard HTTP error codes
 */

websErrorType websErrors[] = {
	{ 200, T("Data follows") },
	{ 204, T("No Content") },
	{ 301, T("Redirect") },
	{ 302, T("Redirect") },
	{ 304, T("User local copy") },
	{ 400, T("Page not found") },
	{ 401, T("Password Required") },
	{ 404, T("Site or Page Not Found") },
	{ 405, T("Access Denied") },
	{ 500, T("Web Error") },
	{ 503, T("Site Temporarily Unavailable. Try again") },
	{ 0, NULL }
};

#if WEBS_LOG_SUPPORT
static char_t	websLogname[64] = T("log.txt");	/* Log filename */
static int 		websLogFd;						/* Log file handle */
#endif

static int		websListenSock;					/* Listen socket */

/**************************** Forward Declarations ****************************/

static int 		 websAccept(int sid, char *ipaddr, int port);
static int 		 websAlloc(int sid);
static char_t 	*websErrorMsg(int code);
static void 	 websFree(webs_t wp);
static void		 websFreeVar(sym_t* sp);
static int 		 websGetInput(webs_t wp, char_t **ptext, int *nbytes);
static int 		 websParseFirst(webs_t wp, char_t *text);
static void 	 websParseRequest(webs_t wp);
static void 	 websReadEvent(webs_t wp);
static void		 websSocketEvent(int sid, int mask, int data);
static void 	 websTimeout(long wp);
static void 	 websTimeoutCancel(webs_t wp);
static void		 websMarkTime(webs_t wp);
static int		 websGetTimeSinceMark(webs_t wp);

#if WEBS_LOG_SUPPORT
static void 	 websLog(webs_t wp, int code);
#endif
#if WEBS_IF_MODIFIED_SUPPORT
static time_t dateParse(time_t tip, char_t *cmd);
#endif

/*********************************** Code *************************************/
/*
 *	Open the GoAhead WebServer
 */

int websOpenServer(int port, int retries)
{
	websMimeType	*mt;

	a_assert(port > 0);
	a_assert(retries >= 0);

#if WEBS_PAGE_ROM
	websRomOpen();
#endif

/*
 *	Create a mime type lookup table for quickly determining the content type
 */
	websMime = symOpen(256);
	a_assert(websMime >= 0);
	for (mt = websMimeList; mt->type; mt++) {
		symEnter(websMime, mt->ext, valueString(mt->type, 0), 0);
	}

/*
 *	Open the URL handler module. The caller should create the required
 *	URL handlers after calling this function.
 */
	if (websUrlHandlerOpen() < 0) {
		return -1;
	}

#if WEBS_LOG_SUPPORT
/*
 *	Optional request log support
 */
	websLogFd = gopen(websLogname, O_CREAT | O_TRUNC | O_APPEND | O_WRONLY, 
		0666);
	a_assert(websLogFd >= 0);
#endif
	
	return websOpenListen(port, retries);
}

/******************************************************************************/
/*
 *	Close the GoAhead WebServer
 */

void websCloseServer()
{
	webs_t	wp;
	int		wid;

/*
 *	Close the listen handle first then all open connections.
 */
	websCloseListen();

/* 
 *	Close each open browser connection and free all resources
 */
	for (wid = websMax; webs && wid >= 0; wid--) {
		if ((wp = webs[wid]) == NULL) {
			continue;
		}
		socketCloseConnection(wp->sid);
		websFree(wp);
	}

#if WEBS_LOG_SUPPORT
	if (websLogFd >= 0) {
		close(websLogFd);
		websLogFd = -1;
	}
#endif

#if WEBS_PAGE_ROM
	websRomClose();
#endif
	symClose(websMime, NULL);
	websFormClose();
	websUrlHandlerClose();
}

/******************************************************************************/
/*
 *	Open the GoAhead WebServer listen port
 */

int websOpenListen(int port, int retries)
{
	int		i, orig;

	a_assert(port > 0);
	a_assert(retries >= 0);

	orig = port;
/*
 *	Open the webs webs listen port. If we fail, try the next port.
 */
	for (i = 0; i <= retries; i++) {
		websListenSock = socketOpenConnection(NULL, port, websAccept, 0);
		if (websListenSock >= 0) {
			break;
		}
		port++;
	}
	if (i > retries) {
		error(E_L, E_USER, T("Couldn't open a socket on ports %d - %d"),
			orig, port - 1);
		return -1;
	} 
	goahead_trace(0, T("webs: Listening for HTTP requests on port %d\n"), port);

/*
 *	Determine the full URL address to access the home page for this web server
 */
	websPort = port;
	bfreeSafe(B_L, websHostUrl);
	websHostUrl = NULL;
	if (port == 80) {
		websHostUrl = bstrdup(B_L, websHost);
	} else {
		gsnprintf(&websHostUrl, WEBS_MAX_URL + 80, T("%s:%d"), websHost, port);
	}
	return port;
}

/******************************************************************************/
/*
 *	Close webs listen port
 */

void websCloseListen()
{
	if (websListenSock >= 0) {
		socketCloseConnection(websListenSock);
		websListenSock = -1;
	}
	bfreeSafe(B_L, websHostUrl);
	websHostUrl = NULL;
}

/******************************************************************************/
/*
 *	Accept a connection
 */

static int websAccept(int sid, char *ipaddr, int port)
{
	webs_t	wp;
	int		wid;

	a_assert(ipaddr && *ipaddr);
	a_assert(sid >= 0);
	a_assert(port >= 0);

/*
 *	Allocate a new handle for this accepted connection. This will allocate
 *	a webs_t structure in the webs[] list
 */
	if ((wid = websAlloc(sid)) < 0) {
		return -1;
	}
	wp = webs[wid];
	a_assert(wp);

	ascToUni(wp->ipaddr, ipaddr, sizeof(wp->ipaddr));

/*
 *	Check if this is a request from a browser on this system. This is useful
 *	to know for permitting administrative operations only for local access
 */
	if (gstrcmp(wp->ipaddr, T("127.0.0.1")) == 0 || 
			gstrcmp(wp->ipaddr, websIpaddr) == 0 || 
			gstrcmp(wp->ipaddr, websHost) == 0) {
		wp->flags |= WEBS_LOCAL_REQUEST;
	}

/*
 *	Arrange for websSocketEvent to be called when read data is available
 */
	socketCreateHandler(sid, SOCKET_READABLE , websSocketEvent, (int) wp);

/*
 *	Arrange for a timeout to kill hung requests
 */
	wp->timeout = emfCreateTimer(WEBS_TIMEOUT, websTimeout, (long) wp);
	goahead_trace(5, T("webs: accept request\n"));
	return 0;
}

/******************************************************************************/
/*
 *	The webs socket handler.  Called in response to I/O. We just pass control
 *	to the relevant read or write handler. A pointer to the webs structure
 *	is passed as an (int) in iwp.
 */

static void websSocketEvent(int sid, int mask, int iwp)
{
	webs_t	wp;

	wp = (webs_t) iwp;
	a_assert(wp);

	if (! websValid(wp)) {
		return;
	}

	if (mask & SOCKET_READABLE) {
		websReadEvent(wp);
	} 
	if (mask & SOCKET_WRITABLE) {
		if (wp->writeSocket) {
			(*wp->writeSocket)(wp);
		}
	} 
}

/******************************************************************************/
/*
 *	The webs read handler. This is the primary read event loop. It uses a
 *	state machine to track progress while parsing the HTTP request. 
 *	Note: we never block as the socket is always in non-blocking mode.
 */

static void websReadEvent(webs_t wp)
{
	char_t 	*text;
	int		rc, nbytes, len, done;

	a_assert(wp);
	a_assert(websValid(wp));

	websMarkTime(wp);

/*
 *	Read as many lines as possible. socketGets is called to read the header
 *	and socketRead is called to read posted data.
 */
	text = NULL;
	for (done = 0; !done; ) {
		if (text) {
			bfree(B_L, text);
			text = NULL;
		}

/*
 *		Get more input into "text". Returns 0, if more data is needed
 *		to continue, -1 if finished with the request, or 1 if all 
 *		required data is available for current state.
 */
		while ((rc = websGetInput(wp, &text, &nbytes)) == 0) {
			;
		}

/*
 *		websGetInput returns -1 if it finishes with the request
 */
		if (rc < 0) {
			break;
		}

/*
 *		This is the state machine for the web server. 
 */
		switch(wp->state) {
		case WEBS_BEGIN:
/*
 *			Parse the first line of the Http header
 */
			if (websParseFirst(wp, text) < 0) {
				done++;
				break;
			}
			wp->state = WEBS_HEADER;
			break;
		
		case WEBS_HEADER:
/*
 *			Store more of the HTTP header. As we are doing line reads, we
 *			need to separate the lines with '\n'
 */
			if (ringqLen(&wp->header) > 0) {
				ringqPutstr(&wp->header, T("\n"));
			}
			ringqPutstr(&wp->header, text);
			break;

		case WEBS_POST_CLEN:
/*
 *			POST request with content specified by a content length
 */
			if (wp->query) {
				if (wp->query[0] && !(wp->flags & WEBS_POST_DATA)) {
/*
 *					Special case where the POST request also had query data 
 *					specified in the URL, ie. url?query_data. In this case
 *					the URL query data is separated by a '&' from the posted
 *					query data.
 */
					len = gstrlen(wp->query);
					wp->query = brealloc(B_L, wp->query, (len + gstrlen(text) +
						2) * sizeof(char_t));
					wp->query[len++] = '&';
					gstrcpy(&wp->query[len], text);

				} else {
/*
 *					The existing query data came from the POST request so just
 *					append it.
 */
					len = gstrlen(wp->query);
					wp->query = brealloc(B_L, wp->query, (len +	gstrlen(text) +
						1) * sizeof(char_t));
					if (wp->query) {
						gstrcpy(&wp->query[len], text);
					}
				}

			} else {
				wp->query = bstrdup(B_L, text);
			}
/*
 *			Calculate how much more post data is to be read.
 */
			wp->flags |= WEBS_POST_DATA;
			wp->clen -= nbytes;
			if (wp->clen > 0) {
				if (nbytes > 0) {
					done++;
					break;
				}
				done++;
				break;
			}
/*
 *			No more data so process the request
 */
			websUrlHandlerRequest(wp);
			done++;
			break;

		case WEBS_POST:
/*
 *			POST without content-length specification
 */
			if (wp->query && *wp->query && !(wp->flags & WEBS_POST_DATA)) {
				len = gstrlen(wp->query);
				wp->query = brealloc(B_L, wp->query, (len + gstrlen(text) +
					2) * sizeof(char_t));
				if (wp->query) {
					wp->query[len++] = '&';
					gstrcpy(&wp->query[len], text);
				}

			} else {
				wp->query = bstrdup(B_L, text);
			}
			wp->flags |= WEBS_POST_DATA;
			done++;
			break;

		default:
			websError(wp, 404, T("Bad state"));
			done++;
			break;
		}
	}
	if (text) {
		bfree(B_L, text);
	}
}

/******************************************************************************/
/*
 *	Get input from the browser. Return TRUE (!0) if the request has been 
 *	handled. Return -1 on errors, 1 if input read, and 0 to instruct the 
 *	caller to call again for more input.
 *
 *	Note: socketRead will Return the number of bytes read if successful. This
 *	may be less than the requested "bufsize" and may be zero. It returns -1 for
 *	errors. It returns 0 for EOF. Otherwise it returns the number of bytes 
 *	read. Since this may be zero, callers should use socketEof() to 
 *	distinguish between this and EOF.
 */

static int websGetInput(webs_t wp, char_t **ptext, int *pnbytes) 
{
	char_t	*text;
	char	buf[WEBS_SOCKET_BUFSIZ+1];
	int		nbytes, len, clen;

	a_assert(websValid(wp));
	a_assert(ptext);
	a_assert(pnbytes);

	*ptext = text = NULL;
	*pnbytes = 0;

/*
 *	If this request is a POST with a content length, we know the number
 *	of bytes to read so we use socketRead().
 */
	if (wp->state == WEBS_POST_CLEN) {
		len = (wp->clen > WEBS_SOCKET_BUFSIZ) ? WEBS_SOCKET_BUFSIZ : wp->clen;
	} else {
		len = 0;
	}

	if (len > 0) {
		nbytes = socketRead(wp->sid, buf, len);

		if (nbytes < 0) {						/* Error */
			websDone(wp, 0);
			return -1;

		}  else if (nbytes == 0) {				/* EOF or No data available */
			return -1;

		} else {								/* Valid data */
/*
 *			Convert to UNICODE if necessary.  First be sure the string 
 *			is NULL terminated.
 */
			buf[nbytes] = '\0';
			if ((text = ballocAscToUni(buf)) == NULL) {
				websError(wp, 503, T("Insufficient memory"));
				return -1;
			}
		}

	} else {
		nbytes = socketGets(wp->sid, &text);

		if (nbytes < 0) {
/*
 *			Error, EOF or incomplete
 */
			if (socketEof(wp->sid)) {
/*
 *				If this is a post request without content length, process 
 *				the request as we now have all the data. Otherwise just 
 *				close the connection.
 */
				if (wp->state == WEBS_POST) {
					websUrlHandlerRequest(wp);
				} else {
					websDone(wp, 0);
				}
			}
/*
 *			If state is WEBS_HEADER and the ringq is empty, then this is a
 *			simple request with no additional header fields to process and
 *			no empty line terminator.
 */
			if (wp->state == WEBS_HEADER && ringqLen(&wp->header) <= 0) {
				websParseRequest(wp);
				websUrlHandlerRequest(wp);
			}
			return -1;

		} else if (nbytes == 0) {
			if (wp->state == WEBS_HEADER) {
/*
 *				Valid empty line, now finished with header
 */
				websParseRequest(wp);
				if (wp->flags & WEBS_POST_REQUEST) {
					if (wp->flags & WEBS_CLEN) {
						wp->state = WEBS_POST_CLEN;
						clen = wp->clen;
					} else {
						wp->state = WEBS_POST;
						clen = 1;
					}
					if (clen > 0) {
						return 0;							/* Get more data */
					}
					return 1;

				}
/*
 *				We've read the header so go and handle the request
 */
				websUrlHandlerRequest(wp);
			}
			return -1;

		}
	}
	a_assert(text);
	a_assert(nbytes > 0);
	*ptext = text;
	*pnbytes = nbytes;
	return 1;
}

/******************************************************************************/
/*
 *	Parse the first line of a HTTP request
 */

static int websParseFirst(webs_t wp, char_t *text)
{
	char_t 	*op, *proto, *url, *host, *query, *path, *port, *ext, *buf;

	a_assert(websValid(wp));
	a_assert(text && *text);

/*
 *	Determine the request type: GET, HEAD or POST
 */
	op = gstrtok(text, T(" \t"));
	if (op == NULL || *op == '\0') {
		websError(wp, 400, T("Bad HTTP request"));
		return -1;
	}
	if (gstrcmp(op, T("GET")) != 0) {
		if (gstrcmp(op, T("POST")) == 0) {
			wp->flags |= WEBS_POST_REQUEST;
		} else if (gstrcmp(op, T("HEAD")) == 0) {
			wp->flags |= WEBS_HEAD_REQUEST;
		} else {
			websError(wp, 400, T("Bad request type"));
			return -1;
		}
	}

/*
 *	Store result in the form (CGI) variable store
 */
	websSetVar(wp, T("REQUEST_METHOD"), op);

	url = gstrtok(NULL, T(" \t\n"));
	if (url == NULL || *url == '\0') {
		websError(wp, 400, T("Bad HTTP request"));
		return -1;
	}

/*
 *	Parse the URL and store all the various URL components. websUrlParse
 *	returns an allocated buffer in buf which we must free. We support both
 *	proxied and non-proxied requests. Proxied requests will have http://host/
 *	at the start of the URL. Non-proxied will just be local path names.
 */
	host = path = port = proto = query = ext = NULL;
	if (websUrlParse(url, &buf, &host, &path, &port, &query, &proto, 
			NULL, &ext) < 0) {
		websError(wp, 400, T("Bad URL format"));
		return -1;
	}

	wp->url = bstrdup(B_L, url);
	wp->query = bstrdup(B_L, query);
	wp->host = bstrdup(B_L, host);
	wp->path = bstrdup(B_L, path);
	wp->port = gatoi(port);
	if (gstrcmp(ext, T(".asp")) == 0) {
		wp->flags |= WEBS_ASP;
	}
	bfree(B_L, buf);

	websUrlType(url, wp->type, TSZ(wp->type));

#if WEBS_PROXY_SUPPORT
/*
 *	Determine if this is a request for local webs data. If it is not a proxied 
 *	request from the browser, we won't see the "http://" or the system name, so
 *	we assume it must be talking to us directly for local webs data.
 *	Note: not fully implemented yet.
 */
	if (gstrstr(wp->url, T("http://")) == NULL || 
		((gstrcmp(wp->host, T("localhost")) == 0 || 
			gstrcmp(wp->host, websHost) == 0) && (wp->port == websPort))) {
		wp->flags |= WEBS_LOCAL_PAGE;
		if (gstrcmp(wp->path, T("/")) == 0) {
			wp->flags |= WEBS_HOME_PAGE;
		}
	}
#endif

	ringqFlush(&wp->header);
	return 0;
}

/******************************************************************************/
/*
 *	Parse a full request
 */

static void websParseRequest(webs_t wp)
{
	char_t	*upperKey, *cp, *browser, *lp, *key, *value;

	a_assert(websValid(wp));

/*
 *	Define default CGI values
 */
	websSetVar(wp, T("HTTP_AUTHORIZATION"), T(""));

/* 
 *	Parse the header and create the Http header keyword variables
 *	We rewrite the header as we go for non-local requests.  NOTE: this
 * 	modifies the header string directly and tokenizes each line with '\0'.
 */
	browser = NULL;
	for (lp = (char_t*) wp->header.servp; lp && *lp; ) {
		cp = lp;
		if ((lp = gstrchr(lp, '\n')) != NULL) {
			lp++;
		}

		if ((key = gstrtok(cp, T(": \t\n"))) == NULL) {
			continue;
		}

		if ((value = gstrtok(NULL, T("\n"))) == NULL) {
			value = T("");
		}

		while (gisspace(*value)) {
			value++;
		}
		strlower(key);

/*
 *		Create a variable (CGI) for each line in the header
 */
		gsnprintf(&upperKey, (gstrlen(key) + 6), T("HTTP_%s"), key);
		for (cp = upperKey; *cp; cp++) {
			if (*cp == '-')
				*cp = '_';
		}
		strupper(upperKey);
		websSetVar(wp, upperKey, value);
		bfree(B_L, upperKey);

/*
 *		Track the requesting agent (browser) type
 */
		if (gstrcmp(key, T("user-agent")) == 0) {
			wp->userAgent = bstrdup(B_L, value);

/*
 *		Parse the user authorization. ie. password
 */
		} else if (gstrcmp(key, T("authorization")) == 0) {
			char_t	password[FNAMESIZE];

/*
 *			The incoming value is password:username
 */
			if ((cp = gstrchr(value, ' ')) != NULL) {
				websDecode64(password, ++cp, sizeof(password));
			} else {
				websDecode64(password, value, sizeof(password));
			}
			if ((cp = gstrchr(password, ':')) != NULL) {
				*cp++ = '\0';
			}
			if (cp) {
				wp->password = bstrdup(B_L, cp);
			} else {
				wp->password = bstrdup(B_L, T(""));
			}

/*
 *		Parse the content length
 */
		} else if (gstrcmp(key, T("content-length")) == 0) {
			wp->flags |= WEBS_CLEN;
			wp->clen = gatoi(value);
			websSetVar(wp, T("CONTENT_LENGTH"), value);

#if WEBS_KEEP_ALIVE_SUPPORT
		} else if (gstrcmp(key, T("connection")) == 0) {
			strlower(value);
			if (gstrcmp(value, T("keep-alive")) == 0) {
				wp->flags |= WEBS_KEEP_ALIVE;
			}
#endif

#if WEBS_PROXY_SUPPORT
/*
 *		This may be useful if you wish to keep a local cache of web pages
 *		for proxied requests.
 */
		} else if (gstrcmp(key, T("pragma")) == 0) {
			char_t	tmp[256];
			gstrncpy(tmp, value, TSZ(tmp));
			strlower(tmp);
			if (gstrstr(tmp, T("no-cache"))) {
				wp->flags |= WEBS_DONT_USE_CACHE;
			}
#endif

/*
 *		Store the cookie
 */
		} else if (gstrcmp(key, T("cookie")) == 0) {
			wp->flags |= WEBS_COOKIE;
			wp->cookie = bstrdup(B_L, value);

#if WEBS_IF_MODIFIED_SUPPORT
/*
 *		See if the local page has been modified since the browser last
 *		requested this document. If not, just return a 302
 */
		} else if (gstrcmp(key, T("if-modified-since")) == 0) {
			char_t *cmd;
			time_t tip = 0;

			if (cp = gstrchr(value, ';')) {
				*cp = '\0';
			}

			if (cp = gstrstr(value, T(", "))) {
				cp += 2;
			}

			if (gstrstr(cp, T("GMT"))) {
				gsnprintf(&cmd, 64, T("clock scan %s -gmt 1"), cp);
			} else {
				gsnprintf(&cmd, 64, T("clock scan %s"), cp);
			}
			if (wp->since = dateParse(tip, cmd)) {
				wp->flags |= WEBS_IF_MODIFIED;
			}
			bfreeSafe(B_L, cmd);
#endif
		}
	}
}


#if WEBS_IF_MODIFIED_SUPPORT
/******************************************************************************/
/*
 *		Parse the date and time string.
 */

static time_t dateParse(time_t tip, char_t *cmd)
{
	return (time_t)0;
}
#endif


/******************************************************************************/
/*
 *	Set the variable (CGI) environment for this request. Create variables
 *	for all standard CGI variables. Also decode the query string and create
 *	a variable for each name=value pair.
 */

void websSetEnv(webs_t wp)
{
	char_t	portBuf[8];
	char_t	*keyword, *value;

	a_assert(websValid(wp));

	websSetVar(wp, T("QUERY_STRING"), wp->query);
	websSetVar(wp, T("GATEWAY_INTERFACE"), T("CGI/1.1"));
	websSetVar(wp, T("SERVER_HOST"), websHost);
	websSetVar(wp, T("SERVER_URL"), websHostUrl);
	websSetVar(wp, T("REMOTE_HOST"), wp->ipaddr);
	websSetVar(wp, T("REMOTE_ADDR"), wp->ipaddr);
	websSetVar(wp, T("PATH_INFO"), wp->path);
	stritoa(websPort, portBuf, sizeof(portBuf));
	websSetVar(wp, T("SERVER_PORT"), portBuf);
	
/*
 *	Decode and create an environment query variable for each query keyword.
 *	We split into pairs at each '&', then split pairs at the '='.
 *	Note: we rely on wp->decodedQuery preserving the decoded values in the
 *	symbol table.
 */
	wp->decodedQuery = bstrdup(B_L, wp->query);
	keyword = gstrtok(wp->decodedQuery, T("&"));
	while (keyword != NULL) {
		if ((value = gstrchr(keyword, '=')) != NULL) {
			*value++ = '\0';
			websDecodeUrl(keyword, keyword, gstrlen(keyword));
			websDecodeUrl(value, value, gstrlen(value));

		} else {
			value = T("");
		}

		if (*keyword) {
			websSetVar(wp, keyword, value);
		}
		keyword = gstrtok(NULL, T("&"));
	}

#if EMF
/*
 *	Add GoAhead Embedded Management Framework defines
 */
	websSetEmfEnvironment(wp);
#endif
}

/******************************************************************************/
/*
 *	Define a webs (CGI) variable for this connection. Also create in relevant
 *	scripting engines. Note: the incoming value may be volatile. 
 */

void websSetVar(webs_t wp, char_t *var, char_t *value)
{
	value_t		 v;

	a_assert(websValid(wp));

/*
 *	value_instring will allocate the string if required.
 */
	if (value) {
		v = valueString(value, VALUE_ALLOCATE);
	} else {
		v = valueString(T(""), VALUE_ALLOCATE);
	}
	symEnter(wp->cgiVars, var, v, 0);
}

/******************************************************************************/
/*
 *	Return TRUE if a webs variable exists for this connection.
 */

int websTestVar(webs_t wp, char_t *var)
{
	sym_t		*sp;

	a_assert(websValid(wp));

	if (var == NULL || *var == '\0') {
		return 0;
	}

	if ((sp = symLookup(wp->cgiVars, var)) == NULL) {
		return 0;
	}
	return 1;
}

/******************************************************************************/
/*
 *	Get a webs variable but return a default value if string not found.
 *	Note, defaultGetValue can be NULL to permit testing existance.
 */

char_t *websGetVar(webs_t wp, char_t *var, char_t *defaultGetValue)
{
	sym_t	*sp;

	a_assert(websValid(wp));
	a_assert(var && *var);
 
	if ((sp = symLookup(wp->cgiVars, var)) != NULL) {
		a_assert(sp->content.type == string);
		if (sp->content.value.string) {
			return sp->content.value.string;
		} else {
			return T("");
		}
	}
	return defaultGetValue;
}

/******************************************************************************/
/*
 *	Cancel the request timeout. Note may be called multiple times.
 */

static void websTimeoutCancel(webs_t wp)
{
	a_assert(websValid(wp));

	if (wp->timeout) {
		emfDeleteTimer(wp->timeout);
		wp->timeout = NULL;
	}
}

/******************************************************************************/
/*
 *	Output a HTTP response back to the browser. If redirect is set to a 
 *	URL, the browser will be sent to this location.
 */

void websResponse(webs_t wp, int code, char_t *message, char_t *redirect)
{
	char_t		*date;

	a_assert(websValid(wp));

/*
 *	IE3.0 needs no Keep Alive for some return codes.
 */
	wp->flags &= ~WEBS_KEEP_ALIVE;

/*
 *	Only output the header if a header has not already been output.
 */
	if ( !(wp->flags & WEBS_HEADER_DONE)) {
		wp->flags |= WEBS_HEADER_DONE;
		websWrite(wp, T("HTTP/1.0 %d %s\r\n"), code, websErrorMsg(code));

		/* by license terms the following line of code must
		 * not be modified.
		 */
		websWrite(wp, T("Server: GoAhead-Webs\r\n"));

		if (wp->flags & WEBS_KEEP_ALIVE) {
			websWrite(wp, T("Connection: keep-alive\r\n"));
		}
		websWrite(wp, T("Pragma: no-cache\r\nCache-Control: no-cache\r\n"));

		if ((date = websGetDateString(NULL)) != NULL) {
			websWrite(wp, T("Date: %s\r\n"), date);
			bfree(B_L, date);
		}
		websWrite(wp, T("Content-Type: text/html\r\n"));

/*
 *		We don't do a string length here as the message may be multi-line. 
 *		Ie. <CR><LF> will count as only one and we will have a content-length 
 *		that is too short.
 *
 *		websWrite(wp, T("Content-Length: %s\r\n"), message);
 */
		if (redirect) {
			websWrite(wp, T("Location: %s\r\n"), redirect);
		}
		websWrite(wp, T("\r\n"));
	}

	if (message && *message) {
		websWrite(wp, T("%s\r\n"), message);
	}
	websDone(wp, code);
}

/******************************************************************************/
/*
 *	Redirect the user to another webs page
 */

void websRedirect(webs_t wp, char_t *url)
{
	char_t	*msgbuf, *urlbuf;

	a_assert(websValid(wp));
	a_assert(url);

	websStats.redirects++;
	msgbuf = urlbuf = NULL;

/*
 *	Some browsers require a http://host qualified URL for redirection
 */
	if (gstrstr(url, T("http://")) == NULL) {
		if (*url == '/') {
			url++;
		}
		gsnprintf(&urlbuf, WEBS_MAX_URL + 80, T("http://%s/%s"),
			websGetVar(wp, T("HTTP_HOST"), 	websHostUrl), url);
		url = urlbuf;
	}

/*
 *	Add human readable message for completeness. Should not be required.
 */
	gsnprintf(&msgbuf, WEBS_MAX_URL + 80, 
		T("<html><head></head><body>\r\n\
		This document has moved to a new <a href=\"%s\">location</a>.\r\n\
		Please update your documents to reflect the new location.\r\n\
		</body></html>\r\n"), url);

	websResponse(wp, 302, msgbuf, url);

	bfreeSafe(B_L, msgbuf);
	bfreeSafe(B_L, urlbuf);
}

/******************************************************************************/
/*	
 *	Output an error message and cleanup
 */

void websError(webs_t wp, int code, char_t *fmt, ...)
{
	va_list		args;
	char_t		*msg, *userMsg, *buf;

	a_assert(websValid(wp));
	a_assert(fmt);

	websStats.errors++;

	va_start(args, fmt);
	userMsg = NULL;
	gvsnprintf(&userMsg, WEBS_BUFSIZE, fmt, args);
	va_end(args);

	msg = T("<html><head><title>Document Error: %s</title></head>\r\n\
		<body><h2>Access Error: %s</h2>\r\n\
		when trying to obtain <b>%s</b><br><p>%s</p></body></html>\r\n");
/*
 *	Ensure we have plenty of room
 */
	buf = NULL;
	gsnprintf(&buf, WEBS_BUFSIZE, msg, websErrorMsg(code), 
		websErrorMsg(code), wp->url, userMsg);

	websResponse(wp, code, buf, NULL);
	bfreeSafe(B_L, buf);
	bfreeSafe(B_L, userMsg);
}

/******************************************************************************/
/*
 *	Return the error message for a given code
 */

static char_t *websErrorMsg(int code)
{
	websErrorType*	ep;

	for (ep = websErrors; ep->code; ep++) {
		if (code == ep->code) {
			return ep->msg;
		}
	}
	a_assert(0);
	return T("");
}

/******************************************************************************/
/*
 *	Do formatted output to the browser. This is the public ASP and form
 *	write procedure.
 */

int websWrite(webs_t wp, char_t* fmt, ...)
{
	va_list		 vargs;
	char_t		*buf;
	int			 rc;
	
	a_assert(websValid(wp));

	va_start(vargs, fmt);

	buf = NULL;
	rc = 0;
	if (gvsnprintf(&buf, WEBS_BUFSIZE, fmt, vargs) >= WEBS_BUFSIZE) {
		goahead_trace(0, T("webs: websWrite lost data, buffer overflow\n"));
	}
	va_end(vargs);
	a_assert(buf);
	if (buf) {
		rc = websWriteBlock(wp, buf, gstrlen(buf));
		bfree(B_L, buf);
	}
	return rc;
}

/******************************************************************************/
/*
 *	Write a block of data of length "nChars" to the user's browser. Public
 *	write block procedure.  If unicode is turned on this function expects 
 *	buf to be a unicode string and it converts it to ASCII before writing.
 *	See websWriteBlockData to always write binary or ASCII data with no 
 *	unicode conversion.  This returns the number of char_t's processed.
 */

int websWriteBlock(webs_t wp, char_t *buf, int nChars)
{
#if ! UNICODE
	return websWriteBlockData(wp, buf, nChars);
#else
	int		r;
	char	*charBuf;

	a_assert(buf);
	a_assert(nChars >= 0);

	if ((charBuf = ballocUniToAsc(buf, nChars)) == NULL) {
		return -1;
	}
	r = websWriteBlockData(wp, charBuf, nChars);
	bfree(B_L, charBuf);
	return r;
#endif
}

/******************************************************************************/
/*
 *	Write a block of data of length "nChars" to the user's browser. Same as
 *	websWriteBlock except that it expects straight ASCII or binary and does no
 *	unicode conversion before writing the data.
 *	This returns the number of chars processed.
 */

int websWriteBlockData(webs_t wp, char *buf, int nChars)
{
	int		len, done;

	a_assert(wp);
	a_assert(websValid(wp));
	a_assert(buf);
	a_assert(nChars >= 0);

	done = len = 0;
	while (nChars > 0) {
		if ((len = socketWrite(wp->sid, buf, nChars)) < 0) {
			return -1;
		}
/*
 *		Block in flush if the last write could not take any more data
 */
		socketFlush(wp->sid, len == 0);
		nChars -= len;
		buf += len;
		done += len;
	}
	return done;
}

/******************************************************************************/
/*
 *	Decode a URL (or part thereof). Allows insitu decoding.
 */

void websDecodeUrl(char_t *decoded, char_t *token, int len)
{
	char_t	*ip,  *op;
	int		num, i, c;
	
	a_assert(decoded);
	a_assert(token);
	num = 0;

	op = decoded;
	for (ip = token; *ip && len > 0; ip++, op++) {
		if (*ip == '+') {
			*op = ' ';
		} else if (*ip == '%' && gisxdigit(ip[1]) && gisxdigit(ip[2])) {

/*
 *			Convert %nn to a single character
 */
			ip++;
			for (i = 0; i < 2; i++, ip++) {
				c = tolower(*ip);
				if (c >= 'a' && c <= 'f') {
					num = (num * 16) + 10 + c - 'a';
				} else {
					num = (num * 16) + c - '0';
				}
			}
			*op = (char_t) num;
			ip--;

		} else {
			*op = *ip;
		}
		len--;
	}
	*op = '\0';
}

/******************************************************************************/
#if WEBS_LOG_SUPPORT
/*
 *	Output a log message
 */

static void websLog(webs_t wp, int code)
{
	char_t	*buf;
	char	*abuf;
	int		len;

	a_assert(websValid(wp));

	buf = NULL;
	gsnprintf(&buf, WEBS_MAX_URL + 80, T("%d %s %d %d\n"), time(0), 
		wp->url, code, wp->written);
	len = gstrlen(buf);
	abuf = ballocUniToAsc(buf, len+1);
	write(websLogFd, abuf, len);
	bfreeSafe(B_L, buf);
	bfreeSafe(B_L, abuf);
}

#endif /* WEBS_LOG_SUPPORT */

/******************************************************************************/
/*
 *	Request timeout. The timeout triggers if we have not read any data from
 *	the users browser in the last WEBS_TIMEOUT period. If we have heard from
 *	the browser, simply re-issue the timeout.
 */

static void websTimeout(long iwp)
{
	webs_t		wp;
	int			delay, tm;

	wp = (webs_t) iwp;
	a_assert(websValid(wp));

	tm = websGetTimeSinceMark(wp) * 1000;
	if (tm >= WEBS_TIMEOUT) {
		websStats.timeouts++;
		wp->timeout = NULL;
		websDone(wp, 404);

	} else {
		delay = WEBS_TIMEOUT - tm;
		a_assert(delay > 0);
		wp->timeout = emfCreateTimer(delay, websTimeout, (long) wp);
	}
}

/******************************************************************************/
/*
 *	Called when the request is done.
 */

void websDone(webs_t wp, int code)
{
	a_assert(websValid(wp));

/*
 * 	Disable socket handler in case keep alive set.
 */
	socketDeleteHandler(wp->sid);

	if (code != 200) {
		wp->flags &= ~WEBS_KEEP_ALIVE;
	}

#if WEBS_PROXY_SUPPORT
	if (! (wp->flags & WEBS_LOCAL_PAGE)) {
		websStats.activeNetRequests--;
	}
#endif

#if WEBS_LOG_SUPPORT
	if (! (wp->flags & WEBS_REQUEST_DONE)) {
		websLog(wp, code);
	}
#endif

/*
 *	Close any opened document by a handler
 */
	websPageClose(wp);

/*
 *	If using Keep Alive (HTTP/1.1) we keep the socket open for a period
 *	while waiting for another request on the socket. 
 */
	if (wp->flags & WEBS_KEEP_ALIVE) {
		if (socketFlush(wp->sid, 0) == 0) {
			wp->state = WEBS_BEGIN;
			wp->flags |= WEBS_REQUEST_DONE;
			if (wp->header.buf) {
				ringqFlush(&wp->header);
			}
			socketCreateHandler(wp->sid, SOCKET_READABLE, websSocketEvent, 
				(int) wp);
			websTimeoutCancel(wp);
			wp->timeout = emfCreateTimer(WEBS_TIMEOUT, websTimeout, (long) wp);
			return;
		}
	} else {
		websTimeoutCancel(wp);
		socketCloseConnection(wp->sid);
	}
	websFree(wp);
}

/******************************************************************************/
/*
 *	Allocate a new webs structure
 */

static int websAlloc(int sid)
{
	webs_t		wp;
	int			wid;

/*
 *	Allocate a new handle for this connection
 */
	if ((wid = hAllocEntry((void***) &webs, &websMax,
			sizeof(struct websRec))) < 0) {
		return -1;
	}
	wp = webs[wid];

	wp->wid = wid;
	wp->sid = sid;
	wp->state = WEBS_BEGIN;
	wp->docfd = -1;
	wp->dir = NULL;

	ringqOpen(&wp->header, WEBS_HEADER_BUFINC, WEBS_MAX_HEADER);

/*
 *	Create storage for the CGI variables. We supply the symbol tables for
 *	both the CGI variables and for the global functions. The function table
 *	is common to all webs instances (ie. all browsers)
 */
	wp->cgiVars = symOpen(64);

	return wid;
}

/******************************************************************************/
/*
 *	Free a webs structure
 */

static void websFree(webs_t wp)
{
	a_assert(websValid(wp));

	if (wp->path)
		bfree(B_L, wp->path);
	if (wp->url)
		bfree(B_L, wp->url);
	if (wp->host)
		bfree(B_L, wp->host);
	if (wp->lpath)
		bfree(B_L, wp->lpath);
	if (wp->query)
		bfree(B_L, wp->query);
	if (wp->decodedQuery)
		bfree(B_L, wp->decodedQuery);
	if (wp->password)
		bfree(B_L, wp->password);
	if (wp->userName)
		bfree(B_L, wp->userName);
	if (wp->cookie)
		bfree(B_L, wp->cookie);
	if (wp->userAgent)
		bfree(B_L, wp->userAgent);
	if (wp->dir)
		bfree(B_L, wp->dir);

	symClose(wp->cgiVars, websFreeVar);

	if (wp->header.buf) {
		ringqClose(&wp->header);
	}

	websMax = hFree((void***) &webs, wp->wid);
	bfree(B_L, wp);
	a_assert(websMax >= 0);
}

/******************************************************************************/
/*
 *	Callback from symClose. Free the variable.
 */

static void websFreeVar(sym_t* sp)
{
	valueFree(&sp->content);
}

/******************************************************************************/
/*
 *	Return the server address
 */

char_t* websGetHost()
{
	return websHost;
}

/******************************************************************************/
/*
 *	Return the server address
 */

char_t* websGetHostUrl()
{
	return websHostUrl;
}

/******************************************************************************/
/*
 *	Return the listen port
 */

int websGetPort()
{
	return websPort;
}

/******************************************************************************/
/*
 *	Get the number of bytes to write
 */

int websGetRequestBytes(webs_t wp)
{
	a_assert(websValid(wp));

	return wp->numbytes;
}

/******************************************************************************/
/*
 *	Get the directory for this request
 */

char_t *websGetRequestDir(webs_t wp)
{
	a_assert(websValid(wp));

	if (wp->dir == NULL) {
		return T("");
	}

	return wp->dir;
}

/******************************************************************************/
/*
 *	Get the flags for this request
 */

int websGetRequestFlags(webs_t wp)
{
	a_assert(websValid(wp));

	return wp->flags;
}

/******************************************************************************/
/*
 *	Return the IP address
 */

char_t *websGetRequestIpaddr(webs_t wp)
{
	a_assert(websValid(wp));

	return wp->ipaddr;
}

/******************************************************************************/
/*
 *	Set the local path for the request
 */

char_t *websGetRequestLpath(webs_t wp)
{
	a_assert(websValid(wp));

#if WEBS_PAGE_ROM
	return wp->path;
#else
	return wp->lpath;
#endif
}

/******************************************************************************/
/*
 *	Get the path for this request
 */

char_t *websGetRequestPath(webs_t wp)
{
	a_assert(websValid(wp));

	if (wp->path == NULL) {
		return T("");
	}

	return wp->path;
}

/******************************************************************************/
/*
 *	Return the password
 */

char_t* websGetRequestPassword(webs_t wp)
{
	a_assert(websValid(wp));

	return wp->password;
}

/******************************************************************************/
/*
 *	Return the request type
 */

char_t* websGetRequestType(webs_t wp)
{
	a_assert(websValid(wp));

	return wp->type;
}

/******************************************************************************/
/*
 *	Return the username
 */

char_t* websGetRequestUserName(webs_t wp)
{
	a_assert(websValid(wp));

	return wp->userName;
}

/******************************************************************************/
/*
 *	Get the number of bytes written
 */

int websGetRequestWritten(webs_t wp)
{
	a_assert(websValid(wp));

	return wp->written;
}

/******************************************************************************/
/*
 *	Set the hostname
 */

void websSetHost(char_t *host)
{
	gstrncpy(websHost, host, TSZ(websHost));
}

/******************************************************************************/
/*
 *	Set the host URL
 */

void websSetHostUrl(char_t *url)
{
	a_assert(url && *url);

	bfreeSafe(B_L, websHostUrl);
	websHostUrl = gstrdup(B_L, url);
}

/******************************************************************************/
/*
 *	Set the IP address
 */

void websSetIpaddr(char_t *ipaddr)
{
	a_assert(ipaddr && *ipaddr);

	gstrncpy(websIpaddr, ipaddr, TSZ(websIpaddr));
}

/******************************************************************************/
/*
 *	Set the number of bytes to write
 */

void websSetRequestBytes(webs_t wp, int bytes)
{
	a_assert(websValid(wp));
	a_assert(bytes >= 0);

	wp->numbytes = bytes;
}

/******************************************************************************/
/*
 *	Set the flags for this request
 */

void websSetRequestFlags(webs_t wp, int flags)
{
	a_assert(websValid(wp));

	wp->flags = flags;
}

/******************************************************************************/
/*
 *	Set the local path for the request
 */

void websSetRequestLpath(webs_t wp, char_t *lpath)
{
	a_assert(websValid(wp));
	a_assert(lpath && *lpath);

	if (wp->lpath) {
		bfree(B_L, wp->lpath);
	}
	wp->lpath = bstrdup(B_L, lpath);
	websSetVar(wp, T("PATH_TRANSLATED"), wp->lpath);
}

/******************************************************************************/
/*
 *	Update the URL path and the directory containing the web page
 */

void websSetRequestPath(webs_t wp, char_t *dir, char_t *path)
{
	char_t	*tmp;

	a_assert(websValid(wp));

	if (dir) { 
		tmp = wp->dir;
		wp->dir = bstrdup(B_L, dir);
		if (tmp) {
			bfree(B_L, tmp);
		}
	}
	if (path) {
		tmp = wp->path;
		wp->path = bstrdup(B_L, path);
		websSetVar(wp, T("PATH_INFO"), wp->path);
		if (tmp) {
			bfree(B_L, tmp);
		}
	}
}

/******************************************************************************/
/*
 *	Set the Write handler for this socket
 */

void websSetRequestSocketHandler(webs_t wp, int mask, void (*fn)(webs_t wp))
{
	a_assert(websValid(wp));

	wp->writeSocket = fn;
	socketCreateHandler(wp->sid, SOCKET_WRITABLE, websSocketEvent, (int) wp);
}

/******************************************************************************/
/*
 *	Set the number of bytes written
 */

void websSetRequestWritten(webs_t wp, int written)
{
	a_assert(websValid(wp));

	wp->written = written;
}

/******************************************************************************/
/*
 *	Reurn true if the webs handle is valid
 */

int websValid(webs_t wp)
{
	int		wid;

	for (wid = 0; wid < websMax; wid++) {
		if (wp == webs[wid]) {
			return 1;
		}
	}
	return 0;
}

/******************************************************************************/
/*
 *	Close the document handle.
 */

int websCloseFileHandle(webs_t wp)
{
	a_assert(websValid(wp));

#ifndef WEBS_PAGE_ROM
	if (wp->docfd >= 0) {
		close(wp->docfd);
		wp->docfd = -1;
	}
#endif

	return 0;
}

/******************************************************************************/
/*
 *	Build an ASCII time string.  If sbuf is NULL we use the current time,
 *	else we use the last modified time of sbuf;
 */

char_t* websGetDateString(websStatType* sbuf)
{
	char_t*	cp;
	char_t*	r;
	time_t	now;

	if (sbuf == NULL) {
		time(&now);
	} else {
		now = sbuf->mtime;
	}
	if ((cp = gctime(&now)) != NULL) {
		cp[gstrlen(cp) - 1] = '\0';
		r = bstrdup(B_L, cp);
		return r;
	}
	return NULL;
}

/******************************************************************************/
/*
 *	Mark time. Set a timestamp so that, later, we can return the number of
 *	seconds since we made the mark. Note that the mark my not be a
 *	"real" time, but rather a relative marker.
 */

static void websMarkTime(webs_t wp)
{
	wp->timestamp = time(0);
}

/******************************************************************************/
/*
 *	Get the number of seconds since the last mark.
 */

static int websGetTimeSinceMark(webs_t wp)
{
	return time(0) - wp->timestamp;
}

/******************************************************************************/
