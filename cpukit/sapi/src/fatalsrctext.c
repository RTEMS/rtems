/**
 * @file
 *
 * @brief Implementation of rtems_fatal_source_text()
 *
 * @ingroup ClassicFatal
 */

/*
 * Copyright (c) 2013, 2019 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/fatal.h>

static const char *const fatal_source_text[] = {
  "INTERNAL_ERROR_CORE",
  "INTERNAL_ERROR_RTEMS_API",
  "INTERNAL_ERROR_POSIX_API",
  "RTEMS_FATAL_SOURCE_BDBUF",
  "RTEMS_FATAL_SOURCE_APPLICATION",
  "RTEMS_FATAL_SOURCE_EXIT",
  "RTEMS_FATAL_SOURCE_BSP",
  "RTEMS_FATAL_SOURCE_ASSERT",
  "RTEMS_FATAL_SOURCE_STACK_CHECKER",
  "RTEMS_FATAL_SOURCE_EXCEPTION",
  "RTEMS_FATAL_SOURCE_SMP",
  "RTEMS_FATAL_SOURCE_PANIC",
  "RTEMS_FATAL_SOURCE_INVALID_HEAP_FREE",
  "RTEMS_FATAL_SOURCE_HEAP"
};

const char *rtems_fatal_source_text( rtems_fatal_source source )
{
  size_t i = source;
  const char *text = "?";

  if ( i < RTEMS_ARRAY_SIZE( fatal_source_text ) ) {
    text = fatal_source_text[ i ];
  }

  return text;
}
