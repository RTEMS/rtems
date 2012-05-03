/**
 * @file
 *
 * @ingroup bsp_kit
 *
 * @brief Utility macros.
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBCPU_SHARED_UTILITY_H
#define LIBCPU_SHARED_UTILITY_H

#include <stdint.h>

#define BSP_BIT8(bit) \
  ((uint8_t) (((unsigned int) 1) << (bit)))

#define BSP_MSK8(first_bit, last_bit) \
  ((uint8_t) ((BSP_BIT8((last_bit) - (first_bit) + 1) - 1) << (first_bit)))

#define BSP_FLD8(val, first_bit, last_bit) \
  ((uint8_t) \
    ((((unsigned int) (val)) << (first_bit)) & BSP_MSK8(first_bit, last_bit)))

#define BSP_FLD8GET(reg, first_bit, last_bit) \
  ((uint8_t) (((reg) & BSP_MSK8(first_bit, last_bit)) >> (first_bit)))

#define BSP_FLD8SET(reg, val, first_bit, last_bit) \
  ((uint8_t) (((reg) & ~BSP_MSK8(first_bit, last_bit)) \
    | BSP_FLD8(val, first_bit, last_bit)))

#define BSP_BIT16(bit) \
  ((uint16_t) (((unsigned int) 1) << (bit)))

#define BSP_MSK16(first_bit, last_bit) \
  ((uint16_t) ((BSP_BIT16((last_bit) - (first_bit) + 1) - 1) << (first_bit)))

#define BSP_FLD16(val, first_bit, last_bit) \
  ((uint16_t) \
    ((((unsigned int) (val)) << (first_bit)) & BSP_MSK16(first_bit, last_bit)))

#define BSP_FLD16GET(reg, first_bit, last_bit) \
  ((uint16_t) (((reg) & BSP_MSK16(first_bit, last_bit)) >> (first_bit)))

#define BSP_FLD16SET(reg, val, first_bit, last_bit) \
  ((uint16_t) (((reg) & ~BSP_MSK16(first_bit, last_bit)) \
    | BSP_FLD16(val, first_bit, last_bit)))

#define BSP_BIT32(bit) \
  ((uint32_t) (((uint32_t) 1) << (bit)))

#define BSP_MSK32(first_bit, last_bit) \
  ((uint32_t) ((BSP_BIT32((last_bit) - (first_bit) + 1) - 1) << (first_bit)))

#define BSP_FLD32(val, first_bit, last_bit) \
  ((uint32_t) \
    ((((uint32_t) (val)) << (first_bit)) & BSP_MSK32(first_bit, last_bit)))

#define BSP_FLD32GET(reg, first_bit, last_bit) \
  ((uint32_t) (((reg) & BSP_MSK32(first_bit, last_bit)) >> (first_bit)))

#define BSP_FLD32SET(reg, val, first_bit, last_bit) \
  ((uint32_t) (((reg) & ~BSP_MSK32(first_bit, last_bit)) \
    | BSP_FLD32(val, first_bit, last_bit)))

#define BSP_BIT64(bit) \
  ((uint64_t) (((uint64_t) 1) << (bit)))

#define BSP_MSK64(first_bit, last_bit) \
  ((uint64_t) ((BSP_BIT64((last_bit) - (first_bit) + 1) - 1) << (first_bit)))

#define BSP_FLD64(val, first_bit, last_bit) \
  ((uint64_t) \
    ((((uint64_t) (val)) << (first_bit)) & BSP_MSK64(first_bit, last_bit)))

#define BSP_FLD64GET(reg, first_bit, last_bit) \
  ((uint64_t) (((reg) & BSP_MSK64(first_bit, last_bit)) >> (first_bit)))

#define BSP_FLD64SET(reg, val, first_bit, last_bit) \
  ((uint64_t) (((reg) & ~BSP_MSK64(first_bit, last_bit)) \
    | BSP_FLD64(val, first_bit, last_bit)))

#define BSP_BBIT8(bit) \
  BSP_BIT8(7 - (bit))

#define BSP_BMSK8(first_bit, last_bit) \
  BSP_MSK8(7 - (last_bit), 7 - (first_bit))

#define BSP_BFLD8(val, first_bit, last_bit) \
  BSP_FLD8(val, 7 - (last_bit), 7 - (first_bit))

#define BSP_BFLD8GET(reg, first_bit, last_bit) \
  BSP_FLD8GET(reg, 7 - (last_bit), 7 - (first_bit))

#define BSP_BFLD8SET(reg, val, first_bit, last_bit) \
  BSP_FLD8SET(reg, val, 7 - (last_bit), 7 - (first_bit))

#define BSP_BBIT16(bit) \
  BSP_BIT16(15 - (bit))

#define BSP_BMSK16(first_bit, last_bit) \
  BSP_MSK16(15 - (last_bit), 15 - (first_bit))

#define BSP_BFLD16(val, first_bit, last_bit) \
  BSP_FLD16(val, 15 - (last_bit), 15 - (first_bit))

#define BSP_BFLD16GET(reg, first_bit, last_bit) \
  BSP_FLD16GET(reg, 15 - (last_bit), 15 - (first_bit))

#define BSP_BFLD16SET(reg, val, first_bit, last_bit) \
  BSP_FLD16SET(reg, val, 15 - (last_bit), 15 - (first_bit))

#define BSP_BBIT32(bit) \
  BSP_BIT32(31 - (bit))

#define BSP_BMSK32(first_bit, last_bit) \
  BSP_MSK32(31 - (last_bit), 31 - (first_bit))

#define BSP_BFLD32(val, first_bit, last_bit) \
  BSP_FLD32(val, 31 - (last_bit), 31 - (first_bit))

#define BSP_BFLD32GET(reg, first_bit, last_bit) \
  BSP_FLD32GET(reg, 31 - (last_bit), 31 - (first_bit))

#define BSP_BFLD32SET(reg, val, first_bit, last_bit) \
  BSP_FLD32SET(reg, val, 31 - (last_bit), 31 - (first_bit))

#define BSP_BBIT64(bit) \
  BSP_BIT64(63 - (bit))

#define BSP_BMSK64(first_bit, last_bit) \
  BSP_MSK64(63 - (last_bit), 63 - (first_bit))

#define BSP_BFLD64(val, first_bit, last_bit) \
  BSP_FLD64(val, 63 - (last_bit), 63 - (first_bit))

#define BSP_BFLD64GET(reg, first_bit, last_bit) \
  BSP_FLD64GET(reg, 63 - (last_bit), 63 - (first_bit))

#define BSP_BFLD64SET(reg, val, first_bit, last_bit) \
  BSP_FLD64SET(reg, val, 63 - (last_bit), 63 - (first_bit))

#endif /* LIBCPU_SHARED_UTILITY_H */
