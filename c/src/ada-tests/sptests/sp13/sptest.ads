--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 13 of the RTEMS
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
--  Buffer Record similar to that used by RTEMS 3.2.1.  Using this
--  avoids changes to the test.
--

   type BUFFER is
      record
         FIELD1 : RTEMS.NAME;   -- TEMPORARY UNTIL VARIABLE LENGTH
         FIELD2 : RTEMS.NAME;
         FIELD3 : RTEMS.NAME;
         FIELD4 : RTEMS.NAME;
      end record;

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS message
--  queues created by this test.
--

   QUEUE_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   QUEUE_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;

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
--  FILL_BUFFER
--
--  DESCRIPTION:
--
--  This subprogram takes the SOURCE input string and places 
--  up to the first sixteen characters of that string into 
--  the message BUFFER.
--

   procedure FILL_BUFFER (
      SOURCE : in     STRING;
      BUFFER :    out SPTEST.BUFFER
   );

--
--  PUT_BUFFER
--
--  DESCRIPTION:
--
--  This subprogram prints the specified message BUFFER.
--

   procedure PUT_BUFFER (
      BUFFER : in     SPTEST.BUFFER
   );

--
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Message Queue Manager.
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
--  This RTEMS task tests the Message Queue Manager.
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
--  This RTEMS task tests the Message Queue Manager.
--

   procedure TASK_3 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_3);

end SPTEST;
