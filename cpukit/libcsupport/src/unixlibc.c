/*
 *  $Id$
 *  UNIX Port C Library Support
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#if defined(RTEMS_UNIXLIB)

void libc_init(int reentrant)
{
}

#else
 
/* remove ANSI errors.
 *  A program must contain at least one external-declaration
 *  (X3.159-1989 p.82,L3).
 */
void unixlibc_dummy_function( void )
{
}

#endif
