/*  main()
 *
 *  This is the entry point for the application.  It calls
 *  the bsp_start routine to the actual dirty work.
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
#include <bsp.h>

int main(
  int   argc,
  char **argv
)
{
  bsp_start();

  /*
   *  May be able to return to the "crt/start.s" code but also
   *  may not be able to.  Do something here which is board dependent.
   */

  rtems_fatal_error_occurred( 0 );
}
