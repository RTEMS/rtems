/**
 * @file
 *
 * @ingroup RTEMSScoreHeap
 *
 * @brief Heap Handler implementation.
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
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/heapimpl.h>

uintptr_t _Heap_No_extend(
  Heap_Control *unused_0 RTEMS_UNUSED,
  void *unused_1 RTEMS_UNUSED,
  uintptr_t unused_2 RTEMS_UNUSED,
  uintptr_t unused_3 RTEMS_UNUSED
)
{
  return 0;
}
