/*  stackchk.h
 *
 *  This include file contains information necessary to utilize
 *  and install the stack checker mechanism.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __STACK_CHECK_h
#define __STACK_CHECK_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Stack_check_Initialize
 */

void Stack_check_Initialize( void );

/*
 *  Stack_check_Dump_usage
 */

void Stack_check_Dump_usage( void );

/*
 *  Stack_check_Create_extension
 */

boolean Stack_check_Create_extension(
  Thread_Control *running,
  Thread_Control *the_thread
);

/*
 *  Stack_check_Begin_extension
 */

void Stack_check_Begin_extension(
  Thread_Control *the_thread
);

/*
 *  Stack_check_Switch_extension
 */

void Stack_check_Switch_extension(
  Thread_Control *running,
  Thread_Control *heir
);

/*
 *  Extension set definition
 */

#define STACK_CHECKER_EXTENSION \
{ \
  Stack_check_Create_extension,        /* rtems_task_create  */ \
  0,                                   /* rtems_task_start   */ \
  0,                                   /* rtems_task_restart */ \
  0,                                   /* rtems_task_delete  */ \
  Stack_check_Switch_extension,        /* task_switch  */ \
  Stack_check_Begin_extension,         /* task_begin   */ \
  0,                                   /* task_exitted */ \
  0 /* Stack_check_Fatal_extension */, /* fatal        */ \
}

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
