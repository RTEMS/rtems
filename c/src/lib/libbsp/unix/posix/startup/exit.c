/*
 *  exit
 *
 *  This routine returns control to "the pre-RTEMS environment".
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

#include <bsp.h>
#include <clockdrv.h>

void _exit( void )
{
   /* Clock or Timer cleanup is run by at_exit() */

   Io_cleanup();

   bsp_cleanup();
}
