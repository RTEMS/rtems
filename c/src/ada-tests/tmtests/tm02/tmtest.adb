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
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with INTERFACES; use INTERFACES;
with RTEMS_CALLING_OVERHEAD;
with TEST_SUPPORT;
with TEXT_IO;
with TIME_TEST_SUPPORT;
with TIMER_DRIVER;
with RTEMS.SEMAPHORE;

package body TMTEST is

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
      TEXT_IO.PUT_LINE( "*** TIME TEST 2 ***" );

      TMTEST.TEST_INIT;

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  TEST_INIT
--

   procedure TEST_INIT is
      PRIORITY : RTEMS.TASKS.PRIORITY;
      HIGH_ID  : RTEMS.ID;
      LOW_ID   : RTEMS.ID;
      TASK_ID  : RTEMS.ID;
      STATUS   : RTEMS.STATUS_CODES;
   begin

      PRIORITY := 5;

      RTEMS.TASKS.CREATE( 
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

      RTEMS.TASKS.START( 
         HIGH_ID, 
         TMTEST.HIGH_TASK'ACCESS, 
         0, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF HIGH TASK" );

      for INDEX in 2 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASKS.CREATE( 
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

         RTEMS.TASKS.START( 
            TASK_ID, 
            TMTEST.MIDDLE_TASKS'ACCESS, 
            0, 
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START MIDDLE" );

      end loop;

      RTEMS.TASKS.CREATE( 
         RTEMS.BUILD_NAME( 'L', 'O', 'W', ' ' ),
         PRIORITY, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         LOW_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF LOW TASK" );

      RTEMS.TASKS.START( LOW_ID, TMTEST.LOW_TASK'ACCESS, 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF LOW TASK" );

      RTEMS.SEMAPHORE.CREATE(
         RTEMS.BUILD_NAME( 'S', 'M', '1', ' ' ),
         0,
         RTEMS.DEFAULT_ATTRIBUTES,
         RTEMS.TASKS.NO_PRIORITY,
         TMTEST.SEMAPHORE_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE OF SM1" );

   end TEST_INIT;

-- 
--  HIGH_TASK
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS                   : RTEMS.STATUS_CODES;
   begin

      TIMER_DRIVER.INITIALIZE;
      RTEMS.SEMAPHORE.OBTAIN(
         TMTEST.SEMAPHORE_ID,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN" );

   end HIGH_TASK;

-- 
--  MIDDLE_TASKS
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS                   : RTEMS.STATUS_CODES;
   begin

      RTEMS.SEMAPHORE.OBTAIN(
         TMTEST.SEMAPHORE_ID,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN" );

   end MIDDLE_TASKS;

-- 
--  LOW_TASK
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
   begin

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_OBTAIN (blocking)", 
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.SEMAPHORE_OBTAIN 
      );

      TEXT_IO.PUT_LINE( "*** END OF TIME TEST 2 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end LOW_TASK;

end TMTEST;
