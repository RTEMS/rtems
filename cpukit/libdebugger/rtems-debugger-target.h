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

#ifndef _RTEMS_DEBUGGER_TARGET_h
#define _RTEMS_DEBUGGER_TARGET_h

#include <setjmp.h>

#include <rtems/rtems-debugger.h>

#include "rtems-debugger-threads.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Software breakpoint block size.
 */
#define RTEMS_DEBUGGER_TARGET_SWBREAK_NUM 64

/**
 * Target capabilities mask.
 */
#define RTEMS_DEBUGGER_TARGET_CAP_SWBREAK      (1 << 0)
#define RTEMS_DEBUGGER_TARGET_CAP_HWBREAK      (1 << 1)
#define RTEMS_DEBUGGER_TARGET_CAP_HWWATCH      (1 << 2)
/*
 * This target capability indicates that the target implementation uses a pure
 * software break implementation which must not allow breakpoints to be
 * inserted before the actual switch to the thread, be it in interrupt context
 * or otherwise. Such implementations must necessarily implement a thread
 * switch hook and interrupt hooks to handle these situations.
 */
#define RTEMS_DEBUGGER_TARGET_CAP_PURE_SWBREAK (1 << 3)

/**
 * Types of hardware breakpoints.
 */
typedef enum rtems_debugger_target_watchpoint
{
  rtems_debugger_target_hw_read,
  rtems_debugger_target_hw_write,
  rtems_debugger_target_hw_read_write,
  rtems_debugger_target_hw_execute
} rtems_debugger_target_watchpoint;

/**
 * Target exception actions.
 */
typedef enum rtems_debugger_target_exc_action
{
  rtems_debugger_target_exc_consumed, /*<< The exception has been consumed. */
  rtems_debugger_target_exc_cascade,  /*<< Cascade to a previous handler. */
  rtems_debugger_target_exc_step,     /*<< Step an instruction. */
} rtems_debugger_target_exc_action;

/**
 * Memory breakpoint. We use thumb mode BKPT which is 2 bytes.
 */
#define RTEMS_DEBUGGER_TARGET_SWBREAK_MAX_SIZE (4)
typedef struct rtems_debugger_target_swbreak {
  void*   address;
  uint8_t contents[RTEMS_DEBUGGER_TARGET_SWBREAK_MAX_SIZE];
} rtems_debugger_target_swbreak;

/**
 * Target executable memory update handler. If set the handler is
 * called to modify the executable memory. The target's executable
 * memory may not be directly writable. The handler is responsible for
 * making sure the code update is ready to be executed, for example
 * caches are flushed.
 */
typedef int (*rtems_debugger_target_code_writer)(void*       address,
                                                 const void* data,
                                                 size_t      size);

/**
 * The target data.
 *
 * reg_offset: Table of size_t offset of a register in the register
 *             table. The table has one more entry than reg_num where
 *             the last entry is the size of the register table.
 */
typedef struct rtems_debugger_target {
  int                               capabilities;     /*<< The capabilities to report. */
  size_t                            reg_num;          /*<< The number of registers. */
  const size_t*                     reg_offset;       /*<< The reg offsettable, len = reg_num + 1. */
  const uint8_t*                    breakpoint;       /*<< The breakpoint instruction(s). */
  size_t                            breakpoint_size;  /*<< The breakpoint size. */
  rtems_debugger_block              swbreaks;         /*<< The software breakpoint block. */
  rtems_debugger_target_code_writer code_writer;      /*<< If set use to write to code memory */
  bool                              memory_access;    /*<< Accessing target memory. */
  jmp_buf                           access_return;    /*<< Return from an access fault. */
  uintptr_t                         step_bp_address;  /*<< Stepping break point address */
  rtems_id                          step_tid;         /*<< Stepping task id */

} rtems_debugger_target;

/**
 * Create the target.
 */
extern int rtems_debugger_target_create(void);

/**
 * Destroy the target.
 */
extern int rtems_debugger_target_destroy(void);

/**
 * Configure the target. This is architecture specific.
 */
extern int rtems_debugger_target_configure(rtems_debugger_target* target);

/**
 * Enable the target.
 */
extern int rtems_debugger_target_enable(void);

/**
 * Disable the target.
 */
extern int rtems_debugger_target_disable(void);

/**
 * Return the capabilities mask for the target.
 */
extern uint32_t rtems_debugger_target_capabilities(void);

