
/*
 *  Find and initialize irqmp and gptimer.
 */

/*
 * Copyright (C) 2019 Jiri Gaisler <jiri@gaisler.se>
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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
 */

#include <bsp.h>
#include <bsp/fatal.h>
#include <bsp/irqimpl.h>
#include <bsp/rvintctrl.h>
#include <amba.h>

#include <rtems/sysinit.h>

struct ambapp_dev *GRLIB_IrqCtrl_Adev;

volatile RISCV_CLINT_regs *noel_clint;

volatile RISCV_PLIC_regs *noel_plic;

void noel_irq_initialize( void )
{
#ifndef RISCV_USE_S_MODE
  struct ambapp_dev      *adev;
  struct ambapp_ahb_info *ahb;

  adev = (void *) (uintptr_t) ambapp_for_each(
    ambapp_plb(),
    ( OPTIONS_ALL | OPTIONS_AHB_SLVS ),
    VENDOR_GAISLER,
    GAISLER_RVINTCTRL,
    ambapp_find_by_idx,
    NULL
  );

  /* No RVINTCTRL core, check for individual CLINT and PLIC */
  if ( adev == NULL ) {
    /* First try for ACLINT */
    adev = (void *) (uintptr_t) ambapp_for_each(
      ambapp_plb(),
      ( OPTIONS_ALL | OPTIONS_AHB_SLVS ),
      VENDOR_GAISLER,
      GAISLER_ACLINT,
      ambapp_find_by_idx,
      NULL
    );

    /* Otherwise try for basic CLINT */
    if ( adev == NULL ) {
      adev = (void *) (uintptr_t) ambapp_for_each(
        ambapp_plb(),
        ( OPTIONS_ALL | OPTIONS_AHB_SLVS ),
        VENDOR_GAISLER,
        GAISLER_CLINT,
        ambapp_find_by_idx,
        NULL
      );
    }

    if ( adev == NULL ) {
      bsp_fatal( RISCV_FATAL_NO_CLINT_REG_IN_DEVICE_TREE );
    }

    GRLIB_IrqCtrl_Adev = adev;

    ahb = DEV_TO_AHB( adev );
    noel_clint = (volatile RISCV_CLINT_regs *) (uintptr_t) ( ahb->start[ 0 ] );

    adev = (void *) (uintptr_t) ambapp_for_each(
      ambapp_plb(),
      ( OPTIONS_ALL | OPTIONS_AHB_SLVS ),
      VENDOR_GAISLER,
      GAISLER_GRPLIC,
      ambapp_find_by_idx,
      NULL
    );
    if ( adev == NULL ) {
      bsp_fatal( RISCV_FATAL_NO_PLIC_REG_IN_DEVICE_TREE );
    }

    ahb = DEV_TO_AHB( adev );
    noel_plic = (volatile RISCV_PLIC_regs *) (uintptr_t) ( ahb->start[ 0 ] );
  } else {
    /* RVINTCTRL found, map CLINT and PLIC */
    GRLIB_IrqCtrl_Adev = adev;

    ahb = DEV_TO_AHB( adev );

    noel_clint =
      (volatile RISCV_CLINT_regs
         *) (uintptr_t) ( ahb->start[ GAISLER_RVINTCTRL_ACLINT_ID ] );
    if ( noel_clint == NULL ) {
      bsp_fatal( RISCV_FATAL_NO_CLINT_REG_IN_DEVICE_TREE );
    }

    noel_plic = (volatile RISCV_PLIC_regs
                   *) (uintptr_t) ( ahb->start[ GAISLER_RVINTCTRL_PLIC_ID ] );
    if ( noel_plic == NULL ) {
      bsp_fatal( RISCV_FATAL_NO_PLIC_REG_IN_DEVICE_TREE );
    }
  }
#endif /* !RISCV_USE_S_MODE */
}

/* GPTIMER */

unsigned int grlib_timer_prescaler __attribute__(( weak )) = 0;
int          grlib_timer_core_index __attribute__(( weak )) = 0;

volatile struct gptimer_regs *GRLIB_Timer_Regs;
struct ambapp_dev            *GRLIB_Timer_Adev;

/*
 *  gptimer_initialize
 *
 */

void gptimer_initialize( void )
{
  struct ambapp_dev *adev;

  /* find GP Timer */
  adev = (void *) (uintptr_t) ambapp_for_each(
    ambapp_plb(),
    ( OPTIONS_ALL | OPTIONS_APB_SLVS ),
    VENDOR_GAISLER,
    GAISLER_GPTIMER,
    ambapp_find_by_idx,
    &grlib_timer_core_index
  );
  if ( adev ) {
    GRLIB_Timer_Regs =
      (volatile struct gptimer_regs *) (uintptr_t) DEV_TO_APB( adev )->start;
    GRLIB_Timer_Adev = adev;

    /* Register AMBA Bus Frequency */
    ambapp_freq_init(
      ambapp_plb(),
      GRLIB_Timer_Adev,
      ( GRLIB_Timer_Regs->scaler_reload + 1 ) *
        GRLIB_GPTIMER_0_FREQUENCY_SET_BY_BOOT_LOADER
    );
    /* Set user prescaler configuration. Use this to increase accuracy of timer
     * and associated services like cpucounter.
     * Note that minimum value is the number of timer instances present in
     * GRTIMER/GPTIMER hardware. See HW manual.
     */
    if ( grlib_timer_prescaler ) {
      GRLIB_Timer_Regs->scaler_reload = grlib_timer_prescaler;
    }
  }
}

#ifdef RTEMS_DRVMGR_STARTUP
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus_grlib.h>
#include <rtems/sysinit.h>

extern void         gptimer_register_drv( void );
extern void         apbuart_cons_register_drv( void );
/* All drivers included by BSP, this is overridden by the user by including
 * the drvmgr_confdefs.h. By default the Timer and UART driver are included.
 */
drvmgr_drv_reg_func drvmgr_drivers[] __attribute__(( weak )) = {
  gptimer_register_drv,
  apbuart_cons_register_drv,
  NULL /* End array with NULL */
};

/* Driver resources configuration for AMBA root bus. It is declared weak
 * so that the user may override it, if the default settings are not
 * enough.
 */
struct drvmgr_bus_res grlib_drv_resources __attribute__(( weak )) = {
  .next = NULL,
  .resource = {
    DRVMGR_RES_EMPTY,
  }
};

/* GRLIB AMBA bus configuration (the LEON3 root bus configuration) */
struct grlib_config grlib_bus_config;

static void ambapp_grlib_root_initialize( void )
{
  /* Register Root bus, Use GRLIB AMBA PnP bus as root bus for LEON3 */
  grlib_bus_config.abus = ambapp_plb();
  grlib_bus_config.resources = &grlib_drv_resources;
  ambapp_grlib_root_register( &grlib_bus_config );
}

RTEMS_SYSINIT_ITEM(
  ambapp_grlib_root_initialize,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_SECOND
);

#endif

static struct ambapp_bus ambapp_plb_instance;

struct ambapp_bus *ambapp_plb( void )
{
  struct ambapp_bus *plb;

  plb = &ambapp_plb_instance;

  if ( plb->root == NULL ) {
    ambapp_scan( plb, GRLIB_IO_AREA, NULL, NULL );
    gptimer_initialize();
  }

  return plb;
}

static void amba_initialize( void )
{
  (void) ambapp_plb();
}

RTEMS_SYSINIT_ITEM(
  amba_initialize,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_FIRST
);
