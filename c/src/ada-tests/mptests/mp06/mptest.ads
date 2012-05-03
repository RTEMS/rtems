--
--  MPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 6 of the RTEMS
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
with RTEMS.EVENT;
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
--  The number of signals to process per dot printed out.
--

   MAXIMUM_DOTS : constant RTEMS.UNSIGNED32 := 25;

--
--  The following is a table of the event sets which consist of
--  a single event.  This test cycles through all of these
--  events.
-- 

   EVENT_SET_TABLE : constant array ( 0 .. 30 ) of RTEMS.EVENT_SET := (
      RTEMS.EVENT_0,
      RTEMS.EVENT_1,
      RTEMS.EVENT_2,
      RTEMS.EVENT_3,
      RTEMS.EVENT_4,
      RTEMS.EVENT_5,
      RTEMS.EVENT_6,
      RTEMS.EVENT_7,
      RTEMS.EVENT_8,
      RTEMS.EVENT_9,
      RTEMS.EVENT_10,
      RTEMS.EVENT_11,
      RTEMS.EVENT_12,
      RTEMS.EVENT_13,
      RTEMS.EVENT_14,
      RTEMS.EVENT_15,
      RTEMS.EVENT_16,
      RTEMS.EVENT_17,
      RTEMS.EVENT_18,
      RTEMS.EVENT_19,
      RTEMS.EVENT_20,
      RTEMS.EVENT_21,
      RTEMS.EVENT_22,
      RTEMS.EVENT_23,
      RTEMS.EVENT_24,
      RTEMS.EVENT_25,
      RTEMS.EVENT_26,
      RTEMS.EVENT_27,
      RTEMS.EVENT_28,
      RTEMS.EVENT_29,
      RTEMS.EVENT_30
   );

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
