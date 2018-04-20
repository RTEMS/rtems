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

#define NDEBUG

#include <bsp/bestcomm.h>

#include <string.h>

#include <bsp/mpc5200.h>

static void bestcomm_irq_handler(void *arg)
{
  bestcomm_irq *self = arg;

  bestcomm_irq_clear(self);
  bestcomm_irq_wakeup_event_task(self);
}

void bestcomm_irq_create(bestcomm_irq *self, int task_index)
{
  assert(task_index >= 0 && task_index <= 15);

  self->task_index = task_index;
  self->event_task_id = rtems_task_self();
  bestcomm_glue_irq_install(task_index, bestcomm_irq_handler, self);
}

void bestcomm_irq_destroy(const bestcomm_irq *self)
{
  bestcomm_glue_irq_install(self->task_index, NULL, NULL);
}

void bestcomm_task_create(bestcomm_task *self, TaskId task_index)
{
  self->task_control_register = &mpc5200.sdma.tcr[task_index];
  self->variable_table = BESTCOMM_TASK_ENTRY_TABLE[task_index].var_table;
  self->task_index = task_index;
  self->tdt_begin = NULL;
  self->tdt_opcode_count = 0;
  bestcomm_task_stop(self);
  bestcomm_irq_create(&self->irq, task_index);
}

void bestcomm_task_create_and_load(
  bestcomm_task *self,
  TaskId task_index,
  const uint32_t *tdt_source_begin,
  size_t tdt_size
)
{
  bestcomm_task_create(self, task_index);
  bestcomm_task_load(self, tdt_source_begin, tdt_size);
}

void bestcomm_task_destroy(bestcomm_task *self)
{
  bestcomm_task_stop(self);
  bestcomm_task_free_tdt(self);
}

void bestcomm_task_load(bestcomm_task *self, const uint32_t *tdt_source_begin, size_t tdt_size)
{
  assert(tdt_size % 4 == 0);

  bestcomm_task_irq_disable(self);
  bestcomm_task_stop(self);
  bestcomm_task_irq_clear(self);
  bestcomm_task_irq_enable(self);
  bestcomm_task_free_tdt(self);
  bestcomm_task_clear_variables(self);

  self->tdt_opcode_count = tdt_size / 4;

  self->tdt_begin = bestcomm_malloc(tdt_size);
  assert(self->tdt_begin != NULL);
  uint32_t *tdt_last = self->tdt_begin + self->tdt_opcode_count - 1;

  memcpy(self->tdt_begin, tdt_source_begin, tdt_size);

  volatile bestcomm_task_entry *entry = bestcomm_task_get_task_entry(self);
  entry->tdt_begin = self->tdt_begin;
  entry->tdt_last = tdt_last;

  bestcomm_task_clear_pragmas(self);
  bestcomm_task_set_priority(self, 0);
}

void bestcomm_task_clear_variables(const bestcomm_task *self)
{
  int i;

  for (i = 0; i < 32; ++i) {
    (*self->variable_table)[i] = 0;
  }
}
