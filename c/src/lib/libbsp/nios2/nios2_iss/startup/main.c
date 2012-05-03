/*  main()
 *
 *  This is the entry point for the application.  It calls
 *  the bsp_start routine to the actual dirty work.
 *
 *  COPYRIGHT (c) 2005-2006 Kolja Waschk rtemsdev/ixo.de
 *  Derived from no_cpu/no_bsp/startup/main.c 1.9,
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>

int main(
  int   argc,
  char **argv,
  char **environp
)
{
  extern void bsp_start( int, char**, char ** );

  bsp_start( argc, argv, environp );

  /*
   *  May be able to return to the "crt/start.s" code but also
   *  may not be able to.  Do something here which is board dependent.
   */

  rtems_fatal_error_occurred( 0 );

  return 0; /* just to satisfy the native compiler */
}
