/*
 * handler.c -- URL handler support
 *
 * Copyright (c) Go Ahead Software Inc., 1995-1999. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 */

/******************************** Description *********************************/

/*
 *	This module implements a URL handler interface and API to permit
 *	the addition of user definable URL processors.
 */

/********************************* Includes ***********************************/

#include	"wsIntrn.h"

/*********************************** Locals ***********************************/

static websUrlHandlerType*	websUrlHandler;				/* URL handler list */
static int					websUrlHandlerMax;			/* Number of entries */

/**************************** Forward Declarations ****************************/

static int 	websUrlHandlerSort(const void* p1, const void* p2);
static int 	websPublishHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, 
				int sid, char_t *url, char_t *path, char_t *query);

/*********************************** Code *************************************/
/*
 *	Initialize the URL handler module
 */

int websUrlHandlerOpen()
{
	websAspOpen();
	return 0;
}

/******************************************************************************/
/*
 *	Close the URL handler module
 */

void websUrlHandlerClose()
{
	websUrlHandlerType*	sp;

	websAspClose();
	for (sp = websUrlHandler; sp < &websUrlHandler[websUrlHandlerMax]; sp++) {
		bfree(B_L, sp->urlPrefix);
		if (sp->webDir) {
			bfree(B_L, sp->webDir);
		}
	}
	bfree(B_L, websUrlHandler);
	websUrlHandlerMax = 0;
}

/******************************************************************************/
/*
 *	Define a new URL handler. urlPrefix is the URL prefix to match. webDir is 
 *	an optional root directory path for a web directory. arg is an optional
 *	arg to pass to the URL handler. flags defines the matching order. Valid
 *	flags include WEBS_HANDLER_LAST, WEBS_HANDLER_FIRST. If multiple users 
 *	specify last or first, their order is defined alphabetically by the 
 *	urlPrefix.
 */

int websUrlHandlerDefine(char_t *urlPrefix, char_t *webDir, int arg,
		int (*handler)(webs_t wp, char_t *urlPrefix, char_t *webdir, int arg, 
		char_t *url, char_t *path, char_t *query), int flags)
{
	websUrlHandlerType	*sp;
	int					len;

	a_assert(urlPrefix);
	a_assert(handler);

/*
 *	Grow the URL handler array to create a new slot
 */
	len = (websUrlHandlerMax + 1) * sizeof(websUrlHandlerType);
	if ((websUrlHandler = brealloc(B_L, websUrlHandler, len)) == NULL) {
		return -1;
	}
	sp = &websUrlHandler[websUrlHandlerMax++];
	memset(sp, 0, sizeof(websUrlHandlerType));

	sp->urlPrefix = bstrdup(B_L, urlPrefix);
	sp->len = gstrlen(sp->urlPrefix);
	if (webDir) {
		sp->webDir = bstrdup(B_L, webDir);
	} else {
		sp->webDir = bstrdup(B_L, T(""));
	}
	sp->handler = handler;
	sp->arg = arg;
	sp->flags = flags;

/*
 *	Sort in decreasing URL length order observing the flags for first and last
 */
	qsort(websUrlHandler, websUrlHandlerMax, sizeof(websUrlHandlerType), 
		websUrlHandlerSort);
	return 0;
}

/******************************************************************************/
/*
 *	Delete an existing URL handler. We don't reclaim the space of the old 
 *	handler, just NULL the entry. Return -1 if handler is not found.
 */

int websUrlHandlerDelete(int (*handler)(webs_t wp, char_t *urlPrefix, 
	char_t *webDir, int arg, char_t *url, char_t *path, char_t *query))
{
	websUrlHandlerType	*sp;
	int					i;

	for (i = 0; i < websUrlHandlerMax; i++) {
		sp = &websUrlHandler[i];
		if (sp->handler == handler) {
			sp->handler = NULL;
			return 0;
		}
	}
	return -1;
}

/******************************************************************************/
/*
 *	Sort in decreasing URL length order observing the flags for first and last
 */

