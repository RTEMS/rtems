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
 */

#include <tmacros.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_timer_service_routine Signal_3_to_task_1(
  rtems_id  id,
  void     *pointer
);

rtems_asr Process_asr(
  rtems_signal_set the_signal_set
);

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Task_2(
  rtems_task_argument argument
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS               3
#define CONFIGURE_MAXIMUM_TIMERS              1
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (1 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 4 ];         /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 4 ];       /* array of task names */

TEST_EXTERN rtems_id   Timer_id[ 3 ];      /* array of timer ids */
TEST_EXTERN rtems_name Timer_name[ 3 ];    /* array of timer names */

TEST_EXTERN volatile uint32_t   Signals_sent;
                                      /* set to TRUE to indicate that a */
                                      /* signal set has been sent from  */
                                      /* an ISR to the executing task   */

TEST_EXTERN volatile uint32_t   Asr_fired;
                                      /* set to TRUE to indicate that the */
                                      /* RTEMS_ASR has executed and was   */
                                      /* passed the correct signal set    */

TEST_EXTERN volatile rtems_id  Timer_got_this_id;

TEST_EXTERN volatile void     *Timer_got_this_pointer;

/* end of include file */
