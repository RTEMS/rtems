/*
 *  SPARC Dependent Source
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
 */


void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{
  void *pointer;

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

  pointer = &_CPU_Null_fp_context;
  _CPU_Context_save_fp( &pointer );

  _CPU_Table = *cpu_table;
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 */
 
unsigned32 _CPU_ISR_Get_level( void )
{
  unsigned32 level;
 
  sparc_get_interrupt_level( level );
 
  return level;
}

/*  _CPU_ISR_install_vector
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

   /*
    *  We put the actual user ISR address in '_ISR_vector_table'.  This will
    *  be used by the _ISR_Handler so the user gets control.
    */

    _ISR_Vector_table[ vector ] = new_handler;
}

/*PAGE
 *
 *  _CPU_Install_interrupt_stack
 */

void _CPU_Install_interrupt_stack( void )
{
}

/*PAGE
 *
 *  _CPU_Context_Initialize
 */

/*
 *  The following constants assist in building a thread's initial context.
 */

#define CPU_FRAME_SIZE  (112)   /* based on disassembled test code */
#define ADDR_ADJ_OFFSET  -8

void _CPU_Context_Initialize(
  Context_Control  *_the_context,
  unsigned32       *_stack_base,
  unsigned32        _size,
  unsigned32        _new_level,
  void             *_entry_point
)
{
    unsigned32   jmp_addr;
    unsigned32   _stack_high;  /* highest "stack aligned" address */
    unsigned32   _the_size;
    unsigned32   tmp_psr;
 
    jmp_addr = (unsigned32) _entry_point;
 
    /*
     *  On CPUs with stacks which grow down (i.e. SPARC), we build the stack
     *  based on the _stack_high address.  
     */
 
    _stack_high = ((unsigned32)(_stack_base) + _size);
    _stack_high &= ~(CPU_STACK_ALIGNMENT - 1);
 
    _the_size = _size & ~(CPU_STACK_ALIGNMENT - 1);
 
/* XXX following code is based on unix port */
    /*
     *  XXX SPARC port needs a diagram like this one...
     *  See /usr/include/sys/stack.h in Solaris 2.3 for a nice
     *  diagram of the stack.
     */
 
    _the_context->o7 = jmp_addr + ADDR_ADJ_OFFSET;
    _the_context->o6 = (unsigned32)(_stack_high - CPU_FRAME_SIZE);
    _the_context->i6 = (unsigned32)(_stack_high);
#if 0
    _the_context->rp = jmp_addr + ADDR_ADJ_OFFSET;
    _the_context->sp = (unsigned32)(_stack_high - CPU_FRAME_SIZE);
    _the_context->fp = (unsigned32)(_stack_high);
#endif

    _the_context->wim = 0x01;

    sparc_get_psr( tmp_psr );
    tmp_psr &= ~SPARC_PIL_MASK;
    tmp_psr |= (((_new_level) << 8) & SPARC_PIL_MASK);
    tmp_psr  = (tmp_psr & ~0x07) | 0x07;  /* XXX should use num windows */
    _the_context->psr = tmp_psr;
}

/*PAGE
 *
 *  _CPU_Internal_threads_Idle_thread_body
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
 */

void _CPU_Internal_threads_Idle_thread_body( void )
{

  for( ; ; )
    /* insert your "halt" instruction here */ ;
}
