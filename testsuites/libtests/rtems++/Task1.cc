/*  Task1
 *
 *  This task is the main line for the test. It creates other
 *  tasks which can create
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1997
 *  Objective Design Systems Ltd Pty (ODS)
 *  All rights reserved (R) Objective Design Systems Ltd Pty
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

#include <stdlib.h>
#include <string.h>
#include "System.h"

/* c.f. cpukit/score/include/rtems/score/priority.h */
#define PRIiPriority_Control PRIi32
/* rtems_task_priority is a typedef to Priority_Control */
#define PRIirtems_task_priority PRIiPriority_Control

/* c.f. cpukit/rtems/include/rtems/rtems/modes.h */
#define PRIXModes_Control PRIX32
#define PRIiModes_Control PRIi32
/* rtems_mode is a typedef to Modes_Control */
#define PRIXrtems_mode PRIXModes_Control
#define PRIirtems_mode PRIiModes_Control

/* c.f. cpukit/score/include/rtems/score/isr.h */
#define PRIiISR_Level PRIi32
/* rtems_interrupt_level is a typedef to ISR_Level */
#define PRIirtems_interrupt_level PRIiISR_Level

void Task1::body(rtems_task_argument argument)
{
  rtems_test_pause_and_screen_number(1);
  
  printf(" * START Task Class test *\n");
  
  printf("%s - test argument - ", name_string());
  if (argument != 0xDEADDEAD)
    printf("argument is not 0xDEADDEAD\n");
  else
    printf("argument matched\n");
  
  screen1();
  rtems_test_pause_and_screen_number(2);

  screen2();
  rtems_test_pause_and_screen_number(3);
  
  screen3();
  rtems_test_pause_and_screen_number(4);
  
  screen4();
  rtems_test_pause_and_screen_number(5);
  
  screen5();
  rtems_test_pause_and_screen_number(6);
  
  screen6();

  // do not call exit(0) from this thread as this object is static
  // the static destructor call delete the task which is calling exit
  // so exit never completes
  
  EndTask end_task("ENDT", (rtems_task_priority) 1, RTEMS_MINIMUM_STACK_SIZE * 6);
  end_task.start(0);

  rtemsEvent block_me;
  rtems_event_set out;
  
  block_me.receive(RTEMS_SIGNAL_0, out);

  printf("**** TASK 1 did not block ????\n");
}

void Task1::screen1(void)
{
  // create two local task objects to connect to this task
  rtemsTask local_task_1 = *this;
  rtemsTask local_task_2;

  local_task_2 = *this;
  
  // check the copy constructor works
  printf("%s - copy constructor - ", name_string());
  if (local_task_1.id_is() == id_is())
    printf("local and this id's match\n");
  else
    printf("local and this id's do not match\n");

  printf("%s - copy constructor - ", name_string());
  if (local_task_1.name_is() == name_is())
    printf("local and this name's match\n");
  else
    printf("local and this name's do not match\n");

  // check the copy operator works
  printf("%s - copy operator - ", name_string());
  if (local_task_2.id_is() == id_is())
    printf("local and this id's match\n");
  else
    printf("local and this id's do not match\n");
  printf("%s - copy operator - ", name_string());
  if (local_task_2.name_is() == name_is())
    printf("local and this name's match\n");
  else
    printf("local and this name's do not match\n");
  
  // check that the owner of the id cannot delete this task
  printf("%s - not owner destroy's task - ", local_task_1.name_string());
  local_task_1.destroy();
  printf("%s\n", local_task_1.last_status_string());
  
  // connect to a valid task
  printf("%s - connect to a local valid task name - ", local_task_2.name_string());
  local_task_2.connect("TA1 ", RTEMS_SEARCH_ALL_NODES);
  printf("%s\n", local_task_2.last_status_string());
  
  // connect to an invalid task
  printf("%s - connect to an invalid task name - ", local_task_2.name_string());
  local_task_2.connect("BADT", RTEMS_SEARCH_ALL_NODES);
  printf("%s\n", local_task_2.last_status_string());
  
  // connect to a task an invalid node
  printf("%s - connect to a task on an invalid node - ", local_task_2.name_string());
  local_task_2.connect("BADT", 10);
  printf("%s\n", local_task_2.last_status_string());
  
  // restart this task
  printf("%s - restart from a non-owner - ", name_string());
  local_task_1.restart(0);
  printf("%s\n", local_task_1.last_status_string());
}

