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
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <leon.h>
#include <rtems/bspIo.h>
#include <rtems/bspsmp.h>
#include <rtems/score/smpimpl.h>
#include <stdlib.h>

static inline void sparc_leon3_set_cctrl( unsigned int val )
{
  __asm__ volatile( "sta %0, [%%g0] 2" : : "r" (val) );
}

static inline unsigned int sparc_leon3_get_cctrl( void )
{
  unsigned int v = 0;
  __asm__ volatile( "lda [%%g0] 2, %0" : "=r" (v) : "0" (v) );
  return v;
}

static rtems_isr bsp_ap_ipi_isr(
  rtems_vector_number vector
)
{
  rtems_smp_process_interrupt();
}

void leon3_secondary_cpu_initialize(uint32_t cpu)
{
  sparc_leon3_set_cctrl( 0x80000F );
  LEON_Unmask_interrupt(LEON3_MP_IRQ);
  LEON3_IrqCtrl_Regs->mask[cpu] |= 1 << LEON3_MP_IRQ;

  _SMP_Start_multitasking_on_secondary_processor();
}

uint32_t _CPU_SMP_Initialize( uint32_t configured_cpu_count )
{
  uint32_t max_cpu_count;
  uint32_t used_cpu_count;
  uint32_t cpu;

  sparc_leon3_set_cctrl( 0x80000F );

  max_cpu_count =
    ((LEON3_IrqCtrl_Regs->mpstat >> LEON3_IRQMPSTATUS_CPUNR) & 0xf)  + 1;
  used_cpu_count = configured_cpu_count < max_cpu_count ?
    configured_cpu_count : max_cpu_count;

  #if defined(RTEMS_DEBUG)
    printk( "Found %d CPUs\n", max_cpu_count );

    if ( max_cpu_count > configured_cpu_count ) {
      printk(
        "%d CPUs IS MORE THAN CONFIGURED -- ONLY USING %d\n",
        max_cpu_count,
        configured_cpu_count
      );
    }
  #endif

  if ( used_cpu_count > 1 ) {
    LEON_Unmask_interrupt(LEON3_MP_IRQ);
    set_vector(bsp_ap_ipi_isr, LEON_TRAP_TYPE(LEON3_MP_IRQ), 1);
  }

  for ( cpu = 1 ; cpu < used_cpu_count ; ++cpu ) {
    #if defined(RTEMS_DEBUG)
      printk( "Waking CPU %d\n", cpu );
    #endif

    LEON3_IrqCtrl_Regs->mpstat = 1 << cpu;
  }

  return used_cpu_count;
}

void _CPU_SMP_Send_interrupt(uint32_t target_processor_index)
{
  /* send interrupt to destination CPU */
  LEON3_IrqCtrl_Regs->force[target_processor_index] = 1 << LEON3_MP_IRQ;
}
