--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 16 of the RTEMS
--  Timing Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with RTEMS;
with RTEMS.TASKS;
with TIME_TEST_SUPPORT;

package TMTEST is

--
--  This array contains the IDs of all RTEMS tasks created by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 
      range 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT ) of RTEMS.ID;

--
--  This variable is used to index through the TASK_ID array
--  so that events can be send to the next task.
--

   TASK_COUNT : RTEMS.UNSIGNED32;

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
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, INIT);

--
--  TEST_INIT
--
--  DESCRIPTION:
--
--  This subprogram performs test initialization.  Once initialization
--  has been completed, this task sets the TASK_COUNT to 0 and
--  starts the timer before performing a preemptive EVENT_SEND.
--

   procedure TEST_INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TEST_INIT);

--
--  HIGH_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is highest priority task in the system.
--  Its first action is to perform a blocking EVENT_RECEIVE.
--  It is the last task to have its event set sent to it.  Thus it 
--  is the last task to be awakened by a preemptive EVENT_SEND.
--  After it preempts the last MIDDLE_TASKS, it stops the timer
--  and reports the execution time of preemptive EVENT_SEND.
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, HIGH_TASK);

--
--  MIDDLE_TASKS
--
--  DESCRIPTION:
--
--  These RTEMS tasks perform a blocking EVENT_RECEIVE.  When
--  readied, each task will increment the TASK_COUNT and
--  perform a preemptive EVENT_SEND.  TASK_COUNT is used to
--  index the TASK_ID array in such a way that incrementing
--  TASK_COUNT makes it index the ID of the next task to 
--  send events to. 
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, MIDDLE_TASKS);

end TMTEST;
