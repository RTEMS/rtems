/*  bspclean.c
 *
 *  This file contains cleanup code executed when the application exits.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space 
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995. 
 *  European Space Agency.
 *
 *  $Id$
 */

#include <bsp.h>

/*
 * The app has "exited" (rtems_shutdown_executive returns control to main)
 */

void bsp_cleanup( void )
{
    /*
     * "halt" by trapping to the simulator command line.
     */


  asm volatile( "ta 0" );
}
