/*
 * Copyright (c) 2012-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_SHARED_BSP_GENERIC_FATAL_H
#define LIBBSP_SHARED_BSP_GENERIC_FATAL_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Generic BSP fatal error codes.
 */
typedef enum {
  BSP_GENERIC_FATAL_EXCEPTION_INITIALIZATION,
  BSP_GENERIC_FATAL_INTERRUPT_INITIALIZATION,
  BSP_GENERIC_FATAL_SPURIOUS_INTERRUPT
} bsp_generic_fatal_code;

static inline void bsp_generic_fatal( bsp_generic_fatal_code code )
{
  rtems_fatal( RTEMS_FATAL_SOURCE_BSP_GENERIC, (rtems_fatal_code) code );
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_BSP_GENERIC_FATAL_H */
