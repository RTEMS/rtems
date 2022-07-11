/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC8260ADS
 *
 * @brief Global BSP definitions.
 */

/*
 *  This include file contains all board IO definitions.
 */

/**
 * @defgroup RTEMSBSPsPowerPCMPC8260ADS NXP MPC8260ADS
 *
 * @ingroup RTEMSBSPsPowerPC
 *
 * @brief NXP MPC8260ADS Board Support Package.
 *
 * @{
 */

/*
 *  COPYRIGHT (c) 1989-2014.
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
 */

#ifndef LIBBSP_POWERPC_MPC8260ADS_BSP_H
#define LIBBSP_POWERPC_MPC8260ADS_BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <mpc8260.h>
#include <mpc8260/cpm.h>
#include <mpc8260/mmu.h>
#include <mpc8260/console.h>
#include <bsp/irq.h>
#include <bsp/vectors.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Board configuration registers
 */

typedef struct bcsr

{
    uint32_t          bcsr0; /* Board Control and Status Register */
    uint32_t          bcsr1;
    uint32_t          bcsr2;
    uint32_t          bcsr3;

} BCSR;

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
extern int rtems_enet_driver_attach (struct rtems_bsdnet_ifconfig *config, int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"eth1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_enet_driver_attach

/* miscellaneous stuff assumed to exist */

/*
 * We need to decide how much memory will be non-cacheable. This
 * will mainly be memory that will be used in DMA (network and serial
 * buffers).
 */
/*
#define NOCACHE_MEM_SIZE 512*1024
*/

/* functions */

#if 0
void M8260ExecuteRISC( uint32_t         command );
void *M8260AllocateBufferDescriptors( int count );
void *M8260AllocateRiscTimers( int count );
extern char M8260DefaultWatchdogFeeder;
#endif

/*
 * Prototypes for items shared across file boundaries in the BSP
 */
extern uint32_t bsp_serial_per_sec;
void *bsp_idle_thread( uintptr_t ignored );
void  cpu_init(void);
int mbx8xx_console_get_configuration(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
