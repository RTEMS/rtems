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
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, INIT);

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
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, FIRST_TASK);

--
--  MIDDLE_TASKS
--
--  DESCRIPTION:
--
--  These RTEMS tasks perform a TASK_DELETE on themselves.
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );

   pragma Convention (C, MIDDLE_TASKS);
--
--  LAST_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is the last task to execute.  It stops the
--  timer and reports the execution time of a TASK_DELETE SELF.
--

   procedure LAST_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, LAST_TASK);

end TMTEST;
