/*
 * value.c -- Generic type (holds all types)
 *
 * Copyright (c) Go Ahead Software, Inc., 1995-1999
 *
 * See the file "license.txt" for usage and redistribution license requirements
 */

/******************************** Description *********************************/

/*
 *	This module provides a generic type that can hold all possible types.
 *	It is designed to provide maximum effeciency. 
 */

/********************************* Includes ***********************************/

#include	"uemf.h"

/*********************************** Locals ***********************************/


/*********************************** Code *************************************/
/*
 *	Initialize a integer value.  
 */

value_t valueInteger(long value)
{
	value_t	v = VALUE_VALID;

	v.value.integer = value;
	return v;
}

/******************************************************************************/
/*
 *	Initialize a string value. Note: not allocation
 */

value_t valueString(char_t *value, int flags)
{
	value_t	v = VALUE_VALID;

	v.type = string;
	if (flags & VALUE_ALLOCATE) {
		v.allocated = 1;
		v.value.string = gstrdup(B_L, value);
	} else {
		v.allocated = 0;
		v.value.string = value;
	}
	return v;
}

/******************************************************************************/
/*
 *	Free any storage allocated for a value.  
 */

void valueFree(value_t *v)
{
	a_assert(v);

	if (v->valid && v->allocated && v->type == string && 
			v->value.string != NULL) {
		bfree(B_L, v->value.string);
	}
	v->type = undefined;
	v->valid = 0;
	v->allocated = 0;
}

/******************************************************************************/
