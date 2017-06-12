/*
 *  Copyright (c) 2015 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>

#include <rtems/trace/rtems-trace-buffer-vars.h>

/**
 * External Trace Linker and TBG data. We provide weak versions to allow us to
 * link and be present in an application that has not been trace linked.
 */

/*
 * Trace linker data.
 */
uint32_t __rtld_trace_names_size __attribute__ ((weak));
const char *const __rtld_trace_names[1] __attribute__ ((weak));
uint32_t __rtld_trace_enables_size __attribute__ ((weak));
const uint32_t __rtld_trace_enables[1] __attribute__ ((weak));
uint32_t __rtld_trace_triggers_size __attribute__ ((weak));
const uint32_t __rtld_trace_triggers[1] __attribute__ ((weak));
const __rtld_trace_sig __rtld_trace_signatures[1] __attribute__ ((weak));

/*
 * Trace buffer generator data.
 */
const bool __rtld_tbg_present __attribute__ ((weak));
const uint32_t __rtld_tbg_mode __attribute__ ((weak));
const uint32_t __rtld_tbg_buffer_size __attribute__ ((weak));
uint32_t __rtld_tbg_buffer[1] __attribute__ ((weak));
volatile uint32_t __rtld_tbg_buffer_in __attribute__ ((weak));
volatile bool __rtld_tbg_finished __attribute__ ((weak));
volatile bool __rtld_tbg_triggered __attribute__ ((weak));

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
