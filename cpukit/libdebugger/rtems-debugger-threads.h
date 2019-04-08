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

/*
 * Debugger for RTEMS.
 */

#ifndef _RTEMS_DEBUGGER_THREADS_h
#define _RTEMS_DEBUGGER_THREADS_h

#include <rtems/debugger/rtems-debugger-server.h>

#include <rtems/score/thread.h>

#include "rtems-debugger-block.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Debugger thread name size, fixed size. ASCIIZ format.
 */
#define RTEMS_DEBUGGER_THREAD_NAME_SIZE (5)

/**
 * Debugger thread allocation block size.
 */
#define RTEMS_DEBUGGER_THREAD_BLOCK_SIZE (32)

/**
 * Debugger thread flags.
 */
#define RTEMS_DEBUGGER_THREAD_FLAG_EXCEPTION      (1 << 0)
#define RTEMS_DEBUGGER_THREAD_FLAG_REG_VALID      (1 << 1)
#define RTEMS_DEBUGGER_THREAD_FLAG_REG_DIRTY      (1 << 2)
#define RTEMS_DEBUGGER_THREAD_FLAG_CONTINUE       (1 << 3)
#define RTEMS_DEBUGGER_THREAD_FLAG_STEP           (1 << 4)
#define RTEMS_DEBUGGER_THREAD_FLAG_STEPPING       (1 << 5)
#define RTEMS_DEBUGGER_THREAD_FLAG_INTS_DISABLED  (1 << 6)
/* Target specific flags for use by the target backend. */
#define RTEMS_DEBUGGER_THREAD_FLAG_TARGET_BASE    (24)
#define RTEMS_DEBUGGER_THREAD_FLAG_TARGET_MASK    (0xff << RTEMS_DEBUGGER_THREAD_FLAG_TARGET_BASE)

/**
 * Resume this thread.
 */
#define RTEMS_DEBUGGER_THREAD_FLAG_RESUME \
  (RTEMS_DEBUGGER_THREAD_FLAG_CONTINUE | \
   RTEMS_DEBUGGER_THREAD_FLAG_STEP | \
   RTEMS_DEBUGGER_THREAD_FLAG_STEPPING)

/**
 * Step an instruction.
 */
#define RTEMS_DEBUGGER_THREAD_FLAG_STEP_INSTR \
  (RTEMS_DEBUGGER_THREAD_FLAG_STEP | \
   RTEMS_DEBUGGER_THREAD_FLAG_STEPPING)

/**
 * Debugger thread.
 */
typedef struct rtems_debugger_thread
{
  uint32_t        flags;
  const char      name[RTEMS_DEBUGGER_THREAD_NAME_SIZE];
  Thread_Control* tcb;
  rtems_id        id;
  int             cpu;
  uint8_t*        registers;
  int             signal;
  void*           frame;
} rtems_debugger_thread;

/**
 * Debugger stepping thread. This is a thread that steps while inside an
 * address range.
 */
typedef struct rtems_debugger_thread_stepper
{
  rtems_debugger_thread* thread;
  DB_UINT                start;
  DB_UINT                end;
} rtems_debugger_thread_stepper;

/**
 * Debugger thread control.
 */
struct rtems_debugger_threads
{
  rtems_debugger_block current;       /**< The threads currently available. */
  rtems_debugger_block registers;     /**< The threads that have stopped. */
  rtems_debugger_block excludes;      /**< The threads we cannot touch. */
  rtems_debugger_block stopped;       /**< The threads that have stopped. */
  rtems_debugger_block steppers;      /**< The threads that are stepping. */
  size_t               next;          /**< An iterator. */
  int                  selector_gen;  /**< General thread selector. */
  int                  selector_cont; /**< Continue thread selector. */
};

/**
 * Create the thread support.
 */
extern int rtems_debugger_thread_create(void);

/**
 * Destroy the thread support.
 */
extern int rtems_debugger_thread_destroy(void);

/**
 * Find the index in the thread table for the ID.
 */
extern int rtems_debugger_thread_find_index(rtems_id id);

/**
 * Suspend the system.
 */
extern int rtems_debugger_thread_system_suspend(void);

/**
 * Resume the system.
 */
extern int rtems_debugger_thread_system_resume(bool detaching);

/**
 * Continue all threads.
 */
extern int rtems_debugger_thread_continue_all(void);

/**
 * Continue a thread.
 */
extern int rtems_debugger_thread_continue(rtems_debugger_thread* thread);

/**
 * Step a thread.
 */
extern int rtems_debugger_thread_step(rtems_debugger_thread* thread);

/**
 * Thread is stepping so record the details.
 */
extern int rtems_debugger_thread_stepping(rtems_debugger_thread* thread,
                                          DB_UINT                start,
                                          DB_UINT                end);

/**
 * Thread's PC in the stepping range? Returns the stepper is in range else
 * NULL.
 */
extern const rtems_debugger_thread_stepper*
rtems_debugger_thread_is_stepping(rtems_id id, DB_UINT pc);

/**
 * Return the thread's current priority/
 */
extern int rtems_debugger_thread_current_priority(rtems_debugger_thread* thread);

/**
 * Return the thread's real priority.
 */
extern int rtems_debugger_thread_real_priority(rtems_debugger_thread* thread);

/**
 * Return the thread's state.
 */
extern int rtems_debugger_thread_state(rtems_debugger_thread* thread);

/**
 * Return the thread's state.
 */
//extern bool rtems_debugger_thread_state_debugger(rtems_debugger_thread* thread);

/**
 * Return a string of the thread's state.
 */
extern int rtems_debugger_thread_state_str(rtems_debugger_thread* thread,
                                           char*                  buffer,
                                           size_t                 size);

/**
 * Return the thread's stack size.
 */
extern unsigned long rtems_debugger_thread_stack_size(rtems_debugger_thread* thread);

/**
 * Return the thread's stack area address.
 */
extern void* rtems_debugger_thread_stack_area(rtems_debugger_thread* thread);

/**
 * Check a thread's flag and return true if any of the bits in the mask are
 * set.
 */
static inline bool
rtems_debugger_thread_flag(rtems_debugger_thread* thread, uint32_t mask)
{
  return (thread->flags & mask) != 0;
}

/**
 * Get the current threads.
 */
static inline rtems_debugger_thread*
rtems_debugger_thread_current(rtems_debugger_threads* threads)
{
  return threads->current.block;
}

/**
 * Get the registers.
 */
static inline uint8_t*
rtems_debugger_thread_registers(rtems_debugger_threads* threads)
{
  return threads->registers.block;
}

/**
 * Get the excludes.
 */
static inline rtems_id*
rtems_debugger_thread_excludes(rtems_debugger_threads* threads)
{
  return threads->excludes.block;
}

/**
 * Get the stopped.
 */
static inline rtems_id*
rtems_debugger_thread_stopped(rtems_debugger_threads* threads)
{
  return threads->stopped.block;
}

/**
 * Get the steppers.
 */
static inline rtems_debugger_thread_stepper*
rtems_debugger_thread_steppers(rtems_debugger_threads* threads)
{
  return threads->steppers.block;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
