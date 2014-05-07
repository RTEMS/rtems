/**
 *  @file
 *
 *  @ingroup shared_defaultinitialextension
 *
 *  @brief DEFAULT_INITIAL_EXTENSION Support
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

#ifndef LIBBSP_SHARED_DEFAULT_INITIAL_EXTENSION_H
#define LIBBSP_SHARED_DEFAULT_INITIAL_EXTENSION_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 *  @defgroup shared_defaultinitialextension DEFAULT_INITIAL_EXTENSION Support
 *
 *  @ingroup shared_include
 *
 *  @brief DEFAULT_INITIAL_EXTENSION Support Package
 */

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code error
);

#define BSP_INITIAL_EXTENSION \
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL, bsp_fatal_extension, NULL }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_DEFAULT_INITIAL_EXTENSION_H */
