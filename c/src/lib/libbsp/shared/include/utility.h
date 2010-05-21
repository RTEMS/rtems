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

#define MASK32(first_bit, last_bit) \
  ((BIT32((last_bit) - (first_bit) + 1) - (uint32_t) 1) << (first_bit))

#define FIELD32(val, first_bit, last_bit) \
  (((uint32_t) (val) << (first_bit)) & MASK32(first_bit, last_bit))

#define GETFIELD32(reg, first_bit, last_bit) \
  (((reg) & MASK32(first_bit, last_bit)) >> (first_bit))

#define SETFIELD32(reg, val, first_bit, last_bit) \
  (((reg) & ~MASK32(first_bit, last_bit)) | FIELD32(val, first_bit, last_bit))

#endif /* LIBCPU_SHARED_UTILITY_H */
