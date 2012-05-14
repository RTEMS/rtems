/** @file rtems/stackchk.h
 *
 *  This include file contains information necessary to utilize
 *  and install the stack checker mechanism.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_STACKCHK_H
#define _RTEMS_STACKCHK_H

#include <stdbool.h> /* bool */

#include <rtems/score/percpu.h> /* Thread_Control */
#include <rtems/bspIo.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief Has Current Task Blown Its Stack
 *
 *  This method is used to determine if the current stack pointer
 *  of the currently executing task is within bounds.
 *
 *  @return This method returns true if the currently executing task
 *  has blown its stack.
 *
 */
bool rtems_stack_checker_is_blown( void );

/**
 *  @brief Print Stack Usage Report
 *
 *  This method prints a stack usage report for the curently executing
 *  task.
 *
 *  @note It uses printk to print the report.
 */
void rtems_stack_checker_report_usage( void );

/**
 *  @brief Print Stack Usage Report
 *
 *  This method prints a stack usage report for the curently executing
 *  task.
 *
 *  @param[in] context is the context to pass to the print handler
 *  @param[in] print is the print handler
 *
 *  @note It uses the caller's routine to print the report.
 */
void rtems_stack_checker_report_usage_with_plugin(
  void                  *context,
  rtems_printk_plugin_t  print
);

/*************************************************************
 *************************************************************
 **  Prototyped only so the user extension can be installed **
 *************************************************************
 *************************************************************/

/**
 *  @brief Stack Checker Task Create Extension
 *
 * This method is the task create extension for the stack checker.
 *
 * @param[in] running points to the currently executing task
 * @param[in] the_thread points to the newly created task
 *
 * @note If this this the first task created, the stack checker
 *       will automatically intialize itself.
 */
bool rtems_stack_checker_create_extension(
  Thread_Control *running,
  Thread_Control *the_thread
);

/**
 *  @brief Stack Checker Task Begin Extension
 *
 * This method is the task begin extension for the stack checker.
 *
 * @param[in] the_thread points to task starting to execute
 *
 * @note This is called from the internal method _Thread_Handler.
 */
void rtems_stack_checker_begin_extension(
  Thread_Control *the_thread
);

/**
 *  @brief Stack Checker Task Context Switch Extension
 *
 * This method is the task context switch extension for the stack checker.
 *
 * @param[in] running points to the currently executing task which
 *            is being context switched out
 * @param[in] running points to the heir task which we are switching to
 *
 * @note This is called from the internal method _Thread_Dispatch.
 */
void rtems_stack_checker_switch_extension(
  Thread_Control *running,
  Thread_Control *heir
);

/**
 *  @brief Stack Checker Extension Set Definition
 *
 *  This macro defines the user extension handler set for the stack
 *  checker.  This macro is normally only used by confdefs.h.
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
  0 /* rtems_stack_checker_fatal_extension */, /* fatal        */ \
}

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
