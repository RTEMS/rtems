--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 19 of the RTEMS
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
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 19 ***" );

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
         128,
         1024, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TMTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TASK1" );

      RTEMS.TASK_START( 
         TMTEST.TASK_ID( 1 ),
         TMTEST.TASK_1'ACCESS, 
         0, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TASK1" );

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
         127,
         1024, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TMTEST.TASK_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TASK2" );

      RTEMS.TASK_START( 
         TMTEST.TASK_ID( 2 ),
         TMTEST.TASK_2'ACCESS, 
         0, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TASK2" );

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
         126,
         1024, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TMTEST.TASK_ID( 3 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TASK3" );

      RTEMS.TASK_START( 
         TMTEST.TASK_ID( 3 ),
         TMTEST.TASK_3'ACCESS, 
         0, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TASK3" );

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  PROCESS_ASR_FOR_PASS_1
--

   procedure PROCESS_ASR_FOR_PASS_1 (
      SIGNALS : in     RTEMS.SIGNAL_SET
   ) is
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SIGNAL_SEND (self)",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.SIGNAL_SEND 
      );

      TEXT_IO.PUT_LINE( "SIGNAL_ENTER (preemptive) na" );

      TIMER_DRIVER.INITIALIZE;

   end PROCESS_ASR_FOR_PASS_1;

--PAGE
-- 
--  PROCESS_ASR_FOR_PASS_2
--

   procedure PROCESS_ASR_FOR_PASS_2 (
      SIGNALS : in     RTEMS.SIGNAL_SET
   ) is
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT_LINE( "SIGNAL_ENTER (preemptive) na" );
     
      RTEMS.TASK_RESUME( TMTEST.TASK_ID( 3 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESUME" );

      TIMER_DRIVER.INITIALIZE;

   end PROCESS_ASR_FOR_PASS_2;

--PAGE
-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TIMER_DRIVER.INITIALIZE;
         RTEMS.SIGNAL_CATCH( 
            TMTEST.PROCESS_ASR_FOR_PASS_1'ACCESS, 
            RTEMS.DEFAULT_MODES,
            STATUS 
         );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SIGNAL_CATCH",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.SIGNAL_CATCH 
      );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.SIGNAL_SEND( TMTEST.TASK_ID( 2 ), RTEMS.SIGNAL_1, STATUS );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SIGNAL_SEND (non-preemptive)",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.SIGNAL_SEND 
      );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.SIGNAL_SEND( RTEMS.SELF, RTEMS.SIGNAL_1, STATUS );

      -- end time is done in ASR

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SIGNAL_RETURN (non-preemptive)",
         TMTEST.END_TIME, 
         1,
         0,
         0
      );

      RTEMS.SIGNAL_CATCH( 
         TMTEST.PROCESS_ASR_FOR_PASS_2'ACCESS, 
         RTEMS.NO_PREEMPT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SIGNAL_CATCH" );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.SIGNAL_SEND( RTEMS.SELF, RTEMS.SIGNAL_1, STATUS );

   end TASK_1;

--PAGE
-- 
--  PROCESS_ASR_FOR_TASK_2
--

   procedure PROCESS_ASR_FOR_TASK_2 (
      SIGNALS : in     RTEMS.SIGNAL_SET
   ) is
      STATUS  : RTEMS.STATUS_CODES;
   begin

      NULL;

   end PROCESS_ASR_FOR_TASK_2;

--PAGE
-- 
--  TASK_2
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS  : RTEMS.STATUS_CODES;
   begin

      RTEMS.SIGNAL_CATCH( 
         TMTEST.PROCESS_ASR_FOR_TASK_2'ACCESS, 
         RTEMS.DEFAULT_MODES,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SIGNAL_CATCH" );

      RTEMS.TASK_SUSPEND( RTEMS.SELF, STATUS );

   end TASK_2;

--PAGE
-- 
--  TASK_3
--

   procedure TASK_3 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASK_SUSPEND( RTEMS.SELF, STATUS );

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SIGNAL_RETURN (preemptive)",
         TMTEST.END_TIME, 
         1,
         0,
         0
     );

     RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TASK_3;

end TMTEST;
