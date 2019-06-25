/*
 * Copyright (c) 2016-2017 Chris Johns <chrisj@rtems.org>.
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

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include <rtems.h>
#include <rtems/assoc.h>
#include <rtems/score/threadimpl.h>

#include <rtems/debugger/rtems-debugger-server.h>

#include "rtems-debugger-target.h"
#include "rtems-debugger-threads.h"

static const char * const excludes_defaults[] =
{
  "TIME",
  "_BSD",
  "IRQS",
  "DBSs",
  "DBSe",
  "IDLE",
};

static void
rtems_debugger_thread_free(rtems_debugger_threads* threads)
{
  rtems_debugger_block_destroy(&threads->current);
  rtems_debugger_block_destroy(&threads->registers);
  rtems_debugger_block_destroy(&threads->excludes);
  rtems_debugger_block_destroy(&threads->stopped);
  rtems_debugger_block_destroy(&threads->steppers);
  threads->next = 0;
}

int
rtems_debugger_thread_create(void)
{
  rtems_debugger_threads* threads;
  int                     r;

  threads = calloc(1, sizeof(rtems_debugger_threads));
  if (threads == NULL) {
    errno = ENOMEM;
    rtems_debugger_printf("error: rtems-db: thread: threads alloc: (%d) %s\n",
                          errno, strerror(errno));
    return -1;
  }

  r = rtems_debugger_block_create(&threads->current,
                                  RTEMS_DEBUGGER_THREAD_BLOCK_SIZE,
                                  sizeof(rtems_debugger_thread));
  if (r < 0) {
    rtems_debugger_thread_free(threads);
    free(threads);
    rtems_debugger_printf("error: rtems-db: thread: current alloc: (%d) %s\n",
                          errno, strerror(errno));
    return -1;
  }

  r = rtems_debugger_block_create(&threads->registers,
                                  RTEMS_DEBUGGER_THREAD_BLOCK_SIZE,
                                  rtems_debugger_target_reg_table_size());
  if (r < 0) {
    rtems_debugger_thread_free(threads);
    free(threads);
    rtems_debugger_printf("error: rtems-db: thread: registers alloc: (%d) %s\n",
                          errno, strerror(errno));
    return -1;
  }

  r = rtems_debugger_block_create(&threads->excludes,
                                  RTEMS_DEBUGGER_THREAD_BLOCK_SIZE,
                                  sizeof(rtems_id));
  if (r < 0) {
    rtems_debugger_thread_free(threads);
    free(threads);
    rtems_debugger_printf("error: rtems-db: thread: exlcudes alloc: (%d) %s\n",
                          errno, strerror(errno));
    return -1;
  }

  r = rtems_debugger_block_create(&threads->stopped,
                                  RTEMS_DEBUGGER_THREAD_BLOCK_SIZE,
                                  sizeof(rtems_id));
  if (r < 0) {
    rtems_debugger_thread_free(threads);
    free(threads);
    rtems_debugger_printf("error: rtems-db: thread: stopped alloc: (%d) %s\n",
                          errno, strerror(errno));
    return -1;
  }

  r = rtems_debugger_block_create(&threads->steppers,
                                  RTEMS_DEBUGGER_THREAD_BLOCK_SIZE,
                                  sizeof(rtems_debugger_thread_stepper));
  if (r < 0) {
    rtems_debugger_thread_free(threads);
    free(threads);
    rtems_debugger_printf("error: rtems-db: thread: steppers alloc: (%d) %s\n",
                          errno, strerror(errno));
    return -1;
  }

  rtems_debugger->threads = threads;

  return rtems_debugger_thread_system_suspend();
}

int
rtems_debugger_thread_destroy(void)
{
  rtems_debugger_threads* threads = rtems_debugger->threads;
  rtems_debugger_thread_system_resume(true);
  rtems_debugger_thread_free(threads);
  free(threads);
  rtems_debugger->threads = NULL;
  return 0;
}

int
rtems_debugger_thread_find_index(rtems_id id)
{
  rtems_debugger_threads* threads = rtems_debugger->threads;
  rtems_debugger_thread*  current = rtems_debugger_thread_current(threads);
  int                     r = -1;
  if (threads != NULL) {
    size_t i;
    for (i = 0; i < threads->current.level; ++i) {
      if (id == 0 || current[i].id == id) {
        r = i;
        break;
      }
    }
  }
  return r;
}

static bool
snapshot_thread(rtems_tcb* tcb, void* arg)
{
  rtems_debugger_threads* threads = rtems_debugger->threads;
  rtems_id                id = tcb->Object.id;
  char                    name[RTEMS_DEBUGGER_THREAD_NAME_SIZE];
  bool                    exclude = false;
  size_t                  i;

  /*
   * The only time the threads pointer is NULL is a realloc error so we stop
   * processing threads. There is no way to stop the iterator.
   */
  if (rtems_debugger_thread_current(threads) == NULL)
    return true;

  /*
   * Filter the threads.
   */
  switch (rtems_object_id_get_api(id)) {
  case OBJECTS_NO_API:
  case OBJECTS_INTERNAL_API:
    exclude = true;
    break;
  default:
    rtems_object_get_name(id, sizeof(name), (char*) &name[0]);
    for (i = 0; i < RTEMS_DEBUGGER_NUMOF(excludes_defaults); ++i) {
      if (strcmp(excludes_defaults[i], name) == 0) {
        exclude = true;
        break;
      }
    }
    break;
  }

  if (exclude) {
    rtems_id* excludes;
    int       r;
    r = rtems_debugger_block_resize(&threads->excludes);
    if (r < 0) {
      rtems_debugger_thread_free(threads);
      return true;
    }
    excludes = rtems_debugger_thread_excludes(threads);
    excludes[threads->excludes.level++] = id;
  }
  else {
    rtems_debugger_thread* current;
    uint8_t*               registers;
    rtems_debugger_thread* thread;
    int                    r;

    r = rtems_debugger_block_resize(&threads->current);
    if (r < 0) {
      rtems_debugger_thread_free(threads);
      return true;
    }
    r = rtems_debugger_block_resize(&threads->registers);
    if (r < 0) {
      rtems_debugger_thread_free(threads);
      return true;
    }

    current = rtems_debugger_thread_current(threads);
    registers = rtems_debugger_thread_registers(threads);

    thread = &current[threads->current.level++];
    thread->registers =
        &registers[threads->registers.level++ * rtems_debugger_target_reg_table_size()];

    thread->tcb    = tcb;
    thread->id     = id;
    thread->flags  = 0;
    thread->signal = 0;
    thread->frame  = NULL;
    memcpy((void*) &thread->name[0], &name[0], sizeof(thread->name));

    /*
     * See if there is a valid exception stack frame and if the thread is being
     * debugged.
     */
    rtems_debugger_target_exception_thread(thread);

    /*
     * Exception threads have stopped for breakpoint, segv or other errors.
     */
    if (rtems_debugger_thread_flag(thread,
                                   RTEMS_DEBUGGER_THREAD_FLAG_EXCEPTION)) {
      rtems_id* stopped;
      r = rtems_debugger_block_resize(&threads->stopped);
      if (r < 0) {
        rtems_debugger_thread_free(threads);
        return true;
      }
      stopped = rtems_debugger_thread_stopped(threads);
      stopped[threads->stopped.level++] = id;
    }
    else {
      rtems_status_code sc;
      sc = rtems_task_suspend(id);
      if (sc != RTEMS_SUCCESSFUL && sc != RTEMS_ALREADY_SUSPENDED) {
        rtems_debugger_printf("error: rtems-db: thread: suspend: %08" PRIx32 ": %s\n",
                              id, rtems_status_text(sc));
        r = -1;
      }
    }

    /*
     * Read the target registers into the thread register array.
     */
    rtems_debugger_target_read_regs(thread);

    if (rtems_debugger_server_flag(RTEMS_DEBUGGER_FLAG_VERBOSE))
      rtems_debugger_printf("rtems-db: sys: thd: %08" PRIx32 ": signal: %d\n",
                            id, thread->signal);

    /*
     * Pick up the first non-zero signal.
     */
    if (rtems_debugger->signal == 0) {
      rtems_debugger->signal = thread->signal;
    }
  }

  return false;
}

