--
--  MPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation for Test 14 of the RTEMS
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

with INTERFACES; use INTERFACES;
with INTERFACES.C;
with RTEMS;
with RTEMS.EVENT;
with RTEMS.MESSAGE_QUEUE;
with RTEMS.PARTITION;
with RTEMS.SEMAPHORE;
with RTEMS.TASKS;
with RTEMS.TIMER;
with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;

with System.Storage_Elements; use System.Storage_Elements;

package body MPTEST is

--
--  STOP_TEST_TSR
--
 
   procedure STOP_TEST_TSR (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   ) is
   begin
 
      MPTEST.STOP_TEST := TRUE;
 
   end STOP_TEST_TSR;
 
--
--  EXIT_TEST
--
 
   procedure EXIT_TEST is
      OLD_MODE : RTEMS.MODE;
      STATUS   : RTEMS.STATUS_CODES;
      procedure BSP_MPCI_PRINT_STATISTICS;
      pragma Import (C, BSP_MPCI_PRINT_STATISTICS, "MPCI_Print_statistics" );
   begin
 
      RTEMS.TASKS.MODE(RTEMS.NO_PREEMPT, RTEMS.PREEMPT_MASK, OLD_MODE, STATUS);
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );

      BSP_MPCI_PRINT_STATISTICS;

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );
 
   end EXIT_TEST;
 
