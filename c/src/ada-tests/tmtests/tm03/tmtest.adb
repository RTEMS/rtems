--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 3 of the RTEMS
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
      TASK_ID : RTEMS.ID;
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 3 ***" );

      RTEMS.TASKS.CREATE( 
         RTEMS.BUILD_NAME( 'T', 'A', '1', ' ' ),
         252, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TASK_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TEST_INIT" );

      RTEMS.TASKS.START( TASK_ID, TMTEST.TEST_INIT'ACCESS, 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TEST_INIT" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  TEST_INIT
--

   procedure TEST_INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      PRIORITY : RTEMS.TASKS.PRIORITY;
      TASK_ID  : RTEMS.ID;
      STATUS   : RTEMS.STATUS_CODES;
   begin

      PRIORITY := 250;
 
      RTEMS.SEMAPHORE.CREATE(
         RTEMS.BUILD_NAME( 'S', 'M', '1', ' ' ),
         0,
         RTEMS.DEFAULT_ATTRIBUTES,
         RTEMS.TASKS.NO_PRIORITY,
         TMTEST.SEMAPHORE_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE OF SM1" );

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

         PRIORITY := PRIORITY - 1;

         RTEMS.TASKS.START( TASK_ID, TMTEST.MIDDLE_TASKS'ACCESS, 0, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START MIDDLE" );

      end loop;

      RTEMS.TASKS.CREATE( 
         RTEMS.BUILD_NAME( 'H', 'I', 'G', 'H' ),
         PRIORITY, 
         1024, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TASK_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF HIGH TASK" );

      RTEMS.TASKS.START( TASK_ID, TMTEST.HIGH_TASK'ACCESS, 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF HIGH TASK" );

      TIMER_DRIVER.INITIALIZE;
      RTEMS.SEMAPHORE.RELEASE( TMTEST.SEMAPHORE_ID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE" );

   end TEST_INIT;

-- 
--  MIDDLE_TASKS
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS   : RTEMS.STATUS_CODES;
   begin
   
      RTEMS.SEMAPHORE.OBTAIN( 
         TMTEST.SEMAPHORE_ID, 
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN" );

      RTEMS.SEMAPHORE.RELEASE( TMTEST.SEMAPHORE_ID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE" );

   end MIDDLE_TASKS;

-- 
--  HIGH_TASK
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS   : RTEMS.STATUS_CODES;
   begin
   
      RTEMS.SEMAPHORE.OBTAIN( 
         TMTEST.SEMAPHORE_ID, 
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         STATUS
      );

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "LAST CALL DID NOT WORK" );

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_RELEASE (preemptive)", 
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.SEMAPHORE_RELEASE
      );

      TEXT_IO.PUT_LINE( "*** END OF TIME TEST 3 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end HIGH_TASK;

end TMTEST;
