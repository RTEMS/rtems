--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 3 of the RTEMS
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

package TMTEST is

--
--  The following is the ID of the semaphore used for timing operations.
--

   SEMAPHORE_ID : RTEMS.ID;

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
--  This subprogram performs test initialization.  After completion
--  of test initialization, the timer is started and a preemptive
--  SEMAPHORE_RELEASE is performed.
--

   procedure TEST_INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TEST_INIT);

--
--  MIDDLE_TASKS
--
--  DESCRIPTION:
--
--  This RTEMS task performs a blocking SEMAPHORE_OBTAIN.  Once
--  is obtains the semaphore unit and executes again, it performs
--  a preemptive SEMAPHORE_RELEASE.
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, MIDDLE_TASKS);

--
--  HIGH_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task stops the timer and reports the execution
--  timer for a preemptive SEMAPHORE_RELEASE.
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, HIGH_TASK);

end TMTEST;
