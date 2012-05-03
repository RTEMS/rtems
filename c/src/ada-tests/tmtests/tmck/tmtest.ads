--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Timer Check Test of the RTEMS
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
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;

--
--  The following variable is set to the execution time returned
--  by the timer.
--

   END_TIME  : RTEMS.UNSIGNED32;

--
--  The following defines the number of iterations of each timed
--  operation to perform.
--

   OPERATION_COUNT      : constant RTEMS.UNSIGNED32 := 100000;

--  
--  The following array is used to determine how many times
--  each time between 0 and 1000 microseconds was returned
--  when simply starting and stopping the timer.

   DISTRIBUTION : array ( RTEMS.UNSIGNED32 range 0 .. 10000 ) of RTEMS.UNSIGNED32;

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
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task invokes CHECK_READ_TIMER before demonstrating
--  that increasing the order of magnitude of the number of loop
--  iterations performed has a corresponding impact on the time
--  reported by the timer driver.
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_1);

--
--  CHECK_READ_TIMER
--
--  DESCRIPTION:
--
--  This subprogram is used to determine the overhead associated
--  with starting and stopping the timer.  It is also useful
--  for determining if unexpected times will be reported.
--

   procedure CHECK_READ_TIMER;

end TMTEST;
