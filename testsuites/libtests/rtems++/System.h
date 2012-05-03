/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <tmacros.h>
#include <rtems++/rtemsEvent.h>
#include <rtems++/rtemsMessageQueue.h>
#include <rtems++/rtemsTask.h>
#include <rtems++/rtemsTaskMode.h>

/* functions */

extern "C"
{
  rtems_task Init(
    rtems_task_argument argument
    );
}

rtems_timer_service_routine Delayed_routine(
  rtems_id  ignored_id,
  void     *ignored_address
);

class Task1
  : public rtemsTask
{
  void print_mode(rtems_mode mode, rtems_mode mask);

  void screen1(void);
  void screen2(void);
  void screen3(void);
  void screen4(void);
  void screen5(void);
  void screen6(void);

protected:
  virtual void body(rtems_task_argument argument);

public:
};

class Task2
  : public rtemsTask
{
  void screen4(void);

protected:
  virtual void body(rtems_task_argument argument);

public:
  Task2(const char* name,
        const rtems_task_priority initial_priority,
        const uint32_t   stack_size);
};

class Task3
  : public rtemsTask
{
  void screen6(void);

protected:
  virtual void body(rtems_task_argument argument);

public:
  Task3(const char* name,
        const rtems_task_priority initial_priority,
        const uint32_t   stack_size);
};

class EndTask
  : public rtemsTask
{
protected:
  virtual void body(rtems_task_argument argument);

public:
  EndTask(const char* name,
          const rtems_task_priority initial_priority,
          const uint32_t   stack_size);
};

#if 0

//
// Not sure this can be tested in a generic manner, any ideas anyone !!
//

class Service_routine
  : public rtemsInterrupt
{
};

class Io_during_interrupt
  : pubic rtemsTimer
{

};

#endif

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS               8
#define CONFIGURE_MAXIMUM_TIMERS              1
#define CONFIGURE_MAXIMUM_SEMAPHORES          2
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES      1
#define CONFIGURE_MAXIMUM_PARTITIONS          1
#define CONFIGURE_MAXIMUM_REGIONS             1
#define CONFIGURE_MAXIMUM_PERIODS             1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS     0
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_STACK_SIZE      (4 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_EXTRA_TASK_STACKS         (13 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

/* end of include file */
