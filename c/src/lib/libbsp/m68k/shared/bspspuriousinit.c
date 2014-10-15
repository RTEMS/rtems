/*
 *  M68000 Spurious Trap Handler Install Routine
 *
 *  This is just enough of a trap handler to let us know what
 *  the likely source of the trap was.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/bspIo.h>

/*
 *  bsp_spurious_initialize
 *
 *  Install the spurious handler for most vectors.
 */
void bsp_spurious_initialize(void)
{
  uint32_t         vector;

  for ( vector=0 ; vector<255 ; vector++ ) {

    /*
     *  Skip any vectors that might be generally used for traps.
     *
     *  XXX
     */

    set_vector( bsp_spurious_handler, vector, 1 );
  }

}
