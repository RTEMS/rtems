
/*  no_libc.h
 *
 *  This file contains stubs for the reentrancy hooks when
 *  an unknown C library is used.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */


#include <rtems.h>
#if  !defined(RTEMS_NEWLIB) && !defined(RTEMS_UNIX)

#include "libcsupport.h"
#include "internal.h"

#include <stdlib.h>             /* for free() */

void
libc_init(int reentrant)
{
}

void libc_suspend_main(void)
{
}


void libc_global_exit(rtems_unsigned32 code)
{
}

void _exit(int status)
{
}

#else

/* remove ANSI errors.
 *  A program must contain at least one external-declaration
 *  (X3.159-1989 p.82,L3).
 */
void no_libc_dummy_function( void )
{
}

#endif
