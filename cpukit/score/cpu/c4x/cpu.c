/*
 *  C4x CPU Dependent Source
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
#include <rtems/score/wkspace.h>


/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - address of disptaching routine
 *
 *  C4x Specific Information:
 *
 */

void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{
#if 0
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
#endif

#if (CPU_HARDWARE_FP == TRUE)
  /*
   *  If there is not an easy way to initialize the FP context
   *  during Context_Initialize, then it is usually easier to
   *  save an "uninitialized" FP context here and copy it to
   *  the task's during Context_Initialize.
   */

  /* FP context initialization support goes here */
#endif

  _CPU_Table = *cpu_table;
}

/*PAGE
 *
 *  _CPU_ISR_install_raw_handler
 *
 *  C4x Specific Information:
 *
 */
 
void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  void       **ittp;

  /*
   *  This is where we install the interrupt handler into the "raw" interrupt
   *  table used by the CPU to dispatch interrupt handlers.
   */
    
  ittp = c4x_get_ittp();
  *old_handler = ittp[ vector ];
  ittp[ vector ] = new_handler;
}

/*XXX */

#define C4X_CACHE       1
#define C4X_BASE_ST     (C4X_CACHE==1) ? 0x4800 : 0x4000 

void _CPU_Context_Initialize(
  Context_Control       *_the_context,
  void                  *_stack_base,
  unsigned32            _size,
  unsigned32            _isr,
  void  (*_entry_point)(void),
  int                   _is_fp
)
{
  unsigned int *_stack;
  _stack = (unsigned int *)_stack_base;

  *_stack = (unsigned int) _entry_point;
  _the_context->sp = (unsigned int) _stack;
  _the_context->st = C4X_BASE_ST;
  if ( _isr == 0 )
    _the_context->st |= C4X_ST_GIE;
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
 *  C4x Specific Information:
 *
 */

void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  proc_ptr ignored;
  extern void rtems_irq_prologue_0(void);
  extern void rtems_irq_prologue_1(void);
  void *entry;

  *old_handler = _ISR_Vector_table[ vector ];

  /*
   *  If the interrupt vector table is a table of pointer to isr entry
   *  points, then we need to install the appropriate RTEMS interrupt
   *  handler for this vector number.
   */

  entry = (void *)rtems_irq_prologue_0 +
    ((rtems_irq_prologue_1 - rtems_irq_prologue_0) * vector);
  _CPU_ISR_install_raw_handler( vector, entry, &ignored );

  /*
   *  We put the actual user ISR address in '_ISR_vector_table'.  This will
   *  be used by the _ISR_Handler so the user gets control.
   */

   _ISR_Vector_table[ vector ] = new_handler;
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
 *  C4x Specific Information:
 *
 *
 */

#if (CPU_PROVIDES_IDLE_THREAD_BODY == 1)
void _CPU_Thread_Idle_body( void )
{

  for( ; ; ) {
    __asm__( "idle" );
    __asm__( "nop" );
    __asm__( "nop" );
    __asm__( "nop" );
    /* insert your "halt" instruction here */ ;
  }
}
#endif
