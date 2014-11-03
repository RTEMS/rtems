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
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>

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

static int irqs_enabled[BSP_INTERRUPT_VECTOR_MAX+1];

volatile static int level = 0;

void bsp_interrupt_dispatch(void)
{
  /* get irq */
  uint32_t reg = mmio_read(omap_intr.base + OMAP3_INTCPS_SIR_IRQ);
  int irq;
  irq = reg & OMAP3_INTR_ACTIVEIRQ_MASK;

  if(!irqs_enabled[irq]) {
	/* Ignore spurious interrupt */
  } else {
    bsp_interrupt_vector_disable(irq);

    /* enable new interrupts, and flush data cache to make sure
     * it hits the intc
     */
    mmio_write(omap_intr.base + OMAP3_INTCPS_CONTROL, OMAP3_INTR_NEWIRQAGR);
    flush_data_cache();
    mmio_read(omap_intr.base + OMAP3_INTCPS_SIR_IRQ);
    flush_data_cache();

    /* keep current irq masked but enable unmasked ones */
    uint32_t psr = _ARMV4_Status_irq_enable();
    bsp_interrupt_handler_dispatch(irq);

    _ARMV4_Status_restore(psr);

    bsp_interrupt_vector_enable(irq);
  }
}

static uint32_t get_mir_reg(int vector, uint32_t *mask)
{
  *mask = 1UL << (vector % 32);

  if(vector <   0) while(1) ;
  if(vector <  32) return OMAP3_INTCPS_MIR0;
  if(vector <  64) return OMAP3_INTCPS_MIR1;
  if(vector <  96) return OMAP3_INTCPS_MIR2;
  if(vector < 128) return OMAP3_INTCPS_MIR3;
  while(1) ;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  uint32_t mask, cur;
  uint32_t mir_reg = get_mir_reg(vector, &mask);

  cur = mmio_read(omap_intr.base + mir_reg);
  mmio_write(omap_intr.base + mir_reg, cur & ~mask);
  flush_data_cache();

  irqs_enabled[vector] = 1;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  uint32_t mask, cur;
  uint32_t mir_reg = get_mir_reg(vector, &mask);

  cur = mmio_read(omap_intr.base + mir_reg);
  mmio_write(omap_intr.base + mir_reg, cur | mask);
  flush_data_cache();

  irqs_enabled[vector] = 0;

  return RTEMS_SUCCESSFUL;
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
  arm_cp15_set_vector_base_address((uint32_t) bsp_vector_table_begin);

  return RTEMS_SUCCESSFUL;
}