int
rtems_debugger_thread_system_suspend(void)
{
  rtems_debugger_threads* threads = rtems_debugger->threads;
  int                     r = -1;
  if (threads != NULL && rtems_debugger_thread_current(threads) != NULL) {
    if (rtems_debugger_verbose())
      rtems_debugger_printf("rtems-db: sys:    : suspending\n");
    r = rtems_debugger_target_swbreak_remove();
    if (r == 0)
      r = rtems_debugger_target_hwbreak_remove();
    if (r == 0) {
      rtems_debugger_thread* current;
      threads->current.level = 0;
      threads->registers.level = 0;
      threads->stopped.level = 0;
      threads->excludes.level = 0;
      threads->steppers.level = 0;
      rtems_task_iterate(snapshot_thread, NULL);
      current = rtems_debugger_thread_current(threads);
      if (current == NULL) {
        rtems_debugger_printf("error: rtems-db: thread: snapshot: (%d) %s\n",
                              errno, strerror(errno));
        r = -1;
      }
      else {
        rtems_id* stopped;
        /*
         * If there are no stopped threads pick the first one in the current
         * table and return that.
         */
        threads->selector_gen = 0;
        threads->selector_cont = 0;
        stopped = rtems_debugger_thread_stopped(threads);
        if (threads->stopped.level == 0 && threads->current.level > 0) {
          stopped[threads->stopped.level++] = current[0].id;
        }
        if (threads->stopped.level > 0) {
          threads->selector_gen =
            rtems_debugger_thread_find_index(stopped[0]);
          if (threads->selector_gen < 0)
            threads->selector_gen = 0;
        }
      }
    }
    else {
      errno = EIO;
    }
  }
  return r;
}

