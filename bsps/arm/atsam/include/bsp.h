/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMAtsam
 */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_ATSAM_BSP_H
#define LIBBSP_ARM_ATSAM_BSP_H

/**
 * @defgroup RTEMSBSPsARMAtsam Atmel/Microchip SAM E70, S70, V70 and V71
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief Atmel/Microchip SAM E70, S70, V70 and V71 Board Support Package.
 *
 * @{
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>
#include <sys/ioccom.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_ARMV7M_IRQ_PRIORITY_DEFAULT (13 << 4)

#define BSP_ARMV7M_SYSTICK_PRIORITY (14 << 4)

uint32_t atsam_systick_frequency(void);

#define BSP_ARMV7M_SYSTICK_FREQUENCY atsam_systick_frequency()

struct rtems_bsdnet_ifconfig;

int if_atsam_attach(struct rtems_bsdnet_ifconfig *config, int attaching);

#define RTEMS_BSP_NETWORK_DRIVER_NAME "atsam0"

#define RTEMS_BSP_NETWORK_DRIVER_ATTACH if_atsam_attach

/**
 * @brief Interface driver configuration.
 */
typedef struct {
  /**
   * @brief Maximum retries for MDIO communication.
   */
  uint32_t mdio_retries;

  /**
   * @brief Address of PHY.
   *
   * Use 0xff to search for a PHY.
   */
  uint8_t phy_addr;
} if_atsam_config;

extern char atsam_memory_null_begin[];
extern char atsam_memory_null_end[];
extern char atsam_memory_null_size[];

extern char atsam_memory_dtcm_begin[];
extern char atsam_memory_dtcm_end[];
extern char atsam_memory_dtcm_size[];

extern char atsam_memory_intflash_begin[];
extern char atsam_memory_intflash_end[];
extern char atsam_memory_intflash_size[];

extern char atsam_memory_intsram_begin[];
extern char atsam_memory_intsram_end[];
extern char atsam_memory_intsram_size[];

extern char atsam_memory_itcm_begin[];
extern char atsam_memory_itcm_end[];
extern char atsam_memory_itcm_size[];

extern char atsam_memory_nocache_begin[];
extern char atsam_memory_nocache_end[];
extern char atsam_memory_nocache_size[];

extern char atsam_memory_qspiflash_begin[];
extern char atsam_memory_qspiflash_end[];
extern char atsam_memory_qspiflash_size[];

extern char atsam_memory_sdram_begin[];
extern char atsam_memory_sdram_end[];
extern char atsam_memory_sdram_size[];

void atsam_rtc_get_time(rtems_time_of_day *tod);



void bsp_restart( const void *const addr );

/*
 * This ioctl enables the receive DMA for an UART. The DMA can be usefull if you
 * loose characters in high interrupt load situations.
 *
 * Disabling the DMA again is only possible by closing all file descriptors of
 * that UART.
 *
 * Note that every UART needs one DMA channel and the system has only a limited
 * amount of DMAs. So only use it if you need it.
 */
#define ATSAM_UART_ENABLE_RX_DMA _IO('d', 0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* LIBBSP_ARM_ATSAM_BSP_H */
