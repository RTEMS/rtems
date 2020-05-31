
#include <rtems/score/smpimpl.h>

#include <bsp/apic.h>
#include <bsp/smp-imps.h>
#include <bsp.h>
#include <rtems.h>

void _CPU_SMP_Prepare_start_multitasking( void )
{
  /* Do nothing */
}

bool _CPU_SMP_Start_processor( uint32_t cpu_index )
{
  (void) cpu_index;

  return true;
}


uint32_t _CPU_SMP_Get_current_processor( void )
{
  return imps_apic_cpu_map[APIC_ID(IMPS_LAPIC_READ(LAPIC_ID))];
}

uint32_t _CPU_SMP_Initialize( void )
{
  /* XXX need to deal with finding too many cores */

  return (uint32_t) imps_probe();
}

void _CPU_SMP_Finalize_initialization( uint32_t cpu_count )
{
  if ( cpu_count > 1 )
    ipi_install_irq();
}

void _CPU_SMP_Send_interrupt( uint32_t target_processor_index )
{
  send_ipi( target_processor_index, 0x30 );
}
