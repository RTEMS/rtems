--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 19 of the RTEMS
--  Single Processor Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
--  On-Line Applications Research Corporation (OAR).
--

with INTERFACES; use INTERFACES;
with FLOAT_IO;
with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;
with RTEMS.CLOCK;

include(../../support/fp.inc)
include(../../support/integer.inc)

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
      TEXT_IO.PUT_LINE( "*** TEST 19 ***" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );
      SPTEST.TASK_NAME( 3 ) := RTEMS.BUILD_NAME(  'T', 'A', '3', ' ' );
      SPTEST.TASK_NAME( 4 ) := RTEMS.BUILD_NAME(  'T', 'A', '4', ' ' );
      SPTEST.TASK_NAME( 5 ) := RTEMS.BUILD_NAME(  'T', 'A', '5', ' ' );
      SPTEST.TASK_NAME( 6 ) := RTEMS.BUILD_NAME(  'F', 'P', '1', ' ' );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         2, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.FLOATING_POINT,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 2 ), 
         2, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA2" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 3 ), 
         2, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 3 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA3" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 4 ), 
         2, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.FLOATING_POINT,
         SPTEST.TASK_ID( 4 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA4" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 5 ), 
         2, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.FLOATING_POINT,
         SPTEST.TASK_ID( 5 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA5" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 6 ), 
         1, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.FLOATING_POINT,
         SPTEST.TASK_ID( 6 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF FP1" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 6 ),
         SPTEST.FIRST_FP_TASK'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF FP1" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 1 ),
         SPTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 2 ),
         SPTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA2" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 3 ),
         SPTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA3" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 4 ),
         SPTEST.FP_TASK'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA4" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 5 ),
         SPTEST.FP_TASK'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA5" );

      --
      --  Load "task dependent factors" in the context areas
      --

      SPTEST.FP_FACTORS( 0 ) :=    0.0;
      SPTEST.FP_FACTORS( 1 ) := 1000.1;
      SPTEST.FP_FACTORS( 2 ) := 2000.2;
      SPTEST.FP_FACTORS( 3 ) := 3000.3;
      SPTEST.FP_FACTORS( 4 ) := 4000.4;
      SPTEST.FP_FACTORS( 5 ) := 5000.5;
      SPTEST.FP_FACTORS( 6 ) := 6000.6;
      SPTEST.FP_FACTORS( 7 ) := 7000.7;
      SPTEST.FP_FACTORS( 8 ) := 8000.8;
      SPTEST.FP_FACTORS( 9 ) := 9000.9;

      SPTEST.INTEGER_FACTORS( 0 ) := 16#0000#;
      SPTEST.INTEGER_FACTORS( 1 ) := 16#1000#;
      SPTEST.INTEGER_FACTORS( 2 ) := 16#2000#;
      SPTEST.INTEGER_FACTORS( 3 ) := 16#3000#;
      SPTEST.INTEGER_FACTORS( 4 ) := 16#4000#;
      SPTEST.INTEGER_FACTORS( 5 ) := 16#5000#;
      SPTEST.INTEGER_FACTORS( 6 ) := 16#6000#;
      SPTEST.INTEGER_FACTORS( 7 ) := 16#7000#;
      SPTEST.INTEGER_FACTORS( 8 ) := 16#8000#;
      SPTEST.INTEGER_FACTORS( 9 ) := 16#9000#;

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  FIRST_FP_TASK
--

   procedure FIRST_FP_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      STATUS     : RTEMS.STATUS_CODES;
      TID        : RTEMS.ID;
      TIME       : RTEMS.TIME_OF_DAY;
      TASK_INDEX : RTEMS.UNSIGNED32;
      INTEGER_DECLARE;
      FP_DECLARE;
   begin

      RTEMS.TASKS.IDENT( RTEMS.SELF, RTEMS.SEARCH_ALL_NODES, TID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF SELF" );

      TASK_INDEX := TEST_SUPPORT.TASK_NUMBER( TID );
   
      INTEGER_LOAD( INTEGER_FACTORS( TASK_INDEX ) );
      FP_LOAD( FP_FACTORS( TASK_INDEX ) );

      TEST_SUPPORT.PUT_NAME( SPTEST.TASK_NAME( TASK_INDEX ), FALSE );
      TEXT_IO.PUT( " - integer base = (" );
      UNSIGNED32_IO.PUT( INTEGER_FACTORS( TASK_INDEX ), BASE => 16 );
      TEXT_IO.PUT_LINE( ")" );

      --
      --  C implementation prints NA if no hardware FP support.
      --

      TEST_SUPPORT.PUT_NAME( 
         SPTEST.TASK_NAME( TEST_SUPPORT.TASK_NUMBER( TID ) ),
         FALSE
      );
      TEXT_IO.PUT( " - float base = (" );
      FLOAT_IO.PUT( FP_FACTORS( TASK_INDEX ) );
      TEXT_IO.PUT_LINE( ")" );

      FP_CHECK( FP_FACTORS( TASK_INDEX ) );
      INTEGER_CHECK( INTEGER_FACTORS( TASK_INDEX ) );
      if ARGUMENT = 0 then
         RTEMS.TASKS.RESTART( 
            RTEMS.SELF,
            1,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESTART OF SELF" );
      else
         TIME := ( 1988, 12, 31, 9, 0, 0, 0 );
         RTEMS.CLOCK.SET( TIME, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET" );

         RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );
      end if;

   end FIRST_FP_TASK;

-- 
--  FP_TASK
--

   procedure FP_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS     : RTEMS.STATUS_CODES;
      TID        : RTEMS.ID;
      TIME       : RTEMS.TIME_OF_DAY;
      TASK_INDEX : RTEMS.UNSIGNED32;
      INTEGER_DECLARE;
      FP_DECLARE;
   begin

      RTEMS.TASKS.IDENT( RTEMS.SELF, RTEMS.SEARCH_ALL_NODES, TID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF SELF" );
   
      TASK_INDEX := TEST_SUPPORT.TASK_NUMBER( TID );
  
      INTEGER_LOAD( INTEGER_FACTORS( TASK_INDEX ) );
      FP_LOAD( FP_FACTORS( TASK_INDEX ) );
 
      TEST_SUPPORT.PUT_NAME(
         SPTEST.TASK_NAME( TEST_SUPPORT.TASK_NUMBER( TID ) ),
         FALSE
      );
      TEXT_IO.PUT( " - integer base = (" );
      UNSIGNED32_IO.PUT( INTEGER_FACTORS( TASK_INDEX ), BASE => 16 );
      TEXT_IO.PUT_LINE( ")" );
 
      --
      --  C implementation prints NA if no hardware FP support.
      --
 
      TEST_SUPPORT.PUT_NAME(
         SPTEST.TASK_NAME( TEST_SUPPORT.TASK_NUMBER( TID ) ),
         FALSE
      );
      TEXT_IO.PUT( " - float base = (" );
      FLOAT_IO.PUT( FP_FACTORS( TASK_INDEX ) );
      TEXT_IO.PUT_LINE( ")" );

      loop

         RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

         if TIME.SECOND >= 16 then

            if TEST_SUPPORT.TASK_NUMBER( TID ) = 4 then
               TEXT_IO.PUT_LINE( "TA4 - task_delete - self" );
               RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
               TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA4" );
            end if;
      
            TEXT_IO.PUT_LINE( "TA5 - task_delete - TA3" );
            RTEMS.TASKS.DELETE( SPTEST.TASK_ID( 3 ), STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA3" );
 
            TEXT_IO.PUT_LINE( "*** END OF TEST 19 ***" );
            RTEMS.SHUTDOWN_EXECUTIVE( 0 );
         end if;

         TEST_SUPPORT.PUT_NAME( 
            SPTEST.TASK_NAME( TEST_SUPPORT.TASK_NUMBER( TID ) ),
            FALSE
         );

         TEST_SUPPORT.PRINT_TIME( " - clock_get - ", TIME, "" );
         TEXT_IO.NEW_LINE;

         INTEGER_CHECK( INTEGER_FACTORS( TASK_INDEX ) );
         FP_CHECK( FP_FACTORS( TASK_INDEX ) );
 
         RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
          
      end loop;
   
   end FP_TASK;

-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS     : RTEMS.STATUS_CODES;
      TID        : RTEMS.ID;
      TIME       : RTEMS.TIME_OF_DAY;
      TASK_INDEX : RTEMS.UNSIGNED32;
      INTEGER_DECLARE;
      FP_DECLARE;
   begin

      RTEMS.TASKS.IDENT( RTEMS.SELF, RTEMS.SEARCH_ALL_NODES, TID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF SELF" );
   
      TASK_INDEX := TEST_SUPPORT.TASK_NUMBER( TID );
  
      INTEGER_LOAD( INTEGER_FACTORS( TASK_INDEX ) );
      FP_LOAD( FP_FACTORS( TASK_INDEX ) );
 
      TEST_SUPPORT.PUT_NAME(
         SPTEST.TASK_NAME( TEST_SUPPORT.TASK_NUMBER( TID ) ),
         FALSE
      );
      TEXT_IO.PUT( " - integer base = (" );
      UNSIGNED32_IO.PUT( INTEGER_FACTORS( TASK_INDEX ), BASE => 16 );
      TEXT_IO.PUT_LINE( ")" );
 
      loop

         RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

         TEST_SUPPORT.PUT_NAME( 
            SPTEST.TASK_NAME( TEST_SUPPORT.TASK_NUMBER( TID ) ),
            FALSE
         );

         TEST_SUPPORT.PRINT_TIME( " - clock_get - ", TIME, "" );
         TEXT_IO.NEW_LINE;

         INTEGER_CHECK( INTEGER_FACTORS( TASK_INDEX ) );
         FP_CHECK( FP_FACTORS( TASK_INDEX ) );

         RTEMS.TASKS.WAKE_AFTER( 
            TEST_SUPPORT.TASK_NUMBER( TID ) * 5 * 
              TEST_SUPPORT.TICKS_PER_SECOND, 
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
          
      end loop;
   
   end TASK_1;

end SPTEST;
