/**
 * @file 
 *
 * @ingroup arm_gba
 *
 * @brief Global BSP definitions.
 */

/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004
 *      Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_GBA_H
#define LIBBSP_ARM_GBA_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_FEATURE_IRQ_EXTENSION

/** Define operation count for Tests */
#define OPERATION_COUNT 10

/** gba_zero_memory library function in start.S  */
extern void gba_zero_memory(int start, int stop);
/** gba_move_memory library function in start.S  */
extern void gba_move_memory(int from, int toStart, int toEnd);
/** gba_set_memory library function in start.S  */
extern void gba_set_memory(int start, int stop, int data);


#ifdef __cplusplus
}
#endif


#endif /* __BSP_H_ */
/**
 * @defgroup arm_gba GBA Support
 *
 * @ingroup bsp_arm
 *
 * @brief GBA support package.
 */

