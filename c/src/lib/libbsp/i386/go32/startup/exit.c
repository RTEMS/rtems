/*
 *  exit
 *
 *  This routine returns control to DOS.
 *
 *  COPYRIGHT (c) 1989-1997.
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
#include <bsp.h>
#include <clockdrv.h>
#include <iodrv.h>

#if 0
/* Prefer to pick up _exit() in djgcc */
void _exit( )
{
   Io_cleanup();
   bsp_cleanup();
}
#endif

