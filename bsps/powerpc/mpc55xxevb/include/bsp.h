/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (C) 2008, 2012 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_POWERPC_MPC55XXEVB_BSP_H
#define LIBBSP_POWERPC_MPC55XXEVB_BSP_H

/**
 * @defgroup RTEMSBSPsPowerPCMPC55XX NXP MPC55XX and MPC56XX
 *
 * @ingroup RTEMSBSPsPowerPC
 *
 * @brief NXP MPC55XX and MPC56XX Board Support Package.
 *
 * @{
 */

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#define MPC55XX_PERIPHERAL_CLOCK \
  (MPC55XX_SYSTEM_CLOCK / MPC55XX_SYSTEM_CLOCK_DIVIDER)

#ifndef ASM

#include <rtems.h>

#include <libcpu/powerpc-utility.h>

#include <bsp/tictac.h>
#include <bsp/linker-symbols.h>
#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @brief System clock frequency */
extern unsigned int bsp_clock_speed;

/** @brief Time base clicks per micro second */
extern uint32_t bsp_clicks_per_usec;

/** @brief Convert Decrementer ticks to microseconds */
#define BSP_Convert_decrementer( _value ) \
  (((unsigned long long) (_value)) / ((unsigned long long)bsp_clicks_per_usec))

rtems_status_code mpc55xx_sd_card_init( bool mount);

/* Network driver configuration */

struct rtems_bsdnet_ifconfig;

int smsc9218i_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
);

#define RTEMS_BSP_NETWORK_DRIVER_ATTACH smsc9218i_attach_detach

#define RTEMS_BSP_NETWORK_DRIVER_NAME "eth0"

rtems_status_code bsp_register_i2c(void);

void bsp_restart(void *addr);

void *bsp_idle_thread(uintptr_t arg);

#define BSP_IDLE_TASK_BODY bsp_idle_thread

LINKER_SYMBOL(bsp_section_dsram_begin)
LINKER_SYMBOL(bsp_section_dsram_end)
LINKER_SYMBOL(bsp_section_dsram_size)
LINKER_SYMBOL(bsp_section_dsram_load_begin)
LINKER_SYMBOL(bsp_section_dsram_load_end)

#define BSP_DSRAM_SECTION __attribute__((section(".bsp_dsram")))

LINKER_SYMBOL(bsp_section_sysram_begin)
LINKER_SYMBOL(bsp_section_sysram_end)
LINKER_SYMBOL(bsp_section_sysram_size)
LINKER_SYMBOL(bsp_section_sysram_load_begin)
LINKER_SYMBOL(bsp_section_sysram_load_end)

#define BSP_SYSRAM_SECTION __attribute__((section(".bsp_sysram")))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/** @} */

#endif /* LIBBSP_POWERPC_MPC55XXEVB_BSP_H */
