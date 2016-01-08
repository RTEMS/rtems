/*
 * Idle Thread Body
 *
 * COPYRIGHT (c) 2016.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/asm.h>

.p2align 4

PUBLIC(bsp_idle_thread)
SYM(bsp_idle_thread):
hltloop: hlt
  jmp hltloop
