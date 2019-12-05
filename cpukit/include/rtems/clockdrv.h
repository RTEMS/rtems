/**
 * @file
 *
 * @ingroup RTEMSDriverClock
 *
 * @brief Clock Driver API
 *
 * This file defines the Clock Driver API.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_CLOCKDRV_H
#define _RTEMS_CLOCKDRV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSDriverClock Clock Driver
 *
 * @ingroup RTEMSDeviceDrivers
 *
 * @brief The Clock Driver API.
 *
 * @{
 */

/**
 * @brief Count of clock driver ticks since system boot or last overflow.
 *
 * This counter may overflow.
 */
extern volatile uint32_t Clock_driver_ticks;

/**
 * @brief Initialize the clock driver.
 */
void _Clock_Initialize( void );

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