int
rtems_debugger_thread_system_resume(bool detaching)
{
  rtems_debugger_threads* threads = rtems_debugger->threads;
  rtems_debugger_thread*  current;
  int                     r = 0;
  current = rtems_debugger_thread_current(threads);
  if (threads != NULL && current != NULL) {
    size_t i;
    if (rtems_debugger_verbose())
      rtems_debugger_printf("rtems-db: sys:    : resuming\n");
    if (!detaching) {
      r = rtems_debugger_target_swbreak_insert();
      if (r == 0)
        r = rtems_debugger_target_hwbreak_insert();
    }
    if (r == 0) {
      for (i = 0; i < threads->current.level; ++i) {
        rtems_debugger_thread* thread = &current[i];
        rtems_status_code      sc;
        int                    rr;
        bool                   has_exception;
        /*
         * Check if resuming, which can be continuing, a step, or stepping a
         * range.
         */
        if (detaching ||
            rtems_debugger_thread_flag(thread,
                                       RTEMS_DEBUGGER_THREAD_FLAG_RESUME)) {
          if (!detaching) {
            rr = rtems_debugger_target_write_regs(thread);
            if (rr < 0 && r == 0)
              r = rr;
            if (rtems_debugger_thread_flag(thread,
                                           RTEMS_DEBUGGER_THREAD_FLAG_STEP_INSTR)) {
              rr = rtems_debugger_target_thread_stepping(thread);
              if (rr < 0 && r == 0)
                r = rr;
            }
          }
          has_exception =
            rtems_debugger_thread_flag(thread,
                                       RTEMS_DEBUGGER_THREAD_FLAG_EXCEPTION);
          if (rtems_debugger_verbose())
            rtems_debugger_printf("rtems-db: sys:    : resume: 0x%08" PRIx32 " %c\n",
                                  thread->id, has_exception ? 'E' : ' ');
          if (has_exception) {
              rtems_debugger_target_exception_thread_resume(thread);
          } else {
              sc = rtems_task_resume(thread->id);
              if (sc != RTEMS_SUCCESSFUL) {
                  rtems_debugger_printf("error: rtems-db: thread: resume: %08" PRIx32 ": %s\n",
                                        thread->id, rtems_status_text(sc));
              }
          }
          thread->flags &= ~(RTEMS_DEBUGGER_THREAD_FLAG_CONTINUE |
                             RTEMS_DEBUGGER_THREAD_FLAG_STEP);
          thread->signal = 0;
        }
      }
      /*
       * Excludes are not cleared so the exception handler can find the
       * excluded thread.
       */
      threads->current.level = 0;
      threads->registers.level = 0;
      threads->stopped.level = 0;
    }
    else {
      r = -1;
      errno = EIO;
    }
  }
  return r;
}

