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
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/config.h>

#include <libchip/serial.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/vectors.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <bsp/u-boot.h>
#include <bsp/linker-symbols.h>
#include <bsp/mmu.h>
#include <bsp/qoriq.h>

LINKER_SYMBOL(bsp_exc_vector_base);

/* Configuration parameters for console driver, ... */
unsigned int BSP_bus_frequency;

/* Configuration parameter for clock driver, ... */
uint32_t bsp_clicks_per_usec;

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
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  unsigned long i = 0;

  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type() function
   * store the result in global variables so that it can be used latter...
   */
  myCpu = get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();

  /* Initialize some device driver parameters */
  #ifdef HAS_UBOOT
    BSP_bus_frequency = bsp_uboot_board_info.bi_busfreq;
    bsp_clicks_per_usec = bsp_uboot_board_info.bi_busfreq / 8000000;
  #endif /* HAS_UBOOT */

  /* Initialize some console parameters */
  for (i = 0; i < Console_Configuration_Count; ++i) {
    console_tbl *ct = &Console_Configuration_Ports[i];

    ct->ulClock = BSP_bus_frequency;

    #ifdef HAS_UBOOT
      if (ct->deviceType == SERIAL_NS16550) {
        ct->pDeviceParams = (void *) bsp_uboot_board_info.bi_baudrate;
      }
    #endif
  }

  /* Disable decrementer */
  PPC_CLEAR_SPECIAL_PURPOSE_REGISTER_BITS(BOOKE_TCR, BOOKE_TCR_DIE);

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

  /* Now it is possible to make the code execute only */
  qoriq_mmu_change_perm(
    FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SX,
    FSL_EIS_MAS3_SX,
    FSL_EIS_MAS3_SR
  );

  /* Initalize interrupt support */
  sc = bsp_interrupt_initialize();
  if (sc != RTEMS_SUCCESSFUL) {
    BSP_panic("cannot intitialize interrupts\n");
  }

  /* Disable boot page translation */
  qoriq.lcc.bptr &= ~BPTR_EN;
}
