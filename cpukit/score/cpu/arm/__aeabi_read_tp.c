/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/thread.h>
#include <rtems/score/percpu.h>

#ifndef RTEMS_SMP

void __attribute__((naked)) __aeabi_read_tp(void);

void __attribute__((naked)) __aeabi_read_tp(void)
{
  __asm__ volatile (
    "ldr r0, =_Per_CPU_Information\n"
    "ldr r0, [r0, %[executingoff]]\n"
#if defined(__thumb__) && !defined(__thumb2__)
    "add r0, %[tlsareaoff]\n"
    "ldr r0, [r0]\n"
#else
    "ldr r0, [r0, %[tlsareaoff]]\n"
#endif
    "bx lr\n"
    :
    : [executingoff] "I" (offsetof(Per_CPU_Control, executing)),
      [tlsareaoff] "I" (offsetof(Thread_Control, Start.tls_area))
  );
}

#endif /* RTEMS_SMP */