void Task1::screen2(void)
{
  // wake after using this object
  
  printf("%s - wake after 0 secs - ", name_string());
  wake_after(0);
  printf("%s\n", last_status_string());
  
  printf("%s - wake after 500 msecs - ", name_string());
  wake_after(500000);
  printf("%s\n", last_status_string());
  
  printf("%s - wake after 5 secs - ", name_string());
  wake_after(5000000);
  printf("%s\n", last_status_string());

  printf("%s - wake when - to do\n", name_string());

  rtemsTask task_1 = *this;
  
  // wake after using a connected object
  
  printf("%s - connected object wake after 0 secs - ", task_1.name_string());
  task_1.wake_after(0);
  printf("%s\n", task_1.last_status_string());
  
  printf("%s - connected object wake after 500 msecs - ", task_1.name_string());
  task_1.wake_after(500000);
  printf("%s\n", task_1.last_status_string());
  
  printf("%s - connected object wake after 5 secs - ", task_1.name_string());
  task_1.wake_after(5000000);
  printf("%s\n", task_1.last_status_string());

  printf("%s - connected object wake when - to do\n", task_1.name_string());

  rtemsTask task_2;
  
  // wake after using a self object
  
  printf("%s - self object wake after 0 secs - ", task_2.name_string());
  task_2.wake_after(0);
  printf("%s\n", task_2.last_status_string());
  
  printf("%s - self object wake after 500 msecs - ", task_2.name_string());
  task_2.wake_after(500000);
  printf("%s\n", task_2.last_status_string());
  
  printf("%s - self object wake after 5 secs - ", task_2.name_string());
  task_2.wake_after(5000000);
  printf("%s\n", task_2.last_status_string());

  printf("%s - self object wake when - to do\n", task_2.name_string());

  rtems_task_priority current_priority;
  rtems_task_priority priority;

  // priorities with this object
  
  printf("%s - get priority - ", name_string());
  get_priority(current_priority);
  printf("%s, priority is %" PRIirtems_task_priority "\n", last_status_string(), current_priority);

  printf("%s - set priority to 512 - ", name_string());
  set_priority(512);
  printf("%s\n", last_status_string());

  printf("%s - set priority to 25 - ", name_string());
  set_priority(25);
  printf("%s\n", last_status_string());

  printf("%s - set priority to original - ", name_string());
  set_priority(current_priority, priority);
  printf("%s, priority was %" PRIirtems_task_priority "\n", last_status_string(), priority);

  // priorities with connected object
    
  printf("%s - connected object get priority - ", task_1.name_string());
  task_1.get_priority(current_priority);
  printf("%s, priority is %" PRIirtems_task_priority "\n", task_1.last_status_string(), current_priority);

  printf("%s - connected object set priority to 512 - ", task_1.name_string());
  task_1.set_priority(512);
  printf("%s\n", task_1.last_status_string());

  printf("%s - connected object set priority to 25 - ", task_1.name_string());
  task_1.set_priority(25);
  printf("%s\n", task_1.last_status_string());

  printf("%s - connected object set priority to original - ", task_1.name_string());
  task_1.set_priority(current_priority, priority);
  printf("%s, priority was %" PRIirtems_task_priority "\n", task_1.last_status_string(), priority);  

  // priorities with self object
    
  printf("%s - self object get priority - ", task_2.name_string());
  task_2.get_priority(current_priority);
  printf("%s, priority is %" PRIirtems_task_priority "\n", task_2.last_status_string(), current_priority);

  printf("%s - self object set priority to 512 - ", task_2.name_string());
  task_2.set_priority(512);
  printf("%s\n", task_2.last_status_string());

  printf("%s - self object set priority to 25 - ", task_2.name_string());
  task_2.set_priority(25);
  printf("%s\n", task_2.last_status_string());

  printf("%s - self object set priority to original - ", task_2.name_string());
  task_2.set_priority(current_priority, priority);
  printf("%s, priority was %" PRIirtems_task_priority "\n", task_2.last_status_string(), priority);

  uint32_t   current_note;
  uint32_t   note;
  
  // notepad registers for this object

  printf("%s - get note - ", name_string());
  get_note(0, current_note);
  printf("%s, note is %" PRIi32 "\n", last_status_string(), current_note);

  printf("%s - get with bad notepad number - ", name_string());
  get_note(100, current_note);
  printf("%s, note is %" PRIi32 "\n", last_status_string(), current_note);

  printf("%s - set note to 0xDEADBEEF - ", name_string());
  set_note(0, 0xDEADBEEF);
  printf("%s\n", last_status_string());

  printf("%s - get note - ", name_string());
  get_note(0, note);
  printf("%s, note is 0x%08" PRIX32 "\n", last_status_string(), note);

  printf("%s - set note to original value - ", name_string());
  set_note(0, current_note);
  printf("%s\n", last_status_string());

  // notepad registers for connected object

  printf("%s - connected object get note - ", task_1.name_string());
  task_1.get_note(0, current_note);
  printf("%s, notepad is %" PRIi32 "\n", task_1.last_status_string(), current_note);

  printf("%s - connected object get with bad notepad number - ", task_1.name_string());
  task_1.get_note(100, current_note);
  printf("%s, note is %" PRIi32 "\n", task_1.last_status_string(), current_note);

  printf("%s - connected object set note to 0xDEADBEEF - ", task_1.name_string());
  task_1.set_note(0, 0xDEADBEEF);
  printf("%s\n", task_1.last_status_string());

  printf("%s - connected object get note - ", task_1.name_string());
  task_1.get_note(0, note);
  printf("%s, note is 0x%08" PRIX32 "\n", task_1.last_status_string(), note);

  printf("%s - connected object set note to original value - ", task_1.name_string());
  task_1.set_note(0, current_note);
  printf("%s\n", task_1.last_status_string());

  // notepad registers for self object

  printf("%s - self object get note - ", task_2.name_string());
  task_2.get_note(0, current_note);
  printf("%s, note is %" PRIi32 "\n", task_2.last_status_string(), current_note);

  printf("%s - self object get with bad notepad number - ", task_2.name_string());
  task_2.get_note(100, current_note);
  printf("%s, note is %" PRIi32 "\n", task_2.last_status_string(), current_note);

  printf("%s - self object set note to 0xDEADBEEF - ", task_2.name_string());
  task_2.set_note(0, 0xDEADBEEF);
  printf("%s\n", task_2.last_status_string());

  printf("%s - self object get note - ", task_2.name_string());
  task_2.get_note(0, note);
  printf("%s, notepad is 0x%08" PRIX32 "\n", task_2.last_status_string(), note);

  printf("%s - self object set note to original value - ", task_2.name_string());
  task_2.set_note(0, current_note);
  printf("%s\n", task_2.last_status_string());

  printf(" * END Task Class test *\n");
}

