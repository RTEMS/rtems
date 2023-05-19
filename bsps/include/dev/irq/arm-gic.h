/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @ingroup arm_gic
 *
 *  @brief ARM GIC Support
 */

/*
 * Copyright (C) 2013, 2019 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_SHARED_ARM_GIC_H
#define LIBBSP_ARM_SHARED_ARM_GIC_H

#include <dev/irq/arm-gic-regs.h>

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 *  @defgroup arm_gic ARM GIC
 *
 *  @ingroup RTEMSBSPsARMShared
 *
 *  @brief ARM_GIC Support Package
 */

#define GIC_ID_TO_ONE_BIT_REG_INDEX(id) ((id) >> 5)
#define GIC_ID_TO_ONE_BIT_REG_BIT(id) (1U << ((id) & 0x1fU))

#define GIC_ID_TO_TWO_BITS_REG_INDEX(id) ((id) >> 4)
#define GIC_ID_TO_TWO_BITS_REG_OFFSET(id) (((id) & 0xfU) << 1)

static inline bool gic_id_is_enabled(volatile gic_dist *dist, uint32_t id)
{
  uint32_t i = GIC_ID_TO_ONE_BIT_REG_INDEX(id);
  uint32_t bit = GIC_ID_TO_ONE_BIT_REG_BIT(id);

  return (dist->icdiser[i] & bit) != 0;
}

static inline void gic_id_enable(volatile gic_dist *dist, uint32_t id)
{
  uint32_t i = GIC_ID_TO_ONE_BIT_REG_INDEX(id);
  uint32_t bit = GIC_ID_TO_ONE_BIT_REG_BIT(id);

  dist->icdiser[i] = bit;
}

static inline void gic_id_disable(volatile gic_dist *dist, uint32_t id)
{
  uint32_t i = GIC_ID_TO_ONE_BIT_REG_INDEX(id);
  uint32_t bit = GIC_ID_TO_ONE_BIT_REG_BIT(id);

  dist->icdicer[i] = bit;
}

static inline bool gic_id_is_pending(volatile gic_dist *dist, uint32_t id)
{
  uint32_t i = GIC_ID_TO_ONE_BIT_REG_INDEX(id);
  uint32_t bit = GIC_ID_TO_ONE_BIT_REG_BIT(id);

  return (dist->icdispr[i] & bit) != 0;
}

static inline void gic_id_set_pending(volatile gic_dist *dist, uint32_t id)
{
  uint32_t i = GIC_ID_TO_ONE_BIT_REG_INDEX(id);
  uint32_t bit = GIC_ID_TO_ONE_BIT_REG_BIT(id);

  dist->icdispr[i] = bit;
}

static inline void gic_id_clear_pending(volatile gic_dist *dist, uint32_t id)
{
  uint32_t i = GIC_ID_TO_ONE_BIT_REG_INDEX(id);
  uint32_t bit = GIC_ID_TO_ONE_BIT_REG_BIT(id);

  dist->icdicpr[i] = bit;
}

static inline bool gic_id_is_active(volatile gic_dist *dist, uint32_t id)
{
  uint32_t i = GIC_ID_TO_ONE_BIT_REG_INDEX(id);
  uint32_t bit = GIC_ID_TO_ONE_BIT_REG_BIT(id);

  return (dist->icdabr[i] & bit) != 0;
}

typedef enum {
  GIC_GROUP_0,
  GIC_GROUP_1
} gic_group;

static inline gic_group gic_id_get_group(
  volatile gic_dist *dist,
  uint32_t id
)
{
  uint32_t i = GIC_ID_TO_ONE_BIT_REG_INDEX(id);
  uint32_t bit = GIC_ID_TO_ONE_BIT_REG_BIT(id);

  return (dist->icdigr[i] & bit) != 0 ?  GIC_GROUP_1 : GIC_GROUP_0;
}

static inline void gic_id_set_group(
  volatile gic_dist *dist,
  uint32_t id,
  gic_group group
)
{
  uint32_t i = GIC_ID_TO_ONE_BIT_REG_INDEX(id);
  uint32_t bit = GIC_ID_TO_ONE_BIT_REG_BIT(id);
  uint32_t icdigr = dist->icdigr[i];

  icdigr &= ~bit;

  if (group == GIC_GROUP_1) {
    icdigr |= bit;
  }

  dist->icdigr[i] = icdigr;
}

static inline void gic_id_set_priority(
  volatile gic_dist *dist,
  uint32_t id,
  uint8_t priority
)
{
  dist->icdipr[id] = priority;
}

static inline uint8_t gic_id_get_priority(volatile gic_dist *dist, uint32_t id)
{
  return dist->icdipr[id];
}

static inline void gic_id_set_targets(
  volatile gic_dist *dist,
  uint32_t id,
  uint8_t targets
)
{
  dist->icdiptr[id] = targets;
}

static inline uint8_t gic_id_get_targets(volatile gic_dist *dist, uint32_t id)
{
  return dist->icdiptr[id];
}

typedef enum {
  GIC_LEVEL_SENSITIVE,
  GIC_EDGE_TRIGGERED
} gic_trigger_mode;

static inline gic_trigger_mode gic_id_get_trigger_mode(
  volatile gic_dist *dist,
  uint32_t id
)
{
  uint32_t i = GIC_ID_TO_TWO_BITS_REG_INDEX(id);
  uint32_t o = GIC_ID_TO_TWO_BITS_REG_OFFSET(id) + 1;
  uint32_t bit = 1U << o;

  return (dist->icdicfr[i] & bit) != 0 ?
    GIC_EDGE_TRIGGERED : GIC_LEVEL_SENSITIVE;
}

static inline void gic_id_set_trigger_mode(
  volatile gic_dist *dist,
  uint32_t id,
  gic_trigger_mode mode
)
{
  uint32_t i = GIC_ID_TO_TWO_BITS_REG_INDEX(id);
  uint32_t o = GIC_ID_TO_TWO_BITS_REG_OFFSET(id) + 1;
  uint32_t bit = mode << o;
  uint32_t mask = 1U << o;
  uint32_t icdicfr = dist->icdicfr[i];

  icdicfr &= ~mask;
  icdicfr |= bit;

  dist->icdicfr[i] = icdicfr;
}

typedef enum {
  GIC_N_TO_N,
  GIC_1_TO_N
} gic_handling_model;

static inline gic_handling_model gic_id_get_handling_model(
  volatile gic_dist *dist,
  uint32_t id
)
{
  uint32_t i = GIC_ID_TO_TWO_BITS_REG_INDEX(id);
  uint32_t o = GIC_ID_TO_TWO_BITS_REG_OFFSET(id);
  uint32_t bit = 1U << o;

  return (dist->icdicfr[i] & bit) != 0 ? GIC_1_TO_N : GIC_N_TO_N;
}

static inline void gic_id_set_handling_model(
  volatile gic_dist *dist,
  uint32_t id,
  gic_handling_model model
)
{
  uint32_t i = GIC_ID_TO_TWO_BITS_REG_INDEX(id);
  uint32_t o = GIC_ID_TO_TWO_BITS_REG_OFFSET(id);
  uint32_t bit = model << o;
  uint32_t mask = 1U << o;
  uint32_t icdicfr = dist->icdicfr[i];

  icdicfr &= ~mask;
  icdicfr |= bit;

  dist->icdicfr[i] = icdicfr;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_ARM_GIC_H */
