--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 22 of the RTEMS
--  Single Processor Test Suite.
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
with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;

package body SPTEST is

--PAGE
-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TEST 22 ***" );

      TIME := ( 1988, 12, 31, 9, 0, 0, 0 );

      RTEMS.CLOCK_SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET" );

      SPTEST.TASK_NAME( 1 )  := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TIMER_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'M', '1', ' ' );

      RTEMS.TASK_CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         1, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASK_START(
         SPTEST.TASK_ID( 1 ),
         SPTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      TEXT_IO.PUT_LINE( "INIT - timer_create - creating timer 1" );
      RTEMS.TIMER_CREATE(
         SPTEST.TIMER_NAME( 1 ), 
         SPTEST.TIMER_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE OF TM1" );
      TEXT_IO.PUT( "INIT - timer 1 has id (" );
      UNSIGNED32_IO.PUT( SPTEST.TIMER_ID( 1 ), WIDTH => 8, BASE => 16 );
      TEXT_IO.PUT_LINE( ")" );
      
      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  DELAYED_RESUME
--

   procedure DELAYED_RESUME (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASK_RESUME( SPTEST.TASK_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESUME OF SELF" );

   end DELAYED_RESUME;

--PAGE
-- 
--  PRINT_TIME
--

   procedure PRINT_TIME 
   is
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.CLOCK_GET( RTEMS.CLOCK_GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

      TEST_SUPPORT.PUT_NAME( 
         SPTEST.TASK_NAME( 1 ),
         FALSE
      );

      TEST_SUPPORT.PRINT_TIME( "- clock_get - ", TIME, "" );
      TEXT_IO.NEW_LINE;

   end PRINT_TIME;
   
--PAGE
-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      TMID   : RTEMS.ID;
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin

-- GET ID

      TEXT_IO.PUT_LINE( "TA1 - timer_ident - identing timer 1" );
      RTEMS.TIMER_IDENT( SPTEST.TIMER_NAME( 1 ), TMID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_IDENT OF TM1" );
      TEXT_IO.PUT( "TA1 - timer 1 has id (" );
      UNSIGNED32_IO.PUT( SPTEST.TIMER_ID( 1 ), WIDTH => 8, BASE => 16 );
      TEXT_IO.PUT_LINE( ")" );
   
-- AFTER WHICH IS ALLOWED TO FIRE

      SPTEST.PRINT_TIME;

      TEXT_IO.PUT_LINE( "TA1 - timer_after - timer 1 in 3 seconds" );
      RTEMS.TIMER_FIRE_AFTER( 
         TMID, 
         3 * TEST_SUPPORT.TICKS_PER_SECOND,
         SPTEST.DELAYED_RESUME'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );

      TEXT_IO.PUT_LINE( "TA1 - task_suspend( SELF )" );
      RTEMS.TASK_SUSPEND( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );
          
      SPTEST.PRINT_TIME;

-- AFTER WHICH IS RESET AND ALLOWED TO FIRE

      TEXT_IO.PUT_LINE( "TA1 - timer_after - timer 1 in 3 seconds" );
      RTEMS.TIMER_FIRE_AFTER( 
         TMID, 
         3 * TEST_SUPPORT.TICKS_PER_SECOND,
         SPTEST.DELAYED_RESUME'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - 1 second" );
      RTEMS.TASK_WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
          
      SPTEST.PRINT_TIME;

      TEXT_IO.PUT_LINE( "TA1 - timer_reset - timer 1" );
      RTEMS.TIMER_RESET( TMID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_RESET" );
          
      TEXT_IO.PUT_LINE( "TA1 - task_suspend( SELF )" );
      RTEMS.TASK_SUSPEND( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );

      SPTEST.PRINT_TIME;

TEST_SUPPORT.PAUSE;

-- 
-- Reset the time since we do not know how long the user waited
-- before pressing <cr> at the pause.  This insures that the
-- actual output matches the screen.
--

      TIME := ( 1988, 12, 31, 9, 0, 7, 0 );

      RTEMS.CLOCK_SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET" );

-- after which is canceled

      TEXT_IO.PUT_LINE( "TA1 - timer_after - timer 1 in 3 seconds" );
      RTEMS.TIMER_FIRE_AFTER( 
         TMID, 
         3 * TEST_SUPPORT.TICKS_PER_SECOND,
         SPTEST.DELAYED_RESUME'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );

      TEXT_IO.PUT_LINE( "TA1 - timer_cancel - timer 1" );
      RTEMS.TIMER_CANCEL( TMID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CANCEL" );
          
-- when which is allowed to fire

      SPTEST.PRINT_TIME;

      RTEMS.CLOCK_GET( RTEMS.CLOCK_GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

      TIME.SECOND := TIME.SECOND + 3;

      TEXT_IO.PUT_LINE( "TA1 - timer_when - timer 1 in 3 seconds" );
      RTEMS.TIMER_FIRE_WHEN( 
         TMID, 
         TIME,
         SPTEST.DELAYED_RESUME'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_WHEN" );

      TEXT_IO.PUT_LINE( "TA1 - task_suspend( SELF )" );
      RTEMS.TASK_SUSPEND( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );

      SPTEST.PRINT_TIME;

-- when which is canceled

      RTEMS.CLOCK_GET( RTEMS.CLOCK_GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

      TIME.SECOND := TIME.SECOND + 3;

      TEXT_IO.PUT_LINE( "TA1 - timer_when - timer 1 in 3 seconds" );
      RTEMS.TIMER_FIRE_WHEN( 
         TMID, 
         TIME,
         SPTEST.DELAYED_RESUME'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_WHEN" );

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - 1 second" );
      RTEMS.TASK_WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
          
      SPTEST.PRINT_TIME;

      TEXT_IO.PUT_LINE( "TA1 - timer_cancel - timer 1" );
      RTEMS.TIMER_CANCEL( TMID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CANCEL" );
          
-- delete
          
      TEXT_IO.PUT_LINE(
         "TA1 - task_wake_after - YIELD (only task at priority)"
      );
      RTEMS.TASK_WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER YIELD" );
          
      TEXT_IO.PUT_LINE( "TA1 - timer_delete - timer 1" );
      RTEMS.TIMER_DELETE( TMID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_DELETE" );
          
      TEXT_IO.PUT_LINE( "*** END OF TEST 22 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );
   
   end TASK_1;

end SPTEST;