#define RTEMS_ALL_MODES (RTEMS_PREEMPT_MASK | \
                         RTEMS_TIMESLICE_MASK | \
                         RTEMS_ASR_MASK | \
                         RTEMS_INTERRUPT_MASK)

void Task1::screen3(void)
{
  printf(" * START TaskMode Class test *\n");

  rtemsTask self;
  rtemsTaskMode task_mode;
  rtems_mode current_mode;
  rtems_mode mode;

  printf("%s - get mode - ", self.name_string());
  task_mode.get_mode(current_mode);
  printf("%s,\n\t mode is 0x%08" PRIXrtems_mode ", ", task_mode.last_status_string(), current_mode);
  print_mode(current_mode, RTEMS_ALL_MODES);
  printf("\n");

  // PREEMPTION mode control
  
  printf("%s - get preemption state - ", self.name_string());
  task_mode.get_preemption_state(mode);
  printf("%s,\n\t mode is 0x%08" PRIXrtems_mode ", ", task_mode.last_status_string(), mode);
  print_mode(mode, RTEMS_PREEMPT_MASK);
  printf("\n");
  
  printf("%s - set preemption state to RTEMS_PREEMPT - ", self.name_string());
  task_mode.set_preemption_state(RTEMS_PREEMPT);
  task_mode.get_mode(mode);
  printf("%s,\n\t mode is 0x%08" PRIXrtems_mode ", ", task_mode.last_status_string(), mode);
  print_mode(mode, RTEMS_ALL_MODES);
  printf("\n");
  
  printf("%s - set preemption state to RTEMS_NO_PREEMPT - ", self.name_string());
  task_mode.set_preemption_state(RTEMS_NO_PREEMPT);
  task_mode.get_mode(mode);
  printf("%s,\n\t mode is 0x%08" PRIXrtems_mode ", ", task_mode.last_status_string(), mode);
  print_mode(mode, RTEMS_ALL_MODES);
  printf("\n");

  // TIMESLICE mode control
  
  printf("%s - get timeslice state - ", self.name_string());
  task_mode.get_timeslice_state(mode);
  printf("%s,\n\t mode is 0x%08" PRIXrtems_mode ", ", task_mode.last_status_string(), mode);
  print_mode(mode, RTEMS_TIMESLICE_MASK);
  printf("\n");
  
  printf("%s - set timeslice state to RTEMS_TIMESLICE - ", self.name_string());
  task_mode.set_timeslice_state(RTEMS_TIMESLICE);
  task_mode.get_mode(mode);
  printf("%s,\n\t mode is 0x%08" PRIXrtems_mode ", ", task_mode.last_status_string(), mode);
  print_mode(mode, RTEMS_ALL_MODES);
  printf("\n");
  
  printf("%s - set timeslice state to RTEMS_NO_TIMESLICE - ", self.name_string());
  task_mode.set_timeslice_state(RTEMS_NO_TIMESLICE);
  task_mode.get_mode(mode);
  printf("%s,\n\t mode is 0x%08" PRIXrtems_mode ", ", task_mode.last_status_string(), mode);
  print_mode(mode, RTEMS_ALL_MODES);
  printf("\n");

  // ASR mode control
  
  printf("%s - get asr state - ", self.name_string());
  task_mode.get_asr_state(mode);
  printf("%s,\n\t mode is 0x%08" PRIXrtems_mode ", ", task_mode.last_status_string(), mode);
  print_mode(mode, RTEMS_ASR_MASK);
  printf("\n");
  
  printf("%s - set asr state to RTEMS_ASR - ", self.name_string());
  task_mode.set_asr_state(RTEMS_ASR);
  task_mode.get_mode(mode);
  printf("%s,\n\t mode is 0x%08" PRIXrtems_mode ", ", task_mode.last_status_string(), mode);
  print_mode(mode, RTEMS_ALL_MODES);
  printf("\n");
  
  printf("%s - set asr state to RTEMS_NO_ASR - ", self.name_string());
  task_mode.set_asr_state(RTEMS_NO_ASR);
  task_mode.get_mode(mode);
  printf("%s,\n\t mode is 0x%08" PRIXrtems_mode ", ", task_mode.last_status_string(), mode);
  print_mode(mode, RTEMS_ALL_MODES);
  printf("\n");

  // interrupt level control
  
  rtems_interrupt_level current_level;
  rtems_interrupt_level level;

  printf("%s - get current interrupt level - ", self.name_string());
  task_mode.get_interrupt_level(current_level);
  printf("%s, level is %" PRIirtems_interrupt_level "\n", task_mode.last_status_string(), current_level);

  printf("%s - set interrupt level to 102 - ", self.name_string());
  task_mode.set_interrupt_level(102);
  printf("%s\n", task_mode.last_status_string());
  
  printf("%s - set interrupt level to original level - ", self.name_string());
  task_mode.set_interrupt_level(current_level, level);
  printf("%s, level was %" PRIirtems_interrupt_level "\n", task_mode.last_status_string(), level);

  printf("%s - set mode to original mode - ", self.name_string());
  task_mode.set_mode(current_mode,
                     RTEMS_PREEMPT_MASK | RTEMS_TIMESLICE_MASK |
                     RTEMS_ASR_MASK | RTEMS_INTERRUPT_MASK);
  task_mode.get_mode(mode);
  printf("%s,\n\t mode is 0x%08" PRIXrtems_mode ", ", task_mode.last_status_string(), mode);
  print_mode(mode, RTEMS_ALL_MODES);
  printf("\n");
  
  printf(" * END TaskMode Class test *\n");  
}

