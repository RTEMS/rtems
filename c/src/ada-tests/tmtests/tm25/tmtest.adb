--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 25 of the RTEMS
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
      INDEX   : RTEMS.UNSIGNED32;
      TASK_ID : RTEMS.ID;
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 25 ***" );

      RTEMS.SEMAPHORE_CREATE(
         RTEMS.BUILD_NAME( 'S', 'M', '1', ' ' ),
         0,
         RTEMS.DEFAULT_ATTRIBUTES,
         RTEMS.NO_PRIORITY,
         TMTEST.SEMAPHORE_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE OF SM1" );

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'L', 'O', 'W', ' ' ),
         254, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TASK_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOW" );

      RTEMS.TASK_START( 
         TASK_ID, 
         TMTEST.LOW_TASK'ACCESS, 
         0, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOW" );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASK_CREATE( 
            RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
            128, 
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            TASK_ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         RTEMS.TASK_START( 
            TASK_ID, 
            TMTEST.HIGH_TASKS'ACCESS, 
            0, 
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;
      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  HIGH_TASKS
--

   procedure HIGH_TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.SEMAPHORE_OBTAIN(
         TMTEST.SEMAPHORE_ID,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.INTERVAL'LAST,
         STATUS
      );

   end HIGH_TASKS;

--PAGE
-- 
--  LOW_TASK
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS                   : RTEMS.STATUS_CODES;
   begin

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.CLOCK_TICK( STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "CLOCK_TICK",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.CLOCK_TICK
      );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end LOW_TASK;

end TMTEST;
