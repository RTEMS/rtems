--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 26 of the RTEMS
--  Timing Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR).
--  Copyright assigned to U.S. Government, 1994.
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.OARcorp.com/rtems/license.html.
--
--  $Id$
--

with TIMER_DRIVER;
with RTEMS;

package TMTEST is

--
--  This points to the RTEMS task control block for the MIDDLE_TASK.
--

   MIDDLE_TCB   : RTEMS.TCB_POINTER;

--
--  The following is the ID of the semaphore used for timing operations.
--

   SEMAPHORE_ID : RTEMS.ID;

--
--  The following variable is set to the execution time returned
--  by the timer.
--

   END_TIME     : RTEMS.UNSIGNED32;

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
--  NULL_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task has a NULL body and is only included to 
--  allow TASK_START to be invoked without impacting another
--  task.
--

   procedure NULL_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  HIGH_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is the highest priority test task and 
--  thus executes first.  It is responsible for measuring
--  and reporting the following times:
--
--    + THREAD_DISABLE_DISPATCH
--    + THREAD_ENABLE_DISPATCH
--    + THREAD_SET_STATE
--
--  After measuring and reporting the above times, this task
--  starts the timer and forces dispatch to the MIDDLE_TASK.
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  MIDDLE_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task stops the timer and reports the execution
--  time for THREAD_DISPATCH without any floating point context
--  save and restore involved.  Following this, it starts the
--  timer and forces a CONTEXT_SWITCH to the LOW_TASK.
--

   procedure MIDDLE_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  LOW_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task stops the timer and reports the execution
--  time for a raw CONTEXT_SWITCH without any floating point context
--  save and restore involved.  Following this, it starts the
--  timer and performs a CONTEXT_RESTORE_FP for the FLOATING_POINT_TASK_1
--  and a CONTEXT_SWITCH to FLOATING_POINT_TASK_1.
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  FLOATING_POINT_TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task stops the timer and reports the execution
--  time for a CONTEXT_SWITCH which restores the first floating 
--  point context.  Following this, it starts the
--  timer and performs a CONTEXT_SAVE_FP and CONTEXT_RESTORE_FP 
--  and a CONTEXT_SWITCH to FLOATING_POINT_TASK_2.  Once
--  control is returned to this task FLOATING_POINT_TASK_2, the
--  timer is stopped and the execution time for CONTEXT_SWITCH
--  from a used to an initialized floating point context is reported.
--  Following this, it starts the timer and performs a 
--  CONTEXT_SAVE_FP and CONTEXT_RESTORE_FP and a CONTEXT_SWITCH 
--  to FLOATING_POINT_TASK_2.  
--

   procedure FLOATING_POINT_TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  FLOATING_POINT_TASK_2
--
--  DESCRIPTION:
--
--  This RTEMS task stops the timer and reports the execution
--  time for a CONTEXT_SWITCH which switches from one freshly 
--  initialized floating point context to another freshly 
--  initialized floating point context.  Following this, it 
--  "dirties" its floating point context and starts the
--  timer.  After this it performs a CONTEXT_SAVE_FP and 
--  CONTEXT_RESTORE_FP and a CONTEXT_SWITCH to FLOATING_POINT_TASK_1.
--  Once control is returned to this task FLOATING_POINT_TASK_2, the
--  timer is stopped and the execution time for CONTEXT_SWITCH
--  from one used floating point context to another used floating 
--  point context is reported.  After this, this task invokes
--  the COMPLETE_TEST subprogram to measure and report
--  the remaining execution times measured by this test.
--  

   procedure FLOATING_POINT_TASK_2 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  COMPLETE_TEST
--
--  DESCRIPTION:
--
--  This subprogram is responsible for measuring and reporting the
--  following execution times:
--
--    + THREAD_RESUME 
--    + THREAD_UNBLOCK
--    + THREAD_READY
--    + THREAD_GET with a valid id
--    + THREAD_GET with an invalid id
--

   procedure COMPLETE_TEST;

end TMTEST;