void Task1::screen4(void)
{
  printf(" * START Event Class test *\n");

  printf("%s - create task 2 - ", name_string());
  Task2 task_2("TA2", (rtems_task_priority) 9, RTEMS_MINIMUM_STACK_SIZE * 6);
  printf("%s\n", task_2.last_status_string());

  printf("%s - start task 2 - ", name_string());
  task_2.start(0);
  printf("%s\n", task_2.last_status_string());
  
  printf("%s - construct event connecting to task 2 - ", name_string());
  rtemsEvent event_2("TA2 ");
  printf("%s\n", event_2.last_status_string());

  // wait for task 2 to complete its timeout tests
  wake_after(7000000);

  printf("%s - send event signal 0 using the task id - ", name_string());
  event_2.send(task_2.id_is(), RTEMS_SIGNAL_0);
  printf("%s\n", event_2.last_status_string());
  
  wake_after(1000000);

  printf("%s - send event signal 0 using the task object reference - ", name_string());
  event_2.send(task_2, RTEMS_SIGNAL_0);
  printf("%s\n", event_2.last_status_string());
  
  wake_after(1000000);

  printf("%s - send event signal 31 using connected id - ", name_string());
  event_2.send(RTEMS_SIGNAL_31);
  printf("%s\n", event_2.last_status_string());
  
  wake_after(1000000);

  rtemsEvent event_2_2;

  event_2_2.connect("TA2");
  
  printf("%s - send event signal 0 and 31 - ", name_string());
  event_2_2.send(task_2, RTEMS_SIGNAL_0 | RTEMS_SIGNAL_31);
  printf("%s\n", event_2_2.last_status_string());
  
  printf("%s - waiting 5 secs for TA2 to finish\n", name_string());
  wake_after(500000);
  
  printf(" * END Event Class test *\n");  
}

