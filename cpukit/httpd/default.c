/*
 * default.c -- Default URL handler. Includes support for ASP.
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 * $Id$
 */

/******************************** Description *********************************/

/*
 *	This module provides default URL handling and Active Server Page support.
 *
 *	In many cases we don't check the return code of calls to websWrite as
 *	it is easier, smaller and non-fatal to continue even when the requesting
 *	browser has gone away.
 */

/********************************* Includes ***********************************/

#include	"wsIntrn.h"

/*********************************** Locals ***********************************/

static char_t	*websDefaultPage;			/* Default page name */
static char_t	*websDefaultDir;			/* Default Web page directory */

/**************************** Forward Declarations ****************************/

static void websDefaultWriteEvent(webs_t wp);

/*********************************** Code *************************************/
/*
 *	Process a default URL request. This will validate the URL and handle "../"
 *	and will provide support for Active Server Pages. As the handler is the
 *	last handler to run, it always indicates that it has handled the URL 
 *	by returning 1. 
 */

int websDefaultHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg,
						char_t *url, char_t *path, char_t *query)
{
	websStatType	sbuf;
	char_t			*lpath, *tmp, *date;
	int				bytes, flags, nchars;

	a_assert(websValid(wp));
	a_assert(url && *url);
	a_assert(path);
	a_assert(query);

/*
 *	Validate the URL and ensure that ".."s don't give access to unwanted files
 */
	flags = websGetRequestFlags(wp);

	if (websValidateUrl(wp, path) < 0) {
		websError(wp, 500, T("Invalid URL %s"), url);
		return 1;
	}
	lpath = websGetRequestLpath(wp);
	nchars = gstrlen(lpath) - 1;
	if (lpath[nchars] == '/' || lpath[nchars] == '\\') {
		lpath[nchars] = '\0';
	}

/*
 *	If the file is a directory, redirect using the nominated default page
 */
	if (websPageIsDirectory(lpath)) {
		nchars = gstrlen(path);
		if (path[nchars-1] == '/' || path[nchars-1] == '\\') {
			path[--nchars] = '\0';
		}
		nchars += gstrlen(websDefaultPage) + 2;
		fmtAlloc(&tmp, nchars, T("%s/%s"), path, websDefaultPage);
		websRedirect(wp, tmp);
		bfreeSafe(B_L, tmp);
		return 1;
	}

/*
 *	Open the document. Stat for later use.
 */
	if (websPageOpen(wp, lpath, path, SOCKET_RDONLY | SOCKET_BINARY, 
		0666) < 0) {
		websError(wp, 400, T("Cannot open URL <b>%s</b>"), url);
		return 1;
	} 

	if (websPageStat(wp, lpath, path, &sbuf) < 0) {
		websError(wp, 400, T("Cannot stat page for URL <b>%s</b>"), url);
		return 1;
	}

/*
 *	If the page has not been modified since the user last received it and it
 *	is not dynamically generated each time (ASP), then optimize request by
 *	sending a 304 Use local copy response
 */
	websStats.localHits++;
#ifdef WEBS_IF_MODIFIED_SUPPORT
	if (flags & WEBS_IF_MODIFIED && !(flags & WEBS_ASP)) {
		if (sbuf.mtime <= wp->since) {
			websWrite(wp, T("HTTP/1.0 304 Use local copy\r\n"));

/*
 *			by license terms the following line of code must
 *			not be modified.
 */
			websWrite(wp, T("Server: %s\r\n"), WEBS_NAME);

			if (flags & WEBS_KEEP_ALIVE) {
				websWrite(wp, T("Connection: keep-alive\r\n"));
			}
			websWrite(wp, T("\r\n"));
			websSetRequestFlags(wp, flags |= WEBS_HEADER_DONE);
			websDone(wp, 304);
			return 1;
		}
	}
#endif

/*
 *	Output the normal HTTP response header
 */
	if ((date = websGetDateString(NULL)) != NULL) {
		websWrite(wp, T("HTTP/1.0 200 OK\r\nDate: %s\r\n"), date);

/*
 *		By license terms the following line of code must not be modified.
 */
		websWrite(wp, T("Server: %s\r\n"), WEBS_NAME);
		bfree(B_L, date);
	}
	flags |= WEBS_HEADER_DONE;

/*
 *	If this is an ASP request, ensure the remote browser doesn't cache it.
 *	Send back both HTTP/1.0 and HTTP/1.1 cache control directives
 */
	if (flags & WEBS_ASP) {
		bytes = 0;
		websWrite(wp, T("Pragma: no-cache\r\nCache-Control: no-cache\r\n"));

	} else {
		if ((date = websGetDateString(&sbuf)) != NULL) {
			websWrite(wp, T("Last-modified: %s\r\n"), date);
			bfree(B_L, date);
		}
		bytes = sbuf.size;
	}

	if (bytes) {
		websWrite(wp, T("Content-length: %d\r\n"), bytes);
		websSetRequestBytes(wp, bytes);
	}
	websWrite(wp, T("Content-type: %s\r\n"), websGetRequestType(wp));

	if ((flags & WEBS_KEEP_ALIVE) && !(flags & WEBS_ASP)) {
		websWrite(wp, T("Connection: keep-alive\r\n"));
	}
	websWrite(wp, T("\r\n"));

/*
 *	All done if the browser did a HEAD request
 */
	if (flags & WEBS_HEAD_REQUEST) {
		websDone(wp, 200);
		return 1;
	}

/*
 *	Evaluate ASP requests
 */
	if (flags & WEBS_ASP) {
		if (websAspRequest(wp, lpath) < 0) {
			return 1;
		}
		websDone(wp, 200);
		return 1;
	}

#ifdef WEBS_SSL_SUPPORT
	if (wp->flags & WEBS_SECURE) {
		websDefaultWriteEvent(wp);
	} else {
		websSetRequestSocketHandler(wp, SOCKET_WRITABLE, websDefaultWriteEvent);
	}
#else
/*
 *	For normal web documents, return the data via background write
 */
	websSetRequestSocketHandler(wp, SOCKET_WRITABLE, websDefaultWriteEvent);
#endif
	return 1;
}

