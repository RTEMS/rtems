#if  !defined(RTEMS_NEWLIB) && !defined(RTEMS_UNIX)

/*  no_libc.h
 *
 *  This file contains stubs for the reentrancy hooks when
 *  an unknown C library is used.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */


#include <rtems.h>

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
