/*  bsp_cleanup()
 *
 *  This routine normally is part of start.s and returns
 *  control to a monitor but on the UNIX simulator
 *  we do that directly from main.c.
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
 *  $Id$
 */

#include <bsp.h>

#include <stdio.h>

/*
 * The app has "exited" (called rtems_shutdown_executive)
 */

void bsp_cleanup( void )
{
  /*
   * Invoke any fatal error extension and "halt"
   * By definition, rtems_fatal_error_occurred does not return.
   */

  fflush(stdout);
  fflush(stderr);

  rtems_fatal_error_occurred(0);
}
