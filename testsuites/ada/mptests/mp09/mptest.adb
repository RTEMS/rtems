--
--  MPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation for Test 9 of the RTEMS
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
--  http://www.rtems.org/license/LICENSE.
--

with INTERFACES; use INTERFACES;
with RTEMS.MESSAGE_QUEUE;
with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;

package body MPTEST is

--
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEST_SUPPORT.ADA_TEST_BEGIN;
      TEXT_IO.PUT( "*** NODE " );
      UNSIGNED32_IO.PUT(
         TEST_SUPPORT.NODE,
         WIDTH => 1
      );
      TEXT_IO.PUT_LINE( " ***" );

      MPTEST.RECEIVE_BUFFER := MPTEST.RECEIVE_BUFFER_AREA'ADDRESS;

      MPTEST.BUFFER_1 := MPTEST.BUFFER_AREA_1'ADDRESS;

      MPTEST.BUFFER_2 := MPTEST.BUFFER_AREA_2'ADDRESS;

      MPTEST.BUFFER_3 := MPTEST.BUFFER_AREA_3'ADDRESS;

      MPTEST.BUFFER_4 := MPTEST.BUFFER_AREA_4'ADDRESS;

      MPTEST.FILL_BUFFER( "123456789012345 ", MPTEST.BUFFER_AREA_1 );
      MPTEST.FILL_BUFFER( "abcdefghijklmno ", MPTEST.BUFFER_AREA_2 );
      MPTEST.FILL_BUFFER( "ABCDEFGHIJKLMNO ", MPTEST.BUFFER_AREA_3 );
      MPTEST.FILL_BUFFER( "PQRSTUVWXYZ(){} ", MPTEST.BUFFER_AREA_4 );

      MPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  '1', '1', '1', ' ' );
      MPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  '2', '2', '2', ' ' );

      MPTEST.QUEUE_NAME( 1 ) := RTEMS.BUILD_NAME(  'M', 'S', 'G', ' ' );

      if TEST_SUPPORT.NODE = 1 then

         TEXT_IO.PUT_LINE( "Creating Message Queue (Global)" );
         RTEMS.MESSAGE_QUEUE.CREATE(
            MPTEST.QUEUE_NAME( 1 ),
            3,
            3,
            RTEMS.GLOBAL,
            MPTEST.QUEUE_ID( 1 ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_CREATE" );

      end if;

      TEXT_IO.PUT_LINE( "Creating Test_task (local)" );
      RTEMS.TASKS.CREATE(
         MPTEST.TASK_NAME( TEST_SUPPORT.NODE ),
         TEST_SUPPORT.NODE,
         2048,
         RTEMS.TIMESLICE,
         RTEMS.DEFAULT_ATTRIBUTES,
         MPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

      TEXT_IO.PUT_LINE( "Starting Test_task (local)" );
      RTEMS.TASKS.START(
         MPTEST.TASK_ID( 1 ),
         MPTEST.TEST_TASK'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START" );

      TEXT_IO.PUT_LINE( "Deleting initialization task" );
      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--
--  SEND_MESSAGES
--

   procedure SEND_MESSAGES is
      BROADCAST_COUNT     : RTEMS.UNSIGNED32;
      STATUS              : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT( "message_queue_send : " );
      MPTEST.PUT_BUFFER( MPTEST.BUFFER_AREA_1 );
      TEXT_IO.NEW_LINE;

      RTEMS.MESSAGE_QUEUE.SEND(
         MPTEST.QUEUE_ID( 1 ),
         MPTEST.BUFFER_1,
         16,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_SEND" );

      TEXT_IO.PUT_LINE( "Delaying for a second" );
      RTEMS.TASKS.WAKE_AFTER(
         1 * TEST_SUPPORT.TICKS_PER_SECOND,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

      TEXT_IO.PUT( "message_queue_urgent : " );
      MPTEST.PUT_BUFFER( MPTEST.BUFFER_AREA_2 );
      TEXT_IO.NEW_LINE;

      RTEMS.MESSAGE_QUEUE.URGENT(
         MPTEST.QUEUE_ID( 1 ),
         MPTEST.BUFFER_2,
         16,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_URGENT" );

      TEXT_IO.PUT_LINE( "Delaying for a second" );
      RTEMS.TASKS.WAKE_AFTER(
         1 * TEST_SUPPORT.TICKS_PER_SECOND,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

      TEXT_IO.PUT( "message_queue_broadcast : " );
      MPTEST.PUT_BUFFER( MPTEST.BUFFER_AREA_3 );
      TEXT_IO.NEW_LINE;

      RTEMS.MESSAGE_QUEUE.BROADCAST(
         MPTEST.QUEUE_ID( 1 ),
         MPTEST.BUFFER_3,
         16,
         BROADCAST_COUNT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_BROADCAST" );

      TEXT_IO.PUT_LINE( "Delaying for a second" );
      RTEMS.TASKS.WAKE_AFTER(
         1 * TEST_SUPPORT.TICKS_PER_SECOND,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

   end SEND_MESSAGES;

--
--  RECEIVE_MESSAGES
--

   procedure RECEIVE_MESSAGES is
      STATUS              : RTEMS.STATUS_CODES;
      MESSAGE_SIZE        : RTEMS.SIZE := 0;
   begin

      for INDEX in 1 .. 3
      loop

         TEXT_IO.PUT_LINE( "Receiving message ..." );
         RTEMS.MESSAGE_QUEUE.RECEIVE(
            MPTEST.QUEUE_ID( 1 ),
            MPTEST.RECEIVE_BUFFER,
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.NO_TIMEOUT,
            MESSAGE_SIZE,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_RECEIVE" );

         TEXT_IO.PUT( "Received : " );
         MPTEST.PUT_BUFFER( MPTEST.RECEIVE_BUFFER_AREA );
         TEXT_IO.NEW_LINE;

      end loop;

      TEXT_IO.PUT_LINE( "Receiver delaying for a second" );

      RTEMS.TASKS.WAKE_AFTER( 1 * TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

   end RECEIVE_MESSAGES;

--
--  FILL_BUFFER
--

--
-- Depends on tricks to make the copy work.
--

   procedure FILL_BUFFER (
      SOURCE : in     STRING;
      BUFFER :    out MPTEST.BUFFER
   ) is
   begin

      BUFFER.FIELD1 := RTEMS.BUILD_NAME(
         SOURCE( SOURCE'FIRST ),
         SOURCE( SOURCE'FIRST + 1 ),
         SOURCE( SOURCE'FIRST + 2 ),
         SOURCE( SOURCE'FIRST + 3 )
      );

      BUFFER.FIELD2 := RTEMS.BUILD_NAME(
         SOURCE( SOURCE'FIRST + 4 ),
         SOURCE( SOURCE'FIRST + 5 ),
         SOURCE( SOURCE'FIRST + 6 ),
         SOURCE( SOURCE'FIRST + 7 )
      );

      BUFFER.FIELD3 := RTEMS.BUILD_NAME(
         SOURCE( SOURCE'FIRST + 8 ),
         SOURCE( SOURCE'FIRST + 9 ),
         SOURCE( SOURCE'FIRST + 10 ),
         SOURCE( SOURCE'FIRST + 11 )
      );

      BUFFER.FIELD4 := RTEMS.BUILD_NAME(
         SOURCE( SOURCE'FIRST + 12 ),
         SOURCE( SOURCE'FIRST + 13 ),
         SOURCE( SOURCE'FIRST + 14 ),
         SOURCE( SOURCE'FIRST + 15 )
      );

   end FILL_BUFFER;

--
--  PUT_BUFFER
--

--
-- Depends on tricks to make the output work.
--

   procedure PUT_BUFFER (
      BUFFER : in     MPTEST.BUFFER
   ) is
   begin

      TEST_SUPPORT.PUT_NAME( BUFFER.FIELD1, FALSE );
      TEST_SUPPORT.PUT_NAME( BUFFER.FIELD2, FALSE );
      TEST_SUPPORT.PUT_NAME( BUFFER.FIELD3, FALSE );
      TEST_SUPPORT.PUT_NAME( BUFFER.FIELD4, FALSE );

   end PUT_BUFFER;

--
--  TEST_TASK
--

   procedure TEST_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      COUNT        : RTEMS.UNSIGNED32;
      STATUS       : RTEMS.STATUS_CODES;
      MESSAGE_SIZE : RTEMS.SIZE := 0;
   begin

      RTEMS.TASKS.WAKE_AFTER( 1 * TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

      TEXT_IO.PUT_LINE( "Getting QID of message queue" );

      loop

         RTEMS.MESSAGE_QUEUE.IDENT(
            MPTEST.QUEUE_NAME( 1 ),
            RTEMS.SEARCH_ALL_NODES,
            MPTEST.QUEUE_ID( 1 ),
            STATUS
         );

         exit when RTEMS.IS_STATUS_SUCCESSFUL( STATUS );

      end loop;

      if TEST_SUPPORT.NODE = 2 then

         RTEMS.MESSAGE_QUEUE.DELETE( MPTEST.QUEUE_ID( 1 ), STATUS );

         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.ILLEGAL_ON_REMOTE_OBJECT,
            "MESSAGE_QUEUE_DELETE"
         );

         TEXT_IO.PUT_LINE(
         "message_queue_delete correctly returned ILLEGAL_ON_REMOTE_OBJECT"
         );

         MPTEST.SEND_MESSAGES;

         MPTEST.RECEIVE_MESSAGES;

         TEXT_IO.PUT_LINE( "Flushing remote empty queue" );
         RTEMS.MESSAGE_QUEUE.FLUSH( MPTEST.QUEUE_ID( 1 ), COUNT, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_FLUSH" );
         UNSIGNED32_IO.PUT( COUNT, WIDTH => 1 );
         TEXT_IO.PUT_LINE(
            " messages were flushed from remote empty queue"
         );

         TEXT_IO.PUT_LINE(
            "Send messages to be flushed from remote queue"
         );
         RTEMS.MESSAGE_QUEUE.SEND(
            MPTEST.QUEUE_ID( 1 ),
            MPTEST.BUFFER_1,
            16,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_SEND" );

         TEXT_IO.PUT_LINE( "Flushing remote queue" );
         RTEMS.MESSAGE_QUEUE.FLUSH( MPTEST.QUEUE_ID( 1 ), COUNT, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_FLUSH" );
         UNSIGNED32_IO.PUT( COUNT, WIDTH => 1 );
         TEXT_IO.PUT_LINE(
            " messages were flushed from the remote queue"
         );

         TEXT_IO.PUT_LINE( "Waiting for message queue to be deleted" );
         RTEMS.MESSAGE_QUEUE.RECEIVE(
            MPTEST.QUEUE_ID( 1 ),
            MPTEST.RECEIVE_BUFFER,
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.NO_TIMEOUT,
            MESSAGE_SIZE,
            STATUS
         );
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.OBJECT_WAS_DELETED,
            "MESSAGE_QUEUE_FLUSH"
         );

      else

         MPTEST.RECEIVE_MESSAGES;

         MPTEST.SEND_MESSAGES;

         RTEMS.TASKS.WAKE_AFTER(
            5 * TEST_SUPPORT.TICKS_PER_SECOND,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

         RTEMS.MESSAGE_QUEUE.DELETE( MPTEST.QUEUE_ID( 1 ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_DELETE" );

      end if;

      TEST_SUPPORT.ADA_TEST_END;

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TEST_TASK;

end MPTEST;
