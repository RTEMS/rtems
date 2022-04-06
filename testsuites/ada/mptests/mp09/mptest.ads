-- SPDX-License-Identifier: BSD-2-Clause

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
--  Redistribution and use in source and binary forms, with or without
--  modification, are permitted provided that the following conditions
--  are met:
--  1. Redistributions of source code must retain the above copyright
--     notice, this list of conditions and the following disclaimer.
--  2. Redistributions in binary form must reproduce the above copyright
--     notice, this list of conditions and the following disclaimer in the
--     documentation and/or other materials provided with the distribution.
--
--  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
--  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
--  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
--  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
--  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
--  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
--  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
--  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
--  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
--  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
--  POSSIBILITY OF SUCH DAMAGE.
--

with RTEMS;
with RTEMS.TASKS;

package MPTEST is

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

   QUEUE_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;
   QUEUE_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.NAME;

--
--  The following are message buffers used to contain the test messages
--  and pointers to those buffers.
--

   RECEIVE_BUFFER_AREA : MPTEST.BUFFER;
   BUFFER_AREA_1       : MPTEST.BUFFER;
   BUFFER_AREA_2       : MPTEST.BUFFER;
   BUFFER_AREA_3       : MPTEST.BUFFER;
   BUFFER_AREA_4       : MPTEST.BUFFER;

   RECEIVE_BUFFER : RTEMS.ADDRESS;
   BUFFER_1       : RTEMS.ADDRESS;
   BUFFER_2       : RTEMS.ADDRESS;
   BUFFER_3       : RTEMS.ADDRESS;
   BUFFER_4       : RTEMS.ADDRESS;

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
      BUFFER :    out MPTEST.BUFFER
   );
 
--
--  PUT_BUFFER
--
--  DESCRIPTION:
--
--  This subprogram prints the specified message BUFFER.
--
 
   procedure PUT_BUFFER (
      BUFFER : in     MPTEST.BUFFER
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