/******************************************************************************/
/*
 *	Validate the URL path and process ".." path segments. Return -1 if the URL
 *	is bad.
 */

int websValidateUrl(webs_t wp, char_t *path)
{
	char_t	*parts[64];					/* Array of ptr's to URL parts */
	char_t	*token, *dir, *lpath;
	int		i, len, npart;

	a_assert(websValid(wp));
	a_assert(path);

	dir = websGetRequestDir(wp);
	if (dir == NULL || *dir == '\0') {
		return -1;
	}

/*
 *	Copy the string so we don't destroy the original
 */
	path = bstrdup(B_L, path);
	websDecodeUrl(path, path, gstrlen(path));

	len = npart = 0;
	parts[0] = NULL;

   /*
    * 22 Jul 02 -- there were reports that a directory traversal exploit was
    * possible in the WebServer running under Windows if directory paths
    * outside the server's specified root web were given by URL-encoding the
    * backslash character, like:
    *
    *  GoAhead is vulnerable to a directory traversal bug. A request such as
    *  
    *  GoAhead-server/../../../../../../../ results in an error message
    *  'Cannot open URL'.

    *  However, by encoding the '/' character, it is possible to break out of
    *  the
    *  web root and read arbitrary files from the server.
    *  Hence a request like:
    * 
    *  GoAhead-server/..%5C..%5C..%5C..%5C..%5C..%5C/winnt/win.ini returns the
    *  contents of the win.ini file.
    * (Note that the description uses forward slashes (0x2F), but the example
    * uses backslashes (0x5C). In my tests, forward slashes are correctly
    * trapped, but backslashes are not. The code below substitutes forward
    * slashes for backslashes before attempting to validate that there are no
    * unauthorized paths being accessed.
    */
   token = gstrchr(path, '\\');
   while (token != NULL)
   {
      *token = '/';
      token = gstrchr(token, '\\');
   }
   
	token = gstrtok(path, T("/"));

/*
 *	Look at each directory segment and process "." and ".." segments
 *	Don't allow the browser to pop outside the root web. 
 */
	while (token != NULL) {
		if (gstrcmp(token, T("..")) == 0) {
			if (npart > 0) {
				npart--;
			}

		} else if (gstrcmp(token, T(".")) != 0) {
			parts[npart] = token;
			len += gstrlen(token) + 1;
			npart++;
		}
		token = gstrtok(NULL, T("/"));
	}

/*
 *	Create local path for document. Need extra space all "/" and null.
 */
	if (npart || (gstrcmp(path, T("/")) == 0) || (path[0] == '\0')) {
		lpath = balloc(B_L, (gstrlen(dir) + 1 + len + 1) * sizeof(char_t));
		gstrcpy(lpath, dir);

		for (i = 0; i < npart; i++) {
			gstrcat(lpath, T("/"));
			gstrcat(lpath, parts[i]);
		}
		websSetRequestLpath(wp, lpath);
		bfree(B_L, path);
		bfree(B_L, lpath);

	} else {
		bfree(B_L, path);
		return -1;
	}
	return 0;
}

