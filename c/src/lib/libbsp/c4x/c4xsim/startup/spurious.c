/*
 *  CXX Simulator Spurious Trap Handler Assistant
 *
 *  This is just enough of a trap handler to let us know what 
 *  the likely source of the trap was.
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
#include <rtems/bspIo.h>
#include <simio.h>

/*
 *  bsp_spurious_handler_assistant
 *
 *  We can't recover so just return to gdb.
 */

void bsp_spurious_handler_assistant(
  rtems_vector_number  vector,
  CPU_Interrupt_frame *isf
)
{
  sim_exit();
}
