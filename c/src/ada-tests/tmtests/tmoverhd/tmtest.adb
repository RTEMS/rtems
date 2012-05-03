--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Calling Overhead Test of the RTEMS
--  Timing Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-2009.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with DUMMY_RTEMS;
with TEST_SUPPORT;
with TEXT_IO;
with TIME_TEST_SUPPORT;
with TIMER_DRIVER;
with RTEMS.CLOCK;

package body TMTEST is

--PAGE
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
      TEXT_IO.PUT_LINE( "*** TIME TEST OVERHEAD ***" );

      TIMER_DRIVER.SET_FIND_AVERAGE_OVERHEAD( TRUE );

      TMTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );

      RTEMS.TASKS.CREATE( 
         TMTEST.TASK_NAME( 1 ), 
         254, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TMTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASKS.START(
         TMTEST.TASK_ID( 1 ),
         TMTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  TIMER_HANDLER
--

   procedure TIMER_HANDLER (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   ) is
   begin

      NULL;

   end TIMER_HANDLER;

--PAGE
-- 
--  ISR_HANDLER
--

--   procedure ISR_HANDLER (
--      VECTOR : in     RTEMS.VECTOR_NUMBER
--   ) is
--   begin
--
--      NULL;
--
--   end ISR_HANDLER;

--PAGE
-- 
--  ASR_HANDLER
--

   procedure ASR_HANDLER (
      SIGNALS : in     RTEMS.SIGNAL_SET
   ) is
   begin

      NULL;

   end ASR_HANDLER;

--PAGE
-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      NAME           : RTEMS.NAME;
      OVERHEAD       : RTEMS.UNSIGNED32;
      ID             : RTEMS.ID;
      IN_PRIORITY    : RTEMS.TASKS.PRIORITY;
      OUT_PRIORITY   : RTEMS.TASKS.PRIORITY;
      IN_MODE        : RTEMS.MODE;
      MASK           : RTEMS.MODE;
      OUT_MODE       : RTEMS.MODE;
      NOTE           : RTEMS.UNSIGNED32;
      TIME           : RTEMS.TIME_OF_DAY;
      TIMEOUT        : RTEMS.INTERVAL;
      SIGNALS        : RTEMS.SIGNAL_SET;
      ADDRESS_1      : RTEMS.ADDRESS;
      EVENTS         : RTEMS.SIGNAL_SET;
      BUFFER         : TMTEST.BUFFER;
      BUFFER_POINTER : RTEMS.ADDRESS;
      MESSAGE_SIZE   : RTEMS.UNSIGNED32;
      COUNT          : RTEMS.UNSIGNED32;
      ERROR          : RTEMS.UNSIGNED32;
      STATUS         : RTEMS.STATUS_CODES;
   begin

      NAME := RTEMS.BUILD_NAME(  'N', 'A', 'M', 'E' );

      BUFFER_POINTER := BUFFER'ADDRESS;
      OVERHEAD := 0;
      IN_PRIORITY := 0;
      IN_MODE := 0;
      MASK := 0;
      SIGNALS := 0;
      EVENTS := 0;
      ERROR := 0;
     
-- TASK_CREATE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TASK_CREATE( 
               NAME, 
               IN_PRIORITY, 
               2048, 
               RTEMS.DEFAULT_MODES, 
               RTEMS.DEFAULT_ATTRIBUTES, 
               ID, 
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_CREATE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TASK_IDENT

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TASK_IDENT(
               NAME,
               RTEMS.SEARCH_ALL_NODES,
               ID,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TASK_START

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TASK_START( ID, TMTEST.TASK_1'ACCESS, 0, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_START",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TASK_RESTART

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TASK_RESTART( ID, 0, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_RESTART",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TASK_DELETE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TASK_DELETE( ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_DELETE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TASK_SUSPEND

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TASK_SUSPEND( ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_SUSPEND",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TASK_RESUME

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TASK_RESUME( ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_RESUME",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TASK_SET_PRIORITY

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TASK_SET_PRIORITY( 
               ID, 
               IN_PRIORITY, 
               OUT_PRIORITY, 
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_SET_PRIORITY",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TASK_MODE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TASK_MODE( 
               IN_MODE, 
               MASK,
               OUT_MODE, 
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_MODE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TASK_GET_NOTE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TASK_GET_NOTE( ID, 1, NOTE, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_GET_NOTE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TASK_SET_NOTE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TASK_SET_NOTE( ID, 1, NOTE, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_SET_NOTE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TASK_WAKE_WHEN

      TIME.YEAR    := 2000;
      TIME.MONTH   := 12;
      TIME.DAY     := 25;
      TIME.HOUR    := 6;
      TIME.MINUTE  := 1;
      TIME.SECOND  := 2;
      TIME.TICKS   := 0;
      
      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TASK_WAKE_WHEN( TIME, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_WAKE_WHEN",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TASK_WAKE_AFTER

      TIMEOUT := 1;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TASK_WAKE_AFTER( TIMEOUT, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_WAKE_AFTER",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT,
         OVERHEAD,
         0
      );

-- INTERRUPT_CATCH

--      TIMER_DRIVER.INITIALIZE;
--         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
--         loop
--            DUMMY_RTEMS.INTERRUPT_CATCH( 
--               TMTEST.ISR_HANDLER'ADDRESS,  
--               RTEMS.VECTOR_NUMBER'FIRST,
--               ADDRESS_1,
--               STATUS
--            );
--         end loop;
--      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
--
--      TIME_TEST_SUPPORT.PUT_TIME( 
--         "INTERRUPT_CATCH",
--         TMTEST.END_TIME, 
--         TIME_TEST_SUPPORT.OPERATION_COUNT, 
--         OVERHEAD,
--         0
--      );

-- CLOCK_GET

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.CLOCK_GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "CLOCK_GET",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- CLOCK_SET

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.CLOCK_SET( TIME, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "CLOCK_SET",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- CLOCK_TICK

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.CLOCK_TICK( STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "CLOCK_TICK",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

TEST_SUPPORT.PAUSE;

-- TIMER_CREATE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TIMER_CREATE( NAME, ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_CREATE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TIMER_DELETE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TIMER_DELETE( ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_DELETE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TIMER_IDENT

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TIMER_IDENT(
               NAME,
               ID,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TIMER_FIRE_AFTER

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TIMER_FIRE_AFTER( 
               ID, 
               TIMEOUT, 
               TMTEST.TIMER_HANDLER'ACCESS,
               RTEMS.NULL_ADDRESS,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_FIRE_AFTER",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TIMER_FIRE_WHEN

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TIMER_FIRE_WHEN(
               ID, 
               TIME, 
               TMTEST.TIMER_HANDLER'ACCESS,
               RTEMS.NULL_ADDRESS,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_FIRE_WHEN",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TIMER_RESET

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TIMER_RESET( ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_RESET",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- TIMER_CANCEL

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.TIMER_CANCEL( ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_CANCEL",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- SEMAPHORE_CREATE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.SEMAPHORE_CREATE( 
               NAME, 
               128,
               RTEMS.DEFAULT_ATTRIBUTES,
               RTEMS.TASKS.NO_PRIORITY,
               ID, 
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_CREATE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- SEMAPHORE_DELETE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.SEMAPHORE_DELETE( ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_DELETE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- SEMAPHORE_IDENT

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.SEMAPHORE_IDENT(
               NAME,
               RTEMS.SEARCH_ALL_NODES,
               ID,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- SEMAPHORE_OBTAIN

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.SEMAPHORE_OBTAIN( 
               ID, 
               RTEMS.DEFAULT_OPTIONS,
               TIMEOUT,
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_OBTAIN",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- SEMAPHORE_RELEASE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.SEMAPHORE_RELEASE( ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_RELEASE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- MESSAGE_QUEUE_CREATE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.MESSAGE_QUEUE_CREATE( 
               NAME, 
               128,
               16,
               RTEMS.DEFAULT_ATTRIBUTES,
               ID, 
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "MESSAGE_QUEUE_CREATE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- MESSAGE_QUEUE_IDENT

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.MESSAGE_QUEUE_IDENT(
               NAME,
               RTEMS.SEARCH_ALL_NODES,
               ID,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "MESSAGE_QUEUE_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- MESSAGE_QUEUE_DELETE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.MESSAGE_QUEUE_DELETE( ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "MESSAGE_QUEUE_DELETE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- MESSAGE_QUEUE_SEND

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.MESSAGE_QUEUE_SEND( ID, BUFFER_POINTER, 16, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "MESSAGE_QUEUE_SEND",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- MESSAGE_QUEUE_URGENT

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.MESSAGE_QUEUE_URGENT( ID, BUFFER_POINTER, 16, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "MESSAGE_QUEUE_URGENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- MESSAGE_QUEUE_BROADCAST

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.MESSAGE_QUEUE_BROADCAST( 
               ID, 
               BUFFER_POINTER, 
               16,
               COUNT, 
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "MESSAGE_QUEUE_BROADCAST",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- MESSAGE_QUEUE_RECEIVE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.MESSAGE_QUEUE_RECEIVE( 
               ID, 
               BUFFER_POINTER,
               RTEMS.DEFAULT_OPTIONS,
               TIMEOUT,
               MESSAGE_SIZE,
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "MESSAGE_QUEUE_RECEIVE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- MESSAGE_QUEUE_FLUSH

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.MESSAGE_QUEUE_FLUSH( ID, COUNT, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "MESSAGE_QUEUE_FLUSH",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

TEST_SUPPORT.PAUSE;

-- EVENT_SEND

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.EVENT_SEND( ID, EVENTS, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "EVENT_SEND",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- EVENT_RECEIVE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.EVENT_RECEIVE( 
               RTEMS.EVENT_16, 
               EVENTS, 
               RTEMS.DEFAULT_OPTIONS,
               TIMEOUT,
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "EVENT_RECEIVE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- SIGNAL_CATCH

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.SIGNAL_CATCH( 
               TMTEST.ASR_HANDLER'ACCESS, 
               RTEMS.DEFAULT_MODES,
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SIGNAL_CATCH",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- SIGNAL_SEND

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.SIGNAL_SEND( ID, SIGNALS, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SIGNAL_SEND",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- PARTITION_CREATE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.PARTITION_CREATE( 
               NAME, 
               TMTEST.MEMORY_AREA'ADDRESS,
               2048,
               128,
               RTEMS.DEFAULT_ATTRIBUTES,
               ID, 
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PARTITION_CREATE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- PARTITION_IDENT

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.PARTITION_IDENT(
               NAME,
               RTEMS.SEARCH_ALL_NODES,
               ID,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PARTITION_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- PARTITION_DELETE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.PARTITION_DELETE( ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PARTITION_DELETE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- PARTITION_GET_BUFFER

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.PARTITION_GET_BUFFER( ID, ADDRESS_1, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PARTITION_GET_BUFFER",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- PARTITION_RETURN_BUFFER

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.PARTITION_RETURN_BUFFER( ID, ADDRESS_1, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PARTITION_RETURN_BUFFER",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- REGION_CREATE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.REGION_CREATE( 
               NAME, 
               TMTEST.MEMORY_AREA'ADDRESS,
               2048,
               128,
               RTEMS.DEFAULT_ATTRIBUTES,
               ID, 
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "REGION_CREATE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- REGION_IDENT

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.REGION_IDENT(
               NAME,
               ID,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "REGION_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- REGION_DELETE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.REGION_DELETE( ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "REGION_DELETE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- REGION_GET_SEGMENT

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.REGION_GET_SEGMENT( 
               ID, 
               243,
               RTEMS.DEFAULT_OPTIONS,
               TIMEOUT,
               ADDRESS_1,
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "REGION_GET_SEGMENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- REGION_RETURN_SEGMENT

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.REGION_RETURN_SEGMENT( ID, ADDRESS_1, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "REGION_RETURN_SEGMENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- PORT_CREATE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.PORT_CREATE( 
               NAME, 
               TMTEST.INTERNAL_PORT_AREA'ADDRESS,
               TMTEST.EXTERNAL_PORT_AREA'ADDRESS,
               TMTEST.INTERNAL_PORT_AREA'LENGTH,
               ID, 
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PORT_CREATE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- PORT_IDENT

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.PORT_IDENT( NAME, ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PORT_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- PORT_DELETE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.PORT_DELETE( ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PORT_DELETE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- PORT_EXTERNAL_TO_INTERNAL

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.PORT_EXTERNAL_TO_INTERNAL( 
               ID, 
               TMTEST.EXTERNAL_PORT_AREA( 7 )'ADDRESS,
               ADDRESS_1,
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PORT_EXTERNAL_TO_INTERNAL",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- PORT_INTERNAL_TO_EXTERNAL

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.PORT_INTERNAL_TO_EXTERNAL( 
               ID, 
               TMTEST.INTERNAL_PORT_AREA( 7 )'ADDRESS,
               ADDRESS_1,
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PORT_INTERNAL_TO_EXTERNAL",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

TEST_SUPPORT.PAUSE;

-- FATAL_ERROR_OCCURRED

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.FATAL_ERROR_OCCURRED( ERROR );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "FATAL_ERROR_OCCURRED",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- RATE_MONOTONIC_CREATE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.RATE_MONOTONIC_CREATE( NAME, ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "RATE_MONOTONIC_CREATE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- RATE_MONOTONIC_IDENT

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.RATE_MONOTONIC_IDENT(
               NAME,
               ID,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "RATE_MONOTONIC_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- RATE_MONOTONIC_DELETE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.RATE_MONOTONIC_DELETE( ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "RATE_MONOTONIC_DELETE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- RATE_MONOTONIC_CANCEL

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.RATE_MONOTONIC_CANCEL( ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "RATE_MONOTONIC_CANCEL",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- RATE_MONOTONIC_PERIOD

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.RATE_MONOTONIC_PERIOD( ID, TIMEOUT, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "RATE_MONOTONIC_PERIOD",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

-- MULTIPROCESSING_ANNOUNCE

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            DUMMY_RTEMS.MULTIPROCESSING_ANNOUNCE;
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "MULTIPROCESSING_ANNOUNCE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         0
      );

      TEXT_IO.PUT_LINE( "*** END OF TIME TEST OVERHEAD ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );
   
   end TASK_1;

end TMTEST;
