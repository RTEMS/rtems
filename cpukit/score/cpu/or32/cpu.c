/*
 *  Opencore Or1k CPU Dependent Source
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  This file adapted from no_bsp board library of the RTEMS distribution.
 *  The body has been modified for the Bender Or1k implementation by
 *  Chris Ziomkowski. <chris@asics.ws>
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - address of disptaching routine
 *
 */


void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)  
)
{
  /*
   *  The thread_dispatch argument is the address of the entry point
   *  for the routine called at the end of an ISR once it has been
   *  decided a context switch is necessary.  On some compilation
   *  systems it is difficult to call a high-level language routine
   *  from assembly.  This allows us to trick these systems.
   *
   *  If you encounter this problem save the entry point in a CPU
   *  dependent variable.
   */

  _CPU_Thread_dispatch_pointer = thread_dispatch;

  /*
   *  If there is not an easy way to initialize the FP context
   *  during Context_Initialize, then it is usually easier to
   *  save an "uninitialized" FP context here and copy it to
   *  the task's during Context_Initialize.
   */

  /* FP context initialization support goes here */

  _CPU_Table = *cpu_table;
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 *
 *  or1k Specific Information:
 *
 *  There are only 2 interrupt levels for the or1k architecture.
 *  Either interrupts are enabled or disabled. They are considered
 *  enabled if both exceptions are enabled (SR_EXR) and interrupts
 *  are enabled (SR_EIR). If either of these conditions are not
 *  met, interrupts are disabled, and a level of 1 is returned.
 */

inline unsigned32 _CPU_ISR_Get_level( void )
{
  register unsigned32 sr;
  asm("l.mfspr %0,r0,0x17" : "=r" (sr));
  return !((sr & SR_EXR) && (sr & SR_EIR));
}

/*PAGE
 *
 *  _CPU_ISR_install_raw_handler
 *
 *  or1k Specific Information:
 *
 *  As a general rule the following is done for interrupts:
 *  
 *  For normal exceptions, exceptions are immediately reenabled
 *  by setting the SR_EXR bit. For interrupt exceptions, the
 *  SR_EIR bit is first cleared, and then exceptions are reenabled.
 */
 
void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  register unsigned32 sr;
  register unsigned32 tmp;
  extern unsigned32 Or1k_Interrupt_Vectors[];

  asm volatile ("l.mfspr %0,r0,0x11\n\t"
	       "l.addi  %1,r0,-5\n\t"
	       "l.and   %1,%1,%0\n\t": "=r" (sr) : "r" (tmp));
  *old_handler = *((proc_ptr*)&Or1k_Interrupt_Vectors[vector]);
  *((proc_ptr*)&Or1k_Interrupt_Vectors[vector]) = new_handler;
  asm volatile ("l.mtspr r0,%0,0x11\n\t":: "r" (sr));
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
 *    old_handler - former ISR for this vector number
 *    new_handler - replacement ISR for this vector number
 *
 *  Output parameters:  NONE
 *
 *
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */

void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
   *old_handler = _ISR_Vector_table[ vector ];

   /*
    *  If the interrupt vector table is a table of pointer to isr entry
    *  points, then we need to install the appropriate RTEMS interrupt
    *  handler for this vector number.
    */

   _CPU_ISR_install_raw_handler( vector, new_handler, old_handler );

   /*
    *  We put the actual user ISR address in '_ISR_vector_table'.  This will
    *  be used by the _ISR_Handler so the user gets control.
    */

    _ISR_Vector_table[ vector ] = new_handler;
}

/*PAGE
 *
 *  _CPU_Install_interrupt_stack
 *  
 *  We don't use a separate interrupt stack.
 *
 */

void _CPU_Install_interrupt_stack( void )
{
}

/*PAGE
 *
 *  _CPU_Thread_Idle_body
 *
 *  NOTES:
 *
 *  1. This is the same as the regular CPU independent algorithm.
 *
 *  2. If you implement this using a "halt", "idle", or "shutdown"
 *     instruction, then don't forget to put it in an infinite loop.
 *
 *  3. Be warned. Some processors with onboard DMA have been known
 *     to stop the DMA if the CPU were put in IDLE mode.  This might
 *     also be a problem with other on-chip peripherals.  So use this
 *     hook with caution.
 *
 */

void _CPU_Thread_Idle_body( void )
{

  for( ; ; )
    /* insert your "halt" instruction here */ ;
}
