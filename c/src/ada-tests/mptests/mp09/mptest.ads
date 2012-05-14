--
--  MPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 9 of the RTEMS
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
--  These arrays contain the IDs and NAMEs of all RTEMS message 
--  queues created by this test.
--

   QUEUE_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;
   QUEUE_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.NAME;

--
--  The following are message buffers used to contain the test messages
--  and pointers to those buffers.
--

   RECEIVE_BUFFER_AREA : RTEMS.BUFFER;
   BUFFER_AREA_1       : RTEMS.BUFFER;
   BUFFER_AREA_2       : RTEMS.BUFFER;
   BUFFER_AREA_3       : RTEMS.BUFFER;
   BUFFER_AREA_4       : RTEMS.BUFFER;

   RECEIVE_BUFFER : RTEMS.BUFFER_POINTER;
   BUFFER_1       : RTEMS.BUFFER_POINTER;
   BUFFER_2       : RTEMS.BUFFER_POINTER;
   BUFFER_3       : RTEMS.BUFFER_POINTER;
   BUFFER_4       : RTEMS.BUFFER_POINTER;

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
--  SEND_MESSAGES
--
--  This subprogram prints and sends a sequence of three test messages.
--  One of the messages is sent, one is urgent, and one is broadcast.
--  A one second pause is between each the sending of each message.
--

   procedure SEND_MESSAGES;

--
--  RECEIVE_MESSAGES
--
--  This subprogram receives and prints a sequence of three test messages.
--

   procedure RECEIVE_MESSAGES;

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
      BUFFER :    out RTEMS.BUFFER
   );
 
--
--  PUT_BUFFER
--
--  DESCRIPTION:
--
--  This subprogram prints the specified message BUFFER.
--
 
   procedure PUT_BUFFER (
      BUFFER : in     RTEMS.BUFFER
   );

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
