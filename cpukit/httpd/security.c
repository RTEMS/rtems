/*
 * security.c -- Security handler
 *
 * Copyright (c) Go Ahead Software Inc., 1995-1999. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 */

/******************************** Description *********************************/

/*
 *	This module provides a basic security policy. It supports a single global
 *	password and ignores the username. Encoding/decoding of the password is 
 *	-not- done.
 */

/********************************* Includes ***********************************/

#include	"wsIntrn.h"

/******************************** Local Data **********************************/

static char_t	websPassword[WEBS_MAX_PASS];	/* Access password (decoded) */

/*********************************** Code *************************************/
/*
 *	Determine if this request should be honored
 */

int websSecurityHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
						char_t *url, char_t *path, char_t *query)
{
	char_t	*type, *password;
	int		flags;

	a_assert(websValid(wp));
	a_assert(url && *url);
	a_assert(path && *path);

/*
 *	Get the critical request details
 */
	type = websGetRequestType(wp);
	password = websGetRequestPassword(wp);
	flags = websGetRequestFlags(wp);

/*
 *	Validate the users password if required (local access is always allowed)
 *	We compare the decoded form of the password.
 */
	if (*websPassword && !(flags & WEBS_LOCAL_REQUEST)) {

		if (password && *password) {
			if (gstrcmp(password, websPassword) != 0) {
				websStats.access++;
				websError(wp, 200, T("Access Denied\nWrong Password"));
				websSetPassword(T(""));
				return 1;
			}
		} else {
/*
 *			This will cause the browser to display a password / username
 *			dialog
 */
			websStats.errors++;
			websError(wp, 401, T("<html><head>Access Denied</head><body>\r\n\
				Access to this document requires a password.</body>\
				</html>\r\n"));
			return 1;
		}
	}
	return 0;
}

/******************************************************************************/
/*
 *	Delete the default security handler
 */

void websSecurityDelete()
{
	websUrlHandlerDelete(websSecurityHandler);
}

/******************************************************************************/
/*
 *	Store the new password, expect a decoded password. Store in websPassword in 
 *	the decoded form.
 */

void websSetPassword(char_t *password)
{
	a_assert(password);

	gstrncpy(websPassword, password, TSZ(websPassword));
}

/******************************************************************************/
/*
 *	Get password, return the decoded form
 */

char_t *websGetPassword()
{
	return websPassword;
}

/******************************************************************************/

