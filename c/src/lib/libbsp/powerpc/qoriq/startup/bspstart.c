/**
 * @file
 *
 * @ingroup QorIQ
 *
 * @brief BSP startup.
 */

/*
 * Copyright (c) 2010-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/config.h>
#include <rtems/counter.h>

#include <libchip/ns16550.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/vectors.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <bsp/u-boot.h>
#include <bsp/linker-symbols.h>
#include <bsp/mmu.h>
#include <bsp/qoriq.h>
#include <bsp/console-termios.h>

LINKER_SYMBOL(bsp_exc_vector_base);

/* Configuration parameters for console driver, ... */
unsigned int BSP_bus_frequency;

/* Configuration parameter for clock driver, ... */
uint32_t bsp_clicks_per_usec;

void BSP_panic(char *s)
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  (void) level;

  printk("%s PANIC %s\n", rtems_get_version_string(), s);

  while (1) {
    /* Do nothing */
  }
}

void _BSP_Fatal_error(unsigned n)
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  (void) level;

  printk("%s PANIC ERROR %u\n", rtems_get_version_string(), n);

  while (1) {
    /* Do nothing */
  }
}

void bsp_start(void)
{
  unsigned long i = 0;

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type() function
   * store the result in global variables so that it can be used latter...
   */
  get_ppc_cpu_type();
  get_ppc_cpu_revision();

  /* Initialize some device driver parameters */
  #ifdef HAS_UBOOT
    BSP_bus_frequency = bsp_uboot_board_info.bi_busfreq
      / QORIQ_BUS_CLOCK_DIVIDER;
    bsp_clicks_per_usec = BSP_bus_frequency / 8000000;
    rtems_counter_initialize_converter(
      #ifdef __PPC_CPU_E6500__
        bsp_uboot_board_info.bi_intfreq
      #else
        BSP_bus_frequency / 8
      #endif
    );
  #endif /* HAS_UBOOT */

  /* Initialize some console parameters */
  for (i = 0; i < console_device_count; ++i) {
    const console_device *dev = &console_device_table[i];
    const rtems_termios_device_handler *ns16550 =
      #ifdef BSP_USE_UART_INTERRUPTS
        &ns16550_handler_interrupt;
      #else
        &ns16550_handler_polled;
      #endif

    if (dev->handler == ns16550) {
      ns16550_context *ctx = (ns16550_context *) dev->context;

      ctx->clock = BSP_bus_frequency;

      #ifdef HAS_UBOOT
        #ifdef U_BOOT_GENERIC_BOARD_INFO
          ctx->initial_baud = 115200;
        #else
          ctx->initial_baud = bsp_uboot_board_info.bi_baudrate;
        #endif
      #endif
    }
  }

  /* Initialize exception handler */
  ppc_exc_initialize_with_vector_base(
    (uintptr_t) bsp_section_work_begin,
    rtems_configuration_get_interrupt_stack_size(),
    bsp_exc_vector_base
  );

  /* Now it is possible to make the code execute only */
  qoriq_mmu_change_perm(
    FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SX,
    FSL_EIS_MAS3_SX,
    FSL_EIS_MAS3_SR
  );

  /* Initalize interrupt support */
  bsp_interrupt_initialize();

  /* Disable boot page translation */
#if QORIQ_CHIP_IS_T_VARIANT(QORIQ_CHIP_VARIANT)
  qoriq.lcc.bstar &= ~LCC_BSTAR_EN;
#else
  qoriq.lcc.bptr &= ~BPTR_EN;
#endif
}
