/**
 * @file
 *
 * @ingroup RTEMSBSPsARMRaspberryPi
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (c) 2015 Yang Qiao
 * Copyright (c) 2013 Alan Cudmore
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#ifndef LIBBSP_ARM_RASPBERRYPI_BSP_H
#define LIBBSP_ARM_RASPBERRYPI_BSP_H

/**
 * @defgroup RTEMSBSPsARMRaspberryPi Raspberry Pi
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief Raspberry Pi Board Support Package.
 *
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <bsp/raspberrypi.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_FEATURE_IRQ_EXTENSION

#define RPI_L2_CACHE_ENABLE 1

#define BSP_GPIO_PIN_COUNT 32
#define BSP_GPIO_PINS_PER_BANK 32
#define BSP_GPIO_PINS_PER_SELECT_BANK 10

#define BSP_CONSOLE_UART0   0
#define BSP_CONSOLE_FB      1

void rpi_init_cmdline(void);
const char *rpi_cmdline_get_cached(void);
const char *rpi_cmdline_get_raw(void);
const char *rpi_cmdline_get_arg(const char* arg);

void  rpi_video_init(void);
void  rpi_fb_outch  (char);
int rpi_video_is_initialized(void);

void rpi_ipi_initialize(void);
void rpi_start_rtems_on_secondary_processor(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* @} */

#endif /* LIBBSP_ARM_RASPBERRYPI_BSP_H */

