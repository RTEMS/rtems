--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 22 of the RTEMS
--  Timing Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
--  On-Line Applications Research Corporation (OAR).
--  All rights assigned to U.S. Government, 1994.
--
--  This material may be reproduced by or for the U.S. Government pursuant
--  to the copyright license under the clause at DFARS 252.227-7013.  This
--  notice must appear in all copies of this file and its derivatives.
--
--  tmtest.ads,v 1.3 1995/07/12 19:44:11 joel Exp
--

with TIMER_DRIVER;
with RTEMS;

package TMTEST is

--
--  The following is the ID of the message queue used for timing operations.
--

   MESSAGE_QUEUE_ID : RTEMS.ID;

--
--  The following variable is set to the execution time returned
--  by the timer.
--

   END_TIME  : RTEMS.UNSIGNED32;

--
--  INIT
--
--  DESCRIPTION:
--
--  This RTEMS task initializes the application.
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  HIGH_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is created by LOW_TASK and executes after
--  LOW_TASK has performed a blocking MESSAGE_QUEUE_RECEIVE.
--  This task then executes and performs a readying 
--  MESSAGE_QUEUE_BROADCAST and reports its execution time.
--  Finally, the task suspends itself so LOW_TASK can continue
--  execution.
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  LOW_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task performs a blocking MESSAGE_QUEUE_RECEIVE
--  so that when HIGH_TASK performs a MESSAGE_QUEUE_BROADCAST,
--  there will be a task to be readied.  Following this, the 
--  execution time for a MESSAGE_QUEUE_BROADCAST to a message 
--  queue with no waiting tasks is measured and reported.
--  Then this task performs another blocking MESSAGE_QUEUE_RECEIVE
--  so that when PREEMPT_TASK performs a MESSAGE_QUEUE_BROADCAST,
--  PREEMPT_TASK will be preempted by this task.  After control
--  of the processor is transferred back to this task by the
--  preemption, the timer is stopped and the execution time of
--  a preemptive MESSAGE_QUEUE_BROADCAST is reported.
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  PREEMPT_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task starts the timer and performs a preemptive
--   MESSAGE_QUEUE_BROADCAST.
--

   procedure PREEMPT_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  This is the Initialization Tasks Table for this test.
--

   INITIALIZATION_TASKS : aliased RTEMS.INITIALIZATION_TASKS_TABLE( 1 .. 1 ) := 
   (1=>
     (
       RTEMS.BUILD_NAME( 'U', 'I', '1', ' ' ),        -- task name
       2048,                                          -- stack size
       1,                                             -- priority
       RTEMS.DEFAULT_ATTRIBUTES,                      -- attributes
       TMTEST.INIT'ACCESS,                            -- entry point
       RTEMS.NO_PREEMPT,                              -- initial mode
       0                                              -- argument list
     )
   );

--
--  This is the Configuration Table for this test.
--

   CONFIGURATION : aliased RTEMS.CONFIGURATION_TABLE := (
      RTEMS.NULL_ADDRESS,        -- will be replaced by BSP
      256 * 1024,                -- executive RAM size
      103,                       -- maximum # tasks
      0,                         -- maximum # timers
      0,                         -- maximum # semaphores
      1,                         -- maximum # message queues
      101,                       -- maximum # messages
      0,                         -- maximum # partitions
      0,                         -- maximum # regions
      0,                         -- maximum # dp memory areas
      0,                         -- maximum # periods
      0,                         -- maximum # user extensions
      RTEMS.MILLISECONDS_TO_MICROSECONDS(10), -- # us in a tick
      0                          -- # ticks in a timeslice
  );

end TMTEST;
