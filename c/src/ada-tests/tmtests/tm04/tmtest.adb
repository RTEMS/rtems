--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 4 of the RTEMS
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
      TEXT_IO.PUT_LINE( "*** TIME TEST 4 ***" );

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
      INDEX    : RTEMS.UNSIGNED32;
      STATUS   : RTEMS.STATUS_CODES;
   begin

      TMTEST.TASK_COUNT := TIME_TEST_SUPPORT.OPERATION_COUNT;

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASK_CREATE( 
            RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
            10, 
            1024, 
            RTEMS.NO_PREEMPT,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.TASK_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         RTEMS.TASK_START( 
            TMTEST.TASK_ID( INDEX ),
            TMTEST.LOW_TASKS'ACCESS, 
            0, 
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;

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
--  HIGHEST_TASK
--

   procedure HIGHEST_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      OLD_PRIORITY : RTEMS.TASK_PRIORITY;
      STATUS       : RTEMS.STATUS_CODES;
   begin

      if ARGUMENT = 1 then

         TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

         TIME_TEST_SUPPORT.PUT_TIME( 
            "TASK_RESTART (blocked, preempt)",
            TMTEST.END_TIME, 
            1, 
            0,
            RTEMS_CALLING_OVERHEAD.TASK_RESTART
         );
 
         RTEMS.TASK_SET_PRIORITY( RTEMS.SELF, 254, OLD_PRIORITY, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY" );

      elsif ARGUMENT = 2 then

         TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

         TIME_TEST_SUPPORT.PUT_TIME( 
            "TASK_RESTART (ready, preempt)",
            TMTEST.END_TIME, 
            1, 
            0,
            RTEMS_CALLING_OVERHEAD.TASK_RESTART
         );

         RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE" );
 
      else

         RTEMS.SEMAPHORE_OBTAIN( 
            TMTEST.SEMAPHORE_ID, 
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.NO_TIMEOUT,
            STATUS
         );

      end if;

   end HIGHEST_TASK;

--PAGE
-- 
--  HIGH_TASK
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      OLD_PRIORITY : RTEMS.TASK_PRIORITY;
      INDEX        : RTEMS.UNSIGNED32;
      OVERHEAD     : RTEMS.UNSIGNED32;
      NAME         : RTEMS.NAME;
      STATUS       : RTEMS.STATUS_CODES;
   begin
 
      TIMER_DRIVER.INITIALIZE;
         RTEMS.TASK_RESTART( TMTEST.HIGHEST_ID, 1, STATUS );
      -- preempted by Higher_task

      TIMER_DRIVER.INITIALIZE;
         RTEMS.TASK_RESTART( TMTEST.HIGHEST_ID, 2, STATUS );
      -- preempted by Higher_task

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            TIMER_DRIVER.EMPTY_FUNCTION;
         end loop;
      OVERHEAD := TIMER_DRIVER.READ_TIMER;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.SEMAPHORE_RELEASE( TMTEST.SEMAPHORE_ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_RELEASE (readying)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.SEMAPHORE_RELEASE
      );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TASK_DELETE( TMTEST.TASK_ID( INDEX ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE" );
      end loop;
     
      NAME := RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_CREATE( 
               NAME,
               10, 
               1024, 
               RTEMS.NO_PREEMPT,
               RTEMS.DEFAULT_ATTRIBUTES,
               TMTEST.TASK_ID( INDEX ),
               STATUS
            );
         end loop; 
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_CREATE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_CREATE
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_START( 
               TMTEST.TASK_ID( INDEX ),
               TMTEST.LOW_TASKS'ACCESS, 
               0, 
               STATUS
            );
         end loop; 
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_START",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_START
      );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TASK_DELETE( TMTEST.TASK_ID( INDEX ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE LOOP" );
      end loop;
     
      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TASK_CREATE( 
            NAME,
            250, 
            1024, 
            RTEMS.NO_PREEMPT,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.TASK_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );
         RTEMS.TASK_START( 
            TMTEST.TASK_ID( INDEX ),
            TMTEST.RESTART_TASK'ACCESS, 
            0, 
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );
         RTEMS.TASK_SUSPEND( TMTEST.TASK_ID( INDEX ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND LOOP" );
      end loop;
     
      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_RESTART( TMTEST.TASK_ID( INDEX ), 0, STATUS );
         end loop; 
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_RESTART (suspended)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_RESTART
      );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TASK_SUSPEND( TMTEST.TASK_ID( INDEX ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND LOOP" );
      end loop;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_DELETE( TMTEST.TASK_ID( INDEX ), STATUS );
         end loop; 
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_DELETE (suspended)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_DELETE
      );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TASK_CREATE( 
            NAME,
            250, 
            1024, 
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.TASK_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );
         RTEMS.TASK_START( 
            TMTEST.TASK_ID( INDEX ),
            TMTEST.RESTART_TASK'ACCESS, 
            0, 
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );
      end loop;
     
      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_RESTART( TMTEST.TASK_ID( INDEX ), 1, STATUS );
         end loop; 
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_RESTART (ready)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_RESTART
      );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TASK_SET_PRIORITY( 
            TMTEST.TASK_ID( INDEX ), 
            5, 
            OLD_PRIORITY, 
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY LOOP" );
      end loop; 

      -- yield processor -- tasks block
      RTEMS.TASK_WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS ); 
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
    
      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_RESTART( TMTEST.TASK_ID( INDEX ), 1, STATUS );
         end loop; 
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_RESTART (blocked, no preempt)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_RESTART
      );

      -- yield processor -- tasks block
      RTEMS.TASK_WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS ); 
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
    
      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_DELETE( TMTEST.TASK_ID( INDEX ), STATUS );
         end loop; 
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_DELETE (blocked)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_DELETE
      );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end HIGH_TASK;

--PAGE
-- 
--  LOW_TASKS
--

   procedure LOW_TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      ID         : RTEMS.ID;
      STATUS     : RTEMS.STATUS_CODES;
   begin
   
      TMTEST.TASK_COUNT := TMTEST.TASK_COUNT - 1;

      if TMTEST.TASK_COUNT = 0 then

         RTEMS.TASK_CREATE( 
            RTEMS.BUILD_NAME( 'H', 'I', ' ', ' ' ),
            5, 
            2048, 
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.DEFAULT_ATTRIBUTES,
            ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE HI" );

         RTEMS.TASK_START( 
            ID, 
            TMTEST.HIGH_TASK'ACCESS, 
            0, 
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START HI" );


         RTEMS.TASK_CREATE( 
            RTEMS.BUILD_NAME( 'H', 'I', 'G', 'H' ),
            3, 
            2048, 
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.HIGHEST_ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE HIGH" );

         RTEMS.TASK_START( 
            TMTEST.HIGHEST_ID, 
            TMTEST.HIGHEST_TASK'ACCESS, 
            0, 
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START HIGH" );


      end if;

      RTEMS.SEMAPHORE_OBTAIN( 
         TMTEST.SEMAPHORE_ID, 
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         STATUS
      );

   end LOW_TASKS;

--PAGE
-- 
--  RESTART_TASK
--

   procedure RESTART_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      INDEX    : RTEMS.UNSIGNED32;
      PRIORITY : RTEMS.TASK_PRIORITY;
      ID       : RTEMS.ID;
      STATUS   : RTEMS.STATUS_CODES;
   begin
   
      if ARGUMENT = 1 then
         RTEMS.SEMAPHORE_OBTAIN( 
            TMTEST.SEMAPHORE_ID, 
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.NO_TIMEOUT,
            STATUS
         );
      end if;

   end RESTART_TASK;

end TMTEST;
