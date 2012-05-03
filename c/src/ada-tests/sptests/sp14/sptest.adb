--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 14 of the RTEMS
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

with ADDRESS_IO;
with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;
with INTERFACES; use INTERFACES;
with RTEMS.TIMER;
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
      TEXT_IO.PUT_LINE( "*** TEST 14 ***" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         1, 
         RTEMS.MINIMUM_STACK_SIZE * 2, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 2 ), 
         1, 
         RTEMS.MINIMUM_STACK_SIZE * 2, 
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

      SPTEST.TIMER_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'M', '1', ' ' );

      RTEMS.TIMER.CREATE( 
         SPTEST.TIMER_NAME( 1 ), 
         SPTEST.TIMER_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE OF TM1" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  SIGNAL_3_TO_TASK_1
--

   procedure SIGNAL_3_TO_TASK_1 (
      ID      : in     RTEMS.ID;
      POINTER : in     RTEMS.ADDRESS
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.SIGNAL.SEND( SPTEST.TASK_ID( 1 ), RTEMS.SIGNAL_3, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SIGNAL_SEND of 3" );

      SPTEST.TIMER_GOT_THIS_ID      := ID;
      SPTEST.TIMER_GOT_THIS_POINTER := POINTER;

      SPTEST.SIGNAL_SENT := TRUE;

   end SIGNAL_3_TO_TASK_1;

-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      PREVIOUS_MODE : RTEMS.MODE;
      STATUS        : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT_LINE( "TA1 - signal_catch - INTERRUPT_LEVEL( 3 )" );
      RTEMS.SIGNAL.CATCH( 
         SPTEST.PROCESS_ASR'ACCESS, 
         RTEMS.INTERRUPT_LEVEL( 3 ), 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SIGNAL_CATCH INTERRUPT(3)" );
   
      TEXT_IO.PUT_LINE( "TA1 - signal_send - SIGNAL_16 to self" );
      RTEMS.SIGNAL.SEND( RTEMS.SELF, RTEMS.SIGNAL_16, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( 
         STATUS, 
         "SIGNAL_SEND - SIGNAL_16 to SELF"
      );

      TEXT_IO.PUT_LINE( "TA1 - signal_send - SIGNAL_0 to self" );
      RTEMS.SIGNAL.SEND( RTEMS.SELF, RTEMS.SIGNAL_0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( 
         STATUS, 
         "SIGNAL_SEND - SIGNAL_0 to SELF"
      );

      TEXT_IO.PUT_LINE( "TA1 - signal_catch - NO_ASR" );
      RTEMS.SIGNAL.CATCH(SPTEST.PROCESS_ASR'ACCESS, RTEMS.NO_ASR, STATUS);
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SIGNAL_CATCH - NO_ASR" );

      TEST_SUPPORT.PAUSE;

      TEXT_IO.PUT_LINE( "TA1 - signal_send - SIGNAL_1 to self" );
      RTEMS.SIGNAL.SEND( RTEMS.SELF, RTEMS.SIGNAL_1, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( 
         STATUS, 
         "SIGNAL_SEND - SIGNAL_1 to SELF"
      );

      TEXT_IO.PUT_LINE( "TA1 - task_mode - disable ASRs" );
      RTEMS.TASKS.MODE( 
         RTEMS.NO_ASR, 
         RTEMS.ASR_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );

      SPTEST.TIMER_GOT_THIS_ID := 0;
      SPTEST.TIMER_GOT_THIS_POINTER := RTEMS.NULL_ADDRESS;

      TEXT_IO.PUT_LINE( "TA1 - sending signal to SELF from timer" );
      RTEMS.TIMER.FIRE_AFTER( 
         SPTEST.TIMER_ID( 1 ),
         TEST_SUPPORT.TICKS_PER_SECOND / 2,
         SPTEST.SIGNAL_3_TO_TASK_1'ACCESS,
         SPTEST.TASK_1'ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );

      TEXT_IO.PUT_LINE( "TA1 - waiting for signal to arrive" );

      SPTEST.SIGNAL_SENT := FALSE;
      SPTEST.ASR_FIRED   := FALSE;

      loop
         exit when SPTEST.SIGNAL_SENT;
      end loop;

      if SPTEST.TIMER_GOT_THIS_ID = SPTEST.TIMER_ID( 1 ) and then
         RTEMS.ARE_EQUAL(SPTEST.TIMER_GOT_THIS_POINTER,
             SPTEST.TASK_1'ADDRESS) then
 
         TEXT_IO.PUT_LINE( "TA1 - timer routine got the correct arguments" );

      else

         TEXT_IO.PUT( "TA1 - timer got (" );
         UNSIGNED32_IO.PUT( SPTEST.TIMER_GOT_THIS_ID  );
         TEXT_IO.PUT( "," );
         ADDRESS_IO.PUT( SPTEST.TIMER_GOT_THIS_POINTER  );
         TEXT_IO.PUT( ") instead of (" );
         UNSIGNED32_IO.PUT( SPTEST.TIMER_ID( 1 ) );
         TEXT_IO.PUT( "," );
         ADDRESS_IO.PUT( SPTEST.TASK_1'ADDRESS  );
         TEXT_IO.PUT_LINE( ")!!!!" );

      end if;

      TEXT_IO.PUT_LINE( "TA1 - task_mode - enable ASRs" );
      RTEMS.TASKS.MODE( 
         RTEMS.ASR, 
         RTEMS.ASR_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );

      TEXT_IO.PUT_LINE( "TA1 - signal_catch - ASR ADDRESS of NULL" );
      RTEMS.SIGNAL.CATCH( NULL, RTEMS.DEFAULT_MODES, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( 
         STATUS, 
         "SIGNAL_CATCH - NULL ADDRESS"
      );
   
      TEXT_IO.PUT_LINE( "TA1 - task_delete - delete self" );
      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

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

      TEXT_IO.PUT_LINE( "TA2 - signal_send - SIGNAL_17 to TA1" );
      RTEMS.SIGNAL.SEND( SPTEST.TASK_ID( 1 ), RTEMS.SIGNAL_17, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( 
         STATUS, 
         "SIGNAL_SEND - SIGNAL_17 to TA1"
      );

      TEXT_IO.PUT_LINE( "TA2 - task_wake_after - yield processor" );
      RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER YIELD" );

      TEXT_IO.PUT_LINE( 
         "TA2 - signal_send - SIGNAL_18 and SIGNAL_19 to TA1"
      );
      RTEMS.SIGNAL.SEND( 
         SPTEST.TASK_ID( 1 ), 
         RTEMS.SIGNAL_18 + RTEMS.SIGNAL_19,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( 
         STATUS, 
         "SIGNAL_SEND - SIGNAL_18 and SIGNAL_19 to TA1"
      );

      TEXT_IO.PUT_LINE( "TA2 - task_wake_after - yield processor" );
      RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER YIELD" );

      TEXT_IO.PUT_LINE( "*** END OF TEST 14 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TASK_2;

-- 
--  PROCESS_ASR
--

   procedure PROCESS_ASR (
      THE_SIGNAL_SET : in     RTEMS.SIGNAL_SET
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT( "ASR - ENTRY - signal => " );
      UNSIGNED32_IO.PUT( THE_SIGNAL_SET, BASE => 16, WIDTH => 8 );
      TEXT_IO.NEW_LINE;

      if THE_SIGNAL_SET = RTEMS.SIGNAL_16 or else 
             THE_SIGNAL_SET = RTEMS.SIGNAL_17 or else 
             THE_SIGNAL_SET = RTEMS.SIGNAL_18 + RTEMS.SIGNAL_19 then
         NULL;

      elsif THE_SIGNAL_SET = RTEMS.SIGNAL_0 or else 
              THE_SIGNAL_SET = RTEMS.SIGNAL_1 then
         TEXT_IO.PUT_LINE( "ASR - task_wake_after - yield processor" );
         RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( 
            STATUS, 
            "TASK_WAKE_AFTER YIELD"
         );

      elsif THE_SIGNAL_SET = RTEMS.SIGNAL_3 then

         SPTEST.ASR_FIRED := TRUE;

      end if;

      TEXT_IO.PUT( "ASR - EXIT  - signal => " );
      UNSIGNED32_IO.PUT( THE_SIGNAL_SET, BASE => 16, WIDTH => 8 );
      TEXT_IO.NEW_LINE;

   end PROCESS_ASR;
      
end SPTEST;
