/*
 *  Intel i960CA Dependent Source
 *
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

#include <rtems/system.h>
#include <rtems/score/isr.h>

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - address of disptaching routine
 *
 *  OUTPUT PARAMETERS: NONE
 */

void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{

  _CPU_Table = *cpu_table;

}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 */
 
unsigned32 _CPU_ISR_Get_level( void )
{
  unsigned32 level;
 
  i960_get_interrupt_level( level );
 
  return level;
}

/*PAGE
 *
 *  _CPU__ISR_install_vector
 *
 *  Install the RTEMS vector wrapper in the CPU's interrupt table.
 *
 *  Input parameters:
 *    vector      - interrupt vector number
 *    old_handler - former ISR for this vector number
 *    new_handler - replacement ISR for this vector number
 *
 *  Output parameters:  NONE
 *
 */

void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  proc_ptr ignored;

  *old_handler = _ISR_Vector_table[ vector ];

  _CPU_ISR_install_raw_handler( vector, _ISR_Handler, &ignored );

  _ISR_Vector_table[ vector ] = new_handler;
}
