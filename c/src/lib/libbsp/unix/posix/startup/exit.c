/*
 *  exit
 *
 *  This routine returns control to "the pre-RTEMS environment".
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
