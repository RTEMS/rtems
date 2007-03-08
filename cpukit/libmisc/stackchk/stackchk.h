/*  stackchk.h
 *
 *  This include file contains information necessary to utilize
 *  and install the stack checker mechanism.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_STACK_CHECKER_h
#define __RTEMS_STACK_CHECKER_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  rtems_stack_checker_initialize
 */

void rtems_stack_checker_initialize( void );

/*
 *  rtems_stack_checker_Dump_usage
 */

void rtems_stack_checker_report_usage( void );

/*
 *  rtems_stack_checker_create_extension
 */

boolean rtems_stack_checker_create_extension(
  Thread_Control *running,
  Thread_Control *the_thread
);

/*
 *  rtems_stack_checker_begin_extension
 */

void rtems_stack_checker_begin_extension(
  Thread_Control *the_thread
);

/*
 *  rtems_stack_checker_switch_extension
 */

void rtems_stack_checker_switch_extension(
  Thread_Control *running,
  Thread_Control *heir
);

/*
 *  Extension set definition
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
