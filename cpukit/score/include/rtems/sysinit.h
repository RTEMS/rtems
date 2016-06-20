/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_SYSINIT_H
#define _RTEMS_SYSINIT_H

#include <rtems/linkersets.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * The value of each module define must consist of exactly six hexadecimal
 * digits without a 0x-prefix.  A 0x-prefix is concatenated with the module and
 * order values to form a proper integer literal.
 */
#define RTEMS_SYSINIT_BSP_WORK_AREAS             000100
#define RTEMS_SYSINIT_BSP_START                  000200
#define RTEMS_SYSINIT_INITIAL_EXTENSIONS         000300
#define RTEMS_SYSINIT_MP_EARLY                   000301
#define RTEMS_SYSINIT_DATA_STRUCTURES            000302
#define RTEMS_SYSINIT_CPU_SET                    00030d
#define RTEMS_SYSINIT_MP                         00030e
#define RTEMS_SYSINIT_USER_EXTENSIONS            000320
#define RTEMS_SYSINIT_CLASSIC_TASKS              000340
#define RTEMS_SYSINIT_CLASSIC_TIMER              000341
#define RTEMS_SYSINIT_CLASSIC_SIGNAL             000342
#define RTEMS_SYSINIT_CLASSIC_EVENT              000343
#define RTEMS_SYSINIT_CLASSIC_MESSAGE_QUEUE      000344
#define RTEMS_SYSINIT_CLASSIC_SEMAPHORE          000345
#define RTEMS_SYSINIT_CLASSIC_PARTITION          000346
#define RTEMS_SYSINIT_CLASSIC_REGION             000347
#define RTEMS_SYSINIT_CLASSIC_DUAL_PORTED_MEMORY 000348
#define RTEMS_SYSINIT_CLASSIC_RATE_MONOTONIC     000349
#define RTEMS_SYSINIT_CLASSIC_BARRIER            00034a
#define RTEMS_SYSINIT_POSIX_SIGNALS              000360
#define RTEMS_SYSINIT_POSIX_THREADS              000361
#define RTEMS_SYSINIT_POSIX_CONDITION_VARIABLE   000362
#define RTEMS_SYSINIT_POSIX_MUTEX                000363
#define RTEMS_SYSINIT_POSIX_MESSAGE_QUEUE        000364
#define RTEMS_SYSINIT_POSIX_SEMAPHORE            000365
#define RTEMS_SYSINIT_POSIX_TIMER                000366
#define RTEMS_SYSINIT_POSIX_BARRIER              000367
#define RTEMS_SYSINIT_POSIX_RWLOCK               000368
#define RTEMS_SYSINIT_POSIX_SPINLOCK             000369
#define RTEMS_SYSINIT_POSIX_CLEANUP              00036a
#define RTEMS_SYSINIT_POSIX_KEYS                 00036b
#define RTEMS_SYSINIT_IDLE_THREADS               000380
#define RTEMS_SYSINIT_LIBIO                      000400
#define RTEMS_SYSINIT_ROOT_FILESYSTEM            000401
#define RTEMS_SYSINIT_DRVMGR                     000500
#define RTEMS_SYSINIT_MP_SERVER                  000501
#define RTEMS_SYSINIT_BSP_PRE_DRIVERS            000600
#define RTEMS_SYSINIT_DRVMGR_LEVEL_1             000700
#define RTEMS_SYSINIT_DEVICE_DRIVERS             000701
#define RTEMS_SYSINIT_DRVMGR_LEVEL_2             000702
#define RTEMS_SYSINIT_DRVMGR_LEVEL_3             000703
#define RTEMS_SYSINIT_DRVMGR_LEVEL_4             000704
#define RTEMS_SYSINIT_MP_FINALIZE                000705
#define RTEMS_SYSINIT_CLASSIC_USER_TASKS         000706
#define RTEMS_SYSINIT_POSIX_USER_THREADS         000707
#define RTEMS_SYSINIT_STD_FILE_DESCRIPTORS       000800
#define RTEMS_SYSINIT_LAST                       ffffff

/*
 * The value of each order define must consist of exactly two hexadecimal
 * digits without a 0x-prefix.  A 0x-prefix is concatenated with the module and
 * order values to form a proper integer literal.
 */
#define RTEMS_SYSINIT_ORDER_FIRST   00
#define RTEMS_SYSINIT_ORDER_SECOND  01
#define RTEMS_SYSINIT_ORDER_THIRD   02
#define RTEMS_SYSINIT_ORDER_FOURTH  03
#define RTEMS_SYSINIT_ORDER_FIFTH   04
#define RTEMS_SYSINIT_ORDER_SIXTH   05
#define RTEMS_SYSINIT_ORDER_SEVENTH 06
#define RTEMS_SYSINIT_ORDER_EIGHTH  07
#define RTEMS_SYSINIT_ORDER_NINETH  08
#define RTEMS_SYSINIT_ORDER_TENTH   09
#define RTEMS_SYSINIT_ORDER_MIDDLE  80
#define RTEMS_SYSINIT_ORDER_LAST    ff

typedef void ( *rtems_sysinit_handler )( void );

typedef struct {
  rtems_sysinit_handler handler;
} rtems_sysinit_item;

/* The enum helps to detect typos in the module and order parameters */
#define _RTEMS_SYSINIT_INDEX_ITEM( handler, index ) \
  enum { _Sysinit_##handler = index }; \
  RTEMS_LINKER_ROSET_ITEM_ORDERED( \
    _Sysinit, \
    rtems_sysinit_item, \
    handler, \
    index \
  ) = { handler }

/* Create index from module and order */
#define _RTEMS_SYSINIT_ITEM( handler, module, order ) \
  _RTEMS_SYSINIT_INDEX_ITEM( handler, 0x##module##order )

/* Perform parameter expansion */
#define RTEMS_SYSINIT_ITEM( handler, module, order ) \
  _RTEMS_SYSINIT_ITEM( handler, module, order )

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SYSINIT_H */
