/*
 * Copyright (c) 2016-2019 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define TARGET_DEBUG 0

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/score/threadimpl.h>

#include "rtems-debugger-target.h"
#include "rtems-debugger-threads.h"

/**
 * Exception local stack frame data to synchronise with the debugger
 * server's events loop processor.
 */
typedef struct {
  rtems_chain_node     node;
  rtems_id             id;
  CPU_Exception_frame* frame;
  rtems_rx_cond        cond;
} rtems_debugger_exception;

#if TARGET_DEBUG
#include <rtems/bspIo.h>
static void target_printk(const char* format, ...) RTEMS_PRINTFLIKE(1, 2);
static void
target_printk(const char* format, ...)
{
  rtems_interrupt_lock_context lock_context;
  va_list ap;
  va_start(ap, format);
  rtems_debugger_printk_lock(&lock_context);
  vprintk(format, ap);
  rtems_debugger_printk_unlock(&lock_context);
  va_end(ap);
}
#else
#define target_printk(_fmt, ...)
#endif

int
rtems_debugger_target_create(void)
{
  if (rtems_debugger->target == NULL) {
    rtems_debugger_target* target;
    int                    r;

    target = calloc(1, sizeof(rtems_debugger_target));
    if (target == NULL) {
      errno = ENOMEM;
      return -1;
    }

    r = rtems_debugger_target_configure(target);
    if (r < 0) {
      free(target);
      return -1;
    }

    if (target->breakpoint_size > RTEMS_DEBUGGER_TARGET_SWBREAK_MAX_SIZE) {
      free(target);
      rtems_debugger_printf("error: rtems-db: target: breakpoint size too big\n");
      return -1;
    }

    r = rtems_debugger_block_create(&target->swbreaks,
                                    RTEMS_DEBUGGER_TARGET_SWBREAK_NUM,
                                    sizeof(rtems_debugger_target_swbreak));
    if (r < 0) {
      free(target);
      return -1;
    }

    rtems_debugger->target = target;
  }

  return 0;
}

int
rtems_debugger_target_destroy(void)
{
  if (rtems_debugger->target != NULL) {
    rtems_debugger_target* target = rtems_debugger->target;
    rtems_debugger_target_swbreak_remove();
    rtems_debugger_target_disable();
    rtems_debugger_block_destroy(&target->swbreaks);
    free(target);
    rtems_debugger->target = NULL;
  }
  return 0;
}

uint32_t
rtems_debugger_target_capabilities(void)
{
  if (rtems_debugger->target != NULL)

    return rtems_debugger->target->capabilities;
  return 0;
}

size_t
rtems_debugger_target_reg_num(void)
{
  rtems_debugger_target* target = rtems_debugger->target;
  if (target != NULL)
    return target->reg_num;
  return 0;
}

size_t
rtems_debugger_target_reg_size(size_t reg)
{
  rtems_debugger_target* target = rtems_debugger->target;
  if (target != NULL && reg < target->reg_num)
    return target->reg_offset[reg + 1] - target->reg_offset[reg];
  return 0;
}

size_t
rtems_debugger_target_reg_offset(size_t reg)
{
  rtems_debugger_target* target = rtems_debugger->target;
  if (target != NULL && reg < target->reg_num)
    return target->reg_offset[reg];
  return 0;
}

size_t
rtems_debugger_target_reg_table_size(void)
{
  rtems_debugger_target* target = rtems_debugger->target;
  if (target != NULL)
    return target->reg_offset[target->reg_num];
  return 0;
}

int
rtems_debugger_target_swbreak_control(bool insert, DB_UINT addr, DB_UINT kind)
{
  rtems_debugger_target*         target = rtems_debugger->target;
  rtems_debugger_target_swbreak* swbreaks = target->swbreaks.block;
  size_t                         swbreak_size;
  uint8_t*                       loc = (void*) addr;
  size_t                         i;
  int                            r;

  if (target == NULL || swbreaks == NULL || kind != target->breakpoint_size) {
    errno = EIO;
    return -1;
  }

  swbreak_size =
    sizeof(rtems_debugger_target_swbreak) + target->breakpoint_size;

  for (i = 0; i < target->swbreaks.level; ++i) {
    if (loc == swbreaks[i].address) {
      size_t remaining;
      if (!insert) {
        --target->swbreaks.level;
        remaining = (target->swbreaks.level - i) * swbreak_size;
        memmove(&swbreaks[i], &swbreaks[i + 1], remaining);
      }
      return 0;
    }
  }

  if (!insert)
    return 0;

  r = rtems_debugger_block_resize(&target->swbreaks);
  if (r < 0)
    return -1;

  swbreaks = target->swbreaks.block;

  swbreaks[target->swbreaks.level].address = loc;
  if (target->breakpoint_size > 4)
    memcpy(&swbreaks[target->swbreaks.level].contents[0],
           loc,
           target->breakpoint_size);
  else {
    uint8_t* contents = &swbreaks[target->swbreaks.level].contents[0];
    switch (target->breakpoint_size) {
    case 4:
      contents[3] = loc[3];
    case 3:
      contents[2] = loc[2];
    case 2:
      contents[1] = loc[1];
    case 1:
      contents[0] = loc[0];
      break;
    }
  }
  ++target->swbreaks.level;

  return 0;
}

