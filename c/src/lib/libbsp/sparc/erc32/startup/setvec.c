/*  set_vector
 *
 *  This routine installs an interrupt vector on the SPARC simulator.
 *
 *  INPUT PARAMETERS:
 *    handler - interrupt handler entry point
 *    vector  - vector number
 *    type    - 0 indicates raw hardware connect
 *              1 indicates RTEMS interrupt connect
 *
 *  OUTPUT PARAMETERS:  NONE
 *
 *  RETURNS:
 *    address of previous interrupt handler
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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

rtems_isr_entry set_vector(                   /* returns old vector */
  rtems_isr_entry     handler,                /* isr routine        */
  rtems_vector_number vector,                 /* vector number      */
  int                 type                    /* RTEMS or RAW intr  */
)
{
  rtems_isr_entry previous_isr;
  unsigned32      real_trap;
  unsigned32      source;

  if ( type )
    rtems_interrupt_catch( handler, vector, &previous_isr );
  else 
    _CPU_ISR_install_raw_handler( vector, handler, (void *)&previous_isr );

  real_trap = SPARC_REAL_TRAP_NUMBER( vector );

  if ( ERC32_Is_MEC_Trap( real_trap ) ) {
    
    source = ERC32_TRAP_SOURCE( real_trap );

    ERC32_Clear_interrupt( source );
    ERC32_Unmask_interrupt( source );
  }

  return previous_isr;
}

/* ERC32 power-down function */

void _CPU_Thread_Idle_body( void )
{
  while (1) {
    ERC32_MEC.Power_Down = 0;   /* value is irrelevant */
  }
}

