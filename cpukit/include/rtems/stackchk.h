/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libmisc_stackchk
 *
 * @brief This header file provides the Stack Checker API.
 */

/*
 * COPYRIGHT (c) 1989-2024 On-Line Applications Research Corporation (OAR).
 * COPYRIGHT (c) 2024 Mohamed Hassan <muhammad.hamdy.hassan@gmail.com>
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

#ifndef _RTEMS_STACKCHK_H
#define _RTEMS_STACKCHK_H

#include <rtems.h>
#include <rtems/print.h>

/**
 *  @defgroup libmisc_stackchk Stack Checker Mechanism
 *
 *  @ingroup RTEMSAPIClassic
 */
/**@{*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Checks if current task is blown its stack.
 *
 * This method is used to determine if the current stack pointer
 * of the currently executing task is within bounds.
 *
 * @retval This method returns true if the currently executing task
 *         has blown its stack.
 */
bool rtems_stack_checker_is_blown( void );

/**
 * @brief Print the stack usage report using printk.
 *
 * This method prints a stack usage report for the curently executing
 * task.
 *
 * @note It uses printk to print the report.
 */
void rtems_stack_checker_report_usage( void );

/**
 * @brief Print the stack usage report using caller's routine.
 *
 * This method prints a stack usage report for the curently executing
 * task.
 *
 * @param[in] context is the context to pass to the print handler
 * 
 * @param[in] print is the print handler
 *
 * @note It uses the caller's routine to print the report.
 */
void rtems_stack_checker_report_usage_with_plugin(
  const rtems_printer *printer
);

/**
 * @brief Stack information provided by the stack checker.
 */
typedef struct {
  /**
   * This member contains the object identifier associated with the
   * object using the stack.
   * 
   * For interrupt stacks, the object identifier is the processor index.
   */
  rtems_id id;

  /**
   * This member provides the object name associated with the
   * object using the stack.
   * 
   * For interrupt stacks, the object name is "Interrupt Stack".
   */
  const char *name;

  /**
   * This member provides the begin address of the stack area.
   */
  const void *begin;

  /**
   *  This member contains the size in bytes of the stack area.
   */
  uintptr_t size;

  /**
   * This member provides the current stack pointer of the stack.
   * 
   * If the current stack pointer is not available, then the value is 
   * set to NULL.
   */
  const void *current;

  /**
   * This member contains the size in bytes of the used stack area.
   *
   * If the stack checker is not initialized, then the value is set to
   * UINTPTR_MAX.
   */
  uintptr_t used;
} rtems_stack_checker_info;

/**
 * @brief Visitor routines invoked by rtems_stack_checker_iterate() shall 
 *        have this type.
 *
 * @param[in] info is the stack information.
 *
 * @param[in] arg is the argument passed to rtems_stack_checker_iterate().
 */
typedef void ( *rtems_stack_checker_visitor )(
  const rtems_stack_checker_info *info,
  void                           *arg
);

/**
 * @brief Iterates over all stacks used by the system and invokes the visitor
 *        routine for each stack.
 *
 * This method prints a stack usage report for the curently executing
 * task.
 *
 * @param[in] visitor is the visitor routine invoked for each stack.
 *
 * @param[in] arg is the argument passed to each visitor routine invocation 
 *                during the iteration.
 */
void rtems_stack_checker_iterate( rtems_stack_checker_visitor visit, void *arg );

/*************************************************************
 *************************************************************
 **  Prototyped only so the user extension can be installed **
 *************************************************************
 *************************************************************/

/**
 * @brief Stack Checker Task Create Extension
 *
 * This method is the task create extension for the stack checker.
 *
 * @param[in] running points to the currently executing task
 * 
 * @param[in] the_thread points to the newly created task
 *
 * @note If this this the first task created, the stack checker
 *       will automatically intialize itself.
 */
bool rtems_stack_checker_create_extension(
  rtems_tcb *running,
  rtems_tcb *the_thread
);

void rtems_stack_checker_begin_extension( rtems_tcb *executing );

/**
 * @brief Stack Checker Task Context Switch Extension
 *
 * This method is the task context switch extension for the stack checker.
 *
 * @param[in] running points to the currently executing task which
 *                    is being context switched out
 * 
 * @param[in] heir points to the heir task which we are switching to
 *
 * @note This is called from the internal method _Thread_Dispatch.
 */
void rtems_stack_checker_switch_extension(
  rtems_tcb *running,
  rtems_tcb *heir
);

/**
 * @brief A Quiet Version of Stack Checker Reporter.
 * 
 * @param[in] running running points to the currently executing thread which
 *                    is being context switched out.
 * 
 * @param[in] pattern_ok bool variable to check if the pattern is
 *                       still valid or not
 */
 
void rtems_stack_checker_reporter_quiet(
  const rtems_tcb *running,
  bool pattern_ok
);

/**
 * @brief The Default Function to Report a Blown Stack.
 * 
 * @param[in] running running points to the currently executing thread which
 *                    is being context switched out.
 * 
 * @param[in] pattern_ok bool variable to check if the pattern is
 *                       still valid or not
 */
void rtems_stack_checker_reporter_print_details(
  const rtems_tcb *running,
  bool pattern_ok
);

/**
 *  @brief Stack Checker Extension Set Definition
 *
 *  This macro defines the user extension handler set for the stack
 *  checker. This macro is normally only used by confdefs.h.
 */
#define RTEMS_STACK_CHECKER_EXTENSION \
{ \
  rtems_stack_checker_create_extension,        /* rtems_task_create  */ \
  0,                                           /* rtems_task_start   */ \
  0,                                           /* rtems_task_restart */ \
  0,                                           /* rtems_task_delete  */ \
  rtems_stack_checker_switch_extension,        /* task_switch  */ \
  rtems_stack_checker_begin_extension,         /* task_begin   */ \
  0,                                           /* task_exitted */ \
  0,                                           /* fatal        */ \
  0                                            /* terminate    */ \
}

/**
 * @brief The Stack Checker Reporter Initialization Handler.
 * 
 * @param[in] running running points to the currently executing thread which
 *                    is being context switched out.
 * 
 * @param[in] pattern_ok bool variable to check if the pattern is
 *                       still valid or not.
 */
typedef void (*Stack_checker_Reporter_handler)(
  const rtems_tcb *running,
  bool pattern_ok
);

/**
 * @brief The Stack Checker Reporter Initialization Handler.
 * 
 * Application provided via <rtems/confdefs.h>
 */
extern const Stack_checker_Reporter_handler Stack_checker_Reporter;

#ifdef __cplusplus
}
#endif
/**@}*/
#endif
/* end of include file */
