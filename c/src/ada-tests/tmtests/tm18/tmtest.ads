--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 18 of the RTEMS
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

with RTEMS;
with TIME_TEST_SUPPORT;
with TIMER_DRIVER;

package TMTEST is

--
--  The following variable is set to the execution time returned
--  by the timer.
--

   END_TIME       : RTEMS.UNSIGNED32;

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
--  TEST_INIT
--
--  DESCRIPTION:
--
--  This subprogram performs test initialization.
--

   procedure TEST_INIT;

--
--  FIRST_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is the first task to execute.  It starts the
--  timer and performs a TASK_DELETE on itself.
--

   procedure FIRST_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  MIDDLE_TASK
--
--  DESCRIPTION:
--
--  These RTEMS tasks perform a TASK_DELETE on themselves.
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  LAST_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is the last task to execute.  It stops the
--  timer and reports the execution time of a TASK_DELETE SELF.
--

   procedure LAST_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

end TMTEST;
