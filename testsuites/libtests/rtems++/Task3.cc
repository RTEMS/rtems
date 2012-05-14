/*  Task_3
 *
 *  This routine serves as a test task. Loopback the messages and test
 *  timeouts
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2007.
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

Task3::Task3(const char* name,
             const rtems_task_priority initial_priority,
             const uint32_t   stack_size)
  : rtemsTask(name, initial_priority, stack_size, RTEMS_NO_PREEMPT)
{
}
  
void Task3::body(rtems_task_argument )
{
  screen6();

  printf("%s - destroy itself\n", name_string());
  destroy();
}

void Task3::screen6()
{
  rtemsMessageQueue mq_2("MQ2");
  printf("%s - construction connect mq_2 - %s\n", name_string(), mq_2.last_status_string());

  if (mq_2.successful())
  {
    char in[100];
    char out[100];
    size_t   size;
    bool loopback = true;
  
    while (loopback)
    {
    printf("%s - loopback from mq_2 to mq_2 ...\n", name_string()); fflush(stdout);

    mq_2.receive(in, size);
      printf("%s - mq_2 receive - %s, size=%zu, message string size=%zu\n",
             name_string(), mq_2.last_status_string(), size, strlen(in));
      if (mq_2.successful())
      {
        if (size > (100 - 5))
          printf("%s - size to large\n", name_string());
        else
        {
          strcpy(out, name_string());
          strcpy(out + 4, in);
      
          printf("%s - loopback to mq_2 - ", name_string());
          mq_2.send(out, strlen(out) + 1);
          printf("%s\n", mq_2.last_status_string());
        }

        if (strcmp(in, "broadcast message") == 0)
          loopback = false;
        else
          wake_after(1500000);
      }
    }
  }
}


