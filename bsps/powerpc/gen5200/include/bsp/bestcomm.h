/*
 * Copyright (c) 2010-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef GEN5200_BESTCOMM_H
#define GEN5200_BESTCOMM_H

#include "bestcomm_ops.h"

#include <assert.h>

#include <rtems.h>

#include <bsp/mpc5200.h>
#include <bsp/bestcomm/bestcomm_api.h>
#include <bsp/bestcomm/bestcomm_glue.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup BestComm BestComm Support
 *
 * @ingroup RTEMSBSPsPowerPCGen5200
 *
 * @brief BestComm support.
 *
 * @{
 */

typedef struct {
  uint32_t *tdt_begin;
  uint32_t *tdt_last;
  volatile uint32_t (*var_table)[32];
  uint32_t fdt_and_pragmas;
  uint32_t reserved_0;
  uint32_t reserved_1;
  uint32_t *context_begin;
  uint32_t reserved_2;
} bestcomm_task_entry;

#define BESTCOMM_TASK_ENTRY_TABLE ((volatile bestcomm_task_entry *) 0xf0008000)

#define BESTCOMM_IRQ_EVENT RTEMS_EVENT_13

typedef struct {
  int task_index;
  rtems_id event_task_id;
} bestcomm_irq;

void bestcomm_irq_create(bestcomm_irq *self, int task_index);

void bestcomm_irq_destroy(const bestcomm_irq *self);

static inline void bestcomm_irq_enable(const bestcomm_irq *self)
{
  bestcomm_glue_irq_enable(self->task_index);
}

static inline void bestcomm_irq_disable(const bestcomm_irq *self)
{
  bestcomm_glue_irq_disable(self->task_index);
}

static inline void bestcomm_irq_clear(const bestcomm_irq *self)
{
  SDMA_CLEAR_IEVENT(&mpc5200.sdma.IntPend, self->task_index);
}

static inline int bestcomm_irq_get_task_index(const bestcomm_irq *self)
{
  return self->task_index;
}

static inline rtems_id bestcomm_irq_get_event_task_id(const bestcomm_irq *self)
{
  return self->event_task_id;
}

static inline void bestcomm_irq_set_event_task_id(bestcomm_irq *self, rtems_id id)
{
  self->event_task_id = id;
}

static inline void bestcomm_irq_wakeup_event_task(const bestcomm_irq *self)
{
  rtems_status_code sc = rtems_event_send(self->event_task_id, BESTCOMM_IRQ_EVENT);
  assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;
}

static inline void bestcomm_irq_wait(const bestcomm_irq *self)
{
  rtems_event_set events;
  rtems_status_code sc = rtems_event_receive(
    BESTCOMM_IRQ_EVENT,
    RTEMS_EVENT_ALL | RTEMS_WAIT,
    RTEMS_NO_TIMEOUT,
    &events
  );
  assert(sc == RTEMS_SUCCESSFUL);
  assert(events == BESTCOMM_IRQ_EVENT);
  (void) sc;
}

static inline bool bestcomm_irq_peek(const bestcomm_irq *self)
{
  rtems_event_set events;
  rtems_status_code sc = rtems_event_receive(0, 0, 0, &events);
  assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;

  return (events & BESTCOMM_IRQ_EVENT) != 0;
}

typedef struct {
  volatile uint16_t *task_control_register;

  volatile uint32_t (*variable_table)[32];

  TaskId task_index;

  bestcomm_irq irq;

  uint32_t *tdt_begin;

  size_t tdt_opcode_count;
} bestcomm_task;

void bestcomm_task_create(bestcomm_task *self, TaskId task_index);

void bestcomm_task_create_and_load(
  bestcomm_task *self,
  TaskId task_index,
  const uint32_t *tdt_source_begin,
  size_t tdt_size
);

void bestcomm_task_destroy(bestcomm_task *self);

void bestcomm_task_load(bestcomm_task *self, const uint32_t *tdt_source_begin, size_t tdt_size);

static inline void bestcomm_task_set_priority(bestcomm_task *self, int priority)
{
  /* Allow higher priority initiator to block current initiator */
  mpc5200.sdma.ipr[self->task_index] = SDMA_IPR_PRIOR(priority);
}

static inline void bestcomm_task_irq_enable(const bestcomm_task *self)
{
  bestcomm_irq_enable(&self->irq);
}

static inline void bestcomm_task_irq_disable(const bestcomm_task *self)
{
  bestcomm_irq_disable(&self->irq);
}

static inline void bestcomm_task_irq_clear(const bestcomm_task *self)
{
  bestcomm_irq_clear(&self->irq);
}

static inline rtems_id bestcomm_task_get_event_task_id(const bestcomm_task *self)
{
  return bestcomm_irq_get_event_task_id(&self->irq);
}

static inline void bestcomm_task_set_event_task_id(bestcomm_task *self, rtems_id id)
{
  bestcomm_irq_set_event_task_id(&self->irq, id);
}

static inline void bestcomm_task_associate_with_current_task(bestcomm_task *self)
{
  bestcomm_task_set_event_task_id(self, rtems_task_self());
}

static inline void bestcomm_task_start(const bestcomm_task *self)
{
  *self->task_control_register = SDMA_TCR_EN | SDMA_TCR_HIGH_EN;
}

static inline void bestcomm_task_start_with_autostart(const bestcomm_task *self)
{
  *self->task_control_register = (uint16_t)
    (SDMA_TCR_EN | SDMA_TCR_HIGH_EN | SDMA_TCR_AUTO_START | SDMA_TCR_AS(self->task_index));
}

static inline void bestcomm_task_stop(const bestcomm_task *self)
{
  *self->task_control_register = 0;
}

