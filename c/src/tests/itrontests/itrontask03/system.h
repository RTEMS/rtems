/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
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

#include <tmacros.h>
#include <itron.h>

/* functions */

void ITRON_Init( void );
void Preempt_task();
void Task_1();
void Task_2();
void Task_3();

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_ITRON_TASKS      6

#define CONFIGURE_ITRON_INIT_TASK_TABLE

#include <confdefs.h>

/* global variables */

#define PREEMPT_TASK_ID         2
#define TA1_ID                  3
#define TA2_ID                  4
#define TA3_ID                  5


#define PREEMPT_PRIORITY        1
TEST_EXTERN rtems_id Global_variable;   /* example global variable     */

/* end of include file */
