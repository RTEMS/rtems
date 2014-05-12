/**
 * @file
 * @ingroup sparc_leon3
 * @brief LEON3 SMP BSP Support
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <leon.h>
#include <rtems/bspIo.h>
#include <rtems/score/smpimpl.h>
#include <stdlib.h>

#if !defined(__leon__)
uint32_t _CPU_SMP_Get_current_processor( void )
{
  return _LEON3_Get_current_processor();
}
#endif

static rtems_isr bsp_inter_processor_interrupt(
  rtems_vector_number vector
)
{
  _SMP_Inter_processor_interrupt_handler();
}

void leon3_secondary_cpu_initialize(uint32_t cpu_index)
{
  leon3_set_cache_control_register(0x80000F);
  /* Unmask IPI interrupts at Interrupt controller for this CPU */
  LEON3_IrqCtrl_Regs->mask[cpu_index] |= 1U << LEON3_MP_IRQ;

  _SMP_Start_multitasking_on_secondary_processor();
}

uint32_t _CPU_SMP_Initialize( void )
{
  leon3_set_cache_control_register(0x80000F);

  if ( rtems_configuration_get_maximum_processors() > 1 ) {
    LEON_Unmask_interrupt(LEON3_MP_IRQ);
    set_vector(bsp_inter_processor_interrupt, LEON_TRAP_TYPE(LEON3_MP_IRQ), 1);
  }

  return leon3_get_cpu_count(LEON3_IrqCtrl_Regs);
}

bool _CPU_SMP_Start_processor( uint32_t cpu_index )
{
  #if defined(RTEMS_DEBUG)
    printk( "Waking CPU %d\n", cpu_index );
  #endif

  LEON3_IrqCtrl_Regs->mpstat = 1U << cpu_index;

  return true;
}

void _CPU_SMP_Finalize_initialization( uint32_t cpu_count )
{
  (void) cpu_count;

  /* Nothing to do */
}

void _CPU_SMP_Send_interrupt(uint32_t target_processor_index)
{
  /* send interrupt to destination CPU */
  LEON3_IrqCtrl_Regs->force[target_processor_index] = 1 << LEON3_MP_IRQ;
}
