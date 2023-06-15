/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup RTEMSBSPsSPARCLEON2
 * @brief LEON2 Cache Snooping Support
 */

/*
 *  LEON2 Cache Snooping Support
 *
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <rtems/sysinit.h>

/*
 * Tells us if data cache snooping is available
 */
int CPU_SPARC_HAS_SNOOPING;

/*
 * set_snooping
 *
 * Read the data cache configuration register to determine if
 * bus snooping is available. This is needed for some drivers so
 * that they can select the most efficient copy routines.
 */
static inline int set_snooping(void)
{
  unsigned int tmp = *(unsigned int *)0x80000014; /* Cache control register */
  return ((tmp>>23) & 1); /* Data cache snooping enabled */
}

void bsp_start( void )
{
  CPU_SPARC_HAS_SNOOPING = set_snooping();
}

/* If RTEMS_DRVMGR_STARTUP is defined extra code is added that
 * registers the LEON2 AMBA bus driver as root driver into the
 * driver manager.
 *
 * The structues here are declared weak so that the user can override
 * the configuration and add custom cores in the RTEMS project.
 */
#ifdef RTEMS_DRVMGR_STARTUP
#include <drvmgr/drvmgr.h>
#include <drvmgr/leon2_amba_bus.h>

/* All drivers included by BSP, this is overridden by the user by including
 * the devmgr_confdefs.h. No specifc drivers needed by BSP since IRQ/TIMER/UART
 * is not drvmgr drivers.
 */
drvmgr_drv_reg_func drvmgr_drivers[] __attribute__((weak)) =
{
  NULL /* End array with NULL */
};

/* Defines what cores are avilable on the bus in addition to the standard
 * LEON2 peripherals.
 */
struct leon2_core leon2_amba_custom_cores[] __attribute__((weak)) =
{
  EMPTY_LEON2_CORE
};

/* Configure LEON2 Root bus driver */
struct leon2_bus leon2_bus_config __attribute__((weak)) =
{
  &leon2_std_cores[0], /* The standard cores, defined by driver */
  &leon2_amba_custom_cores[0],   /* custom cores, defined by us */
  DRVMGR_TRANSLATE_ONE2ONE,
  DRVMGR_TRANSLATE_ONE2ONE,
};

/* Driver resources on LEON2 AMBA bus. Used to set options for particular
 * LEON2 cores, it is up to the driver to look at the configuration paramters
 * once started.
 */
struct drvmgr_bus_res leon2_amba_res __attribute__((weak)) =
{
  .next = NULL,
  .resource = {
    DRVMGR_RES_EMPTY
  },
};
#endif /* RTEMS_DRVMGR_STARTUP */

/*
 * Called just before drivers are initialized.  Is used to initialize shared
 * interrupt handling.
 */
static void leon2_pre_driver_hook( void )
{
  /* Initialize shared interrupt handling, must be done after IRQ
   * controller has been found and initialized.
   */
  bsp_interrupt_initialize();

#ifdef RTEMS_DRVMGR_STARTUP
  leon2_root_register(&leon2_bus_config, &leon2_amba_res);
#endif
}

RTEMS_SYSINIT_ITEM(
  leon2_pre_driver_hook,
  RTEMS_SYSINIT_BSP_PRE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
