/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_ARM_GIC_H
#define LIBBSP_ARM_SHARED_ARM_GIC_H

#include <bsp/arm-gic-regs.h>

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GIC_ID_TO_ONE_BIT_REG_INDEX(id) ((id) >> 5)
#define GIC_ID_TO_ONE_BIT_REG_BIT(id) (1U << ((id) & 0x1fU))

#define GIC_ID_TO_TWO_BITS_REG_INDEX(id) ((id) >> 4)
#define GIC_ID_TO_TWO_BITS_REG_OFFSET(id) ((id) & 0xfU)

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
