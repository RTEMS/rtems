--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 3 of the RTEMS
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
--  http://www.rtems.com/license/LICENSE.
--
--  $Id$
--

with INTERFACES; use INTERFACES;
with RTEMS;
with TEST_SUPPORT;
with TEXT_IO;

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
      TEXT_IO.PUT_LINE( "*** TEST 3 ***" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );

      RTEMS.TASK_CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         1, 
         RTEMS.MINIMUM_STACK_SIZE, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASK_CREATE( 
         SPTEST.TASK_NAME( 2 ), 
         1, 
         RTEMS.MINIMUM_STACK_SIZE, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA2" );

      RTEMS.TASK_START(
         SPTEST.TASK_ID( 1 ),
         SPTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      RTEMS.TASK_START(
         SPTEST.TASK_ID( 2 ),
         SPTEST.TASK_2'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA2" );

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin

      TIME := ( 1988, 12, 31, 9, 15, 0, 0 );

      RTEMS.CLOCK_SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET" );

      TEST_SUPPORT.PUT_NAME( SPTEST.TASK_NAME( 1 ), FALSE );
      TEST_SUPPORT.PRINT_TIME( "sets clock: ", TIME, "" );
      TEXT_IO.NEW_LINE;

      loop

         RTEMS.CLOCK_GET( RTEMS.CLOCK_GET_TOD, TIME'ADDRESS, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

         TEST_SUPPORT.PUT_NAME( SPTEST.TASK_NAME( 1 ), FALSE );
         TEST_SUPPORT.PRINT_TIME( "going to sleep: ", TIME, "" );
         TEXT_IO.NEW_LINE;

         TIME.SECOND := TIME.SECOND + 5;
         TIME.MINUTE := TIME.MINUTE + (TIME.SECOND / 60 );
         TIME.SECOND := TIME.SECOND mod 60;
         TIME.HOUR   := TIME.HOUR + (TIME.MINUTE / 60 );
         TIME.MINUTE := TIME.MINUTE mod 60;
         TIME.HOUR   := TIME.HOUR mod 24;

         RTEMS.TASK_WAKE_WHEN( TIME, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_WHEN" );
          
         TEST_SUPPORT.PUT_NAME( SPTEST.TASK_NAME( 1 ), FALSE );
         TEST_SUPPORT.PRINT_TIME( "awakened: ", TIME, "" );
         TEXT_IO.NEW_LINE;

      end loop;
   
   end TASK_1;

--PAGE
-- 
--  TASK_2
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin

      loop

         RTEMS.TASK_WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
          
         RTEMS.CLOCK_GET( RTEMS.CLOCK_GET_TOD, TIME'ADDRESS, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

         if TIME.SECOND >= 17 then
            TEXT_IO.PUT_LINE( "*** END OF TEST 3 ***" );
            RTEMS.SHUTDOWN_EXECUTIVE( 0 );
         end if;

         TEST_SUPPORT.PUT_NAME( SPTEST.TASK_NAME( 2 ), FALSE );
         TEST_SUPPORT.PRINT_TIME( " ", TIME, "" );
         TEXT_IO.NEW_LINE;

      end loop;

   end TASK_2;

end SPTEST;