void Task1::screen5(void)
{
  printf(" * START Interrupt Class test *\n");

  printf(" do not know a portable BSP type interrupt test\n");

  printf(" * END Interrupt Class test *\n");  
}

void Task1::screen6(void)
{
  printf(" * START MessageQueue Class test *\n");

  printf("%s - construct message queue 1 with no memory error - ", name_string());
  rtemsMessageQueue mq_1("MQ1", 1000000, 1000);
  printf("%s\n", mq_1.last_status_string());

  printf("%s - construct/create message queue 2 - ", name_string());
  rtemsMessageQueue mq_2("MQ2", 4, 50);
  printf("%s\n", mq_2.last_status_string());

  const char *u1 = "normal send";
  const char *u2 = "urgent send";
  char in[100];
  size_t  size;
  uint32_t count;
  
  printf("%s - send u1 to mq_2 - ", name_string());
  mq_2.send(u1, strlen(u1) + 1);
  printf("%s\n", mq_2.last_status_string());
  
  printf("%s - urgent send u2 to mq_2 - ", name_string());
  mq_2.urgent(u2, strlen(u2) + 1);
  printf("%s\n", mq_2.last_status_string());
  
  printf("%s - create task 3_1 - ", name_string());
  Task3 task_3_1("TA31", 9, RTEMS_MINIMUM_STACK_SIZE * 6);
  printf("%s\n", task_3_1.last_status_string());

  printf("%s - start task 3_1 - ", name_string());
  task_3_1.start(0);
  printf("%s\n", task_3_1.last_status_string());
  
  printf("%s - create task 3_2 - ", name_string());
  Task3 task_3_2("TA32", 9, RTEMS_MINIMUM_STACK_SIZE * 6);
  printf("%s\n", task_3_2.last_status_string());

  printf("%s - start task 3_2 - ", name_string());
  task_3_2.start(0);
  printf("%s\n", task_3_1.last_status_string());
  
  wake_after(1000000);
  
  printf("%s - receive u2 on mq_2 ...\n", name_string()); fflush(stdout);
  mq_2.receive(in, size, 5000000);
  printf("%s - %s\n", name_string(), mq_2.last_status_string());

  if (size == (strlen(u2) + 5))
  {
    if ((strncmp(in, task_3_1.name_string(), 4) == 0) &&
        (strcmp(in + 4, u2) == 0))
    {
      printf("%s - message u2 received correctly\n", name_string());
    }
    else
    {
      printf("%s - message u2 received incorrectly, message='%s', size=%zu\n",
             name_string(), in, size);
    }
  }
  else
    printf("%s - message u2 size incorrect, size=%zu\n", name_string(), size);

  printf("%s - receive u1 on mq_2 ...\n", name_string()); fflush(stdout);
  mq_2.receive(in, size, 5000000);
  printf("%s - %s\n", name_string(), mq_2.last_status_string());

  if (size == (strlen(u1) + 5))
  {
    if ((strncmp(in, task_3_2.name_string(), 4) == 0) &&
        (strcmp(in + 4, u1) == 0))
    {
      printf("%s - message u1 received correctly\n", name_string());
    }
    else
    {
      printf("%s - message u1 received incorrectly, message='%s', size=%zu\n",
             name_string(), in, size);
    }
  }
  else
    printf("%s - message u1 size incorrect, size=%zu\n", name_string(), size);

  wake_after(3000000);
  
  const char *b1 = "broadcast message";
  
  printf("%s - broadcast send b1 ...\n", name_string());
  mq_2.broadcast(b1, strlen(b1) + 1, count);
  printf("%s - mq_2 broadcast send - %s, count=%" PRIi32 "\n",
         name_string(), mq_2.last_status_string(), count);

  wake_after(1000000);
  
  printf("%s - receive message b1 on mq_2 from %s...\n",
         name_string(), task_3_1.name_string()); fflush(stdout);
  mq_2.receive(in, size, 5000000);
  printf("%s - %s\n", name_string(), mq_2.last_status_string());

  if (size == (strlen(b1) + 5))
  {
    if ((strncmp(in, task_3_1.name_string(), 4) == 0) &&
        (strcmp(in + 4, b1) == 0))
    {
      printf("%s - message b1 received correctly\n", name_string());
    }
    else
    {
      printf("%s - message b1 received incorrectly, message='%s'\n",
             name_string(), in);
    }
  }
  else
    printf("%s - message b1 size incorrect, size=%zu\n", name_string(), size);

  printf("%s - receive message b1 on mq_2 from %s...\n",
         name_string(), task_3_1.name_string()); fflush(stdout);
  mq_2.receive(in, size, 5000000);
  printf("%s - %s\n", name_string(), mq_2.last_status_string());

  if (size == (strlen(b1) + 5))
  {
    if ((strncmp(in, task_3_2.name_string(), 4) == 0) &&
        (strcmp(in + 4, b1) == 0))
    {
      printf("%s - message b1 received correctly\n", name_string());
    }
    else
    {
      printf("%s - message b1 received incorrectly, message='%s', size=%zu\n",
             name_string(), in, size);
    }
  }
  else
    printf("%s - message b1 size incorrect, size=%zu\n", name_string(), size);

  // wait for task 3_1, and 3_2 to complete their timeout tests, will
  // start these after getting the broadcast message
  wake_after(7000000);  

  const char *f1 = "flush message";
  
  printf("%s - send f1 to mq_2 - ", name_string());
  mq_2.send(f1, strlen(f1) + 1);
  printf("%s\n", mq_2.last_status_string());
  
  printf("%s - send f1 to mq_2 - ", name_string());
  mq_2.send(f1, strlen(f1) + 1);
  printf("%s\n", mq_2.last_status_string());
  
  printf("%s - send f1 to mq_2 - ", name_string());
  mq_2.send(f1, strlen(f1) + 1);
  printf("%s\n", mq_2.last_status_string());

  printf("%s - flush mq_2 - ", name_string());
  mq_2.flush(count);
  printf("%s, flushed=%" PRIi32 "\n", mq_2.last_status_string(), count);
  
  printf(" * END MessageQueue Class test *\n");  
}

void Task1::print_mode(rtems_mode mode, rtems_mode mask)
{
  rtemsTaskMode task_mode;
  if (mask & RTEMS_PREEMPT_MASK)
    printf("RTEMS_%sPREEMPT ",
           task_mode.preemption_set(mode) ? "" : "NO_");
  if (mask & RTEMS_TIMESLICE_MASK)
    printf("RTEMS_%sTIMESLICE ",
           task_mode.preemption_set(mode) ? "" : "NO_");
  if (mask & RTEMS_ASR_MASK)
    printf("RTEMS_%sASR ",
           task_mode.asr_set(mode) ? "" : "NO_");
  if (mask & RTEMS_INTERRUPT_MASK)
    printf("INTMASK=%" PRIirtems_mode,
           mode & RTEMS_INTERRUPT_MASK);
}

EndTask::EndTask(const char* name,
                 const rtems_task_priority initial_priority,
                 const uint32_t   stack_size)
  : rtemsTask(name, initial_priority, stack_size, RTEMS_NO_PREEMPT)
{
}
  
void EndTask::body(rtems_task_argument)
{
 printf("*** END OF RTEMS++ TEST ***\n");
 exit(0);
}

