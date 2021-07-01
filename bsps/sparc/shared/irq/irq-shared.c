/*
*  COPYRIGHT (c) 2012-2015
*  Cobham Gaisler
*
*  The license and distribution terms for this file may be
*  found in the file LICENSE in this distribution or at
*  http://www.rtems.org/license/LICENSE.
*
*/

#include <bsp.h>
#include <bsp/irq-generic.h>

static inline int bsp_irq_cpu(int irq)
{
#if defined(RTEMS_SMP)
  Processor_mask affinity;

  (void) bsp_interrupt_get_affinity((rtems_vector_number) irq, &affinity);
  return (int) _Processor_mask_Find_last_set(&affinity);
#elif defined(LEON3)
  return _LEON3_Get_current_processor();
#else
  return 0;
#endif
}

#if !defined(LEON3)
bool bsp_interrupt_is_valid_vector(rtems_vector_number vector)
{
  if (vector == 0) {
    return false;
  }

  return vector <= BSP_INTERRUPT_VECTOR_MAX_STD;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  /* Nothing to do */
  return RTEMS_SUCCESSFUL;
}

static bool is_maskable(rtems_vector_number vector)
{
  return vector != 15;
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  attributes->is_maskable = is_maskable(vector);
  attributes->can_enable = true;
  attributes->maybe_enable = true;
  attributes->can_disable = is_maskable(vector);
  attributes->maybe_disable = is_maskable(vector);
  attributes->can_raise = true;
  attributes->can_raise_on = true;
  attributes->can_clear = true;
  attributes->cleared_by_acknowledge = true;
  attributes->can_set_affinity = true;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);
  *pending = BSP_Is_interrupt_pending(vector) ||
    BSP_Is_interrupt_forced(vector);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  BSP_Force_interrupt(vector);
  return RTEMS_SUCCESSFUL;
}

#if defined(RTEMS_SMP)
rtems_status_code bsp_interrupt_raise_on(
  rtems_vector_number vector,
  uint32_t            cpu_index
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  BSP_Force_interrupt(vector);
  return RTEMS_SUCCESSFUL;
}
#endif

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  BSP_Clear_forced_interrupt(vector);
  BSP_Clear_interrupt(vector);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);
  *enabled = !BSP_Cpu_Is_interrupt_masked(vector, bsp_irq_cpu(vector));
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  BSP_Cpu_Unmask_interrupt(vector, 0);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (!is_maskable(vector)) {
    return RTEMS_UNSATISFIED;
  }

  BSP_Cpu_Mask_interrupt(vector, 0);
  return RTEMS_SUCCESSFUL;
}
#endif

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
