/*
 * websuemf.c -- GoAhead Micro Embedded Management Framework
 *
 * Copyright (c) Go Ahead Software Inc., 1995-1999. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 */

/********************************** Description *******************************/

/*
 *	This modules provides compatibility with the full GoAhead EMF.
 */

/*********************************** Includes *********************************/

#include	"wsIntrn.h"

/************************************* Code ***********************************/
/*
 *	Evaluate a script
 */

int scriptEval(int engine, char_t* cmd, char_t** result, int chan)
{
	int		ejid;

	if (engine == EMF_SCRIPT_EJSCRIPT) {
		ejid = (int) chan;
		if (ejEval(ejid, cmd, NULL) ) {
			return 0;
		} else {
			return -1;
		}
	}
	return -1;
}

/******************************************************************************/
