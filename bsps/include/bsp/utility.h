/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsShared
 *
 * @brief This header file provides utility macros for BSPs.
 */

/*
 * Copyright (C) 2008, 2011 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
