/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Copyright (c) 2015 Chris Johns <chrisj@rtems.org>
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

#include <rtems/trace/rtems-trace-buffer-vars.h>

#include <rtems.h>

ISR_LOCK_DEFINE (static, __rtld_tbg_lock, "RTLD TBG")

uint32_t
rtems_trace_names_size (void)
{
  return __rtld_trace_names_size;
}

const char*
rtems_trace_names (const uint32_t index)
{
  return __rtld_trace_names[index];
}

uint32_t
rtems_trace_enables_size (void)
{
  return __rtld_trace_enables_size;
}

uint32_t
rtems_trace_enables (const uint32_t index)
{
  return __rtld_trace_enables[index];
}

uint32_t
rtems_trace_triggers_size (void)
{
  return __rtld_trace_triggers_size;
}

uint32_t
rtems_trace_triggers (const uint32_t index)
{
  return __rtld_trace_triggers[index];
}

const rtems_trace_sig*
rtems_trace_signatures (const uint32_t index)
{
  return &__rtld_trace_signatures[index];
}

bool
rtems_trace_enable_set(const uint32_t index)
{
  return (__rtld_trace_enables[index / 32] & (1 << (index & (32 - 1)))) != 0 ? true : false;
}

bool
rtems_trace_trigger_set(const uint32_t index)
{
  return (__rtld_trace_triggers[index / 32] & (1 << (index & (32 - 1)))) != 0 ? true : false;
}

bool
rtems_trace_buffering_present (void)
{
  return __rtld_tbg_present;
}

uint32_t
rtems_trace_buffering_mode (void)
{
  return __rtld_tbg_mode;
}

uint32_t
rtems_trace_buffering_buffer_size (void)
{
  return __rtld_tbg_buffer_size;
}

uint32_t*
rtems_trace_buffering_buffer (void)
{
  return &__rtld_tbg_buffer[0];
}

uint32_t
rtems_trace_buffering_buffer_in (void)
{
  rtems_interrupt_lock_context lcontext;
  uint32_t                     in;
  rtems_interrupt_lock_acquire(&__rtld_tbg_lock, &lcontext);
  in = __rtld_tbg_buffer_in;
  rtems_interrupt_lock_release(&__rtld_tbg_lock, &lcontext);
  return in;
}

bool
rtems_trace_buffering_finished (void)
{
  rtems_interrupt_lock_context lcontext;
  bool                         finished;
  rtems_interrupt_lock_acquire(&__rtld_tbg_lock, &lcontext);
  finished = __rtld_tbg_finished;
  rtems_interrupt_lock_release(&__rtld_tbg_lock, &lcontext);
  return finished;
}

bool
rtems_trace_buffering_triggered (void)
{
  rtems_interrupt_lock_context lcontext;
  bool                         triggered;
  rtems_interrupt_lock_acquire(&__rtld_tbg_lock, &lcontext);
  triggered = __rtld_tbg_triggered;
  rtems_interrupt_lock_release(&__rtld_tbg_lock, &lcontext);
  return triggered;
}

void
rtems_trace_buffering_start (void)
{
  rtems_interrupt_lock_context lcontext;
  rtems_interrupt_lock_acquire(&__rtld_tbg_lock, &lcontext);
  __rtld_tbg_triggered = false;
  __rtld_tbg_buffer_in = 0;
  __rtld_tbg_finished = false;
  rtems_interrupt_lock_release(&__rtld_tbg_lock, &lcontext);
}

void
rtems_trace_buffering_stop (void)
{
  rtems_interrupt_lock_context lcontext;
  rtems_interrupt_lock_acquire(&__rtld_tbg_lock, &lcontext);
  __rtld_tbg_finished = true;
  rtems_interrupt_lock_release(&__rtld_tbg_lock, &lcontext);
}

void
rtems_trace_buffering_resume (void)
{
  rtems_interrupt_lock_context lcontext;
  rtems_interrupt_lock_acquire(&__rtld_tbg_lock, &lcontext);
  __rtld_tbg_finished = false;
  rtems_interrupt_lock_release(&__rtld_tbg_lock, &lcontext);
}
