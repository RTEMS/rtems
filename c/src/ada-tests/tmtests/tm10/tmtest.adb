--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 10 of the RTEMS
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
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 10 ***" );

      TMTEST.TEST_INIT;

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  TEST_INIT
--

   procedure TEST_INIT
   is
      INDEX          : RTEMS.UNSIGNED32;
      TASK_ENTRY     : RTEMS.TASK_ENTRY;
      PRIORITY       : RTEMS.TASK_PRIORITY;
      OVERHEAD       : RTEMS.UNSIGNED32;
      TASK_ID        : RTEMS.ID;
      BUFFER         : TMTEST.BUFFER;
      BUFFER_POINTER : RTEMS.ADDRESS;
      MESSAGE_SIZE   : RTEMS.UNSIGNED32;
      STATUS         : RTEMS.STATUS_CODES;
   begin

      BUFFER_POINTER := BUFFER'ADDRESS;

      PRIORITY := 5;

      for INDEX in 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASK_CREATE( 
            RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
            PRIORITY, 
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            TASK_ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         PRIORITY := PRIORITY + 1;

         if INDEX = 0 then
            TASK_ENTRY := TMTEST.HIGH_TASK'ACCESS;
         elsif INDEX = TIME_TEST_SUPPORT.OPERATION_COUNT then
            TASK_ENTRY := TMTEST.LOW_TASK'ACCESS;
         else
            TASK_ENTRY := TMTEST.MIDDLE_TASKS'ACCESS;
         end if;

         RTEMS.TASK_START( TASK_ID, TASK_ENTRY, 0, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;

      RTEMS.MESSAGE_QUEUE_CREATE(
         1,
         TIME_TEST_SUPPORT.OPERATION_COUNT,
         16,
         RTEMS.DEFAULT_OPTIONS,
         TMTEST.QUEUE_ID,
         STATUS
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
            RTEMS.MESSAGE_QUEUE_RECEIVE( 
               TMTEST.QUEUE_ID,
               BUFFER_POINTER,
               RTEMS.NO_WAIT,
               RTEMS.NO_TIMEOUT,
               MESSAGE_SIZE,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME(
         "MESSAGE_QUEUE_RECEIVE (NO_WAIT)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.MESSAGE_QUEUE_RECEIVE 
      );

   end TEST_INIT;

--PAGE
-- 
--  HIGH_TASK
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      BUFFER         : TMTEST.BUFFER;
      BUFFER_POINTER : RTEMS.ADDRESS;
      MESSAGE_SIZE   : RTEMS.UNSIGNED32;
      STATUS         : RTEMS.STATUS_CODES;
   begin

      BUFFER_POINTER := BUFFER'ADDRESS;

      TIMER_DRIVER.INITIALIZE;

      RTEMS.MESSAGE_QUEUE_RECEIVE( 
         TMTEST.QUEUE_ID,
         BUFFER_POINTER,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         MESSAGE_SIZE,
         STATUS
      );

   end HIGH_TASK;

--PAGE
-- 
--  MIDDLE_TASKS
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      BUFFER         : TMTEST.BUFFER;
      BUFFER_POINTER : RTEMS.ADDRESS;
      MESSAGE_SIZE   : RTEMS.UNSIGNED32;
      STATUS         : RTEMS.STATUS_CODES;
   begin
 
      BUFFER_POINTER := BUFFER'ADDRESS;

      RTEMS.MESSAGE_QUEUE_RECEIVE( 
         TMTEST.QUEUE_ID,
         BUFFER_POINTER,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         MESSAGE_SIZE,
         STATUS
      );
 
   end MIDDLE_TASKS;

--PAGE
-- 
--  LOW_TASK
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS   : RTEMS.STATUS_CODES;
   begin

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME(
         "MESSAGE_QUEUE_RECEIVE (blocking)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.MESSAGE_QUEUE_RECEIVE 
      );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end LOW_TASK;

end TMTEST;
