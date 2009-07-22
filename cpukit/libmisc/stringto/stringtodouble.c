/*
 *  COPYRIGHT (c) 2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

/*
 *  Instantiate an error checking wrapper for strtod (double)
 */
#define STRING_TO_FLOAT
#define STRING_TO_TYPE double
#define STRING_TO_NAME rtems_string_to_double
#define STRING_TO_METHOD strtod
#define STRING_TO_MAX HUGE_VAL
#include "stringto_template.h"

