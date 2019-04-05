/*
 *  GRLIB/LEON3 extended interrupt controller
 *
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#include <leon.h>
#include <bsp/irq.h>

/* GRLIB extended IRQ controller IRQ number */
int LEON3_IrqCtrl_EIrq = -1;

/* Initialize Extended Interrupt controller */
void leon3_ext_irq_init(void)
{
  if ( (LEON3_IrqCtrl_Regs->mpstat >> 16) & 0xf ) {
    /* Extended IRQ controller available */
    LEON3_IrqCtrl_EIrq = (LEON3_IrqCtrl_Regs->mpstat >> 16) & 0xf;
  }
}

void bsp_interrupt_set_affinity(
  rtems_vector_number vector,
  const Processor_mask *affinity
)
{
  uint32_t unmasked = 0;
  uint32_t cpu_count = rtems_scheduler_get_processor_maximum();
  uint32_t cpu_index;

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    if (_Processor_mask_Is_set(affinity, cpu_index)) {
      BSP_Cpu_Unmask_interrupt(vector, cpu_index);
      ++unmasked;
    }
  }

  if (unmasked > 1) {
    LEON_Enable_interrupt_broadcast(vector);
  } else {
    LEON_Disable_interrupt_broadcast(vector);
  }
}

void bsp_interrupt_get_affinity(
  rtems_vector_number vector,
  Processor_mask *affinity
)
{
  uint32_t cpu_count = rtems_scheduler_get_processor_maximum();
  uint32_t cpu_index;

  _Processor_mask_Zero(affinity);

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    if (!BSP_Cpu_Is_interrupt_masked(vector, cpu_index)) {
      _Processor_mask_Set(affinity, cpu_index);
    }
  }
}
