/**
 * @file
 *
 * @ingroup bsp_kit
 *
 * @brief Utility macros.
 */

/*
 * Copyright (c) 2008, 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBCPU_SHARED_UTILITY_H
#define LIBCPU_SHARED_UTILITY_H

#include <stdint.h>

#define BIT32(bit) \
  ((uint32_t) 1 << (bit))

#define MASK32(shift, length) \
  ((BIT32(length) - (uint32_t) 1) << (shift))

#define FIELD32(val, shift, length) \
  (((uint32_t) (val) << (shift)) & MASK32(shift, length)) 

#define GETFIELD32(reg, shift, length) \
  (((uint32_t) (reg) & MASK32(shift, length)) >> (shift)) 

#define SETFIELD32(reg, val, shift, length) \
  (((uint32_t) (reg) & ~MASK32(shift, length)) | FIELD(val, shift, length))

#endif /* LIBCPU_SHARED_UTILITY_H */
