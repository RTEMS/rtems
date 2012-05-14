--
--  MPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 3 of the RTEMS
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
--  DELAYED_SEND_EVENT
--
--  DESCRIPTION:
--
--  This subprogram is a timer service routine which sends an 
--  event set to a waiting task.
--

   procedure DELAYED_SEND_EVENT (
      IGNORED_ID      : in     RTEMS.ID;  
      IGNORED_ADDRESS : in     RTEMS.ADDRESS 
   );
   pragma Convention (C, DELAYED_SEND_EVENT);

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

--
--  TEST_TASK_SUPPORT
--
--  DESCRIPTION:
--
--  This subprogram performs the bulk of the test.  Based on the NODE
--  specified, this subprogram loops suspending/resuming a remote task
--  or waiting for itself to be suspended/resumed.
--

   procedure TEST_TASK_SUPPORT (
      NODE : in     RTEMS.UNSIGNED32
   );

end MPTEST;
