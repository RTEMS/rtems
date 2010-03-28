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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/*
 *  Instantiate an error checking wrapper for strtol (long)
 */
#define STRING_TO_INTEGER
#define STRING_TO_TYPE long int
#define STRING_TO_NAME rtems_string_to_long
#define STRING_TO_METHOD strtol
#define STRING_TO_MIN LONG_MIN
#define STRING_TO_MAX LONG_MAX
#include "stringto_template.h"