--
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      INDEX             : RTEMS.UNSIGNED32;
      STATUS            : RTEMS.STATUS_CODES;
      PREVIOUS_PRIORITY : RTEMS.TASKS.PRIORITY;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT( "*** TEST 14 -- NODE " );
      UNSIGNED32_IO.PUT(
         TEST_SUPPORT.NODE,
         WIDTH => 1
      );
      TEXT_IO.PUT_LINE( " ***" );

      MPTEST.STOP_TIMER_NAME := RTEMS.BUILD_NAME( 'S', 'T', 'O', 'P' );

      MPTEST.STOP_TEST := FALSE;

      RTEMS.TIMER.CREATE(
         MPTEST.STOP_TIMER_NAME,
         MPTEST.STOP_TIMER_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE" );

      RTEMS.TIMER.FIRE_AFTER(
         MPTEST.STOP_TIMER_ID,
         TEST_SUPPORT.MAXIMUM_LONG_TEST_DURATION *
            TEST_SUPPORT.TICKS_PER_SECOND,
         MPTEST.STOP_TEST_TSR'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );
 
      MPTEST.EVENT_TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  '1', '1', '1', ' ' );
      MPTEST.EVENT_TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  '2', '2', '2', ' ' );

      MPTEST.QUEUE_TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'M', 'T', '1', ' ' );
      MPTEST.QUEUE_TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'M', 'T', '2', ' ' );

      MPTEST.PARTITION_TASK_NAME( 1 ) := 
         RTEMS.BUILD_NAME(  'P', 'T', '1', ' ' );
      MPTEST.PARTITION_TASK_NAME( 2 ) := 
         RTEMS.BUILD_NAME(  'P', 'T', '2', ' ' );

      MPTEST.SEMAPHORE_TASK_NAME( 1 ) := 
         RTEMS.BUILD_NAME(  'S', 'M', '1', ' ' );
      MPTEST.SEMAPHORE_TASK_NAME( 2 ) := 
         RTEMS.BUILD_NAME(  'S', 'M', '2', ' ' );

      MPTEST.SEMAPHORE_NAME( 1 ) := RTEMS.BUILD_NAME(  'S', 'E', 'M', ' ' );

      MPTEST.QUEUE_NAME( 1 ) := RTEMS.BUILD_NAME(  'M', 'S', 'G', ' ' );

      MPTEST.PARTITION_NAME( 1 ) := RTEMS.BUILD_NAME(  'P', 'A', 'R', ' ' );

      MPTEST.TIMER_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'M', 'R', ' ' );

      for INDEX in MPTEST.BUFFERS'FIRST .. MPTEST.BUFFERS'LAST
      loop

         MPTEST.BUFFERS( INDEX ) :=
            MPTEST.BUFFER_AREAS( INDEX )'ADDRESS;

      end loop;

      if TEST_SUPPORT.NODE = 1 then

         TEXT_IO.PUT_LINE( "Creating Semaphore (Global)" );
         RTEMS.SEMAPHORE.CREATE(
            MPTEST.SEMAPHORE_NAME( 1 ),
            1,
            RTEMS.GLOBAL,
            MPTEST.SEMAPHORE_ID( 1 ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE" );

         TEXT_IO.PUT_LINE( "Creating Message Queue (Global)" );
         RTEMS.MESSAGE_QUEUE.CREATE(
            MPTEST.QUEUE_NAME( 1 ),
            1,
            RTEMS.GLOBAL,
            MPTEST.QUEUE_ID( 1 ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_CREATE" );

         TEXT_IO.PUT_LINE( "Creating Partition (Global)" );
         RTEMS.PARTITION.CREATE(
            MPTEST.PARTITION_NAME( 1 ),
            MPTEST.PARTITION_AREA( 0 )'ADDRESS,
            16#8000#,
            16#3000#,
            RTEMS.GLOBAL,
            MPTEST.PARTITION_ID( 1 ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_CREATE" );

      end if;

      TEXT_IO.PUT_LINE( "Creating Event task (Global)" );
      RTEMS.TASKS.CREATE(
         MPTEST.EVENT_TASK_NAME(
            TEST_SUPPORT.NODE
         ),
         2,
         2048,
         RTEMS.TIMESLICE,
         RTEMS.GLOBAL,
         MPTEST.EVENT_TASK_ID( 1 ),
         STATUS
      ); 
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

      TEXT_IO.PUT_LINE( "Starting Event task (Global)" );
      RTEMS.TASKS.START(
         MPTEST.EVENT_TASK_ID( 1 ),
         MPTEST.TEST_TASK'ACCESS,
         0,
         STATUS
      ); 
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START" );

      TEXT_IO.PUT_LINE( "Creating Semaphore task (Global)" );
      RTEMS.TASKS.CREATE(
         MPTEST.SEMAPHORE_TASK_NAME(
            TEST_SUPPORT.NODE
         ),
         2,
         2048,
         RTEMS.TIMESLICE,
         RTEMS.GLOBAL,
         MPTEST.SEMAPHORE_TASK_ID( 1 ),
         STATUS
      ); 
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

      TEXT_IO.PUT_LINE( "Starting Semaphore task (Global)" );
      RTEMS.TASKS.START(
         MPTEST.SEMAPHORE_TASK_ID( 1 ),
         MPTEST.SEMAPHORE_TASK'ACCESS,
         0,
         STATUS
      ); 
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START" );

      TEXT_IO.PUT_LINE( "Creating Message Queue task (Global)" );
      RTEMS.TASKS.CREATE(
         MPTEST.QUEUE_TASK_NAME(
            TEST_SUPPORT.NODE
         ),
         2,
         2048,
         RTEMS.TIMESLICE,
         RTEMS.GLOBAL,
         MPTEST.QUEUE_TASK_ID( 1 ),
         STATUS
      ); 
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

      TEXT_IO.PUT_LINE( "Starting Message Queue task (Global)" );
      RTEMS.TASKS.START(
         MPTEST.QUEUE_TASK_ID( 1 ),
         MPTEST.MESSAGE_QUEUE_TASK'ACCESS,
         1,                          -- index of buffer
         STATUS
      ); 
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START" );

      TEXT_IO.PUT_LINE( "Creating Partition task (Global)" );
      RTEMS.TASKS.CREATE(
         MPTEST.PARTITION_TASK_NAME(
            TEST_SUPPORT.NODE 
         ),
         2,
         2048,
         RTEMS.TIMESLICE,
         RTEMS.GLOBAL,
         MPTEST.PARTITION_TASK_ID( 1 ),
         STATUS
      ); 
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

      TEXT_IO.PUT_LINE( "Starting Partition task (Global)" );
      RTEMS.TASKS.START(
         MPTEST.PARTITION_TASK_ID( 1 ),
         MPTEST.PARTITION_TASK'ACCESS,
         0,
         STATUS
      ); 
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START" );
    
      RTEMS.TASKS.SET_PRIORITY( RTEMS.SELF, 2, PREVIOUS_PRIORITY, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY" );

      MPTEST.DELAYED_EVENTS_TASK( 1 );

   end INIT;

--
--  DELAYED_SEND_EVENT
--
--  DESCRIPTION:
--
--  This subprogram is a timer service routine which sends an
--  event set to a waiting task.
--

   procedure DELAYED_SEND_EVENT (
      TIMER_ID        : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.EVENT.SEND(
         MPTEST.TASK_ID( RTEMS.GET_INDEX( TIMER_ID ) ),
         RTEMS.EVENT_16,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND" );

   end DELAYED_SEND_EVENT;
 
--
--  TEST_TASK
--
--  DESCRIPTION:
--
--  This is one of the test tasks.
--
 
   procedure TEST_TASK ( 
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      REMOTE_NODE : RTEMS.UNSIGNED32;
      REMOTE_TID  : RTEMS.ID;
      COUNT       : RTEMS.UNSIGNED32;
      EVENT_OUT   : RTEMS.EVENT_SET;
      STATUS      : RTEMS.STATUS_CODES;
   begin

      if TEST_SUPPORT.NODE = 1 then
         REMOTE_NODE := 2;
      else
         REMOTE_NODE := 1;
      end if;

      TEXT_IO.PUT_LINE( "About to go to sleep!" );
      RTEMS.TASKS.WAKE_AFTER( 1 * TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
      TEXT_IO.PUT_LINE( "Waking up!" );
 
      TEXT_IO.PUT( "Remote task's name is : " );
      TEST_SUPPORT.PUT_NAME( MPTEST.EVENT_TASK_NAME( REMOTE_NODE ), TRUE );

      TEXT_IO.PUT_LINE( "Getting TID of remote task" );

      loop

         RTEMS.TASKS.IDENT(
            MPTEST.EVENT_TASK_NAME( REMOTE_NODE ),
            RTEMS.SEARCH_ALL_NODES,
            REMOTE_TID,
            STATUS
         ); 

         exit when RTEMS.IS_STATUS_SUCCESSFUL( STATUS );

         TEXT_IO.PUT_LINE( "task_ident" );

      end loop;

      if TEST_SUPPORT.NODE = 1 then
         TEXT_IO.PUT_LINE( "Sending events to remote task" );

         loop
            exit when MPTEST.STOP_TEST = TRUE;

            for COUNT in 1 .. MPTEST.EVENT_TASK_DOT_COUNT
            loop
               RTEMS.EVENT.SEND(
                  REMOTE_TID,
                  RTEMS.EVENT_16,
                  STATUS
               );
               TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND" );

               exit when MPTEST.STOP_TEST = TRUE;

            end loop;

            TEST_SUPPORT.PUT_DOT( "e" );
  
         end loop;

      end if;
          
      TEXT_IO.PUT_LINE( "Receiving events from remote task" );

      loop
         exit when MPTEST.STOP_TEST = TRUE;

         for COUNT in 1 .. MPTEST.EVENT_TASK_DOT_COUNT 
         loop 
            exit when MPTEST.STOP_TEST = TRUE;

            RTEMS.EVENT.RECEIVE(
               RTEMS.EVENT_16,
               RTEMS.DEFAULT_OPTIONS,
               RTEMS.NO_TIMEOUT,
               EVENT_OUT,
               STATUS 
            );   
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE" );

         end loop;
 
         TEST_SUPPORT.PUT_DOT( "e" );
 
      end loop;

      MPTEST.EXIT_TEST;
 
   end TEST_TASK;
 
-- 
--  DELAYED_EVENTS_TASK 
-- 
--  DESCRIPTION: 
-- 
--  This is one of the test tasks. 
-- 
  
   procedure DELAYED_EVENTS_TASK (  
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT 
   ) is
      COUNT         : RTEMS.UNSIGNED32;
      PREVIOUS_MODE : RTEMS.MODE;
      EVENTS_OUT    : RTEMS.EVENT_SET;
      STATUS        : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASKS.MODE(
         RTEMS.PREEMPT + RTEMS.TIMESLICE,
         RTEMS.PREEMPT_MASK + RTEMS.TIMESLICE_MASK,
         PREVIOUS_MODE,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
 
      RTEMS.TIMER.CREATE(
         MPTEST.TIMER_NAME( 1 ),
         MPTEST.TIMER_ID( 1 ),
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE" );

      RTEMS.TASKS.IDENT(
         RTEMS.SELF,
         RTEMS.SEARCH_ALL_NODES,
         MPTEST.TASK_ID( RTEMS.GET_INDEX( MPTEST.TIMER_ID( 1 ) ) ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENTS" );

      loop

         for COUNT in 1 .. MPTEST.DELAYED_EVENT_DOT_COUNT 
         loop 
            RTEMS.TIMER.FIRE_AFTER(
               MPTEST.TIMER_ID( 1 ),
               1,
               MPTEST.DELAYED_SEND_EVENT'ACCESS,
               RTEMS.NULL_ADDRESS,
               STATUS 
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );

            RTEMS.EVENT.RECEIVE(
               RTEMS.EVENT_16,
               RTEMS.DEFAULT_OPTIONS,
               RTEMS.NO_TIMEOUT,
               EVENTS_OUT,
               STATUS 
            );   
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE" );

         end loop;
 
         TEST_SUPPORT.PUT_DOT( "." );
 
      end loop;
 
      MPTEST.EXIT_TEST;
      
   end DELAYED_EVENTS_TASK; 
 
-- 
--  MESSAGE_QUEUE_TASK 
-- 
--  DESCRIPTION: 
-- 
--  This is one of the test tasks. 
-- 
  
   procedure MESSAGE_QUEUE_TASK (  
      INDEX : in     RTEMS.TASKS.ARGUMENT 
   ) is
      COUNT          : RTEMS.UNSIGNED32;
      YIELD_COUNT    : RTEMS.UNSIGNED32;
      OVERFLOW_COUNT : RTEMS.UNSIGNED32_POINTER;
      BUFFER_COUNT   : RTEMS.UNSIGNED32_POINTER;
      STATUS         : RTEMS.STATUS_CODES;
   begin

      MPTEST.BUFFERS( INDEX ).FIELD1 := 0;
      MPTEST.BUFFERS( INDEX ).FIELD2 := 0;
      MPTEST.BUFFERS( INDEX ).FIELD3 := 0;
      MPTEST.BUFFERS( INDEX ).FIELD4 := 0;

      TEXT_IO.PUT_LINE( "Getting ID of message queue" );

      loop

         RTEMS.MESSAGE_QUEUE.IDENT(
            MPTEST.QUEUE_NAME( 1 ),
            RTEMS.SEARCH_ALL_NODES,
            MPTEST.QUEUE_ID( 1 ),
            STATUS
         );
         exit when RTEMS.IS_STATUS_SUCCESSFUL( STATUS );

         TEXT_IO.PUT_LINE( "message_queue_ident FAILED!!" );

      end loop;

      if TEST_SUPPORT.NODE = 1 then

         RTEMS.MESSAGE_QUEUE.SEND(
            MPTEST.QUEUE_ID( 1 ),
            MPTEST.BUFFERS( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_SEND" );

         OVERFLOW_COUNT := RTEMS.TO_UNSIGNED32_POINTER( 
                              MPTEST.BUFFERS( INDEX ).FIELD1'ADDRESS
                           );
                
         BUFFER_COUNT := RTEMS.TO_UNSIGNED32_POINTER( 
                              MPTEST.BUFFERS( INDEX ).FIELD2'ADDRESS
                           );
                
      else

         OVERFLOW_COUNT := RTEMS.TO_UNSIGNED32_POINTER( 
                              MPTEST.BUFFERS( INDEX ).FIELD3'ADDRESS
                           );
                
         BUFFER_COUNT := RTEMS.TO_UNSIGNED32_POINTER( 
                              MPTEST.BUFFERS( INDEX ).FIELD4'ADDRESS
                           );

      end if;

      loop

         exit when MPTEST.STOP_TEST = TRUE;

         YIELD_COUNT := 100;

         for COUNT in 1 .. MPTEST.MESSAGE_DOT_COUNT
         loop

           exit when MPTEST.STOP_TEST = TRUE;

            RTEMS.MESSAGE_QUEUE.RECEIVE(
               MPTEST.QUEUE_ID( 1 ),
               MPTEST.BUFFERS( INDEX ),
               RTEMS.DEFAULT_OPTIONS,
               RTEMS.NO_TIMEOUT,
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( 
               STATUS, 
               "MESSAGE_QUEUE_RECEIVE" 
            );

            if BUFFER_COUNT.ALL = RTEMS.UNSIGNED32'LAST then
               BUFFER_COUNT.ALL   := 0;
               OVERFLOW_COUNT.ALL := OVERFLOW_COUNT.ALL + 1;
            else
               BUFFER_COUNT.ALL := BUFFER_COUNT.ALL + 1;
            end if;

            RTEMS.MESSAGE_QUEUE.SEND(
               MPTEST.QUEUE_ID( 1 ),
               MPTEST.BUFFERS( INDEX ),
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_SEND" );

            if MPTEST.STOP_TEST = FALSE then
               if TEST_SUPPORT.NODE = 1 then

                  YIELD_COUNT := YIELD_COUNT - 1;

                  if YIELD_COUNT = 0 then

                     RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
                     TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "YIELD" );

                     YIELD_COUNT := 100;

                  end if;

               end if;

            end if;
         
         end loop;

         TEST_SUPPORT.PUT_DOT( "m" );

      end loop;

      MPTEST.EXIT_TEST;

   end MESSAGE_QUEUE_TASK;
 
-- 
--  PARTITION_TASK 
--
--  DESCRIPTION: 
-- 
--  This is one of the test tasks. 
-- 
  
   procedure PARTITION_TASK (  
      IGNORED : in     RTEMS.TASKS.ARGUMENT 
   ) is
      COUNT  : RTEMS.UNSIGNED32;
      BUFFER : RTEMS.ADDRESS;
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT_LINE( "Getting ID of partition" );
 
      loop

         RTEMS.PARTITION.IDENT(
            MPTEST.PARTITION_NAME( 1 ),
            RTEMS.SEARCH_ALL_NODES,
            MPTEST.PARTITION_ID( 1 ),
            STATUS
         );
         exit when RTEMS.IS_STATUS_SUCCESSFUL( STATUS );

         TEXT_IO.PUT_LINE( "partition_ident FAILED!!" );

      end loop;

      loop

         exit when MPTEST.STOP_TEST = TRUE;

         for COUNT in 1 .. MPTEST.PARTITION_DOT_COUNT
         loop

            exit when MPTEST.STOP_TEST = TRUE;

            RTEMS.PARTITION.GET_BUFFER(
               MPTEST.PARTITION_ID( 1 ),
               BUFFER,
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_GET_BUFFER" );

            RTEMS.PARTITION.RETURN_BUFFER(
               MPTEST.PARTITION_ID( 1 ),
               BUFFER,
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( 
               STATUS, 
               "PARTITION_RETURN_BUFFER" 
            );

            if TEST_SUPPORT.NODE = 1 then

               RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
               TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "YIELD" );

            end if;

         end loop;

         TEST_SUPPORT.PUT_DOT( "p" );

      end loop;

      MPTEST.EXIT_TEST;

   end PARTITION_TASK; 
 
-- 
--  SEMAPHORE_TASK 
-- 
--  DESCRIPTION: 
-- 
--  This is one of the test tasks. 
-- 
  
   procedure SEMAPHORE_TASK (  
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT 
   ) is
      COUNT          : RTEMS.UNSIGNED32;
      YIELD_COUNT    : RTEMS.UNSIGNED32;
      STATUS         : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT_LINE( "Getting ID of semaphore" );

      loop

         RTEMS.SEMAPHORE.IDENT(
            MPTEST.SEMAPHORE_NAME( 1 ),
            RTEMS.SEARCH_ALL_NODES,
            MPTEST.SEMAPHORE_ID( 1 ),
            STATUS
         );
         exit when RTEMS.IS_STATUS_SUCCESSFUL( STATUS );

         TEXT_IO.PUT_LINE( "semaphore_ident FAILED!!" );

      end loop;

      loop

         YIELD_COUNT := 100;

         exit when MPTEST.STOP_TEST = TRUE;

         for COUNT in 1 .. MPTEST.SEMAPHORE_DOT_COUNT
         loop

            exit when MPTEST.STOP_TEST = TRUE;

            RTEMS.SEMAPHORE.OBTAIN(
               MPTEST.SEMAPHORE_ID( 1 ),
               RTEMS.DEFAULT_OPTIONS,
               RTEMS.NO_TIMEOUT,
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN" );

            RTEMS.SEMAPHORE.RELEASE( MPTEST.SEMAPHORE_ID( 1 ), STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE" );

            if TEST_SUPPORT.NODE = 1 then

               YIELD_COUNT := YIELD_COUNT - 1;

               if YIELD_COUNT = 0 then

                  RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
                  TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "YIELD" );

                  YIELD_COUNT := 100;

               end if;

            end if;
         
         end loop;

         TEST_SUPPORT.PUT_DOT( "s" );

      end loop;

      MPTEST.EXIT_TEST;

   end SEMAPHORE_TASK; 
 
end MPTEST;
