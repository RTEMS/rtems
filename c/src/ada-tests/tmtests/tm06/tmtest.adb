--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 6 of the RTEMS
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
      TEXT_IO.PUT_LINE( "*** TIME TEST 6 ***" );

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
      INDEX      : RTEMS.UNSIGNED32;
      ID         : RTEMS.ID;
      STATUS     : RTEMS.STATUS_CODES;
   begin

      TMTEST.TASK_RESTARTED := TIME_TEST_SUPPORT.OPERATION_COUNT;

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
         128, 
         1024, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

      RTEMS.TASK_START( ID, TMTEST.TASK_1'ACCESS, 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START" );

   end TEST_INIT;

--PAGE
-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      INDEX        : RTEMS.UNSIGNED32;
      OVERHEAD     : RTEMS.UNSIGNED32;
      STATUS       : RTEMS.STATUS_CODES;
   begin

      if TMTEST.TASK_RESTARTED = TIME_TEST_SUPPORT.OPERATION_COUNT then
         TIMER_DRIVER.INITIALIZE;
      end if;

      TMTEST.TASK_RESTARTED := TMTEST.TASK_RESTARTED - 1;

      if TMTEST.TASK_RESTARTED /= 0 then
         RTEMS.TASK_RESTART( RTEMS.SELF, 0, STATUS );
      end if;

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            TIMER_DRIVER.EMPTY_FUNCTION;
         end loop;
      OVERHEAD := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_RESTART self",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TASK_RESTART
      );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASK_CREATE( 
            RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
            254, 
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.TASK_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         RTEMS.TASK_START( 
            TMTEST.TASK_ID( INDEX ), 
            TMTEST.NULL_TASK'ACCESS, 
            0, 
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_SUSPEND( TMTEST.TASK_ID( INDEX ), STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_SUSPEND no preempt",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_SUSPEND
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_RESUME( TMTEST.TASK_ID( INDEX ), STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_RESUME no preempt",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_RESUME
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_DELETE( TMTEST.TASK_ID( INDEX ), STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_DELETE others",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_DELETE
      );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TASK_1;

--PAGE
-- 
--  NULL_TASK
--

   procedure NULL_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
   begin

      NULL;

   end NULL_TASK;

end TMTEST;