/**
 * Return the number of regisers.
 */
extern size_t rtems_debugger_target_reg_num(void);

/**
 * Return the offset of a register in the register table.
 */
extern size_t rtems_debugger_target_reg_size(size_t reg);

/**
 * Return the offset of a register in the register table.
 */
extern size_t rtems_debugger_target_reg_offset(size_t reg);

/**
 * Return the size of register table.
 */
extern size_t rtems_debugger_target_reg_table_size(void);

/**
 * Read the regosters.
 */
extern int rtems_debugger_target_read_regs(rtems_debugger_thread* thread);

/**
 * Write the regosters.
 */
extern int rtems_debugger_target_write_regs(rtems_debugger_thread* thread);

/**
 * Return the thread's program counter (PC).
 */
extern uintptr_t rtems_debugger_target_reg_pc(rtems_debugger_thread* thread);

/**
 * Return the frame's program counter (PC).
 */
extern uintptr_t rtems_debugger_target_frame_pc(CPU_Exception_frame* frame);

/**
 * Return the thread's stack pointer (SP).
 */
extern uintptr_t rtems_debugger_target_reg_sp(rtems_debugger_thread* thread);

/**
 * Return the thread's TCB stack pointer (SP).
 */
extern uintptr_t rtems_debugger_target_tcb_sp(rtems_debugger_thread* thread);

/**
 * The thread is stepping. Setup the thread to step an instruction.
 */
extern int rtems_debugger_target_thread_stepping(rtems_debugger_thread* thread);

/**
 * Return the signal for the exception.
 */
extern int rtems_debugger_target_exception_to_signal(CPU_Exception_frame* frame);

/**
 * Print the target exception registers.
 */
extern void rtems_debugger_target_exception_print(CPU_Exception_frame* frame);

/**
 * Software breakpoints. These are also referred to as memory breakpoints.
 */
extern int rtems_debugger_target_swbreak_control(bool    insert,
                                                 uintptr_t addr,
                                                 DB_UINT kind);

/**
 * Insert software breakpoints into the memory.
 */
extern int rtems_debugger_target_swbreak_insert(void);

/**
 * Remove software breakpoints from the memory.
 */
extern int rtems_debugger_target_swbreak_remove(void);

/**
 * Determine whether a software breakpoint is configured for the given address.
 */
extern bool rtems_debugger_target_swbreak_is_configured( uintptr_t addr );

/**
 * Insert hardware breakpoints into the hardware.
 */
extern int rtems_debugger_target_hwbreak_insert(void);

/**
 * Remove hardware breakpoints from the hardware.
 */
extern int rtems_debugger_target_hwbreak_remove(void);

/**
 * Hardware breakpoints.
 */
extern int rtems_debugger_target_hwbreak_control(rtems_debugger_target_watchpoint type,
                                                 bool                             insert,
                                                 uintptr_t                        addr,
                                                 DB_UINT                          kind);

/**
 * Target exception processor.
 */
extern rtems_debugger_target_exc_action
rtems_debugger_target_exception(CPU_Exception_frame* frame);

/**
 * See if the thread is an exception thread.
 */
extern void rtems_debugger_target_exception_thread(rtems_debugger_thread* thread);

/**
 * If the thread is an exception thread, resume it.
 */
extern void rtems_debugger_target_exception_thread_resume(rtems_debugger_thread* thread);

/**
 * Target instruction cache sync. This depends on the target but it normally
 * means a data cache flush and an instruction cache invalidate.
 */
extern int rtems_debugger_target_cache_sync(rtems_debugger_target_swbreak* swbreak);

/**
 * Start a target memory access. If 0 is return the access can proceed and if
 * -1 is return the access has failed.
 *
 * The call to setjmp() must not reside inside a stack frame relative to the
 * expected location of the possible failure. If the debugger is already
 * executing in exception context and setjmp is called with its own stack frame,
 * that stack frame will be released before the failure. The failure causes an
 * exception which will continue using the same stack and is highly likely to
 * overwrite stack information which is critical for the debugger to continue
 * execution.
 */
#define rtems_debugger_target_start_memory_access() \
  ({ \
    rtems_debugger->target->memory_access = true, \
    setjmp(rtems_debugger->target->access_return); \
  })

/**
 * End a target memory access.
 */
extern void rtems_debugger_target_end_memory_access(void);

/**
 * Is this a target memory access?
 */
extern bool rtems_debugger_target_is_memory_access(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
