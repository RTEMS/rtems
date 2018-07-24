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
#include <bsp/bootcard.h>
#include <bsp/fatal.h>
#include <leon.h>
#include <rtems/bspIo.h>
#include <rtems/score/smpimpl.h>
#include <stdlib.h>

#if !defined(__leon__) || defined(RTEMS_PARAVIRT)
uint32_t _CPU_SMP_Get_current_processor( void )
{
  return _LEON3_Get_current_processor();
}
#endif

static rtems_isr bsp_inter_processor_interrupt(
  rtems_vector_number vector
)
{
  _SMP_Inter_processor_interrupt_handler(_Per_CPU_Get());
}

void bsp_start_on_secondary_processor(Per_CPU_Control *cpu_self)
{
  uint32_t cpu_index_self;

  /*
   * If data cache snooping is not enabled we terminate using BSP_fatal_exit()
   * instead of bsp_fatal().  This is done since the latter function tries to
   * acquire a ticket lock, an operation which requires data cache snooping to
   * be enabled.
   */
  if ( !leon3_data_cache_snooping_enabled() )
    BSP_fatal_exit( LEON3_FATAL_INVALID_CACHE_CONFIG_SECONDARY_PROCESSOR );

  /* Unmask IPI interrupts at Interrupt controller for this CPU */
  cpu_index_self = _Per_CPU_Get_index(cpu_self);
  LEON3_IrqCtrl_Regs->mask[cpu_index_self] |= 1U << LEON3_mp_irq;

  _SMP_Start_multitasking_on_secondary_processor(cpu_self);
}

uint32_t _CPU_SMP_Initialize( void )
{
  if ( !leon3_data_cache_snooping_enabled() )
    bsp_fatal( LEON3_FATAL_INVALID_CACHE_CONFIG_MAIN_PROCESSOR );

  if ( rtems_configuration_get_maximum_processors() > 1 ) {
    LEON_Unmask_interrupt(LEON3_mp_irq);
    set_vector(bsp_inter_processor_interrupt, LEON_TRAP_TYPE(LEON3_mp_irq), 1);
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

void _CPU_SMP_Prepare_start_multitasking( void )
{
  rtems_cache_invalidate_entire_instruction();
}

void _CPU_SMP_Send_interrupt(uint32_t target_processor_index)
{
  /* send interrupt to destination CPU */
  LEON3_IrqCtrl_Regs->force[target_processor_index] = 1 << LEON3_mp_irq;
}