int
rtems_debugger_thread_continue(rtems_debugger_thread* thread)
{
  thread->flags |= RTEMS_DEBUGGER_THREAD_FLAG_CONTINUE;
  return 0;
}

int
rtems_debugger_thread_continue_all(void)
{
  rtems_debugger_threads* threads = rtems_debugger->threads;
  rtems_debugger_thread*  current;
  int                     r = 0;
  current = rtems_debugger_thread_current(threads);
  if (threads != NULL && current != NULL) {
    size_t i;
    for (i = 0; i < threads->current.level; ++i) {
      rtems_debugger_thread* thread = &current[i];
      if (!rtems_debugger_thread_flag(thread,
                                      RTEMS_DEBUGGER_THREAD_FLAG_STEP_INSTR)) {
        r = rtems_debugger_thread_continue(thread);
        if (r < 0)
          break;
      }
    }
  }
  else {
    r = -1;
    errno = EIO;
  }
  return r;
}

int
rtems_debugger_thread_step(rtems_debugger_thread* thread)
{
  thread->flags |= RTEMS_DEBUGGER_THREAD_FLAG_STEP;
  return 0;
}

int
rtems_debugger_thread_stepping(rtems_debugger_thread* thread,
                               DB_UINT                start,
                               DB_UINT                end)
{
  /* add lock */
  rtems_debugger_threads*        threads = rtems_debugger->threads;
  rtems_debugger_thread_stepper* stepper;
  int                            r;
  /*
   * The resize will automatically extend the block when we are full. The
   * steppers are cleared in suspend by setting the level to 0.
   */
  r = rtems_debugger_block_resize(&threads->steppers);
  if (r < 0) {
    rtems_debugger_thread_free(threads);
    return -1;
  }
  stepper = rtems_debugger_thread_steppers(threads);
  stepper = &stepper[threads->steppers.level];
  stepper->thread = thread;
  stepper->start = start;
  stepper->end = end;
  threads->steppers.level++;
  thread->flags |= RTEMS_DEBUGGER_THREAD_FLAG_STEPPING;
  return 0;
}

const rtems_debugger_thread_stepper*
rtems_debugger_thread_is_stepping(rtems_id id, DB_UINT pc)
{
  /* add lock */
  rtems_debugger_threads*        threads = rtems_debugger->threads;
  rtems_debugger_thread_stepper* stepper;
  size_t                         i;
  stepper = rtems_debugger_thread_steppers(threads);
  for (i = 0; i < threads->steppers.level; ++i, ++stepper) {
    if (stepper->thread->id == id) {
      if (pc == stepper->start || (pc > stepper->start && pc < stepper->end))
        return stepper;
      break;
    }
  }
  return NULL;
}

int
rtems_debugger_thread_current_priority(rtems_debugger_thread* thread)
{
  return _Thread_Get_unmapped_priority(thread->tcb);
}

int
rtems_debugger_thread_real_priority(rtems_debugger_thread* thread)
{
  return _Thread_Get_unmapped_real_priority(thread->tcb);
}

int
rtems_debugger_thread_state(rtems_debugger_thread* thread)
{
  return thread->tcb->current_state;
}

int
rtems_debugger_thread_state_str(rtems_debugger_thread* thread,
                                char*                  buf,
                                size_t                 size)
{
  rtems_assoc_thread_states_to_string(thread->tcb->current_state, buf, size);
  return 0;
}

unsigned long
rtems_debugger_thread_stack_size(rtems_debugger_thread* thread)
{
  return thread->tcb->Start.Initial_stack.size;
}

void*
rtems_debugger_thread_stack_area(rtems_debugger_thread* thread)
{
  return thread->tcb->Start.Initial_stack.area;
}
