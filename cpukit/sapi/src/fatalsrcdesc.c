/**
 * @file
 *
 * @brief Implementation of rtems_fatal_source_description()
 *
 * @ingroup ClassicFatal
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/fatal.h>

static const char *const fatal_source_desc [] = {
  "INTERNAL_ERROR_CORE",
  "INTERNAL_ERROR_RTEMS_API",
  "INTERNAL_ERROR_POSIX_API",
  "RTEMS_FATAL_SOURCE_BDBUF",
  "RTEMS_FATAL_SOURCE_APPLICATION",
  "RTEMS_FATAL_SOURCE_EXIT",
  "RTEMS_FATAL_SOURCE_BSP_GENERIC",
  "RTEMS_FATAL_SOURCE_BSP_SPECIFIC",
  "RTEMS_FATAL_SOURCE_ASSERT",
  "RTEMS_FATAL_SOURCE_STACK_CHECKER",
  "RTEMS_FATAL_SOURCE_EXCEPTION"
};

const char *rtems_fatal_source_description( rtems_fatal_source source )
{
  size_t i = source;
  const char *desc = "?";

  if ( i < RTEMS_ARRAY_SIZE( fatal_source_desc ) ) {
    desc = fatal_source_desc [i];
  }

  return desc;
}
