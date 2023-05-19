/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2010, 2013 embedded brains GmbH & Co. KG
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
