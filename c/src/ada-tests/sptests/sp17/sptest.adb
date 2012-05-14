--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 17 of the RTEMS
--  Single Processor Test Suite.
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
with TEST_SUPPORT;
with TEXT_IO;
with RTEMS.SIGNAL;

package body SPTEST is

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
      TEXT_IO.PUT_LINE( "*** TEST 17 ***" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );

      SPTEST.TASK_2_PREEMPTED := FALSE;

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         2, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 2 ), 
         1, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA2" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 1 ),
         SPTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 2 ),
         SPTEST.TASK_2'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA2" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  PROCESS_ASR
--

   procedure PROCESS_ASR (
      SIGNALS : in     RTEMS.SIGNAL_SET
   ) is
      pragma Unreferenced(SIGNALS);
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASKS.RESUME( SPTEST.TASK_ID( 2 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "ASR - TASK_RESUME OF TA2" );

   end PROCESS_ASR;

-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT_LINE( "TA1 - signal_catch: initializing signal catcher" );
      RTEMS.SIGNAL.CATCH( 
         SPTEST.PROCESS_ASR'ACCESS, 
         RTEMS.NO_ASR + RTEMS.NO_PREEMPT,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SIGNAL_CATCH" );

      TEXT_IO.PUT_LINE( "TA1 - Sending signal to self" );
      RTEMS.SIGNAL.SEND( 
         SPTEST.TASK_ID( 1 ),
         RTEMS.SIGNAL_16,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SIGNAL_SEND" );

      if SPTEST.TASK_2_PREEMPTED then
         TEXT_IO.PUT_LINE( "TA1 - TA2 correctly preempted me" );
      end if;

      TEXT_IO.PUT_LINE( "TA1 - Got Back!!!" );
     
      TEXT_IO.PUT_LINE( "*** END OF TEST 17 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );
   
   end TASK_1;

-- 
--  TASK_2
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS : RTEMS.STATUS_CODES;
   begin

      SPTEST.TASK_2_PREEMPTED := FALSE;

      TEXT_IO.PUT_LINE( "TA2 - Suspending self" );
      RTEMS.TASKS.SUSPEND( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND OF TA2" );

      TEXT_IO.PUT_LINE( "TA2 - signal_return preempted correctly" );

      SPTEST.TASK_2_PREEMPTED := TRUE;

      RTEMS.TASKS.SUSPEND( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND OF TA2" );

   end TASK_2;

end SPTEST;
