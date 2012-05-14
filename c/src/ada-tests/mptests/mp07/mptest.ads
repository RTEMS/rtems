--
--  MPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 7 of the RTEMS
--  Multiprocessor Test Suite.
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

package MPTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS timers created
--  by this test.
--

   TIMER_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;
   TIMER_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.NAME;

--
--  This variable is set when the test should stop executing.
--
   STOP_TEST : BOOLEAN;

--
--  This variable contains the ID of the remote task with which this
--  test interacts.
--

   REMOTE_TID  : RTEMS.ID;

--
--  This variable contains the node on which the remote task with which 
--  this test interacts resides.
--

   REMOTE_NODE : RTEMS.UNSIGNED32;

--
--  The number of events to process per dot printed out.
--

   PER_DOT : constant RTEMS.UNSIGNED32 := 100;

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
--  STOP_TEST_TSR
--
--  DESCRIPTION:
--
--  This subprogram is a TSR which sets the "stop test" flag.
--
 
   procedure STOP_TEST_TSR (
      IGNORED1 : in     RTEMS.ID;
      IGNORED2 : in     RTEMS.ADDRESS
   );
   pragma Convention (C, STOP_TEST_TSR);

--
--  TEST_TASK
--
--  DESCRIPTION:
--
--  This is the body of the RTEMS tasks which constitute this test.
--

   procedure TEST_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TEST_TASK);

end MPTEST;
