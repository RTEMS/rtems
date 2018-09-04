/**
 * @file
 *
 * @ingroup mpc83xx
 *
 * @brief Source for BSP startup code.
 */

/*
 * Copyright (c) 2008-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/counter.h>

#include <libchip/ns16550.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/vectors.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>
#include <bsp/u-boot.h>
#include <bsp/console-termios.h>

/* Configuration parameters for console driver, ... */
unsigned int BSP_bus_frequency;

/* Configuration parameter for clock driver */
uint32_t bsp_time_base_frequency;

/* Legacy */
uint32_t bsp_clicks_per_usec;

/* Default decrementer exception handler */
static int mpc83xx_decrementer_exception_handler( BSP_Exception_frame *frame, unsigned number)
{
  ppc_set_decrementer_register(UINT32_MAX);

  return 0;
}

uint32_t _CPU_Counter_frequency(void)
{
  return bsp_time_base_frequency;
}

void bsp_start( void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  unsigned long i = 0;

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type() function
   * store the result in global variables so that it can be used latter...
   */
  get_ppc_cpu_type();
  get_ppc_cpu_revision();

  /* Basic CPU initialization */
  cpu_init();

  /*
   * Enable instruction and data caches. Do not force writethrough mode.
   */

#ifdef BSP_INSTRUCTION_CACHE_ENABLED
  rtems_cache_enable_instruction();
#endif

#ifdef BSP_DATA_CACHE_ENABLED
  rtems_cache_enable_data();
#endif

  /*
   * This is evaluated during runtime, so it should be ok to set it
   * before we initialize the drivers.
   */

  /* Initialize some device driver parameters */

#ifdef HAS_UBOOT
  BSP_bus_frequency = bsp_uboot_board_info.bi_busfreq;
#else /* HAS_UBOOT */
  BSP_bus_frequency = BSP_CLKIN_FRQ * BSP_SYSPLL_MF / BSP_SYSPLL_CKID;
#endif /* HAS_UBOOT */
  bsp_time_base_frequency = BSP_bus_frequency / 4;
  bsp_clicks_per_usec = bsp_time_base_frequency / 1000000;

  /* Initialize some console parameters */
  for (i = 0; i < console_device_count; ++i) {
    ns16550_context *ctx = (ns16550_context *) console_device_table[i].context;

    ctx->clock = BSP_bus_frequency;

    #ifdef HAS_UBOOT
      ctx->initial_baud = bsp_uboot_board_info.bi_baudrate;
    #endif
  }

  /* Initialize exception handler */
#ifndef BSP_DATA_CACHE_ENABLED
  ppc_exc_cache_wb_check = 0;
#endif
  ppc_exc_initialize();

  /* Install default handler for the decrementer exception */
  sc = ppc_exc_set_handler( ASM_DEC_VECTOR, mpc83xx_decrementer_exception_handler);
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_panic("cannot install decrementer exception handler");
  }

  /* Initalize interrupt support */
  bsp_interrupt_initialize();

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Exit from bspstart\n");
#endif
}
