/*  bsp_cleanup()
 *
 *  This routine normally is part of start.s and usually returns
 *  control to a monitor.
 *
 *  INPUT:  NONE
 *
 *  OUTPUT: NONE
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

/*
 *  Rather than deleting this, it is commented out to (hopefully) help
 *  the submitter send updates.
 *
 *  static char _sccsid[] = "@(#)bspclean.c 03/15/96     1.1\n";
 */



#include <rtems.h>
#include <bsp.h>

void idtsim__exit();

void bsp_cleanup( void )
{
  idtsim__exit();
}
