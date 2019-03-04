/**
 * @file
 *
 * @ingroup RTEMSBSPsARMAtsam
 */

/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* LIBBSP_ARM_ATSAM_BSP_H */
