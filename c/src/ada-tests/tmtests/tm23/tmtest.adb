--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 23 of the RTEMS
--  Timing Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--
--  $Id$
--

with INTERFACES; use INTERFACES;
with RTEMS;
with RTEMS_CALLING_OVERHEAD;
with TEST_SUPPORT;
with TEXT_IO;
with TIME_TEST_SUPPORT;
with UNSIGNED32_IO;

package body TMTEST is

--PAGE
-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      OVERHEAD      : RTEMS.UNSIGNED32;
      TASK_PRIORITY : RTEMS.TASK_PRIORITY;
      INDEX         : RTEMS.UNSIGNED32;
      ID            : RTEMS.ID;
      TASK_ENTRY    : RTEMS.TASK_ENTRY;
      STATUS        : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 23 ***" );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            TIMER_DRIVER.EMPTY_FUNCTION;
         end loop;
      OVERHEAD := TIMER_DRIVER.READ_TIMER;

      TASK_PRIORITY := 5;

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASK_CREATE( 
            RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
            TASK_PRIORITY, 
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         if INDEX = 1 then
            TASK_ENTRY := TMTEST.HIGH_TASK'ACCESS;
         elsif INDEX = TIME_TEST_SUPPORT.OPERATION_COUNT then
            TASK_ENTRY := TMTEST.LOW_TASK'ACCESS;
         else
            TASK_ENTRY := TMTEST.MIDDLE_TASKS'ACCESS;
         end if;

         RTEMS.TASK_START( ID, TASK_ENTRY, 0, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

         TASK_PRIORITY := TASK_PRIORITY + 1;

      end loop;

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  NULL_DELAY
--

   procedure NULL_DELAY (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   ) is
   begin

      NULL;

   end NULL_DELAY;

--PAGE
-- 
--  HIGH_TASK
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      INDEX    : RTEMS.TASK_PRIORITY;
      OVERHEAD : RTEMS.UNSIGNED32;
      STATUS   : RTEMS.STATUS_CODES;
   begin

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            TIMER_DRIVER.EMPTY_FUNCTION;
         end loop;
      OVERHEAD := TIMER_DRIVER.READ_TIMER;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TIMER_CREATE(
               INDEX,
               TMTEST.TIMER_ID( INDEX ),
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_CREATE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TIMER_CREATE
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TIMER_FIRE_AFTER(
               TMTEST.TIMER_ID( INDEX ),
               500,
               TMTEST.NULL_DELAY'ACCESS,
               RTEMS.NULL_ADDRESS,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_FIRE_AFTER (inactive)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TIMER_FIRE_AFTER
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TIMER_FIRE_AFTER(
               TMTEST.TIMER_ID( INDEX ),
               500,
               TMTEST.NULL_DELAY'ACCESS,
               RTEMS.NULL_ADDRESS,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_FIRE_AFTER (active)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TIMER_FIRE_AFTER
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TIMER_CANCEL( TMTEST.TIMER_ID( INDEX ), STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_CANCEL (active)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TIMER_CANCEL
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TIMER_CANCEL( TMTEST.TIMER_ID( INDEX ), STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_CANCEL (inactive)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TIMER_CANCEL
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TIMER_RESET( TMTEST.TIMER_ID( INDEX ), STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_RESET (inactive)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TIMER_RESET
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TIMER_RESET( TMTEST.TIMER_ID( INDEX ), STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_RESET (active)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TIMER_RESET
      );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TIMER_CANCEL( TMTEST.TIMER_ID( INDEX ), STATUS );
      end loop;

      TMTEST.TIME_OF_DAY := ( 1988, 12, 31, 9, 0, 0, 0 );

      RTEMS.CLOCK_SET( TMTEST.TIME_OF_DAY, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET" );

      TMTEST.TIME_OF_DAY.YEAR := 1989;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TIMER_FIRE_WHEN(
               TMTEST.TIMER_ID( INDEX ),
               TMTEST.TIME_OF_DAY,
               TMTEST.NULL_DELAY'ACCESS,
               RTEMS.NULL_ADDRESS,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_FIRE_WHEN (inactive)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TIMER_FIRE_WHEN
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TIMER_FIRE_WHEN(
               TMTEST.TIMER_ID( INDEX ),
               TMTEST.TIME_OF_DAY,
               TMTEST.NULL_DELAY'ACCESS,
               RTEMS.NULL_ADDRESS,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_FIRE_WHEN (active)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TIMER_FIRE_WHEN
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TIMER_DELETE( TMTEST.TIMER_ID( INDEX ), STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_DELETE (active)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TIMER_DELETE
      );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TIMER_CREATE( INDEX, TMTEST.TIMER_ID( INDEX ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE" );

         RTEMS.TIMER_FIRE_AFTER( 
            TMTEST.TIMER_ID( INDEX ), 
            500,
            TMTEST.NULL_DELAY'ACCESS,
            RTEMS.NULL_ADDRESS,
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );

         RTEMS.TIMER_CANCEL( TMTEST.TIMER_ID( INDEX ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CANCEL" );

      end loop;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TIMER_DELETE( TMTEST.TIMER_ID( INDEX ), STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_DELETE (inactive)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TIMER_DELETE
      );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.TASK_WAKE_WHEN( TMTEST.TIME_OF_DAY, STATUS );

   end HIGH_TASK;

--PAGE
-- 
--  MIDDLE_TASKS
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASK_WAKE_WHEN( TMTEST.TIME_OF_DAY, STATUS );

   end MIDDLE_TASKS;

--PAGE
-- 
--  LOW_TASK
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
   begin

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_WAKE_WHEN",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TASK_WAKE_WHEN
      );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end LOW_TASK;

end TMTEST;
