/**
 * @file
 *
 * @ingroup bsp_interrupt
 * @ingroup arm_beagle
 *
 * @brief Interrupt support.
 */

/*
 * Copyright (c) 2014 Ben Gras <beng@shrike-systems.com>. All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>
#include <bsp/fatal.h>

#include <rtems/score/armv4.h>

#include <libcpu/arm-cp15.h>

struct omap_intr
{
  uint32_t base;
  int size;
};

#if IS_DM3730
static struct omap_intr omap_intr = {
  .base = OMAP3_DM37XX_INTR_BASE,
  .size = 0x1000,
};
#endif

#if IS_AM335X
static struct omap_intr omap_intr = {
  .base = OMAP3_AM335X_INTR_BASE,
  .size = 0x1000,
};
#endif

/* Enables interrupts at the Interrupt Controller side. */
static inline void omap_irq_ack(void)
{
  mmio_write(omap_intr.base + OMAP3_INTCPS_CONTROL, OMAP3_INTR_NEWIRQAGR);

  /* Flush data cache to make sure all the previous writes are done
     before re-enabling interrupts. */
  flush_data_cache();
}

void bsp_interrupt_dispatch(void)
{
  const uint32_t reg = mmio_read(omap_intr.base + OMAP3_INTCPS_SIR_IRQ);

  if ((reg & OMAP3_INTR_SPURIOUSIRQ_MASK) != OMAP3_INTR_SPURIOUSIRQ_MASK) {
    const rtems_vector_number irq = reg & OMAP3_INTR_ACTIVEIRQ_MASK;

    bsp_interrupt_handler_dispatch(irq);
  } else {
    /* Ignore spurious interrupts. We'll still ACK it so new interrupts
       can be generated. */
  }

  omap_irq_ack();
}

/* There are 4 32-bit interrupt mask registers for a total of 128 interrupts.
   The IRQ number tells us which register to use. */
static uint32_t omap_get_mir_reg(rtems_vector_number vector, uint32_t *const mask)
{
  uint32_t mir_reg;

  /* Select which bit to set/clear in the MIR register. */
  *mask = 1ul << (vector % 32u);

  if (vector < 32u) {
    mir_reg = OMAP3_INTCPS_MIR0;
  } else if (vector < 64u) {
    mir_reg = OMAP3_INTCPS_MIR1;
  } else if (vector < 96u) {
    mir_reg = OMAP3_INTCPS_MIR2;
  } else if (vector < 128u) {
    mir_reg = OMAP3_INTCPS_MIR3;
  } else {
    /* Invalid IRQ number. This should never happen. */
    bsp_fatal(0);
  }

  return mir_reg;
}

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  uint32_t mask, cur;
  uint32_t mir_reg = omap_get_mir_reg(vector, &mask);

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  cur = mmio_read(omap_intr.base + mir_reg);
  mmio_write(omap_intr.base + mir_reg, cur & ~mask);
  flush_data_cache();
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  uint32_t mask, cur;
  uint32_t mir_reg = omap_get_mir_reg(vector, &mask);

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  cur = mmio_read(omap_intr.base + mir_reg);
  mmio_write(omap_intr.base + mir_reg, cur | mask);
  flush_data_cache();
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  int i;
  uint32_t intc_ilrx;

  /* AM335X TRM 6.2.1 Initialization Sequence */
  mmio_write(omap_intr.base + OMAP3_INTCPS_SYSCONFIG, OMAP3_SYSCONFIG_AUTOIDLE);
  mmio_write(omap_intr.base + OMAP3_INTCPS_IDLE, 0);
  /* priority 0 to all IRQs */
  for(intc_ilrx = 0x100; intc_ilrx <= 0x2fc; intc_ilrx += 4) {
    mmio_write(omap_intr.base + intc_ilrx, 0);
  }

  /* Mask all interrupts */
  for(i = BSP_INTERRUPT_VECTOR_MIN; i <= BSP_INTERRUPT_VECTOR_MAX; i++)
    bsp_interrupt_vector_disable(i);

  /* Install generic interrupt handler */
  arm_cp15_set_exception_handler(ARM_EXCEPTION_IRQ, _ARMV4_Exception_interrupt);
  arm_cp15_set_vector_base_address(bsp_vector_table_begin);

  return RTEMS_SUCCESSFUL;
}
