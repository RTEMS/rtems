/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPISystemInit
 *
 * @brief This header file provides the API of the @ref RTEMSAPISystemInit.
 */

/*
 * Copyright (C) 2015, 2023 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SYSINIT_H
#define _RTEMS_SYSINIT_H

#include <rtems/linkersets.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup RTEMSImpl
 *
 * @brief Enables a verbose system initialization.
 */
void _Sysinit_Verbose( void );

/**
 * @ingroup RTEMSImpl
 *
 * @brief Creates the system initialization item associated with the handler
 *   and index.
 *
 * The enum helps to detect typos in the module and order parameters of
 * RTEMS_SYSINIT_ITEM().
 */
#define _RTEMS_SYSINIT_INDEX_ITEM( handler, index ) \
  enum { _Sysinit_##handler = index }; \
  RTEMS_LINKER_ROSET_ITEM_ORDERED( \
    _Sysinit, \
    rtems_sysinit_item, \
    handler, \
    index \
  ) = { handler }

/**
 * @ingroup RTEMSImpl
 *
 * @brief Creates the system initialization item associated with the handler,
 *   module, and order.
 *
 * This helper macro is used to perform parameter expansion in
 * RTEMS_SYSINIT_ITEM().
 */
#define _RTEMS_SYSINIT_ITEM( handler, module, order ) \
  _RTEMS_SYSINIT_INDEX_ITEM( handler, 0x##module##order )

/**
 * @defgroup RTEMSAPISystemInit System Initialization Support
 *
 * @ingroup RTEMSAPI
 *
 * @brief The system initialization support provides an ordered invocation of
 *   system initialization handlers registered in a linker set.
 *
 * @{
 */

/*
 * The value of each module define must consist of exactly six hexadecimal
 * digits without a 0x-prefix.  A 0x-prefix is concatenated with the module and
 * order values to form a proper integer literal.
 */
#define RTEMS_SYSINIT_RECORD                     000100
#define RTEMS_SYSINIT_BSP_EARLY                  000140
#define RTEMS_SYSINIT_MEMORY                     000180
#define RTEMS_SYSINIT_DIRTY_MEMORY               0001c0
#define RTEMS_SYSINIT_ZERO_MEMORY                0001e0
#define RTEMS_SYSINIT_ISR_STACK                  000200
#define RTEMS_SYSINIT_PER_CPU_DATA               000220
#define RTEMS_SYSINIT_SBRK                       000240
#define RTEMS_SYSINIT_STACK_ALLOCATOR            000250
#define RTEMS_SYSINIT_WORKSPACE                  000260
#define RTEMS_SYSINIT_MALLOC                     000280
#define RTEMS_SYSINIT_BSP_START                  000300
#define RTEMS_SYSINIT_CPU_COUNTER                000400
#define RTEMS_SYSINIT_TARGET_HASH                000480
#define RTEMS_SYSINIT_INITIAL_EXTENSIONS         000500
#define RTEMS_SYSINIT_MP_EARLY                   000600
#define RTEMS_SYSINIT_DATA_STRUCTURES            000700
#define RTEMS_SYSINIT_SCHEDULER                  000780
#define RTEMS_SYSINIT_MP                         000800
#define RTEMS_SYSINIT_USER_EXTENSIONS            000900
#define RTEMS_SYSINIT_CLASSIC_TASKS              000a00
#define RTEMS_SYSINIT_CLASSIC_TASKS_MP           000a80
#define RTEMS_SYSINIT_CLASSIC_TIMER              000b00
#define RTEMS_SYSINIT_CLASSIC_SIGNAL_MP          000c00
#define RTEMS_SYSINIT_CLASSIC_EVENT_MP           000d00
#define RTEMS_SYSINIT_CLASSIC_MESSAGE_QUEUE      000e00
#define RTEMS_SYSINIT_CLASSIC_MESSAGE_QUEUE_MP   000e80
#define RTEMS_SYSINIT_CLASSIC_SEMAPHORE          000f00
#define RTEMS_SYSINIT_CLASSIC_SEMAPHORE_MP       000f80
#define RTEMS_SYSINIT_CLASSIC_PARTITION          001000
#define RTEMS_SYSINIT_CLASSIC_PARTITION_MP       001080
#define RTEMS_SYSINIT_CLASSIC_REGION             001100
#define RTEMS_SYSINIT_CLASSIC_DUAL_PORTED_MEMORY 001200
#define RTEMS_SYSINIT_CLASSIC_RATE_MONOTONIC     001300
#define RTEMS_SYSINIT_CLASSIC_BARRIER            001400
#define RTEMS_SYSINIT_POSIX_SIGNALS              001500
#define RTEMS_SYSINIT_POSIX_THREADS              001600
#define RTEMS_SYSINIT_POSIX_MESSAGE_QUEUE        001700
#define RTEMS_SYSINIT_POSIX_SEMAPHORE            001800
#define RTEMS_SYSINIT_POSIX_TIMER                001900
#define RTEMS_SYSINIT_POSIX_SHM                  001a00
#define RTEMS_SYSINIT_POSIX_KEYS                 001b00
#define RTEMS_SYSINIT_POSIX_CLEANUP              001c00
#define RTEMS_SYSINIT_IDLE_THREADS               001d00
#define RTEMS_SYSINIT_LIBIO                      001e00
#define RTEMS_SYSINIT_USER_ENVIRONMENT           001e80
#define RTEMS_SYSINIT_ROOT_FILESYSTEM            001f00
#define RTEMS_SYSINIT_DRVMGR                     002000
#define RTEMS_SYSINIT_MP_SERVER                  002100
#define RTEMS_SYSINIT_BSP_PRE_DRIVERS            002200
#define RTEMS_SYSINIT_DRVMGR_LEVEL_1             002300
#define RTEMS_SYSINIT_DEVICE_DRIVERS             002400
#define RTEMS_SYSINIT_DRVMGR_LEVEL_2             002500
#define RTEMS_SYSINIT_DRVMGR_LEVEL_3             002600
#define RTEMS_SYSINIT_DRVMGR_LEVEL_4             002700
#define RTEMS_SYSINIT_MP_FINALIZE                002800
#define RTEMS_SYSINIT_CLASSIC_USER_TASKS         002900
#define RTEMS_SYSINIT_POSIX_USER_THREADS         002a00
#define RTEMS_SYSINIT_STD_FILE_DESCRIPTORS       002b00
#define RTEMS_SYSINIT_LAST                       ffffff

/*
 * The value of each order define must consist of exactly two hexadecimal
 * digits without a 0x-prefix.  A 0x-prefix is concatenated with the module and
 * order values to form a proper integer literal.
 */
#define RTEMS_SYSINIT_ORDER_FIRST      00
#define RTEMS_SYSINIT_ORDER_SECOND     01
#define RTEMS_SYSINIT_ORDER_THIRD      02
#define RTEMS_SYSINIT_ORDER_FOURTH     03
#define RTEMS_SYSINIT_ORDER_FIFTH      04
#define RTEMS_SYSINIT_ORDER_SIXTH      05
#define RTEMS_SYSINIT_ORDER_SEVENTH    06
#define RTEMS_SYSINIT_ORDER_EIGHTH     07
#define RTEMS_SYSINIT_ORDER_NINETH     08
#define RTEMS_SYSINIT_ORDER_TENTH      09
#define RTEMS_SYSINIT_ORDER_MIDDLE     80
#define RTEMS_SYSINIT_ORDER_LAST_BUT_9 f6
#define RTEMS_SYSINIT_ORDER_LAST_BUT_8 f7
#define RTEMS_SYSINIT_ORDER_LAST_BUT_7 f8
#define RTEMS_SYSINIT_ORDER_LAST_BUT_6 f9
#define RTEMS_SYSINIT_ORDER_LAST_BUT_5 fa
#define RTEMS_SYSINIT_ORDER_LAST_BUT_4 fb
#define RTEMS_SYSINIT_ORDER_LAST_BUT_3 fc
#define RTEMS_SYSINIT_ORDER_LAST_BUT_2 fd
#define RTEMS_SYSINIT_ORDER_LAST_BUT_1 fe
#define RTEMS_SYSINIT_ORDER_LAST       ff

typedef void ( *rtems_sysinit_handler )( void );

typedef struct {
  rtems_sysinit_handler handler;
} rtems_sysinit_item;

/**
 * @brief Creates the system initialization item associated with the handler,
 *   module, and order.
 *
 * @param handler is the system initialization handler.
 *
 * @param module is the system initialization module.  It shall be a 6-digit
 *   hex number without a 0x-prefix.
 *
 * @param order is the system initialization order with respect to the module.
 *   It shall be a 2-digit hex number without a 0x-prefix.
 */
#define RTEMS_SYSINIT_ITEM( handler, module, order ) \
  _RTEMS_SYSINIT_ITEM( handler, module, order )

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SYSINIT_H */
