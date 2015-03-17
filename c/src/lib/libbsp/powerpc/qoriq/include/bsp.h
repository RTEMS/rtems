/**
 * @file
 *
 * @ingroup QorIQ
 *
 * @brief BSP API.
 */

/*
 * Copyright (c) 2010-2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_QORIQ_BSP_H
#define LIBBSP_POWERPC_QORIQ_BSP_H

#include <bspopts.h>

#ifndef ASM

#include <rtems.h>

#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_FEATURE_IRQ_EXTENSION

#define QORIQ_CHIP(alpha, num) ((alpha) * 10000 + (num))

#define QORIQ_CHIP_P1020 QORIQ_CHIP('P', 1020)

#define QORIQ_CHIP_T2080 QORIQ_CHIP('T', 2080)

#define QORIQ_CHIP_T4240 QORIQ_CHIP('T', 4240)

#define QORIQ_CHIP_IS_T_VARIANT(variant) ((variant) / 10000 == 'T')

extern unsigned BSP_bus_frequency;

struct rtems_bsdnet_ifconfig;

int BSP_tsec_attach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
);

int qoriq_if_intercom_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
);

#if defined(HAS_UBOOT)
  /* Routine to obtain U-Boot environment variables */
  const char *bsp_uboot_getenv(
    const char *name
  );
#endif

/* Internal SMP startup function */
void qoriq_start_thread(void);

void *bsp_idle_thread( uintptr_t ignored );
#define BSP_IDLE_TASK_BODY bsp_idle_thread

#define RTEMS_BSP_NETWORK_DRIVER_ATTACH BSP_tsec_attach
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH4 qoriq_if_intercom_attach_detach

#define RTEMS_BSP_NETWORK_DRIVER_NAME "tsec1"
#define RTEMS_BSP_NETWORK_DRIVER_NAME2 "tsec2"
#define RTEMS_BSP_NETWORK_DRIVER_NAME3 "tsec3"
#define RTEMS_BSP_NETWORK_DRIVER_NAME4 "intercom1"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_POWERPC_QORIQ_BSP_H */
