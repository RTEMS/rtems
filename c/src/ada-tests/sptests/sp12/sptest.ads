--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 12 of the RTEMS
--  Single Processor Test Suite.
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

package SPTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 5 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 5 ) of RTEMS.NAME;

   PRIORITY_TASK_ID   : array ( 1 .. 5 ) of RTEMS.ID;
   PRIORITY_TASK_NAME : array ( 1 .. 5 ) of RTEMS.NAME;
   TASK_PRIORITY      : array ( 1 .. 5 ) of RTEMS.TASKS.PRIORITY;

   SEMAPHORE_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   SEMAPHORE_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;

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
--  PRIORITY_TEST_DRIVER
--
--  DESCRIPTION:
--
--  This subprogram creates and starts RTEMS tasks of different priority
--  so that those tasks may exercise the priority blocking algorithm.
--

   procedure PRIORITY_TEST_DRIVER (
      PRIORITY_BASE : in     RTEMS.UNSIGNED32
   );

--
--  PRIORITY_TASK
--
--  DESCRIPTION:
--
--  Copies of this RTEMS are executed at different priorities and each
--  instantiation blocks on a semaphore with priority blocking.  This
--  is done to exercise the priority blocking algorithms.
--

   procedure PRIORITY_TASK (
      ITS_INDEX : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, PRIORITY_TASK);

--
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Semaphore Manager.
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_1);

--
--  TASK_2
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Semaphore Manager.
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_2);

--
--  TASK_3
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Semaphore Manager.
--

   procedure TASK_3 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_3);

--
--  TASK_4
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Semaphore Manager.
--

   procedure TASK_4 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_4);

--
--  TASK_5
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Semaphore Manager.
--

   procedure TASK_5 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_5);

end SPTEST;
