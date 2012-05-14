--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 15 of the RTEMS
--  Timing Test Suite.
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

with RTEMS_CALLING_OVERHEAD;
with TEST_SUPPORT;
with TEXT_IO;
with TIMER_DRIVER;
with RTEMS.EVENT;

package body TMTEST is

-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 15 ***" );

      TMTEST.TEST_INIT;

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  TEST_INIT
--

   procedure TEST_INIT
   is
      ID        : RTEMS.ID;
      OVERHEAD  : RTEMS.UNSIGNED32;
      EVENT_OUT : RTEMS.EVENT_SET;
      STATUS    : RTEMS.STATUS_CODES;
   begin

      TMTEST.TIME_SET := FALSE;

      RTEMS.TASKS.CREATE( 
         RTEMS.BUILD_NAME( 'L', 'O', 'W', ' ' ),
         10, 
         1024, 
         RTEMS.NO_PREEMPT,
         RTEMS.DEFAULT_ATTRIBUTES,
         ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOW" );

      RTEMS.TASKS.START( ID, TMTEST.LOW_TASK'ACCESS, 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOW" );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASKS.CREATE( 
            RTEMS.BUILD_NAME( 'H', 'I', 'G', 'H' ),
            5, 
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.TASK_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         RTEMS.TASKS.START( 
            TMTEST.TASK_ID( INDEX ),
            TMTEST.HIGH_TASKS'ACCESS, 
            0, 
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            TIMER_DRIVER.EMPTY_FUNCTION;
         end loop;
      OVERHEAD := TIMER_DRIVER.READ_TIMER;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.EVENT.RECEIVE(
               RTEMS.PENDING_EVENTS,
               RTEMS.DEFAULT_OPTIONS,
               RTEMS.NO_TIMEOUT,
               EVENT_OUT,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "EVENT_RECEIVE (current)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.EVENT_RECEIVE 
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.EVENT.RECEIVE(
               RTEMS.ALL_EVENTS,
               RTEMS.NO_WAIT,
               RTEMS.NO_TIMEOUT,
               EVENT_OUT,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "EVENT_RECEIVE (NO_WAIT)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.EVENT_RECEIVE 
      );

   end TEST_INIT;

-- 
--  LOW_TASK
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      EVENT_OUT : RTEMS.EVENT_SET;
      OVERHEAD  : RTEMS.UNSIGNED32;
      STATUS    : RTEMS.STATUS_CODES;
   begin
   
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "EVENT_RECEIVE (blocking)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.EVENT_RECEIVE 
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            TIMER_DRIVER.EMPTY_FUNCTION;
         end loop;
      OVERHEAD := TIMER_DRIVER.READ_TIMER;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.EVENT.SEND( RTEMS.SELF, RTEMS.EVENT_16, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "EVENT_SEND (returns to caller)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.EVENT_SEND 
      );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.EVENT.RECEIVE(
            RTEMS.EVENT_16,
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.NO_TIMEOUT,
            EVENT_OUT,
            STATUS
         );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "EVENT_RECEIVE (available)",
         TMTEST.END_TIME, 
         1, 
         0,
         RTEMS_CALLING_OVERHEAD.EVENT_RECEIVE 
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.EVENT.SEND( 
               TMTEST.TASK_ID( INDEX ), 
               RTEMS.EVENT_16, 
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "EVENT_SEND (readying)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.EVENT_SEND 
      );

      TEXT_IO.PUT_LINE( "*** END OF TIME TEST 15 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end LOW_TASK;

-- 
--  HIGH_TASKS
--

   procedure HIGH_TASKS (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      EVENT_OUT : RTEMS.EVENT_SET;
      STATUS    : RTEMS.STATUS_CODES;
   begin

      if TMTEST.TIME_SET = FALSE then
         TMTEST.TIME_SET := TRUE;
         TIMER_DRIVER.INITIALIZE;
      end if;

      RTEMS.EVENT.RECEIVE(
         16#7FFFFFFF#,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         EVENT_OUT,
         STATUS
      );
   
   end HIGH_TASKS;

end TMTEST;
