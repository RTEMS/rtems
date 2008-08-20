/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Global BSP variables and functions
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_BSP_H
#define LIBBSP_POWERPC_BSP_H

#include <stdint.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#include <bspopts.h>

#include <bsp/tictac.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef ASM

/** @brief System clock frequency */
extern unsigned int bsp_clock_speed;

/** @brief Time base clicks per micro second */
extern uint32_t bsp_clicks_per_usec;

rtems_status_code mpc55xx_sd_card_init( void);

#endif /* ASM */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_BSP_H */
