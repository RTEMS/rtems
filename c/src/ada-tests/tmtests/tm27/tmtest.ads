--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 27 of the RTEMS
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
--  tmtest.ads,v 1.3 1995/07/12 19:44:30 joel Exp
--

with TIMER_DRIVER;
with RTEMS;

package TMTEST is

--
--  This array contains the IDs of all RTEMS tasks created by this test.
--

   TASK_ID : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;

--
--  The following variable is set to the execution time returned
--  by the timer.
--

   END_TIME : RTEMS.UNSIGNED32;

--
--  The following variable indicates which interrupt case is
--  currently being timed.
--

   INTERRUPT_NEST : RTEMS.UNSIGNED32;

--
--  The following variables are set to the appropriate execution 
--  times by the ISR.  This is done so the times will be available
--  to be printed by a task.
--

   INTERRUPT_ENTER_TIME  : RTEMS.UNSIGNED32;
   INTERRUPT_RETURN_TIME : RTEMS.UNSIGNED32;

   INTERRUPT_ENTER_NESTED_TIME  : RTEMS.UNSIGNED32;
   INTERRUPT_RETURN_NESTED_TIME : RTEMS.UNSIGNED32;

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
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task reports the following execution times:
--
--    + interrupt entry overhead for a nested interrupt
--    + interrupt exit overhead for a nested interrupt
--    + interrupt entry overhead when no preemption occurs
--    + interrupt exit overhead when no preemption occurs
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASK_2
--
--  DESCRIPTION:
--
--  This RTEMS task reports the following execution times:
--
--    + interrupt entry overhead when preemption occurs
--    + interrupt exit overhead when preemption occurs
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  ISR_HANDLER
--
--  DESCRIPTION:
--
--  This subprogram is the test ISR.  It simply stops the
--  timer and save the execution time for ISR_HANDLER_INNER.
--  This is done to minimize the subroutine linkage overhead
--  for this routine.
--

   procedure ISR_HANDLER (
      VECTOR : in     RTEMS.VECTOR_NUMBER
   );

--
--  ISR_HANDLER_INNER
--
--  DESCRIPTION:
--
--  This subprogram is responsible for storing the execution
--  time for the case currently being timed.
--

   procedure ISR_HANDLER_INNER;

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
      111,                       -- maximum # tasks
      0,                         -- maximum # timers
      1,                         -- maximum # semaphores
      0,                         -- maximum # message queues
      0,                         -- maximum # messages
      0,                         -- maximum # partitions
      0,                         -- maximum # regions
      0,                         -- maximum # dp memory areas
      0,                         -- maximum # periods
      0,                         -- maximum # user extensions
      RTEMS.MILLISECONDS_TO_MICROSECONDS(10), -- # us in a tick
      0                          -- # ticks in a timeslice
  );

end TMTEST;
