/*
 * uemf.c -- GoAhead Micro Embedded Management Framework
 *
 * Copyright (c) Go Ahead Software, Inc., 1995-1999
 *
 * See the file "license.txt" for usage and redistribution license requirements
 */

/********************************** Description *******************************/

/*
 *	This module provides compatibility with the full GoAhead EMF.
 *	It is a collection of routines which permits the GoAhead WebServer to
 *	run stand-alone and to also load as a solution pack under the GoAhead EMF.
 */

/*********************************** Includes *********************************/

#include	"uemf.h"

/********************************** Local Data ********************************/

int emfInst;							/* Application instance handle */

/************************************* Code ***********************************/
/*
 *	Error message that doesn't need user attention. Customize this code
 *	to direct error messages to whereever the developer wishes
 */

void error(E_ARGS_DEC, int flags, char_t *fmt, ...)
{
  va_list arglist;
  
  va_start(arglist, fmt);
  
	if (flags & E_LOG) {
		/* Log error message */

	} else if (flags & E_ASSERT) {
		/* Assert message */

	} else if (flags & E_USER) {
		/* Display message to the user */

	}

  vprintf (fmt, arglist);
  va_end(arglist);
}

/******************************************************************************/
/*
 *	Trace log. Customize this function to log trace output
 */

void goahead_trace(int level, char_t *afmt, ...)
{
#if DEBUG
	va_list 	args;
	char_t		*buf;

	va_start(args, afmt);
	buf = NULL;
	gvsnprintf(&buf, VALUE_MAX_STRING, afmt, args);
	if (buf) {
		gprintf(buf);
		bfree(B_L, buf);
	}
	va_end(args);
#endif
}

/******************************************************************************/
/*
 *	Save the instance handle
 */

void emfInstSet(int inst)
{
	emfInst = inst;
}

/******************************************************************************/
/*
 *	Get the instance handle
 */

int emfInstGet()
{
	return emfInst;
}

/******************************************************************************/
/*
 *	Convert a string to lower case
 */

char_t *strlower(char_t *string)
{
	char_t	*s;

	a_assert(string);

	if (string == NULL) {
		return NULL;
	}

	s = string;
	while (*s) {
		if (gisupper(*s)) {
			*s = (char_t) gtolower(*s);
		}
		s++;
	}
	*s = '\0';
	return string;
}

/******************************************************************************/
/* 
 *	Convert a string to upper case
 */

char_t *strupper(char_t *string)
{
	char_t	*s;

	a_assert(string);
	if (string == NULL) {
		return NULL;
	}

	s = string;
	while (*s) {
		if (gislower(*s)) {
			*s = (char_t) gtoupper(*s);
		}
		s++;
	}
	*s = '\0';
	return string;
}

/******************************************************************************/
/*
 *	Convert integer to ascii string. Allow a NULL string in which case we
 *	allocate a dynamic buffer. 
 */

char_t *stritoa(int n, char_t *string, int width)
{
	char_t	*cp, *lim, *s;
	char_t	buf[16];						/* Just temp to hold number */
	int		next, minus;

	a_assert(string && width > 0);

	if (string == NULL) {
		if (width == 0) {
			width = 10;
		}
		if ((string = balloc(B_L, width + 1)) == NULL) {
			return NULL;
		}
	}
	if (n < 0) {
		minus = 1;
		n = -n;
		width--;
	} else {
		minus = 0;
	}

	cp = buf;
	lim = &buf[width - 1];
	while (n > 9 && cp < lim) {
		next = n;
		n /= 10;
		*cp++ = (char_t) (next - n * 10 + '0');
	}
	if (cp < lim) {
		*cp++ = (char_t) (n + '0');
	}

	s = string;
	if (minus) {
		*s++ = '-';
	}

	while (cp > buf) {
		*s++ = *--cp;
	}

	*s++ = '\0';
	return string;
}

/******************************************************************************/

