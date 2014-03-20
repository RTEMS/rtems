/**
 *  @file
 *
 *  @brief CPU Initialize Context
 */

/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
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

#include <string.h>

#include <rtems/score/armv7m.h>
#include <rtems/score/thread.h>
#include <rtems/score/tls.h>

#ifdef ARM_MULTILIB_ARCH_V7M

void _CPU_Context_Initialize(
  Context_Control *context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
)
{
  char *stack_area_end = (char *) stack_area_begin + stack_area_size;

  memset(context, 0, sizeof(*context));

  context->register_lr = entry_point;
  context->register_sp = stack_area_end;

  if ( tls_area != NULL ) {
    _TLS_TCB_at_area_begin_initialize( tls_area );
  }
}

#endif /* ARM_MULTILIB_ARCH_V7M */
