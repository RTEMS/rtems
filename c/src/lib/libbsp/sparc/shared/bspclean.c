/*  bspclean.c
 *
 *  This file contains cleanup code executed when the application exits.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
     * set %g1 to 1 to detect clean exit.
     */


  asm volatile( "mov 1, %g1; ta 0" );
}
