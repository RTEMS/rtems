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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
  bsp_spurious_initialize();

  /* Initialize shared interrupt handling, must be done after IRQ
   * controller has been found and initialized.
   */
  BSP_shared_interrupt_init();

#ifdef RTEMS_DRVMGR_STARTUP
  leon2_root_register(&leon2_bus_config, &leon2_amba_res);
#endif
}

RTEMS_SYSINIT_ITEM(
  leon2_pre_driver_hook,
  RTEMS_SYSINIT_BSP_PRE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
