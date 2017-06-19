/*
*  COPYRIGHT (c) 2012-2015
*  Cobham Gaisler
*
*  The license and distribution terms for this file may be
*  found in the file LICENSE in this distribution or at
*  http://www.rtems.org/license/LICENSE.
*
*/

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq-generic.h>

#if defined(RTEMS_SMP) && defined(LEON3)
/* Interrupt to CPU map. Default to CPU0 since in BSS. */
const unsigned char LEON3_irq_to_cpu[32] __attribute__((weak));

/* On SMP use map table above relative to SMP Boot CPU (normally CPU0) */
static inline int bsp_irq_cpu(int irq)
{
  /* protect from bad user configuration, default to boot cpu */
  if (rtems_configuration_get_maximum_processors() <= LEON3_irq_to_cpu[irq])
    return LEON3_Cpu_Index;
  else
    return LEON3_Cpu_Index + LEON3_irq_to_cpu[irq];
}
#else
/* when not SMP the local CPU is returned */
static inline int bsp_irq_cpu(int irq)
{
#ifdef LEON3
  return _LEON3_Get_current_processor();
#else
  return 0;
#endif
}
#endif

/* Initialize interrupts */
void BSP_shared_interrupt_init(void)
{
  rtems_vector_number vector;
  rtems_isr_entry previous_isr;
  int i;

  for (i=0; i <= BSP_INTERRUPT_VECTOR_MAX_STD; i++) {
#if defined(LEON3) && \
    (defined(RTEMS_SMP) || defined(RTEMS_MULTIPROCESSING))
    /* Don't install IRQ handler on IPI interrupt. An SMP kernel with max one
     * CPU does not use IPIs
     */
#ifdef RTEMS_SMP
    if (rtems_configuration_get_maximum_processors() > 1)
#endif
      if (i == LEON3_mp_irq)
        continue;
#endif
    vector = SPARC_ASYNCHRONOUS_TRAP(i) + 0x10;
    rtems_interrupt_catch(bsp_isr_handler, vector, &previous_isr);
  }

  /* Initalize interrupt support */
  bsp_interrupt_initialize();
}

/* Callback from bsp_interrupt_initialize() */
rtems_status_code bsp_interrupt_facility_initialize(void)
{
  return RTEMS_SUCCESSFUL;
}

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  int irq = (int)vector;
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  BSP_Cpu_Unmask_interrupt(irq, bsp_irq_cpu(irq));
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  int irq = (int)vector;
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  BSP_Cpu_Mask_interrupt(irq, bsp_irq_cpu(irq));
}

void BSP_shared_interrupt_mask(int irq)
{
  BSP_Cpu_Mask_interrupt(irq, bsp_irq_cpu(irq));
}

void BSP_shared_interrupt_unmask(int irq)
{
  BSP_Cpu_Unmask_interrupt(irq, bsp_irq_cpu(irq));
}

void BSP_shared_interrupt_clear(int irq)
{
  /* We don't have to interrupt lock here, because the register is only
   * written and self clearing
   */
  BSP_Clear_interrupt(irq);
}
