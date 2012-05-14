--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 11 of the RTEMS
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
with UNSIGNED32_IO;
with RTEMS.CLOCK;
with RTEMS.EVENT;
with RTEMS.TIMER;

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
      TEXT_IO.PUT_LINE( "*** TEST 11 ***" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         4, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 2 ), 
         4, 
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

      SPTEST.TIMER_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'M', '1', ' ' );
      SPTEST.TIMER_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'M', '2', ' ' );
      SPTEST.TIMER_NAME( 3 ) := RTEMS.BUILD_NAME(  'T', 'M', '3', ' ' );
      SPTEST.TIMER_NAME( 4 ) := RTEMS.BUILD_NAME(  'T', 'M', '4', ' ' );
      SPTEST.TIMER_NAME( 5 ) := RTEMS.BUILD_NAME(  'T', 'M', '5', ' ' );
      SPTEST.TIMER_NAME( 6 ) := RTEMS.BUILD_NAME(  'T', 'M', '6', ' ' );

      RTEMS.TIMER.CREATE( 
         SPTEST.TIMER_NAME( 1 ),
         SPTEST.TIMER_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE OF TM1" );

      RTEMS.TIMER.CREATE( 
         SPTEST.TIMER_NAME( 2 ),
         SPTEST.TIMER_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE OF TM2" );

      RTEMS.TIMER.CREATE( 
         SPTEST.TIMER_NAME( 3 ),
         SPTEST.TIMER_ID( 3 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE OF TM3" );

      RTEMS.TIMER.CREATE( 
         SPTEST.TIMER_NAME( 4 ),
         SPTEST.TIMER_ID( 4 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE OF TM4" );

      RTEMS.TIMER.CREATE( 
         SPTEST.TIMER_NAME( 5 ),
         SPTEST.TIMER_ID( 5 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE OF TM5" );

      RTEMS.TIMER.CREATE( 
         SPTEST.TIMER_NAME( 6 ),
         SPTEST.TIMER_ID( 6 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE OF TM6" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      EVENTOUT : RTEMS.EVENT_SET;
      TIME     : RTEMS.TIME_OF_DAY;
      STATUS   : RTEMS.STATUS_CODES;
      INDEX    : RTEMS.UNSIGNED32;
   begin

      TEXT_IO.PUT_LINE( "TA1 - event_send - send EVENT_16 to TA2" );
      RTEMS.EVENT.SEND( SPTEST.TASK_ID( 2 ), RTEMS.EVENT_16, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND of 16" );

      TEXT_IO.PUT_LINE( 
         "TA1 - event_receive - waiting forever on EVENT_14 and EVENT_15"
      );
      RTEMS.EVENT.RECEIVE( 
         RTEMS.EVENT_14 + RTEMS.EVENT_15,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         EVENTOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE of 14 and 15" );
      TEXT_IO.PUT( "TA1 - EVENT_14 and EVENT_15 received - eventout => ");
      UNSIGNED32_IO.PUT( EVENTOUT, BASE => 16, WIDTH => 8 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( "TA1 - event_send - send EVENT_18 to TA2" );
      RTEMS.EVENT.SEND( SPTEST.TASK_ID( 2 ), RTEMS.EVENT_18, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND of 18" );

      TEXT_IO.PUT_LINE( 
         "TA1 - event_receive - waiting with 10 second timeout on EVENT_14"
      );
      RTEMS.EVENT.RECEIVE( 
         RTEMS.EVENT_14,
         RTEMS.DEFAULT_OPTIONS,
         10 * TEST_SUPPORT.TICKS_PER_SECOND,
         EVENTOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE of 14" );
      TEXT_IO.PUT( "TA1 - EVENT_14 received - eventout => ");
      UNSIGNED32_IO.PUT( EVENTOUT, BASE => 16, WIDTH => 8 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( "TA1 - event_send - send EVENT_19 to TA2" );
      RTEMS.EVENT.SEND( SPTEST.TASK_ID( 2 ), RTEMS.EVENT_19, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND of 19" );

      RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TA1 CLOCK_GET" );
      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_get - ", TIME, "" );
      TEXT_IO.NEW_LINE;

      TEST_SUPPORT.PAUSE;

      TEXT_IO.PUT_LINE( 
         "TA1 - event_send - send EVENT_18 to self after 5 seconds"
      );
      RTEMS.TIMER.FIRE_AFTER( 
         SPTEST.TIMER_ID( 1 ),
         5 * TEST_SUPPORT.TICKS_PER_SECOND,
         SPTEST.TA1_SEND_18_TO_SELF_5_SECONDS'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER 5 seconds" );

      TEXT_IO.PUT_LINE( 
         "TA1 - event_receive - waiting forever on EVENT_18"
      );
      RTEMS.EVENT.RECEIVE( 
         RTEMS.EVENT_18,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         EVENTOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE of 18" );

      RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TA1 CLOCK_GET" );

      TEXT_IO.PUT( "TA1 - EVENT_18 received - eventout => ");
      UNSIGNED32_IO.PUT( EVENTOUT, BASE => 16, WIDTH => 8 );
      TEXT_IO.NEW_LINE;

      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_get - ", TIME, "" );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( "TA1 - event_send - send EVENT_3 to self" );
      RTEMS.EVENT.SEND( RTEMS.SELF, RTEMS.EVENT_3, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND of 3" );

      TEXT_IO.PUT_LINE( 
         "TA1 - event_receive - EVENT_3 or EVENT_22 - NO_WAIT and EVENT_ANY"
      );
      RTEMS.EVENT.RECEIVE( 
         RTEMS.EVENT_3 + RTEMS.EVENT_22,
         RTEMS.NO_WAIT + RTEMS.EVENT_ANY,
         RTEMS.NO_TIMEOUT,
         EVENTOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE of 3 and 22" );
      TEXT_IO.PUT( "TA1 - EVENT_3 received - eventout => ");
      UNSIGNED32_IO.PUT( EVENTOUT, BASE => 16, WIDTH => 8 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( "TA1 - event_send - send EVENT_4 to self" );
      RTEMS.EVENT.SEND( RTEMS.SELF, RTEMS.EVENT_4, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND of 4" );

      TEXT_IO.PUT_LINE( 
"TA1 - event_receive - waiting forever on EVENT_4 or EVENT_5 - EVENT_ANY"
      );
      RTEMS.EVENT.RECEIVE( 
         RTEMS.EVENT_4 + RTEMS.EVENT_5,
         RTEMS.EVENT_ANY,
         RTEMS.NO_TIMEOUT,
         EVENTOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE of 4 and 5" );
      TEXT_IO.PUT( "TA1 - EVENT_4 received - eventout => ");
      UNSIGNED32_IO.PUT( EVENTOUT, BASE => 16, WIDTH => 8 );
      TEXT_IO.NEW_LINE;

      TEST_SUPPORT.PAUSE;

      TEXT_IO.PUT_LINE( 
         "TA1 - event_send - send EVENT_18 to self after 5 seconds"
      );
      RTEMS.TIMER.FIRE_AFTER( 
         SPTEST.TIMER_ID( 1 ),
         5 * TEST_SUPPORT.TICKS_PER_SECOND,
         SPTEST.TA1_SEND_18_TO_SELF_5_SECONDS'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER 5 seconds" );

      TEXT_IO.PUT_LINE( 
         "TA1 - timer_cancel - cancelling timer for event EVENT_18"
      );
      RTEMS.TIMER.CANCEL( SPTEST.TIMER_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CANCEL" );

      TEXT_IO.PUT_LINE( 
         "TA1 - event_send - send EVENT_8 to self after 60 seconds"
      );
      RTEMS.TIMER.FIRE_AFTER( 
         SPTEST.TIMER_ID( 1 ),
         60 * TEST_SUPPORT.TICKS_PER_SECOND,
         SPTEST.TA1_SEND_8_TO_SELF_60_SECONDS'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER 60 secs" );

      TEXT_IO.PUT_LINE( 
         "TA1 - event_send - send EVENT_9 to self after 60 seconds"
      );
      RTEMS.TIMER.FIRE_AFTER( 
         SPTEST.TIMER_ID( 2 ),
         60 * TEST_SUPPORT.TICKS_PER_SECOND,
         SPTEST.TA1_SEND_9_TO_SELF_60_SECONDS'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER 60 secs" );

      TEXT_IO.PUT_LINE( 
         "TA1 - event_send - send EVENT_10 to self after 60 seconds"
      );
      RTEMS.TIMER.FIRE_AFTER( 
         SPTEST.TIMER_ID( 3 ),
         60 * TEST_SUPPORT.TICKS_PER_SECOND,
         SPTEST.TA1_SEND_10_TO_SELF'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER 60 secs" );

      TEXT_IO.PUT_LINE( 
         "TA1 - timer_cancel - cancelling timer for event EVENT_8"
      );
      RTEMS.TIMER.CANCEL( SPTEST.TIMER_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CANCEL" );

      TIME := ( 1988, 2, 12, 8, 15, 0, 0 );

      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_set - ", TIME, "" );
      TEXT_IO.NEW_LINE;
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TA1 CLOCK_SET" );

      TEXT_IO.PUT_LINE( 
         "TA1 - event_send - send EVENT_1 every second"
      );
      RTEMS.TIMER.FIRE_AFTER( 
         SPTEST.TIMER_ID( 1 ),
         TEST_SUPPORT.TICKS_PER_SECOND,
         SPTEST.TA1_SEND_1_TO_SELF_EVERY_SECOND'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER 1 SECOND" );

      INDEX := 0;

      loop

         exit when INDEX = 3;

         RTEMS.EVENT.RECEIVE( 
            RTEMS.EVENT_1,
            RTEMS.EVENT_ANY,
            RTEMS.NO_TIMEOUT,
            EVENTOUT,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE of 1" );

         RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TA1 CLOCK_GET" );

         TEXT_IO.PUT( "TA1 - EVENT_1 received - eventout => ");
         UNSIGNED32_IO.PUT( EVENTOUT, BASE => 16, WIDTH => 8 );
         TEST_SUPPORT.PRINT_TIME( " - at ", TIME, "" );
         TEXT_IO.NEW_LINE;

         if INDEX < 2 then
            RTEMS.TIMER.RESET( SPTEST.TIMER_ID( 1 ), STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TA1 TIMER RESET" );
         end if;

         INDEX := INDEX + 1;
      end loop;

      TEXT_IO.PUT_LINE( 
         "TA1 - timer_cancel - cancelling timer for event EVENT_1"
      );
      RTEMS.TIMER.CANCEL( SPTEST.TIMER_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CANCEL" );

      TEST_SUPPORT.PAUSE;

      TIME.DAY := 13;
      TEXT_IO.PUT_LINE( 
         "TA1 - event_send - send EVENT_11 to self in 1 day"
      );
      RTEMS.TIMER.FIRE_WHEN( 
         SPTEST.TIMER_ID( 1 ),
         TIME,
         SPTEST.TA1_SEND_11_TO_SELF'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_WHEN 1 day" );

      TIME.HOUR := 7;
      TEXT_IO.PUT_LINE( 
         "TA1 - event_send - send EVENT_11 to self in 1 day"
      );
      RTEMS.TIMER.FIRE_WHEN( 
         SPTEST.TIMER_ID( 2 ),
         TIME,
         SPTEST.TA1_SEND_11_TO_SELF'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_WHEN 1 day" );
      TIME.HOUR := 8;

      TIME.DAY := 14;
      TEXT_IO.PUT_LINE( 
         "TA1 - event_send - send EVENT_11 to self in 2 days"
      );
      RTEMS.TIMER.FIRE_WHEN( 
         SPTEST.TIMER_ID( 3 ),
         TIME,
         SPTEST.TA1_SEND_11_TO_SELF'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_WHEN 2 days" );

      TEXT_IO.PUT_LINE( 
         "TA1 - timer_cancel - cancelling EVENT_11 to self in 1 day"
      );
      RTEMS.TIMER.CANCEL( SPTEST.TIMER_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CANCEL" );

      TEXT_IO.PUT_LINE( 
         "TA1 - timer_cancel - cancelling EVENT_11 to self in 2 days"
      );
      RTEMS.TIMER.CANCEL( SPTEST.TIMER_ID( 3 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CANCEL" );

      TEXT_IO.PUT_LINE( 
         "TA1 - event_send - resending EVENT_11 to self in 2 days"
      );
      RTEMS.TIMER.FIRE_WHEN( 
         SPTEST.TIMER_ID( 3 ),
         TIME,
         SPTEST.TA1_SEND_11_TO_SELF'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_WHEN 2 days" );

      TIME.DAY := 15;
      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_set - ", TIME, "" );
      TEXT_IO.NEW_LINE;
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TA1 CLOCK_SET" );
      TEXT_IO.PUT_LINE( 
         "TA1 - event_receive - waiting forever on EVENT_11"
      );
      RTEMS.EVENT.RECEIVE( 
         RTEMS.EVENT_11,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         EVENTOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE of 11" );
      TEXT_IO.PUT( "TA1 - EVENT_11 received - eventout => ");
      UNSIGNED32_IO.PUT( EVENTOUT, BASE => 16, WIDTH => 8 );
      TEXT_IO.NEW_LINE;

      TEST_SUPPORT.PAUSE;

-- The following code tests the case of deleting a timer ???

      TEXT_IO.PUT_LINE( "TA1 - event_send/event_receive combination" );
      RTEMS.TIMER.FIRE_AFTER( 
         SPTEST.TIMER_ID( 1 ),
         10,
         SPTEST.TA1_SEND_11_TO_SELF'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER 10 ticks" );
      RTEMS.EVENT.RECEIVE( 
         RTEMS.EVENT_11,
         RTEMS.DEFAULT_OPTIONS,
         11,
         EVENTOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE" );

      TIME := ( 1988, 2, 12, 8, 15, 0, 0 );

      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_set - ", TIME, "" );
      TEXT_IO.NEW_LINE;
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TA1 CLOCK_SET" );

      TIME.DAY := 13;
      TEXT_IO.PUT_LINE( 
         "TA1 - event_receive all outstanding events"
      );
      RTEMS.EVENT.RECEIVE(
         RTEMS.ALL_EVENTS,
         RTEMS.NO_WAIT + RTEMS.EVENT_ANY,
         RTEMS.NO_TIMEOUT,
         EVENTOUT,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS( 
         STATUS, 
         RTEMS.UNSATISFIED, 
         "EVENT_RECEIVE all events" 
      );

      TEXT_IO.PUT_LINE( 
         "TA1 - event_send - send EVENT_10 to self in 1 day"
      );
      RTEMS.TIMER.FIRE_WHEN( 
         SPTEST.TIMER_ID( 1 ),
         TIME,
         SPTEST.TA1_SEND_10_TO_SELF'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_WHEN 1 day" );

      TIME.DAY := 14;
      TEXT_IO.PUT_LINE( 
         "TA1 - event_send - send EVENT_11 to self in 2 days"
      );
      RTEMS.TIMER.FIRE_WHEN( 
         SPTEST.TIMER_ID( 2 ),
         TIME,
         SPTEST.TA1_SEND_11_TO_SELF'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_WHEN 2 days" );

      TIME := ( 1988, 2, 12, 7, 15, 0, 0 );

      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_set - ", TIME, "" );
      TEXT_IO.NEW_LINE;
      TEXT_IO.PUT_LINE( "TA1 - set time backwards" );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TA1 CLOCK_SET" );

      RTEMS.EVENT.RECEIVE( 
         RTEMS.ALL_EVENTS,
         RTEMS.NO_WAIT + RTEMS.EVENT_ANY,
         RTEMS.NO_TIMEOUT,
         EVENTOUT,
         STATUS
      );
      if EVENTOUT >= RTEMS.EVENT_0 then 
         TEXT_IO.PUT( "ERROR - " );
         UNSIGNED32_IO.PUT( EVENTOUT, BASE => 16, WIDTH => 8 );
         TEXT_IO.PUT_LINE( " events received" );
      else
         TEXT_IO.PUT_LINE( "TA1 - no events received" );
      end if;
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS( 
         STATUS, 
         RTEMS.UNSATISFIED, 
         "EVENT_RECEIVE all events" 
      );

      TIME := ( 1988, 2, 14, 7, 15, 0, 0 );

      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_set - ", TIME, "" );
      TEXT_IO.NEW_LINE;
      TEXT_IO.PUT_LINE( "TA1 - set time forwards (leave a timer)" );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TA1 CLOCK_SET" );

      RTEMS.EVENT.RECEIVE( 
         RTEMS.ALL_EVENTS,
         RTEMS.NO_WAIT + RTEMS.EVENT_ANY,
         RTEMS.NO_TIMEOUT,
         EVENTOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE all events" );
      if EVENTOUT = RTEMS.EVENT_10 then 
         TEXT_IO.PUT_LINE( "TA1 - EVENT_10 received" );
      else
         TEXT_IO.PUT( "ERROR - " );
         UNSIGNED32_IO.PUT( EVENTOUT, BASE => 16, WIDTH => 8 );
         TEXT_IO.PUT_LINE( " events received" );
      end if;
         
      TEXT_IO.PUT_LINE( 
         "TA1 - event_send - send EVENT_11 to self in 100 ticks"
      );
      RTEMS.TIMER.FIRE_AFTER( 
         SPTEST.TIMER_ID( 1 ),
         100,
         SPTEST.TA1_SEND_11_TO_SELF'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER 100 ticks" );

      TEXT_IO.PUT_LINE( 
         "TA1 - event_send - send EVENT_11 to self in 200 ticks"
      );
      RTEMS.TIMER.FIRE_AFTER( 
         SPTEST.TIMER_ID( 2 ),
         100,
         SPTEST.TA1_SEND_11_TO_SELF'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER 200 ticks" );

      TEXT_IO.PUT_LINE( "*** END OF TEST 11 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TASK_1;

-- 
--  TASK_2
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      EVENTOUT : RTEMS.EVENT_SET;
      TIME     : RTEMS.TIME_OF_DAY;
      STATUS   : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASKS.WAKE_AFTER( 1 * TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

      TEXT_IO.PUT_LINE( 
         "TA2 - event_receive - waiting forever on EVENT_16"
      );
      RTEMS.EVENT.RECEIVE( 
         RTEMS.EVENT_16,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         EVENTOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE of 16" );
      TEXT_IO.PUT( "TA2 - EVENT_16 received - eventout => ");
      UNSIGNED32_IO.PUT( EVENTOUT, BASE => 16, WIDTH => 8 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( 
         "TA2 - event_send - send EVENT_14 and EVENT_15 to TA1"
      );
      RTEMS.EVENT.SEND( 
         SPTEST.TASK_ID( 1 ), 
         RTEMS.EVENT_14 + RTEMS.EVENT_15,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND of 14 and 15" );

      TEXT_IO.PUT_LINE( 
   "TA2 - event_receive - waiting forever on EVENT_17 or EVENT_18 - EVENT_ANY"
      );
      RTEMS.EVENT.RECEIVE( 
         RTEMS.EVENT_17 + RTEMS.EVENT_18,
         RTEMS.EVENT_ANY,
         RTEMS.NO_TIMEOUT,
         EVENTOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE of 17 and 18" );
      TEXT_IO.PUT( "TA2 - EVENT_17 or EVENT_18 received - eventout => ");
      UNSIGNED32_IO.PUT( EVENTOUT, BASE => 16, WIDTH => 8 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( 
         "TA2 - event_send - send EVENT_14 to TA1"
      );
      RTEMS.EVENT.SEND( 
         SPTEST.TASK_ID( 1 ), 
         RTEMS.EVENT_14,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND of 14" );

      TIME := ( 1988,  2, 12, 8, 15, 0, 0 );

      TEST_SUPPORT.PRINT_TIME( "TA2 - clock_set - ", TIME, "" );
      TEXT_IO.NEW_LINE;
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TA2 CLOCK_SET" );

      TIME.SECOND := TIME.SECOND + 5;
      TEXT_IO.PUT_LINE( 
         "TA2 - event_send - sending EVENT_10 to self after 5 seconds"
      );
      RTEMS.TIMER.FIRE_WHEN( 
         SPTEST.TIMER_ID( 5 ),
         TIME,
         SPTEST.TA2_SEND_10_TO_SELF'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_WHEN 5 SECONDS" );

      TEXT_IO.PUT_LINE( 
         "TA2 - event_receive - waiting forever on EVENT_10"
      );
      RTEMS.EVENT.RECEIVE( 
         RTEMS.EVENT_10,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         EVENTOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE of 10" );

      RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TA1 CLOCK_GET" );

      TEXT_IO.PUT( "TA2 - EVENT_10 received - eventout => ");
      UNSIGNED32_IO.PUT( EVENTOUT, BASE => 16, WIDTH => 8 );
      TEXT_IO.NEW_LINE;

      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_get - ", TIME, "" );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( "TA2 - event_receive - PENDING_EVENTS" );
      RTEMS.EVENT.RECEIVE( 
         RTEMS.PENDING_EVENTS,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         EVENTOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE of 10" );
      TEXT_IO.PUT( "TA2 - eventout => ");
      UNSIGNED32_IO.PUT( EVENTOUT, BASE => 16, WIDTH => 8 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( 
         "TA2 - event_receive - EVENT_19 - NO_WAIT"
      );
      RTEMS.EVENT.RECEIVE( 
         RTEMS.EVENT_19,
         RTEMS.NO_WAIT,
         RTEMS.NO_TIMEOUT,
         EVENTOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE of 10" );
      TEXT_IO.PUT( "TA2 - EVENT_19 received - eventout => ");
      UNSIGNED32_IO.PUT( EVENTOUT, BASE => 16, WIDTH => 8 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( "TA2 - task_delete - deletes self" );
      RTEMS.TASKS.DELETE( SPTEST.TASK_ID( 2 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE of TA2" );

   end TASK_2;

-- 
--  TA1_SEND_18_TO_SELF_5_SECONDS
--

   procedure TA1_SEND_18_TO_SELF_5_SECONDS (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   )
   is
      pragma Unreferenced(IGNORED_ID);
      pragma Unreferenced(IGNORED_ADDRESS);
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.EVENT.SEND( SPTEST.TASK_ID( 1 ), RTEMS.EVENT_18, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND of 18" );

   end TA1_SEND_18_TO_SELF_5_SECONDS;

-- 
--  TA1_SEND_8_TO_SELF_60_SECONDS
--

   procedure TA1_SEND_8_TO_SELF_60_SECONDS (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   )
   is
      pragma Unreferenced(IGNORED_ID);
      pragma Unreferenced(IGNORED_ADDRESS);
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.EVENT.SEND( SPTEST.TASK_ID( 1 ), RTEMS.EVENT_8, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND of 8" );

   end TA1_SEND_8_TO_SELF_60_SECONDS;

-- 
--  TA1_SEND_9_TO_SELF_60_SECONDS
--

   procedure TA1_SEND_9_TO_SELF_60_SECONDS (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   )
   is
      pragma Unreferenced(IGNORED_ID);
      pragma Unreferenced(IGNORED_ADDRESS);
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.EVENT.SEND( SPTEST.TASK_ID( 1 ), RTEMS.EVENT_9, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND of 9" );

   end TA1_SEND_9_TO_SELF_60_SECONDS;

-- 
--  TA1_SEND_10_TO_SELF
--

   procedure TA1_SEND_10_TO_SELF (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   )
   is
      pragma Unreferenced(IGNORED_ID);
      pragma Unreferenced(IGNORED_ADDRESS);
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.EVENT.SEND( SPTEST.TASK_ID( 1 ), RTEMS.EVENT_10, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND of 10" );

   end TA1_SEND_10_TO_SELF;

-- 
--  TA1_SEND_1_TO_SELF_EVERY_SECOND
--

   procedure TA1_SEND_1_TO_SELF_EVERY_SECOND (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   )
   is
      pragma Unreferenced(IGNORED_ID);
      pragma Unreferenced(IGNORED_ADDRESS);
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.EVENT.SEND( SPTEST.TASK_ID( 1 ), RTEMS.EVENT_1, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND of 1" );

   end TA1_SEND_1_TO_SELF_EVERY_SECOND;

-- 
--  TA1_SEND_11_TO_SELF
--

   procedure TA1_SEND_11_TO_SELF (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   )
   is
      pragma Unreferenced(IGNORED_ID);
      pragma Unreferenced(IGNORED_ADDRESS);
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.EVENT.SEND( SPTEST.TASK_ID( 1 ), RTEMS.EVENT_11, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND of 11" );

   end TA1_SEND_11_TO_SELF;

-- 
--  TA2_SEND_10_TO_SELF
--

   procedure TA2_SEND_10_TO_SELF (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   )
   is
      pragma Unreferenced(IGNORED_ID);
      pragma Unreferenced(IGNORED_ADDRESS);
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.EVENT.SEND( SPTEST.TASK_ID( 2 ), RTEMS.EVENT_10, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND of 10" );

   end TA2_SEND_10_TO_SELF;

end SPTEST;