int
rtems_debugger_target_swbreak_insert(void)
{
  rtems_debugger_target* target = rtems_debugger->target;
  int                    r = -1;
  if (target != NULL && target->swbreaks.block != NULL) {
    rtems_debugger_target_swbreak* swbreaks = target->swbreaks.block;
    size_t                         i;
    r = 0;
    for (i = 0; i < target->swbreaks.level; ++i) {
      uint8_t* loc = swbreaks[i].address;
      if (rtems_debugger_verbose())
        rtems_debugger_printf("rtems-db:  bp:  in: %p\n", swbreaks[i].address);
      if (target->breakpoint_size > 4)
        memcpy(loc, &target->breakpoint[0], target->breakpoint_size);
      else {
        if (rtems_debugger_verbose())
          rtems_debugger_printf("rtems-db:  bp:  in: %p %p %d %d %d\n",
                                loc, &target->breakpoint[0],
                                (int) target->breakpoint_size,
                                (int) i, (int) target->swbreaks.level);
        switch (target->breakpoint_size) {
        case 4:
          loc[3] = target->breakpoint[3];
        case 3:
          loc[2] = target->breakpoint[2];
        case 2:
          loc[1] = target->breakpoint[1];
        case 1:
          loc[0] = target->breakpoint[0];
          break;
        }
      }
      r = rtems_debugger_target_cache_sync(&swbreaks[i]);
    }
  }
  return r;
}

int
rtems_debugger_target_swbreak_remove(void)
{
  rtems_debugger_target* target = rtems_debugger->target;
  int                    r = -1;
  if (target != NULL && target->swbreaks.block != NULL) {
    rtems_debugger_target*         target = rtems_debugger->target;
    rtems_debugger_target_swbreak* swbreaks = target->swbreaks.block;
    size_t                         i;
    r = 0;
    for (i = 0; i < target->swbreaks.level; ++i) {
      uint8_t* loc = swbreaks[i].address;
      uint8_t* contents = &swbreaks[i].contents[0];
      if (rtems_debugger_verbose())
        rtems_debugger_printf("rtems-db:  bp: out: %p\n", swbreaks[i].address);
      if (target->breakpoint_size > 4)
        memcpy(loc, contents, target->breakpoint_size);
      else {
        switch (target->breakpoint_size) {
        case 4:
          loc[3] = contents[3];
        case 3:
          loc[2] = contents[2];
        case 2:
          loc[1] = contents[1];
        case 1:
          loc[0] = contents[0];
          break;
        }
      }
      r = rtems_debugger_target_cache_sync(&swbreaks[i]);
    }
  }
  return r;
}

