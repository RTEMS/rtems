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
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.OARcorp.com/rtems/license.html.
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
      TASK_ID : RTEMS.ID;
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 3 ***" );

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'T', 'A', '1', ' ' ),
         252, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TASK_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TEST_INIT" );

      RTEMS.TASK_START( TASK_ID, TMTEST.TEST_INIT'ACCESS, 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TEST_INIT" );

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  TEST_INIT
--

   procedure TEST_INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      INDEX    : RTEMS.UNSIGNED32;
      PRIORITY : RTEMS.TASK_PRIORITY;
      TASK_ID  : RTEMS.ID;
      STATUS   : RTEMS.STATUS_CODES;
   begin

      PRIORITY := 250;
 
      RTEMS.SEMAPHORE_CREATE(
         RTEMS.BUILD_NAME( 'S', 'M', '1', ' ' ),
         0,
         RTEMS.DEFAULT_ATTRIBUTES,
         RTEMS.NO_PRIORITY,
         TMTEST.SEMAPHORE_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE OF SM1" );

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

         PRIORITY := PRIORITY - 1;

         RTEMS.TASK_START( TASK_ID, TMTEST.MIDDLE_TASKS'ACCESS, 0, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START MIDDLE" );

      end loop;

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'H', 'I', 'G', 'H' ),
         PRIORITY, 
         1024, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TASK_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF HIGH TASK" );

      RTEMS.TASK_START( TASK_ID, TMTEST.HIGH_TASK'ACCESS, 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF HIGH TASK" );

      TIMER_DRIVER.INITIALIZE;
      RTEMS.SEMAPHORE_RELEASE( TMTEST.SEMAPHORE_ID, STATUS );

   end TEST_INIT;

--PAGE
-- 
--  MIDDLE_TASKS
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      INDEX    : RTEMS.UNSIGNED32;
      PRIORITY : RTEMS.TASK_PRIORITY;
      ID       : RTEMS.ID;
      STATUS   : RTEMS.STATUS_CODES;
   begin
   
      RTEMS.SEMAPHORE_OBTAIN( 
         TMTEST.SEMAPHORE_ID, 
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         STATUS
      );

      RTEMS.SEMAPHORE_RELEASE( TMTEST.SEMAPHORE_ID, STATUS );

   end MIDDLE_TASKS;

--PAGE
-- 
--  HIGH_TASK
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      INDEX    : RTEMS.UNSIGNED32;
      PRIORITY : RTEMS.TASK_PRIORITY;
      ID       : RTEMS.ID;
      STATUS   : RTEMS.STATUS_CODES;
   begin
   
      RTEMS.SEMAPHORE_OBTAIN( 
         TMTEST.SEMAPHORE_ID, 
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         STATUS
      );

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_RELEASE (preemptive)", 
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.SEMAPHORE_RELEASE
      );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end HIGH_TASK;

end TMTEST;
