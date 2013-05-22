/**
 *  @file
 *
 *  @brief CPU Thread Idle Body
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/cpu.h>

#ifdef ARM_MULTILIB_HAS_WFI

void *_CPU_Thread_Idle_body( uintptr_t ignored )
{
  while ( true ) {
    __asm__ volatile ("wfi");
  }
}

#endif /* ARM_MULTILIB_HAS_WFI */
