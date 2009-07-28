/*
 *  AVR CPU Dependent Source
 *
 *
 *  COPYRIGHT (c) 1989-2008.
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

#include <rtems/bspIo.h> /* XXX remove me later */

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS: NONE
 *
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */
void _CPU_Initialize(void)
{
  printk( "AVR CPU Initialize\n" );

  /*
   *  If there is not an easy way to initialize the FP context
   *  during Context_Initialize, then it is usually easier to
   *  save an "uninitialized" FP context here and copy it to
   *  the task's during Context_Initialize.
   */

  /* FP context initialization support goes here */
}

/*PAGE
 *
 *  _CPU_Context_Initialize
 *
 *  This kernel routine initializes the basic non-FP context area associated
 *  with each thread.
 *
 *  Input parameters:
 *    the_context  - pointer to the context area
 *    stack_base   - address of memory for the SPARC
 *    size         - size in bytes of the stack area
 *    new_level    - interrupt level for this context area
 *    entry_point  - the starting execution point for this this context
 *    is_fp        - TRUE if this context is associated with an FP thread
 *
 *  Output parameters: NONE
 */

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uint32_t         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp
)
{
  uint16_t stack;
  uint16_t start;
  uint8_t  *tmp_sp;
  uint8_t   start_low;
  uint8_t   start_high;

  /* calc stack high end */
  stack = (uint16_t) (stack_base) + (uint16_t) (size);
  /* calc stack pointer initial value */
  the_context->stack_pointer = (stack - 18);
  /* put the entry point on the stack */
  start = (uint16_t) entry_point;
  start_low = start & 0xff;
  start_high = start >> 8;
  tmp_sp = (uint8_t *) (stack+1);
  tmp_sp[0] = start_high;
  tmp_sp[1] = start_low;

  if (new_level) the_context->status = 0x00; //interrupts are enabled
  else           the_context->status = 0x80; //interrupts are disabled

  /*
   * JOEL: Why if I comment out these three lines does ticker not run?
   */
#if 1
  printk( "tmp_sp=0x%04x ", ((uint16_t)tmp_sp) );
  printk("the_context = 0x%x\n", the_context);
  printk("entry = 0x%x\n", entry_point);
#endif
#if 0
  printk("sp = 0x%x\n\n",stack);

 { int i; uint8_t *p;
   p = (uint8_t *)(stack - 18);
   for( i=0 ; i<=20 ; i+=8 ) {
     printk( "0x%04x ", ((uint16_t)&p[i]) );
     printk(
      "0x%02x 0x%02x 0x%02x 0x%02x   ",
      p[i + 0], p[i + 1], p[i + 2], p[i + 3]
     );
     printk(
      "0x%02x 0x%02x 0x%02x 0x%02x\n",
      p[i + 4], p[i + 5], p[i + 6], p[i + 7]
     );
   }
 }
#endif
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 *
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */
 
uint32_t   _CPU_ISR_Get_level( void )
{
  /*
   *  This routine returns the current interrupt level.
   */

  return 0;
}

/*PAGE
 *
 *  _CPU_ISR_install_raw_handler
 *
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */
 
void _CPU_ISR_install_raw_handler(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  /*
   *  This is where we install the interrupt handler into the "raw" interrupt
   *  table used by the CPU to dispatch interrupt handlers.
   */
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
  uint32_t    vector,
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
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
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
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */

void *_CPU_Thread_Idle_body( uintptr_t ignored )
{

  for( ; ; ) asm volatile ("sleep"::);
    /* insert your "halt" instruction here */ ;
  return (void *) 0;
}
