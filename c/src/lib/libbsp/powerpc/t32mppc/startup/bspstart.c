/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems/config.h>

#include <bsp.h>
#include <bsp/vectors.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>

LINKER_SYMBOL(bsp_exc_vector_base);

/*
 * Configuration parameter for clock driver.  The Trace32 PowerPC simulator has
 * an odd decrementer frequency.  The time base frequency is one tick per
 * instruction.  The decrementer frequency is one tick per ten instructions.
 * The clock driver assumes that the time base and decrementer frequencies are
 * equal.  For now we simulate processor that issues 10000000 instructions per
 * second.
 */
uint32_t bsp_time_base_frequency = 10000000 / 10;

void BSP_panic(char *s)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  printk("%s PANIC %s\n", rtems_get_version_string(), s);

  while (1) {
    /* Do nothing */
  }
}

void _BSP_Fatal_error(unsigned n)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  printk("%s PANIC ERROR %u\n", rtems_get_version_string(), n);

  while (1) {
    /* Do nothing */
  }
}

void bsp_start(void)
{
  rtems_status_code sc;

  get_ppc_cpu_type();
  get_ppc_cpu_revision();

  /* Initialize exception handler */
  ppc_exc_vector_base = (uint32_t) bsp_exc_vector_base;
  sc = ppc_exc_initialize(
    PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
    (uintptr_t) bsp_section_work_begin,
    Configuration.interrupt_stack_size
  );
  if (sc != RTEMS_SUCCESSFUL) {
    BSP_panic("cannot initialize exceptions");
  }

  /* Initalize interrupt support */
  sc = bsp_interrupt_initialize();
  if (sc != RTEMS_SUCCESSFUL) {
    BSP_panic("cannot initialize interrupts\n");
  }
}
