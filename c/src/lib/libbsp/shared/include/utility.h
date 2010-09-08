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

#define BIT8(bit) \
  ((uint8_t) (((uint8_t) 1) << (bit)))

#define MASK8(first_bit, last_bit) \
  ((uint8_t) ((BIT8((last_bit) - (first_bit) + 1) - 1) << (first_bit)))

#define FIELD8(val, first_bit, last_bit) \
  ((uint8_t) (((val) << (first_bit)) & MASK8(first_bit, last_bit)))

#define GETFIELD8(reg, first_bit, last_bit) \
  ((uint8_t) (((reg) & MASK8(first_bit, last_bit)) >> (first_bit)))

#define SETFIELD8(reg, val, first_bit, last_bit) \
  ((uint8_t) (((reg) & ~MASK8(first_bit, last_bit)) \
    | FIELD8(val, first_bit, last_bit)))

#define BIT16(bit) \
  ((uint16_t) (((uint16_t) 1) << (bit)))

#define MASK16(first_bit, last_bit) \
  ((uint16_t) ((BIT16((last_bit) - (first_bit) + 1) - 1) << (first_bit)))

#define FIELD16(val, first_bit, last_bit) \
  ((uint16_t) (((val) << (first_bit)) & MASK16(first_bit, last_bit)))

#define GETFIELD16(reg, first_bit, last_bit) \
  ((uint16_t) (((reg) & MASK16(first_bit, last_bit)) >> (first_bit)))

#define SETFIELD16(reg, val, first_bit, last_bit) \
  ((uint16_t) (((reg) & ~MASK16(first_bit, last_bit)) \
    | FIELD16(val, first_bit, last_bit)))

#define BIT32(bit) \
  ((uint32_t) (((uint32_t) 1) << (bit)))

#define MASK32(first_bit, last_bit) \
  ((uint32_t) ((BIT32((last_bit) - (first_bit) + 1) - 1) << (first_bit)))

#define FIELD32(val, first_bit, last_bit) \
  ((uint32_t) (((val) << (first_bit)) & MASK32(first_bit, last_bit)))

#define GETFIELD32(reg, first_bit, last_bit) \
  ((uint32_t) (((reg) & MASK32(first_bit, last_bit)) >> (first_bit)))

#define SETFIELD32(reg, val, first_bit, last_bit) \
  ((uint32_t) (((reg) & ~MASK32(first_bit, last_bit)) \
    | FIELD32(val, first_bit, last_bit)))

#define BIT64(bit) \
  ((uint64_t) (((uint64_t) 1) << (bit)))

#define MASK64(first_bit, last_bit) \
  ((uint64_t) ((BIT64((last_bit) - (first_bit) + 1) - 1) << (first_bit)))

#define FIELD64(val, first_bit, last_bit) \
  ((uint64_t) (((val) << (first_bit)) & MASK64(first_bit, last_bit)))

#define GETFIELD64(reg, first_bit, last_bit) \
  ((uint64_t) (((reg) & MASK64(first_bit, last_bit)) >> (first_bit)))

#define SETFIELD64(reg, val, first_bit, last_bit) \
  ((uint64_t) (((reg) & ~MASK64(first_bit, last_bit)) \
    | FIELD64(val, first_bit, last_bit)))

#define BBIT8(bit) \
  BIT8(7 - (bit))

#define BMASK8(first_bit, last_bit) \
  MASK8(7 - (last_bit), 7 - (first_bit))

#define BFIELD8(val, first_bit, last_bit) \
  FIELD8(val, 7 - (last_bit), 7 - (first_bit))

#define BGETFIELD8(reg, first_bit, last_bit) \
  GETFIELD8(reg, 7 - (last_bit), 7 - (first_bit))

#define BSETFIELD8(reg, val, first_bit, last_bit) \
  SETFIELD8(reg, val, 7 - (last_bit), 7 - (first_bit))

#define BBIT16(bit) \
  BIT16(15 - (bit))

#define BMASK16(first_bit, last_bit) \
  MASK16(15 - (last_bit), 15 - (first_bit))

#define BFIELD16(val, first_bit, last_bit) \
  FIELD16(val, 15 - (last_bit), 15 - (first_bit))

#define BGETFIELD16(reg, first_bit, last_bit) \
  GETFIELD16(reg, 15 - (last_bit), 15 - (first_bit))

#define BSETFIELD16(reg, val, first_bit, last_bit) \
  SETFIELD16(reg, val, 15 - (last_bit), 15 - (first_bit))

#define BBIT32(bit) \
  BIT32(31 - (bit))

#define BMASK32(first_bit, last_bit) \
  MASK32(31 - (last_bit), 31 - (first_bit))

#define BFIELD32(val, first_bit, last_bit) \
  FIELD32(val, 31 - (last_bit), 31 - (first_bit))

#define BGETFIELD32(reg, first_bit, last_bit) \
  GETFIELD32(reg, 31 - (last_bit), 31 - (first_bit))

#define BSETFIELD32(reg, val, first_bit, last_bit) \
  SETFIELD32(reg, val, 31 - (last_bit), 31 - (first_bit))

#define BBIT64(bit) \
  BIT64(63 - (bit))

#define BMASK64(first_bit, last_bit) \
  MASK64(63 - (last_bit), 63 - (first_bit))

#define BFIELD64(val, first_bit, last_bit) \
  FIELD64(val, 63 - (last_bit), 63 - (first_bit))

#define BGETFIELD64(reg, first_bit, last_bit) \
  GETFIELD64(reg, 63 - (last_bit), 63 - (first_bit))

#define BSETFIELD64(reg, val, first_bit, last_bit) \
  SETFIELD64(reg, val, 63 - (last_bit), 63 - (first_bit))

#endif /* LIBCPU_SHARED_UTILITY_H */
