--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 24 of the RTEMS
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
      ID         : RTEMS.ID;
      INDEX      : RTEMS.UNSIGNED32;
      STATUS     : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 24 ***" );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            TIMER_DRIVER.EMPTY_FUNCTION;
         end loop;
      TMTEST.OVERHEAD := TIMER_DRIVER.READ_TIMER;

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'H', 'I', 'G', 'H' ),
         10,
         1024, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE HIGH" );

      RTEMS.TASK_START( ID, TMTEST.HIGH_TASK'ACCESS, 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START HIGH" );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASK_CREATE( 
            RTEMS.BUILD_NAME( 'R', 'E', 'S', 'T' ),
            128,
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         RTEMS.TASK_START( ID, TMTEST.TASKS'ACCESS, 0, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  HIGH_TASK
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      INDEX    : RTEMS.UNSIGNED32;
      STATUS   : RTEMS.STATUS_CODES;
   begin

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_WAKE_AFTER (no context switch)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         TMTEST.OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_WAKE_AFTER
      );

      TMTEST.TASK_COUNT := 0;

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end HIGH_TASK;

--PAGE
-- 
--  TASKS
--

   procedure TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS            : RTEMS.STATUS_CODES;
   begin

      TMTEST.TASK_COUNT := TMTEST.TASK_COUNT + 1;

      if TMTEST.TASK_COUNT = 1 then
         
         TIMER_DRIVER.INITIALIZE;

      elsif TMTEST.TASK_COUNT = TIME_TEST_SUPPORT.OPERATION_COUNT then
         TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

         TIME_TEST_SUPPORT.PUT_TIME( 
            "TASK_WAKE_AFTER (context switch)",
            TMTEST.END_TIME, 
            TIME_TEST_SUPPORT.OPERATION_COUNT, 
            TMTEST.OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_WAKE_AFTER
         );

         RTEMS.SHUTDOWN_EXECUTIVE( 0 );

      end if;

      RTEMS.TASK_WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );

   end TASKS;

end TMTEST;
