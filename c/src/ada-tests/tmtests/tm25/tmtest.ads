--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 25 of the RTEMS
--  Timing Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--
--  $Id$
--

with TIMER_DRIVER;
with RTEMS;

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
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  HIGH_TASKS
--
--  DESCRIPTION:
--
--  These RTEMS tasks perform a blocking SEMAPHORE_OBTAIN which
--  has an optional timeout.
--

   procedure HIGH_TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  LOW_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is responsible for measuring and reporting
--  the execution time for the CLOCK_TICK directive.
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

end TMTEST;
