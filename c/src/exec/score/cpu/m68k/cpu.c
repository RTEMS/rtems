/*
 *  Motorola MC68020 Dependent Source
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

#include <rtems/system.h>
#include <rtems/fatal.h>
#include <rtems/isr.h>

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - entry pointer to thread dispatcher
 *
 *  OUTPUT PARAMETERS: NONE
 */

void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{

  if ( cpu_table == NULL )
    rtems_fatal_error_occurred( RTEMS_NOT_CONFIGURED );

  _CPU_Table = *cpu_table;

}

/*  _CPU_ISR_install_vector
 *
 *  This kernel routine installs the RTEMS handler for the
 *  specified vector.
 *
 *  Input parameters:
 *    vector      - interrupt vector number
 *    new_handler - replacement ISR for this vector number
 *    old_handler - former ISR for this vector number
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  proc_ptr *interrupt_table = NULL;

  m68k_get_vbr( interrupt_table );

  *old_handler = _ISR_Vector_table[ vector ];

  _ISR_Vector_table[ vector ] = new_handler;
  interrupt_table[ vector ] = _ISR_Handler;
}


/*PAGE
 *
 *  _CPU_Install_interrupt_stack
 */

void _CPU_Install_interrupt_stack( void )
{
#if ( M68K_HAS_SEPARATE_STACKS == 1 )
  void *isp = _CPU_Interrupt_stack_high;

  asm volatile ( "movec %0,%%isp" : "=r" (isp) : "0" (isp) );
#else
#warning "FIX ME... HOW DO I INSTALL THE INTERRUPT STACK!!!"
#endif
}

