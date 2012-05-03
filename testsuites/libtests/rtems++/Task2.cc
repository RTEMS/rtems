/*  Task_2
 *
 *  This routine serves as a test task.  Its only purpose is to generate the
 *  error where a semaphore is deleted while a task is waiting for it.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "System.h"

Task2::Task2(const char* name,
             const rtems_task_priority initial_priority,
             const uint32_t   stack_size)
  : rtemsTask(name, initial_priority, stack_size, RTEMS_NO_PREEMPT)
{
}
  
void Task2::body(rtems_task_argument )
{
  screen4();

  printf("%s - destroy itself\n", name_string());
  destroy();
}

void Task2::screen4()
{
  rtemsEvent event;

  // block waiting for any event
  rtems_event_set out;
  
  printf("%s - event no wait - ", name_string());
  event.receive(RTEMS_SIGNAL_0, out, 0, rtemsEvent::no_wait);
  printf("%s\n", event.last_status_string());
  
  printf("%s - event 5 secs timeout - ", name_string()); fflush(stdout);
  event.receive(RTEMS_SIGNAL_0, out, 5000000);
  printf("%s\n", event.last_status_string());

  // send using task id
  printf("%s - event wait forever for signal 0 from TA1 ....\n", name_string());
  event.receive(RTEMS_SIGNAL_0, out);
  printf("%s - %s, signals out are 0x%08" PRIX32 "\n", name_string(), event.last_status_string(), out);

  // send using task object reference
  printf("%s - event wait forever for signal 0 from TA1 ....\n", name_string());
  event.receive(RTEMS_SIGNAL_0, out);
  printf("%s - %s, signals out are 0x%08" PRIX32 "\n", name_string(), event.last_status_string(), out);
  
  printf("%s - event wait forever for signal 31 from TA1 ....\n", name_string());
  event.receive(RTEMS_SIGNAL_31, out);
  printf("%s - %s, signals out are 0x%08" PRIX32 "\n", name_string(), event.last_status_string(), out);
  
  printf("%s - event wait forever for signal 0 and 31 from TA1 ....\n", name_string());
  event.receive(RTEMS_SIGNAL_0 | RTEMS_SIGNAL_31, out, 0, rtemsEvent::wait, rtemsEvent::all);
  printf("%s - %s, signals out are 0x%08" PRIX32 "\n", name_string(), event.last_status_string(), out);

  printf("%s - send event signal 1 - ", name_string());
  event.send(RTEMS_SIGNAL_1);
  printf("%s\n", event.last_status_string());
  
  printf("%s - event wait forever for signal 1 from TA2 - ", name_string());
  event.receive(RTEMS_SIGNAL_1, out, 0, rtemsEvent::wait, rtemsEvent::all);
  printf("%s, signals out are 0x%08" PRIX32 "\n", event.last_status_string(), out);  
}


