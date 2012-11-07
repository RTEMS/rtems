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

#ifndef LIBBSP_SHARED_DEFAULT_INITIAL_EXTENSION_H
#define LIBBSP_SHARED_DEFAULT_INITIAL_EXTENSION_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void bsp_fatal_extension(
  Internal_errors_Source source,
  bool is_internal,
  Internal_errors_t error
);

#define BSP_INITIAL_EXTENSION \
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL, bsp_fatal_extension }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_DEFAULT_INITIAL_EXTENSION_H */
