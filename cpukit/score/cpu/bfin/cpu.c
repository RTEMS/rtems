/*  Blackfin CPU Dependent Source
 *
 *  COPYRIGHT (c) 2006 by Atos Automacao Industrial Ltda.
 *             written by Alain Schaefer <alain.schaefer@easc.ch>
 *                    and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/bfin.h>
#include <rtems/bfin/bfin.h>

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


extern void _ISR15_Handler(void);
extern void _CPU_Emulation_handler(void);
extern void _CPU_Reset_handler(void);
extern void _CPU_NMI_handler(void);
extern void _CPU_Exception_handler(void);
extern void _CPU_Unhandled_Interrupt_handler(void);

void _CPU_Initialize(void)
{
  /*
   *  If there is not an easy way to initialize the FP context
   *  during Context_Initialize, then it is usually easier to
   *  save an "uninitialized" FP context here and copy it to
   *  the task's during Context_Initialize.
   */

  /* FP context initialization support goes here */



  proc_ptr ignored;

#if 0
  /* occassionally useful debug stuff */
  int i;
  _CPU_ISR_install_raw_handler(0, _CPU_Emulation_handler, &ignored);
  _CPU_ISR_install_raw_handler(1, _CPU_Reset_handler, &ignored);
  _CPU_ISR_install_raw_handler(2, _CPU_NMI_handler, &ignored);
  _CPU_ISR_install_raw_handler(3, _CPU_Exception_handler, &ignored);
  for (i = 5; i < 15; i++)
    _CPU_ISR_install_raw_handler(i, _CPU_Unhandled_Interrupt_handler, &ignored);
#endif

  /* install handler that will be used to call _Thread_Dispatch */
  _CPU_ISR_install_raw_handler( 15, _ISR15_Handler, &ignored );
  /* enable self nesting */
  __asm__ __volatile__ ("syscfg = %0" : : "d" (0x00000004));
}




/*
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

    register uint32_t   _tmpimask;

    /*read from the IMASK registers*/

    _tmpimask = *((uint32_t*)IMASK);

    return (_tmpimask & 0xffe0) ? 0 : 1;
}

/*
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
   proc_ptr *interrupt_table = NULL;
  /*
   *  This is where we install the interrupt handler into the "raw" interrupt
   *  table used by the CPU to dispatch interrupt handlers.
   */

   /* base of vector table on blackfin architecture */
   interrupt_table = (void*)0xFFE02000;

   *old_handler = interrupt_table[ vector ];
   interrupt_table[ vector ] = new_handler;

}

/*
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
   proc_ptr ignored;

   *old_handler = _ISR_Vector_table[ vector ];

   /*
    *  We put the actual user ISR address in '_ISR_vector_table'.  This will
    *  be used by the _ISR_Handler so the user gets control.
    */

    _ISR_Vector_table[ vector ] = new_handler;

    _CPU_ISR_install_raw_handler( vector, _ISR_Handler, &ignored );
}

#if (CPU_PROVIDES_IDLE_THREAD_BODY == TRUE)
void *_CPU_Thread_Idle_body(uint32_t ignored)
{
  while (1) {
    __asm__ __volatile__("ssync; idle; ssync");
  }
}
#endif

/*
 * Copied from the arm port.
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
    uint32_t     stack_high;  /* highest "stack aligned" address */
    stack_high = ((uint32_t)(stack_base) + size);

    /* blackfin abi requires caller to reserve 12 bytes on stack */
    the_context->register_sp = stack_high - 12;
    the_context->register_rets = (uint32_t) entry_point;
    the_context->imask = new_level ? 0 : 0xffff;
}



/*
 *  _CPU_Install_interrupt_stack
 *
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */

void _CPU_Install_interrupt_stack( void )
{
}
