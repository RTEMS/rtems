--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 2 of the RTEMS
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
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 2 ***" );

      TMTEST.TEST_INIT;

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  TEST_INIT
--

   procedure TEST_INIT is
      PRIORITY : RTEMS.TASK_PRIORITY;
      INDEX    : RTEMS.UNSIGNED32;
      HIGH_ID  : RTEMS.ID;
      LOW_ID   : RTEMS.ID;
      TASK_ID  : RTEMS.ID;
      STATUS   : RTEMS.STATUS_CODES;
   begin

      PRIORITY := 5;

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'H', 'I', 'G', 'H' ),
         PRIORITY, 
         1024, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         HIGH_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF HIGH TASK" );

      PRIORITY := PRIORITY + 1;

      RTEMS.TASK_START( 
         HIGH_ID, 
         TMTEST.HIGH_TASK'ACCESS, 
         0, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF HIGH TASK" );

      for INDEX in 2 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASK_CREATE( 
            RTEMS.BUILD_NAME( 'M', 'I', 'D', ' ' ),
            PRIORITY, 
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            TASK_ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE MIDDLE" );

         PRIORITY := PRIORITY + 1;

         RTEMS.TASK_START( 
            TASK_ID, 
            TMTEST.MIDDLE_TASKS'ACCESS, 
            0, 
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START MIDDLE" );

      end loop;

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'L', 'O', 'W', ' ' ),
         PRIORITY, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         LOW_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF LOW TASK" );

      RTEMS.TASK_START( LOW_ID, TMTEST.LOW_TASK'ACCESS, 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF LOW TASK" );

      RTEMS.SEMAPHORE_CREATE(
         RTEMS.BUILD_NAME( 'S', 'M', '1', ' ' ),
         0,
         RTEMS.DEFAULT_ATTRIBUTES,
         RTEMS.NO_PRIORITY,
         TMTEST.SEMAPHORE_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE OF SM1" );

   end TEST_INIT;

--PAGE
-- 
--  HIGH_TASK
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS                   : RTEMS.STATUS_CODES;
   begin

      TIMER_DRIVER.INITIALIZE;
      RTEMS.SEMAPHORE_OBTAIN(
         TMTEST.SEMAPHORE_ID,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
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
      STATUS                   : RTEMS.STATUS_CODES;
   begin

      RTEMS.SEMAPHORE_OBTAIN(
         TMTEST.SEMAPHORE_ID,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
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
      STATUS                   : RTEMS.STATUS_CODES;
   begin

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_OBTAIN (blocking)", 
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.SEMAPHORE_OBTAIN 
      );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end LOW_TASK;

end TMTEST;
