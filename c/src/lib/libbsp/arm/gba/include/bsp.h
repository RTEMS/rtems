/**
 *  @file bsp.h
 *
 *  This include file contains definitions related to the GBA BSP.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004
 *      Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __BSP_H_
#define __BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#define BSP_FEATURE_IRQ_EXTENSION

/** Define operation count for Tests */
#define OPERATION_COUNT 10

/**
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */
#define rtems_bsp_delay( microseconds ) \
  { \
    uint32_t i; \
    for(i = 0; i<microseconds;) {i++;} \
    uint32_t  _cnt = _microseconds; \
    __asm__ volatile ("0: nop; sub %0, %0, #1; cmp %0,#0; bne 0b" : "=c"(_cnt) : "0"(_cnt)); \
  }

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

