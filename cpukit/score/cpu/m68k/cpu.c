/*
 *  Motorola MC68xxx Dependent Source
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
#include <rtems/score/isr.h>

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
#if ( M68K_HAS_VBR == 0 )
  /* fill the isr redirect table with the code to place the format/id
     onto the stack */

  unsigned32 slot;

  for (slot = 0; slot < CPU_INTERRUPT_NUMBER_OF_VECTORS; slot++)
  {
    _CPU_ISR_jump_table[slot].move_a7 = M68K_MOVE_A7;
    _CPU_ISR_jump_table[slot].format_id = slot << 2;
    _CPU_ISR_jump_table[slot].jmp = M68K_JMP;
    _CPU_ISR_jump_table[slot].isr_handler = (unsigned32) 0xDEADDEAD;
  }
#endif /* M68K_HAS_VBR */

  _CPU_Table = *cpu_table;
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 */
 
unsigned32 _CPU_ISR_Get_level( void )
{
  unsigned32 level;

  m68k_get_interrupt_level( level );

  return level;
}

/*PAGE
 *
 *  _CPU_ISR_install_raw_handler
 */
 
void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  proc_ptr *interrupt_table = NULL;

  m68k_get_vbr( interrupt_table );

  *old_handler = interrupt_table[ vector ];

#if ( M68K_HAS_VBR == 1 )
  interrupt_table[ vector ] = new_handler;
#else
  _CPU_ISR_jump_table[vector].isr_handler = (unsigned32) new_handler;
  interrupt_table[ vector ] = (proc_ptr) &_CPU_ISR_jump_table[vector];
#endif /* M68K_HAS_VBR */
}

/*PAGE
 *
 *  _CPU_ISR_install_vector
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

#if ( M68K_HAS_BFFFO != 1 )
/*
 * Returns table for duplication of the BFFFO instruction (16 bits only)
 */
const unsigned char __BFFFOtable[256] = {
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
#endif