static inline void bestcomm_task_wakeup_event_task(const bestcomm_task *self)
{
  bestcomm_irq_wakeup_event_task(&self->irq);
}

static inline void bestcomm_task_wait(const bestcomm_task *self)
{
  bestcomm_irq_wait(&self->irq);
}

static inline bool bestcomm_task_peek(const bestcomm_task *self)
{
  return bestcomm_irq_peek(&self->irq);
}

static inline bool bestcomm_task_is_running(const bestcomm_task *self)
{
  return (*self->task_control_register & SDMA_TCR_EN) != 0;
}

static inline uint32_t bestcomm_get_task_variable(const bestcomm_task *self, size_t index)
{
  assert(index < VAR_COUNT);
  return (*self->variable_table)[index];
}

static inline volatile uint32_t *bestcomm_task_get_address_of_variable(const bestcomm_task *self, size_t index)
{
  assert(index < VAR_COUNT);
  return &(*self->variable_table)[index];
}

static inline void bestcomm_task_set_variable(const bestcomm_task *self, size_t index, uint32_t value)
{
  assert(index < VAR_COUNT);
  (*self->variable_table)[index] = value;
}

static inline uint32_t bestcomm_task_get_increment_and_condition(const bestcomm_task *self, size_t index)
{
  assert(index < INC_COUNT);
  return (*self->variable_table)[INC(index)];
}

static inline void bestcomm_task_set_increment_and_condition_32(
  const bestcomm_task *self,
  size_t index,
  uint32_t inc_and_cond
)
{
  assert(index < INC_COUNT);
  (*self->variable_table)[INC(index)] = inc_and_cond;
}

static inline void bestcomm_task_set_increment_and_condition(
  const bestcomm_task *self,
  size_t index,
  int16_t inc,
  int cond
)
{
  bestcomm_task_set_increment_and_condition_32(self, index, INC_INIT(cond, inc));
}

static inline void bestcomm_task_set_increment(const bestcomm_task *self, size_t index, int16_t inc)
{
  bestcomm_task_set_increment_and_condition_32(self, index, INC_INIT(0, inc));
}

void bestcomm_task_clear_variables(const bestcomm_task *self);

static inline uint32_t bestcomm_task_get_opcode(const bestcomm_task *self, size_t index)
{
  assert(index < self->tdt_opcode_count);
  return self->tdt_begin[index];
}

static inline void bestcomm_task_set_opcode(bestcomm_task *self, size_t index, uint32_t opcode)
{
  assert(index < self->tdt_opcode_count);
  self->tdt_begin[index] = opcode;
}

static inline void bestcomm_task_set_initiator(const bestcomm_task *self, int initiator)
{
  rtems_interrupt_level level;
  rtems_interrupt_disable(level);
  *self->task_control_register = BSP_BFLD16SET(*self->task_control_register, initiator, 3, 7);
  rtems_interrupt_enable(level);
}

static inline volatile bestcomm_task_entry *bestcomm_task_get_task_entry(const bestcomm_task *self)
{
  return &BESTCOMM_TASK_ENTRY_TABLE[self->task_index];
}

static inline void bestcomm_task_set_pragma(const bestcomm_task *self, int bit_pos, bool enable)
{
  volatile bestcomm_task_entry *entry = bestcomm_task_get_task_entry(self);
  uint32_t mask = BSP_BIT32(bit_pos);
  uint32_t bit = enable ? mask : 0;
  entry->fdt_and_pragmas = (entry->fdt_and_pragmas & ~mask) | bit;
}

static inline void bestcomm_task_enable_precise_increment(const bestcomm_task *self, bool enable)
{
  bestcomm_task_set_pragma(self, SDMA_PRAGMA_BIT_PRECISE_INC, enable);
}

static inline void bestcomm_task_enable_error_reset(const bestcomm_task *self, bool enable)
{
  bestcomm_task_set_pragma(self, SDMA_PRAGMA_BIT_RST_ERROR_NO, !enable);
}

static inline void bestcomm_task_enable_pack_data(const bestcomm_task *self, bool enable)
{
  bestcomm_task_set_pragma(self, SDMA_PRAGMA_BIT_PACK, enable);
}

static inline void bestcomm_task_enable_integer_mode(const bestcomm_task *self, bool enable)
{
  bestcomm_task_set_pragma(self, SDMA_PRAGMA_BIT_INTEGER, enable);
}

static inline void bestcomm_task_enable_speculative_read(const bestcomm_task *self, bool enable)
{
  bestcomm_task_set_pragma(self, SDMA_PRAGMA_BIT_SPECREAD, enable);
}

static inline void bestcomm_task_enable_combined_write(const bestcomm_task *self, bool enable)
{
  bestcomm_task_set_pragma(self, SDMA_PRAGMA_BIT_CW, enable);
}

static inline void bestcomm_task_enable_read_buffer(const bestcomm_task *self, bool enable)
{
  bestcomm_task_set_pragma(self, SDMA_PRAGMA_BIT_RL, enable);
}

static inline volatile uint16_t *bestcomm_task_get_task_control_register(const bestcomm_task *self)
{
  return self->task_control_register;
}

static inline int bestcomm_task_get_task_index(const bestcomm_task *self)
{
  return self->task_index;
}

static inline void bestcomm_task_free_tdt(bestcomm_task *self)
{
  bestcomm_free(self->tdt_begin);
  self->tdt_begin = NULL;
}

static inline void bestcomm_task_clear_pragmas(const bestcomm_task *self)
{
  volatile bestcomm_task_entry *entry = bestcomm_task_get_task_entry(self);
  entry->fdt_and_pragmas &= ~0xffU;
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GEN5200_BESTCOMM_H */