rtems_debugger_target_exc_action
rtems_debugger_target_exception(CPU_Exception_frame* frame)
{
  if (!rtems_interrupt_is_in_progress()) {
    rtems_debugger_threads*              threads = rtems_debugger->threads;
    Thread_Control*                      thread = _Thread_Get_executing();
    const rtems_id                       tid = thread->Object.id;
    rtems_id*                            excludes;
    DB_UINT                              pc;
    const rtems_debugger_thread_stepper* stepper;
    rtems_debugger_exception             target_exception;
    size_t                               i;

    target_printk("[} tid:%08" PRIx32 ": thread:%08" PRIxPTR
                  " frame:%08" PRIxPTR "\n",
                  tid, (intptr_t) thread, (intptr_t) frame);

    rtems_debugger_lock();

    /*
     * If the thread is in the debugger recover. If the access is from gdb
     * continue else shutdown and let the user know.
     */
    if (tid == rtems_debugger->server_task ||
        tid == rtems_debugger->events_task) {
      bool memory_access = rtems_debugger_target_is_memory_access();
      rtems_debugger_unlock();
      /*
       * Has GDB has asked us to write to an address?
       */
      if (memory_access) {
        target_printk("[} server fault: memory access\n");
        longjmp(rtems_debugger->target->access_return, -1);
      }
      rtems_debugger_printf("rtems-db: server exception (report)\n");
      rtems_debugger_target_exception_print(frame);
      rtems_debugger_server_crash();
      return rtems_debugger_target_exc_cascade;
    }

    /*
     * See if the thread is excluded.
     */
    excludes = rtems_debugger_thread_excludes(threads);
    for (i = 0; i < threads->excludes.level; ++i) {
      if (tid == excludes[i]) {
        /*
         * We do nothing with this condition and cascade the exception.
         *
         * @todo: if this is a hwbreak carry on, if this is a swbreak replace
         *        the contents of the instruction, step then return the
         *        swbreak's contents.
         */
        target_printk("[} tid:%08" PRIx32 ": excluded\n", tid);
        rtems_debugger_unlock();
        return rtems_debugger_target_exc_cascade;
      }
    }

    /*
     * See if the thread is inside the stepping a range.
     */
    pc = rtems_debugger_target_frame_pc(frame);
    stepper = rtems_debugger_thread_is_stepping(tid, pc);
    if (stepper != NULL) {
      stepper->thread->frame = frame;
      rtems_debugger_target_thread_stepping(stepper->thread);
      target_printk("[} tid:%08" PRIx32 ": stepping\n", tid);
      rtems_debugger_unlock();
      return rtems_debugger_target_exc_step;
    }

    target_printk("[} tid:%08" PRIx32 ": suspending\n", tid);

    /*
     * Initialise the target exception data and queue ready for the debugger
     * server's event processor to handle.
     */
    rtems_chain_initialize_node(&target_exception.node);
    target_exception.frame = frame;
    target_exception.id = tid;
    _Condition_Initialize(&target_exception.cond);

    rtems_chain_append_unprotected(&rtems_debugger->exception_threads,
                                   &target_exception.node);

    /*
     * Signal the debug server's thread.
     */
    rtems_debugger_server_events_signal();

    /*
     * Block on the exception thread's condition variable unlocking the
     * debugger's mutex and letting the server's thread run.
     */
    _Condition_Wait_recursive(&target_exception.cond, &rtems_debugger->lock);

    /*
     * Unlock the debugger's lock now the exception is resuming.
     */
    rtems_debugger_unlock();

    target_printk("[} tid:%08" PRIx32 ": resuming\n", tid);

    return rtems_debugger_target_exc_consumed;
  }

  rtems_debugger_printf("rtems-db: exception in an interrupt, cascading\n");

  return rtems_debugger_target_exc_cascade;
}

void
rtems_debugger_target_exception_thread(rtems_debugger_thread* thread)
{
  rtems_chain_node* node;
  thread->frame = NULL;
  thread->flags &= ~RTEMS_DEBUGGER_THREAD_FLAG_EXCEPTION;
  for (node = rtems_chain_first(&rtems_debugger->exception_threads);
       !rtems_chain_is_tail(&rtems_debugger->exception_threads, node);
       node = rtems_chain_next(node)) {
    rtems_debugger_exception* target_exception = (rtems_debugger_exception*) node;
    if (target_exception->id == thread->id) {
      thread->frame = target_exception->frame;
      thread->flags |= RTEMS_DEBUGGER_THREAD_FLAG_EXCEPTION;
    }
  }
}

void
rtems_debugger_target_exception_thread_resume(rtems_debugger_thread* thread)
{
  rtems_chain_node* node;
  for (node = rtems_chain_first(&rtems_debugger->exception_threads);
       !rtems_chain_is_tail(&rtems_debugger->exception_threads, node);
       node = rtems_chain_next(node)) {
    rtems_debugger_exception* target_exception = (rtems_debugger_exception*) node;
    if (target_exception->id == thread->id) {
      rtems_chain_extract(node);
      thread->frame = NULL;
      thread->flags &= ~RTEMS_DEBUGGER_THREAD_FLAG_EXCEPTION;
      _Condition_Signal(&target_exception->cond);
      break;
    }
  }
}

int
rtems_debugger_target_start_memory_access(void)
{
  rtems_debugger_target* target = rtems_debugger->target;
  target->memory_access = true;
  return setjmp(target->access_return);
}

void
rtems_debugger_target_end_memory_access(void)
{
  rtems_debugger->target->memory_access = false;
}

bool
rtems_debugger_target_is_memory_access(void)
{
  return rtems_debugger->target->memory_access;
}
