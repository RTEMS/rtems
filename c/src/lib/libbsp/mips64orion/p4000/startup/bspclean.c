/*  bsp_cleanup()
 *
 *  This routine normally is part of start.s and usually returns
 *  control to a monitor.
 *
 *  INPUT:  NONE
 *
 *  OUTPUT: NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  bspclean.c,v 1.2 1995/05/31 16:56:27 joel Exp
 */
#ifndef lint
static char _sccsid[] = "@(#)bspclean.c 03/15/96     1.1\n";
#endif


#include <rtems.h>
#include <bsp.h>

void bsp_cleanup( void )
{
}