/******************************************************************************/
/*
 *	Do output back to the browser in the background. This is a socket
 *	write handler.
 */

static void websDefaultWriteEvent(webs_t wp)
{
	int		len, wrote, flags, bytes, written;
	char	*buf;

	a_assert(websValid(wp));

	flags = websGetRequestFlags(wp);

	websSetTimeMark(wp);

	wrote = bytes = 0;
	written = websGetRequestWritten(wp);

/*
 *	We only do this for non-ASP documents
 */
	if ( !(flags & WEBS_ASP)) {
		bytes = websGetRequestBytes(wp);
/*
 *		Note: websWriteDataNonBlock may return less than we wanted. It will
 *		return -1 on a socket error
 */
		if ((buf = balloc(B_L, PAGE_READ_BUFSIZE)) == NULL) {
			websError(wp, 200, T("Can't get memory"));
		} else {
			while ((len = websPageReadData(wp, buf, PAGE_READ_BUFSIZE)) > 0) {
				if ((wrote = websWriteDataNonBlock(wp, buf, len)) < 0) {
					break;
				}
				written += wrote;
				if (wrote != len) {
					websPageSeek(wp, - (len - wrote));
					break;
				}
			}
/*
 *			Safety. If we are at EOF, we must be done
 */
			if (len == 0) {
				a_assert(written >= bytes);
				written = bytes;
			}
			bfree(B_L, buf);
		}
	}

/*
 *	We're done if an error, or all bytes output
 */
	websSetRequestWritten(wp, written);
	if (wrote < 0 || written >= bytes) {
		websDone(wp, 200);
	}
}

/******************************************************************************/
/* 
 *	Closing down. Free resources.
 */

void websDefaultClose(void)
{
	if (websDefaultPage) {
		bfree(B_L, websDefaultPage);
		websDefaultPage = NULL;
	}
	if (websDefaultDir) {
		bfree(B_L, websDefaultDir);
		websDefaultDir = NULL;
	}
}

/******************************************************************************/
/*
 *	Get the default page for URL requests ending in "/"
 */

char_t *websGetDefaultPage(void)
{
	return websDefaultPage;
}

/******************************************************************************/
/*
 *	Get the default web directory
 */

char_t *websGetDefaultDir(void)
{
	return websDefaultDir;
}

/******************************************************************************/
/*
 *	Set the default page for URL requests ending in "/"
 */

void websSetDefaultPage(char_t *page)
{
	a_assert(page && *page);

	if (websDefaultPage) {
		bfree(B_L, websDefaultPage);
	}
	websDefaultPage = bstrdup(B_L, page);
}

/******************************************************************************/
/*
 *	Set the default web directory
 */

void websSetDefaultDir(char_t *dir)
{
	a_assert(dir && *dir);
	if (websDefaultDir) {
		bfree(B_L, websDefaultDir);
	}
	websDefaultDir = bstrdup(B_L, dir);
}

/******************************************************************************/