static int websUrlHandlerSort(const void* p1, const void* p2)
{
	websUrlHandlerType	*s1, *s2;
	int					rc;

	a_assert(p1);
	a_assert(p2);

	s1 = (websUrlHandlerType*) p1;
	s2 = (websUrlHandlerType*) p2;

	if ((s1->flags & WEBS_HANDLER_FIRST) || (s2->flags & WEBS_HANDLER_LAST)) {
		return -1;
	}

	if ((s2->flags & WEBS_HANDLER_FIRST) || (s1->flags & WEBS_HANDLER_LAST)) {
		return 1;
	}

	if ((rc = gstrcmp(s1->urlPrefix, s2->urlPrefix)) == 0) {
		if (s1->len < s2->len) {
			return 1;
		} else if (s1->len > s2->len) {
			return -1;
		}
	}
	return -rc; 
}

/******************************************************************************/
/*
 *	Publish a new web directory (Use the default URL handler)
 */

int websPublish(char_t *urlPrefix, char_t *path)
{
	return websUrlHandlerDefine(urlPrefix, path, 0, websPublishHandler, 0);
}

/******************************************************************************/
/*
 *	Return the directory for a given prefix. Ignore empty prefixes
 */

char_t *websGetPublishDir(char_t *path, char_t **urlPrefix)
{
	websUrlHandlerType	*sp;
	int					i;

	for (i = 0; i < websUrlHandlerMax; i++) {
		sp = &websUrlHandler[i];
		if (sp->urlPrefix[0] == '\0') {
			continue;
		}
		if (sp->handler && gstrncmp(sp->urlPrefix, path, sp->len) == 0) {
			if (urlPrefix) {
				*urlPrefix = sp->urlPrefix;
			}
			return sp->webDir;
		}
	}
	return NULL;
}

/******************************************************************************/
/*
 *	Publish URL handler. We just patch the web page Directory and let the
 *	default handler do the rest.
 */

static int websPublishHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, 
	int sid, char_t *url, char_t *path, char_t *query)
{
	int		len;

	a_assert(websValid(wp));
	a_assert(path);

/*
 *	Trim the urlPrefix off the path and set the webdirectory. Add one to step 
 *	over the trailing '/'
 */
	len = gstrlen(urlPrefix) + 1;
	websSetRequestPath(wp, webDir, &path[len]);
	return 0;
}

/******************************************************************************/
/*
 *	See if any valid handlers are defined for this request. If so, call them
 *	and continue calling valid handlers until one accepts the request. 
 *	Return true if a handler was invoked, else return FALSE.
 */

int websUrlHandlerRequest(webs_t wp)
{
	websUrlHandlerType	*sp;
	int					i, first;

	a_assert(websValid(wp));

/*
 *	Delete the socket handler as we don't want to start reading any
 *	data on the connection as it may be for the next pipelined HTTP/1.1
 *	request if using Keep Alive
 */
	socketDeleteHandler(wp->sid);
	wp->state = WEBS_PROCESSING;
	websStats.handlerHits++;
	
	websSetRequestPath(wp, websGetDefaultDir(), NULL);

/*
 *	We loop over each handler in order till one accepts the request. 
 *	The security handler will handle the request if access is NOT allowed.
 */
	first = 1;
	for (i = 0; i < websUrlHandlerMax; i++) {
		sp = &websUrlHandler[i];
		if (sp->handler && gstrncmp(sp->urlPrefix, wp->path, sp->len) == 0) {
			if (first) {
				websSetEnv(wp);
				first = 0;
			}
			if ((*sp->handler)(wp, sp->urlPrefix, sp->webDir, sp->arg, 
					wp->url, wp->path, wp->query)) {
				return 1;
			}
			if (!websValid(wp)) {
				goahead_trace(0, 
				T("webs: handler %s called websDone, but didn't return 1\n"),
					sp->urlPrefix);
				return 1;
			}
		}
	}
/*
 *	If no handler processed the request, then return an error. Note: It was 
 *	the handlers responsibility to call websDone
 */
	if (i >= websUrlHandlerMax) {
		websError(wp, 200, T("No handler for this URL %s"), wp->url);
	}
	return 0;
}

/******************************************************************************/
