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

--
--  fp.inc
--
--  Macros to produce a large number of LOCAL floating point variables.  This
--  preprocessing is necessary to insure that the variables are
--  scoped properly and to avoid duplicating hundreds of lines
--  of code.
--
--
-- DEFICIENCIES:  
--
-- 1.  This currently does not address whether or not the CPU
--     actually has hardware floating point.  It just does the work.
--
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR). 
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.org/license/LICENSE.

--
-- private definitions for macro use
--

-- macro usage EPSILON()


-- macro usage FP_CHECK_ONE( value, base, factor )


-- macro usage FP_DECLARE



-- macro usage FP_LOAD( factor )


-- macro usage FP_CHECK( factor )


--
--  integer.inc
--
--  Macros to produce a large number of LOCAL integer variables.  This
--  preprocessing is necessary to insure that the variables are
--  scoped properly and to avoid duplicating hundreds of lines
--  of code.
--
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR). 
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.org/license/LICENSE.

--
-- private definitions for macro use
--

-- macro usage INTEGER_CHECK_ONE( value, base, factor )


-- macro usage INTEGER_DECLARE



-- macro usage INTEGER_LOAD( factor )


-- macro usage INTEGER_CHECK( factor )



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
      TEST_SUPPORT.ADA_TEST_BEGIN;

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
         RTEMS.FLOATING_POINT,
         SPTEST.TASK_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA2" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 3 ), 
         2, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.FLOATING_POINT,
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
      
  INT01 : RTEMS.UNSIGNED32 :=  1;
  INT02 : RTEMS.UNSIGNED32 :=  2;
  INT03 : RTEMS.UNSIGNED32 :=  3;
  INT04 : RTEMS.UNSIGNED32 :=  4;
  INT05 : RTEMS.UNSIGNED32 :=  5;
  INT06 : RTEMS.UNSIGNED32 :=  6;
  INT07 : RTEMS.UNSIGNED32 :=  7;
  INT08 : RTEMS.UNSIGNED32 :=  8;
  INT09 : RTEMS.UNSIGNED32 :=  9;
  INT10 : RTEMS.UNSIGNED32 := 10;
  INT11 : RTEMS.UNSIGNED32 := 11;
  INT12 : RTEMS.UNSIGNED32 := 12;
  INT13 : RTEMS.UNSIGNED32 := 13;
  INT14 : RTEMS.UNSIGNED32 := 14;
  INT15 : RTEMS.UNSIGNED32 := 15;
  INT16 : RTEMS.UNSIGNED32 := 16;
  INT17 : RTEMS.UNSIGNED32 := 17;
  INT18 : RTEMS.UNSIGNED32 := 18;
  INT19 : RTEMS.UNSIGNED32 := 19;
  INT20 : RTEMS.UNSIGNED32 := 20;
  INT21 : RTEMS.UNSIGNED32 := 21;
  INT22 : RTEMS.UNSIGNED32 := 22;
  INT23 : RTEMS.UNSIGNED32 := 23;
  INT24 : RTEMS.UNSIGNED32 := 24;
  INT25 : RTEMS.UNSIGNED32 := 25;
  INT26 : RTEMS.UNSIGNED32 := 26;
  INT27 : RTEMS.UNSIGNED32 := 27;
  INT28 : RTEMS.UNSIGNED32 := 28;
  INT29 : RTEMS.UNSIGNED32 := 29;
  INT30 : RTEMS.UNSIGNED32 := 30;
  INT31 : RTEMS.UNSIGNED32 := 31;
  INT32 : RTEMS.UNSIGNED32 := 32

;
      
  FP01 : FLOAT :=  1.0;
  FP02 : FLOAT :=  2.0;
  FP03 : FLOAT :=  3.0;
  FP04 : FLOAT :=  4.0;
  FP05 : FLOAT :=  5.0;
  FP06 : FLOAT :=  6.0;
  FP07 : FLOAT :=  7.0;
  FP08 : FLOAT :=  8.0;
  FP09 : FLOAT :=  9.0;
  FP10 : FLOAT := 10.0;
  FP11 : FLOAT := 11.0;
  FP12 : FLOAT := 12.0;
  FP13 : FLOAT := 13.0;
  FP14 : FLOAT := 14.0;
  FP15 : FLOAT := 15.0;
  FP16 : FLOAT := 16.0;
  FP17 : FLOAT := 17.0;
  FP18 : FLOAT := 18.0;
  FP19 : FLOAT := 19.0;
  FP20 : FLOAT := 20.0;
  FP21 : FLOAT := 21.0;
  FP22 : FLOAT := 22.0;
  FP23 : FLOAT := 23.0;
  FP24 : FLOAT := 24.0;
  FP25 : FLOAT := 25.0;
  FP26 : FLOAT := 26.0;
  FP27 : FLOAT := 27.0;
  FP28 : FLOAT := 28.0;
  FP29 : FLOAT := 29.0;
  FP30 : FLOAT := 30.0;
  FP31 : FLOAT := 31.0;
  FP32 : FLOAT := 32.0

;
   begin

      RTEMS.TASKS.IDENT( RTEMS.SELF, RTEMS.SEARCH_ALL_NODES, TID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF SELF" );

      TASK_INDEX := TEST_SUPPORT.TASK_NUMBER( TID );
   
      
  INT01 := INT01 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT02 := INT02 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT03 := INT03 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT04 := INT04 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT05 := INT05 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT06 := INT06 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT07 := INT07 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT08 := INT08 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT09 := INT09 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT10 := INT10 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT11 := INT11 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT12 := INT12 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT13 := INT13 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT14 := INT14 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT15 := INT15 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT16 := INT16 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT17 := INT17 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT18 := INT18 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT19 := INT19 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT20 := INT20 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT21 := INT21 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT22 := INT22 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT23 := INT23 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT24 := INT24 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT25 := INT25 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT26 := INT26 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT27 := INT27 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT28 := INT28 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT29 := INT29 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT30 := INT30 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT31 := INT31 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT32 := INT32 + INTEGER_FACTORS( TASK_INDEX ) 
;
      
  FP01 := FP01 + FP_FACTORS( TASK_INDEX ) ;
  FP02 := FP02 + FP_FACTORS( TASK_INDEX ) ;
  FP03 := FP03 + FP_FACTORS( TASK_INDEX ) ;
  FP04 := FP04 + FP_FACTORS( TASK_INDEX ) ;
  FP05 := FP05 + FP_FACTORS( TASK_INDEX ) ;
  FP06 := FP06 + FP_FACTORS( TASK_INDEX ) ;
  FP07 := FP07 + FP_FACTORS( TASK_INDEX ) ;
  FP08 := FP08 + FP_FACTORS( TASK_INDEX ) ;
  FP09 := FP09 + FP_FACTORS( TASK_INDEX ) ;
  FP10 := FP10 + FP_FACTORS( TASK_INDEX ) ;
  FP11 := FP11 + FP_FACTORS( TASK_INDEX ) ;
  FP12 := FP12 + FP_FACTORS( TASK_INDEX ) ;
  FP13 := FP13 + FP_FACTORS( TASK_INDEX ) ;
  FP14 := FP14 + FP_FACTORS( TASK_INDEX ) ;
  FP15 := FP15 + FP_FACTORS( TASK_INDEX ) ;
  FP16 := FP16 + FP_FACTORS( TASK_INDEX ) ;
  FP17 := FP17 + FP_FACTORS( TASK_INDEX ) ;
  FP18 := FP18 + FP_FACTORS( TASK_INDEX ) ;
  FP19 := FP19 + FP_FACTORS( TASK_INDEX ) ;
  FP20 := FP20 + FP_FACTORS( TASK_INDEX ) ;
  FP21 := FP21 + FP_FACTORS( TASK_INDEX ) ;
  FP22 := FP22 + FP_FACTORS( TASK_INDEX ) ;
  FP23 := FP23 + FP_FACTORS( TASK_INDEX ) ;
  FP24 := FP24 + FP_FACTORS( TASK_INDEX ) ;
  FP25 := FP25 + FP_FACTORS( TASK_INDEX ) ;
  FP26 := FP26 + FP_FACTORS( TASK_INDEX ) ;
  FP27 := FP27 + FP_FACTORS( TASK_INDEX ) ;
  FP28 := FP28 + FP_FACTORS( TASK_INDEX ) ;
  FP29 := FP29 + FP_FACTORS( TASK_INDEX ) ;
  FP30 := FP30 + FP_FACTORS( TASK_INDEX ) ;
  FP31 := FP31 + FP_FACTORS( TASK_INDEX ) ;
  FP32 := FP32 + FP_FACTORS( TASK_INDEX ) 
;

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

      
  
  if ( FP01 - ( 1.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP01 - ( 1.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP01 wrong -- (" );
     FLOAT_IO.PUT( FP01 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 1.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP02 - ( 2.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP02 - ( 2.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP02 wrong -- (" );
     FLOAT_IO.PUT( FP02 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 2.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP03 - ( 3.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP03 - ( 3.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP03 wrong -- (" );
     FLOAT_IO.PUT( FP03 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 3.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP04 - ( 4.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP04 - ( 4.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP04 wrong -- (" );
     FLOAT_IO.PUT( FP04 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 4.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP05 - ( 5.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP05 - ( 5.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP05 wrong -- (" );
     FLOAT_IO.PUT( FP05 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 5.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP06 - ( 6.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP06 - ( 6.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP06 wrong -- (" );
     FLOAT_IO.PUT( FP06 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 6.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP07 - ( 7.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP07 - ( 7.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP07 wrong -- (" );
     FLOAT_IO.PUT( FP07 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 7.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP08 - ( 8.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP08 - ( 8.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP08 wrong -- (" );
     FLOAT_IO.PUT( FP08 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 8.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP09 - ( 9.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP09 - ( 9.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP09 wrong -- (" );
     FLOAT_IO.PUT( FP09 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 9.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP10 - ( 10.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP10 - ( 10.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP10 wrong -- (" );
     FLOAT_IO.PUT( FP10 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 10.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP11 - ( 11.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP11 - ( 11.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP11 wrong -- (" );
     FLOAT_IO.PUT( FP11 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 11.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP12 - ( 12.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP12 - ( 12.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP12 wrong -- (" );
     FLOAT_IO.PUT( FP12 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 12.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP13 - ( 13.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP13 - ( 13.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP13 wrong -- (" );
     FLOAT_IO.PUT( FP13 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 13.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP14 - ( 14.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP14 - ( 14.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP14 wrong -- (" );
     FLOAT_IO.PUT( FP14 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 14.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP15 - ( 15.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP15 - ( 15.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP15 wrong -- (" );
     FLOAT_IO.PUT( FP15 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 15.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP16 - ( 16.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP16 - ( 16.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP16 wrong -- (" );
     FLOAT_IO.PUT( FP16 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 16.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP17 - ( 17.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP17 - ( 17.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP17 wrong -- (" );
     FLOAT_IO.PUT( FP17 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 17.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP18 - ( 18.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP18 - ( 18.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP18 wrong -- (" );
     FLOAT_IO.PUT( FP18 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 18.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP19 - ( 19.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP19 - ( 19.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP19 wrong -- (" );
     FLOAT_IO.PUT( FP19 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 19.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP20 - ( 20.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP20 - ( 20.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP20 wrong -- (" );
     FLOAT_IO.PUT( FP20 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 20.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP21 - ( 21.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP21 - ( 21.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP21 wrong -- (" );
     FLOAT_IO.PUT( FP21 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 21.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP22 - ( 22.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP22 - ( 22.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP22 wrong -- (" );
     FLOAT_IO.PUT( FP22 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 22.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP23 - ( 23.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP23 - ( 23.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP23 wrong -- (" );
     FLOAT_IO.PUT( FP23 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 23.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP24 - ( 24.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP24 - ( 24.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP24 wrong -- (" );
     FLOAT_IO.PUT( FP24 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 24.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP25 - ( 25.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP25 - ( 25.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP25 wrong -- (" );
     FLOAT_IO.PUT( FP25 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 25.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP26 - ( 26.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP26 - ( 26.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP26 wrong -- (" );
     FLOAT_IO.PUT( FP26 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 26.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP27 - ( 27.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP27 - ( 27.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP27 wrong -- (" );
     FLOAT_IO.PUT( FP27 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 27.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP28 - ( 28.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP28 - ( 28.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP28 wrong -- (" );
     FLOAT_IO.PUT( FP28 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 28.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP29 - ( 29.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP29 - ( 29.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP29 wrong -- (" );
     FLOAT_IO.PUT( FP29 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 29.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP30 - ( 30.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP30 - ( 30.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP30 wrong -- (" );
     FLOAT_IO.PUT( FP30 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 30.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP31 - ( 31.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP31 - ( 31.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP31 wrong -- (" );
     FLOAT_IO.PUT( FP31 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 31.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP32 - ( 32.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP32 - ( 32.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP32 wrong -- (" );
     FLOAT_IO.PUT( FP32 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 32.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if



;
      
  
  if INT01 /= (1 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT01 wrong -- (" );
     UNSIGNED32_IO.PUT( INT01, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 1, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT02 /= (2 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT02 wrong -- (" );
     UNSIGNED32_IO.PUT( INT02, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 2, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT03 /= (3 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT03 wrong -- (" );
     UNSIGNED32_IO.PUT( INT03, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 3, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT04 /= (4 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT04 wrong -- (" );
     UNSIGNED32_IO.PUT( INT04, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 4, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT05 /= (5 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT05 wrong -- (" );
     UNSIGNED32_IO.PUT( INT05, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 5, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT06 /= (6 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT06 wrong -- (" );
     UNSIGNED32_IO.PUT( INT06, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 6, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT07 /= (7 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT07 wrong -- (" );
     UNSIGNED32_IO.PUT( INT07, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 7, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT08 /= (8 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT08 wrong -- (" );
     UNSIGNED32_IO.PUT( INT08, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 8, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT09 /= (9 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT09 wrong -- (" );
     UNSIGNED32_IO.PUT( INT09, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 9, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT10 /= (10 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT10 wrong -- (" );
     UNSIGNED32_IO.PUT( INT10, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 10, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT11 /= (11 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT11 wrong -- (" );
     UNSIGNED32_IO.PUT( INT11, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 11, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT12 /= (12 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT12 wrong -- (" );
     UNSIGNED32_IO.PUT( INT12, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 12, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT13 /= (13 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT13 wrong -- (" );
     UNSIGNED32_IO.PUT( INT13, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 13, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT14 /= (14 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT14 wrong -- (" );
     UNSIGNED32_IO.PUT( INT14, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 14, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT15 /= (15 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT15 wrong -- (" );
     UNSIGNED32_IO.PUT( INT15, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 15, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT16 /= (16 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT16 wrong -- (" );
     UNSIGNED32_IO.PUT( INT16, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 16, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT17 /= (17 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT17 wrong -- (" );
     UNSIGNED32_IO.PUT( INT17, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 17, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT18 /= (18 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT18 wrong -- (" );
     UNSIGNED32_IO.PUT( INT18, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 18, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT19 /= (19 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT19 wrong -- (" );
     UNSIGNED32_IO.PUT( INT19, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 19, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT20 /= (20 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT20 wrong -- (" );
     UNSIGNED32_IO.PUT( INT20, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 20, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT21 /= (21 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT21 wrong -- (" );
     UNSIGNED32_IO.PUT( INT21, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 21, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT22 /= (22 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT22 wrong -- (" );
     UNSIGNED32_IO.PUT( INT22, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 22, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT23 /= (23 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT23 wrong -- (" );
     UNSIGNED32_IO.PUT( INT23, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 23, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT24 /= (24 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT24 wrong -- (" );
     UNSIGNED32_IO.PUT( INT24, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 24, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT25 /= (25 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT25 wrong -- (" );
     UNSIGNED32_IO.PUT( INT25, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 25, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT26 /= (26 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT26 wrong -- (" );
     UNSIGNED32_IO.PUT( INT26, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 26, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT27 /= (27 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT27 wrong -- (" );
     UNSIGNED32_IO.PUT( INT27, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 27, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT28 /= (28 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT28 wrong -- (" );
     UNSIGNED32_IO.PUT( INT28, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 28, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT29 /= (29 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT29 wrong -- (" );
     UNSIGNED32_IO.PUT( INT29, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 29, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT30 /= (30 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT30 wrong -- (" );
     UNSIGNED32_IO.PUT( INT30, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 30, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT31 /= (31 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT31 wrong -- (" );
     UNSIGNED32_IO.PUT( INT31, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 31, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT32 /= (32 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT32 wrong -- (" );
     UNSIGNED32_IO.PUT( INT32, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 32, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if



;
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
      
  INT01 : RTEMS.UNSIGNED32 :=  1;
  INT02 : RTEMS.UNSIGNED32 :=  2;
  INT03 : RTEMS.UNSIGNED32 :=  3;
  INT04 : RTEMS.UNSIGNED32 :=  4;
  INT05 : RTEMS.UNSIGNED32 :=  5;
  INT06 : RTEMS.UNSIGNED32 :=  6;
  INT07 : RTEMS.UNSIGNED32 :=  7;
  INT08 : RTEMS.UNSIGNED32 :=  8;
  INT09 : RTEMS.UNSIGNED32 :=  9;
  INT10 : RTEMS.UNSIGNED32 := 10;
  INT11 : RTEMS.UNSIGNED32 := 11;
  INT12 : RTEMS.UNSIGNED32 := 12;
  INT13 : RTEMS.UNSIGNED32 := 13;
  INT14 : RTEMS.UNSIGNED32 := 14;
  INT15 : RTEMS.UNSIGNED32 := 15;
  INT16 : RTEMS.UNSIGNED32 := 16;
  INT17 : RTEMS.UNSIGNED32 := 17;
  INT18 : RTEMS.UNSIGNED32 := 18;
  INT19 : RTEMS.UNSIGNED32 := 19;
  INT20 : RTEMS.UNSIGNED32 := 20;
  INT21 : RTEMS.UNSIGNED32 := 21;
  INT22 : RTEMS.UNSIGNED32 := 22;
  INT23 : RTEMS.UNSIGNED32 := 23;
  INT24 : RTEMS.UNSIGNED32 := 24;
  INT25 : RTEMS.UNSIGNED32 := 25;
  INT26 : RTEMS.UNSIGNED32 := 26;
  INT27 : RTEMS.UNSIGNED32 := 27;
  INT28 : RTEMS.UNSIGNED32 := 28;
  INT29 : RTEMS.UNSIGNED32 := 29;
  INT30 : RTEMS.UNSIGNED32 := 30;
  INT31 : RTEMS.UNSIGNED32 := 31;
  INT32 : RTEMS.UNSIGNED32 := 32

;
      
  FP01 : FLOAT :=  1.0;
  FP02 : FLOAT :=  2.0;
  FP03 : FLOAT :=  3.0;
  FP04 : FLOAT :=  4.0;
  FP05 : FLOAT :=  5.0;
  FP06 : FLOAT :=  6.0;
  FP07 : FLOAT :=  7.0;
  FP08 : FLOAT :=  8.0;
  FP09 : FLOAT :=  9.0;
  FP10 : FLOAT := 10.0;
  FP11 : FLOAT := 11.0;
  FP12 : FLOAT := 12.0;
  FP13 : FLOAT := 13.0;
  FP14 : FLOAT := 14.0;
  FP15 : FLOAT := 15.0;
  FP16 : FLOAT := 16.0;
  FP17 : FLOAT := 17.0;
  FP18 : FLOAT := 18.0;
  FP19 : FLOAT := 19.0;
  FP20 : FLOAT := 20.0;
  FP21 : FLOAT := 21.0;
  FP22 : FLOAT := 22.0;
  FP23 : FLOAT := 23.0;
  FP24 : FLOAT := 24.0;
  FP25 : FLOAT := 25.0;
  FP26 : FLOAT := 26.0;
  FP27 : FLOAT := 27.0;
  FP28 : FLOAT := 28.0;
  FP29 : FLOAT := 29.0;
  FP30 : FLOAT := 30.0;
  FP31 : FLOAT := 31.0;
  FP32 : FLOAT := 32.0

;
   begin

      RTEMS.TASKS.IDENT( RTEMS.SELF, RTEMS.SEARCH_ALL_NODES, TID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF SELF" );
   
      TASK_INDEX := TEST_SUPPORT.TASK_NUMBER( TID );
  
      
  INT01 := INT01 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT02 := INT02 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT03 := INT03 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT04 := INT04 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT05 := INT05 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT06 := INT06 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT07 := INT07 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT08 := INT08 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT09 := INT09 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT10 := INT10 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT11 := INT11 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT12 := INT12 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT13 := INT13 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT14 := INT14 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT15 := INT15 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT16 := INT16 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT17 := INT17 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT18 := INT18 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT19 := INT19 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT20 := INT20 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT21 := INT21 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT22 := INT22 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT23 := INT23 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT24 := INT24 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT25 := INT25 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT26 := INT26 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT27 := INT27 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT28 := INT28 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT29 := INT29 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT30 := INT30 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT31 := INT31 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT32 := INT32 + INTEGER_FACTORS( TASK_INDEX ) 
;
      
  FP01 := FP01 + FP_FACTORS( TASK_INDEX ) ;
  FP02 := FP02 + FP_FACTORS( TASK_INDEX ) ;
  FP03 := FP03 + FP_FACTORS( TASK_INDEX ) ;
  FP04 := FP04 + FP_FACTORS( TASK_INDEX ) ;
  FP05 := FP05 + FP_FACTORS( TASK_INDEX ) ;
  FP06 := FP06 + FP_FACTORS( TASK_INDEX ) ;
  FP07 := FP07 + FP_FACTORS( TASK_INDEX ) ;
  FP08 := FP08 + FP_FACTORS( TASK_INDEX ) ;
  FP09 := FP09 + FP_FACTORS( TASK_INDEX ) ;
  FP10 := FP10 + FP_FACTORS( TASK_INDEX ) ;
  FP11 := FP11 + FP_FACTORS( TASK_INDEX ) ;
  FP12 := FP12 + FP_FACTORS( TASK_INDEX ) ;
  FP13 := FP13 + FP_FACTORS( TASK_INDEX ) ;
  FP14 := FP14 + FP_FACTORS( TASK_INDEX ) ;
  FP15 := FP15 + FP_FACTORS( TASK_INDEX ) ;
  FP16 := FP16 + FP_FACTORS( TASK_INDEX ) ;
  FP17 := FP17 + FP_FACTORS( TASK_INDEX ) ;
  FP18 := FP18 + FP_FACTORS( TASK_INDEX ) ;
  FP19 := FP19 + FP_FACTORS( TASK_INDEX ) ;
  FP20 := FP20 + FP_FACTORS( TASK_INDEX ) ;
  FP21 := FP21 + FP_FACTORS( TASK_INDEX ) ;
  FP22 := FP22 + FP_FACTORS( TASK_INDEX ) ;
  FP23 := FP23 + FP_FACTORS( TASK_INDEX ) ;
  FP24 := FP24 + FP_FACTORS( TASK_INDEX ) ;
  FP25 := FP25 + FP_FACTORS( TASK_INDEX ) ;
  FP26 := FP26 + FP_FACTORS( TASK_INDEX ) ;
  FP27 := FP27 + FP_FACTORS( TASK_INDEX ) ;
  FP28 := FP28 + FP_FACTORS( TASK_INDEX ) ;
  FP29 := FP29 + FP_FACTORS( TASK_INDEX ) ;
  FP30 := FP30 + FP_FACTORS( TASK_INDEX ) ;
  FP31 := FP31 + FP_FACTORS( TASK_INDEX ) ;
  FP32 := FP32 + FP_FACTORS( TASK_INDEX ) 
;
 
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

         RTEMS.CLOCK.GET_TOD( TIME, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET_TOD" );

         if TIME.SECOND >= 16 then

            if TEST_SUPPORT.TASK_NUMBER( TID ) = 4 then
               TEXT_IO.PUT_LINE( "TA4 - task_delete - self" );
               RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
               TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA4" );
            end if;
      
            TEXT_IO.PUT_LINE( "TA5 - task_delete - TA3" );
            RTEMS.TASKS.DELETE( SPTEST.TASK_ID( 3 ), STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA3" );
 
            TEST_SUPPORT.ADA_TEST_END;
            RTEMS.SHUTDOWN_EXECUTIVE( 0 );
         end if;

         TEST_SUPPORT.PUT_NAME( 
            SPTEST.TASK_NAME( TEST_SUPPORT.TASK_NUMBER( TID ) ),
            FALSE
         );

         TEST_SUPPORT.PRINT_TIME( " - clock_get - ", TIME, "" );
         TEXT_IO.NEW_LINE;

         
  
  if INT01 /= (1 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT01 wrong -- (" );
     UNSIGNED32_IO.PUT( INT01, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 1, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT02 /= (2 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT02 wrong -- (" );
     UNSIGNED32_IO.PUT( INT02, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 2, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT03 /= (3 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT03 wrong -- (" );
     UNSIGNED32_IO.PUT( INT03, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 3, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT04 /= (4 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT04 wrong -- (" );
     UNSIGNED32_IO.PUT( INT04, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 4, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT05 /= (5 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT05 wrong -- (" );
     UNSIGNED32_IO.PUT( INT05, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 5, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT06 /= (6 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT06 wrong -- (" );
     UNSIGNED32_IO.PUT( INT06, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 6, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT07 /= (7 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT07 wrong -- (" );
     UNSIGNED32_IO.PUT( INT07, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 7, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT08 /= (8 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT08 wrong -- (" );
     UNSIGNED32_IO.PUT( INT08, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 8, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT09 /= (9 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT09 wrong -- (" );
     UNSIGNED32_IO.PUT( INT09, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 9, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT10 /= (10 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT10 wrong -- (" );
     UNSIGNED32_IO.PUT( INT10, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 10, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT11 /= (11 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT11 wrong -- (" );
     UNSIGNED32_IO.PUT( INT11, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 11, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT12 /= (12 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT12 wrong -- (" );
     UNSIGNED32_IO.PUT( INT12, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 12, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT13 /= (13 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT13 wrong -- (" );
     UNSIGNED32_IO.PUT( INT13, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 13, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT14 /= (14 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT14 wrong -- (" );
     UNSIGNED32_IO.PUT( INT14, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 14, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT15 /= (15 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT15 wrong -- (" );
     UNSIGNED32_IO.PUT( INT15, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 15, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT16 /= (16 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT16 wrong -- (" );
     UNSIGNED32_IO.PUT( INT16, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 16, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT17 /= (17 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT17 wrong -- (" );
     UNSIGNED32_IO.PUT( INT17, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 17, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT18 /= (18 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT18 wrong -- (" );
     UNSIGNED32_IO.PUT( INT18, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 18, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT19 /= (19 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT19 wrong -- (" );
     UNSIGNED32_IO.PUT( INT19, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 19, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT20 /= (20 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT20 wrong -- (" );
     UNSIGNED32_IO.PUT( INT20, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 20, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT21 /= (21 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT21 wrong -- (" );
     UNSIGNED32_IO.PUT( INT21, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 21, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT22 /= (22 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT22 wrong -- (" );
     UNSIGNED32_IO.PUT( INT22, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 22, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT23 /= (23 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT23 wrong -- (" );
     UNSIGNED32_IO.PUT( INT23, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 23, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT24 /= (24 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT24 wrong -- (" );
     UNSIGNED32_IO.PUT( INT24, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 24, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT25 /= (25 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT25 wrong -- (" );
     UNSIGNED32_IO.PUT( INT25, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 25, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT26 /= (26 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT26 wrong -- (" );
     UNSIGNED32_IO.PUT( INT26, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 26, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT27 /= (27 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT27 wrong -- (" );
     UNSIGNED32_IO.PUT( INT27, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 27, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT28 /= (28 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT28 wrong -- (" );
     UNSIGNED32_IO.PUT( INT28, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 28, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT29 /= (29 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT29 wrong -- (" );
     UNSIGNED32_IO.PUT( INT29, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 29, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT30 /= (30 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT30 wrong -- (" );
     UNSIGNED32_IO.PUT( INT30, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 30, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT31 /= (31 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT31 wrong -- (" );
     UNSIGNED32_IO.PUT( INT31, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 31, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT32 /= (32 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT32 wrong -- (" );
     UNSIGNED32_IO.PUT( INT32, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 32, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if



;
         
  
  if ( FP01 - ( 1.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP01 - ( 1.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP01 wrong -- (" );
     FLOAT_IO.PUT( FP01 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 1.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP02 - ( 2.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP02 - ( 2.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP02 wrong -- (" );
     FLOAT_IO.PUT( FP02 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 2.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP03 - ( 3.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP03 - ( 3.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP03 wrong -- (" );
     FLOAT_IO.PUT( FP03 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 3.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP04 - ( 4.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP04 - ( 4.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP04 wrong -- (" );
     FLOAT_IO.PUT( FP04 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 4.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP05 - ( 5.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP05 - ( 5.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP05 wrong -- (" );
     FLOAT_IO.PUT( FP05 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 5.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP06 - ( 6.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP06 - ( 6.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP06 wrong -- (" );
     FLOAT_IO.PUT( FP06 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 6.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP07 - ( 7.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP07 - ( 7.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP07 wrong -- (" );
     FLOAT_IO.PUT( FP07 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 7.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP08 - ( 8.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP08 - ( 8.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP08 wrong -- (" );
     FLOAT_IO.PUT( FP08 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 8.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP09 - ( 9.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP09 - ( 9.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP09 wrong -- (" );
     FLOAT_IO.PUT( FP09 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 9.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP10 - ( 10.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP10 - ( 10.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP10 wrong -- (" );
     FLOAT_IO.PUT( FP10 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 10.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP11 - ( 11.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP11 - ( 11.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP11 wrong -- (" );
     FLOAT_IO.PUT( FP11 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 11.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP12 - ( 12.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP12 - ( 12.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP12 wrong -- (" );
     FLOAT_IO.PUT( FP12 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 12.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP13 - ( 13.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP13 - ( 13.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP13 wrong -- (" );
     FLOAT_IO.PUT( FP13 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 13.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP14 - ( 14.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP14 - ( 14.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP14 wrong -- (" );
     FLOAT_IO.PUT( FP14 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 14.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP15 - ( 15.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP15 - ( 15.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP15 wrong -- (" );
     FLOAT_IO.PUT( FP15 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 15.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP16 - ( 16.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP16 - ( 16.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP16 wrong -- (" );
     FLOAT_IO.PUT( FP16 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 16.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP17 - ( 17.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP17 - ( 17.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP17 wrong -- (" );
     FLOAT_IO.PUT( FP17 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 17.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP18 - ( 18.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP18 - ( 18.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP18 wrong -- (" );
     FLOAT_IO.PUT( FP18 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 18.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP19 - ( 19.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP19 - ( 19.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP19 wrong -- (" );
     FLOAT_IO.PUT( FP19 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 19.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP20 - ( 20.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP20 - ( 20.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP20 wrong -- (" );
     FLOAT_IO.PUT( FP20 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 20.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP21 - ( 21.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP21 - ( 21.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP21 wrong -- (" );
     FLOAT_IO.PUT( FP21 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 21.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP22 - ( 22.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP22 - ( 22.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP22 wrong -- (" );
     FLOAT_IO.PUT( FP22 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 22.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP23 - ( 23.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP23 - ( 23.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP23 wrong -- (" );
     FLOAT_IO.PUT( FP23 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 23.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP24 - ( 24.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP24 - ( 24.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP24 wrong -- (" );
     FLOAT_IO.PUT( FP24 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 24.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP25 - ( 25.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP25 - ( 25.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP25 wrong -- (" );
     FLOAT_IO.PUT( FP25 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 25.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP26 - ( 26.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP26 - ( 26.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP26 wrong -- (" );
     FLOAT_IO.PUT( FP26 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 26.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP27 - ( 27.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP27 - ( 27.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP27 wrong -- (" );
     FLOAT_IO.PUT( FP27 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 27.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP28 - ( 28.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP28 - ( 28.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP28 wrong -- (" );
     FLOAT_IO.PUT( FP28 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 28.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP29 - ( 29.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP29 - ( 29.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP29 wrong -- (" );
     FLOAT_IO.PUT( FP29 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 29.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP30 - ( 30.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP30 - ( 30.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP30 wrong -- (" );
     FLOAT_IO.PUT( FP30 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 30.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP31 - ( 31.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP31 - ( 31.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP31 wrong -- (" );
     FLOAT_IO.PUT( FP31 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 31.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP32 - ( 32.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP32 - ( 32.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP32 wrong -- (" );
     FLOAT_IO.PUT( FP32 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 32.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if



;
 
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
      
  INT01 : RTEMS.UNSIGNED32 :=  1;
  INT02 : RTEMS.UNSIGNED32 :=  2;
  INT03 : RTEMS.UNSIGNED32 :=  3;
  INT04 : RTEMS.UNSIGNED32 :=  4;
  INT05 : RTEMS.UNSIGNED32 :=  5;
  INT06 : RTEMS.UNSIGNED32 :=  6;
  INT07 : RTEMS.UNSIGNED32 :=  7;
  INT08 : RTEMS.UNSIGNED32 :=  8;
  INT09 : RTEMS.UNSIGNED32 :=  9;
  INT10 : RTEMS.UNSIGNED32 := 10;
  INT11 : RTEMS.UNSIGNED32 := 11;
  INT12 : RTEMS.UNSIGNED32 := 12;
  INT13 : RTEMS.UNSIGNED32 := 13;
  INT14 : RTEMS.UNSIGNED32 := 14;
  INT15 : RTEMS.UNSIGNED32 := 15;
  INT16 : RTEMS.UNSIGNED32 := 16;
  INT17 : RTEMS.UNSIGNED32 := 17;
  INT18 : RTEMS.UNSIGNED32 := 18;
  INT19 : RTEMS.UNSIGNED32 := 19;
  INT20 : RTEMS.UNSIGNED32 := 20;
  INT21 : RTEMS.UNSIGNED32 := 21;
  INT22 : RTEMS.UNSIGNED32 := 22;
  INT23 : RTEMS.UNSIGNED32 := 23;
  INT24 : RTEMS.UNSIGNED32 := 24;
  INT25 : RTEMS.UNSIGNED32 := 25;
  INT26 : RTEMS.UNSIGNED32 := 26;
  INT27 : RTEMS.UNSIGNED32 := 27;
  INT28 : RTEMS.UNSIGNED32 := 28;
  INT29 : RTEMS.UNSIGNED32 := 29;
  INT30 : RTEMS.UNSIGNED32 := 30;
  INT31 : RTEMS.UNSIGNED32 := 31;
  INT32 : RTEMS.UNSIGNED32 := 32

;
      
  FP01 : FLOAT :=  1.0;
  FP02 : FLOAT :=  2.0;
  FP03 : FLOAT :=  3.0;
  FP04 : FLOAT :=  4.0;
  FP05 : FLOAT :=  5.0;
  FP06 : FLOAT :=  6.0;
  FP07 : FLOAT :=  7.0;
  FP08 : FLOAT :=  8.0;
  FP09 : FLOAT :=  9.0;
  FP10 : FLOAT := 10.0;
  FP11 : FLOAT := 11.0;
  FP12 : FLOAT := 12.0;
  FP13 : FLOAT := 13.0;
  FP14 : FLOAT := 14.0;
  FP15 : FLOAT := 15.0;
  FP16 : FLOAT := 16.0;
  FP17 : FLOAT := 17.0;
  FP18 : FLOAT := 18.0;
  FP19 : FLOAT := 19.0;
  FP20 : FLOAT := 20.0;
  FP21 : FLOAT := 21.0;
  FP22 : FLOAT := 22.0;
  FP23 : FLOAT := 23.0;
  FP24 : FLOAT := 24.0;
  FP25 : FLOAT := 25.0;
  FP26 : FLOAT := 26.0;
  FP27 : FLOAT := 27.0;
  FP28 : FLOAT := 28.0;
  FP29 : FLOAT := 29.0;
  FP30 : FLOAT := 30.0;
  FP31 : FLOAT := 31.0;
  FP32 : FLOAT := 32.0

;
   begin

      RTEMS.TASKS.IDENT( RTEMS.SELF, RTEMS.SEARCH_ALL_NODES, TID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF SELF" );
   
      TASK_INDEX := TEST_SUPPORT.TASK_NUMBER( TID );
  
      
  INT01 := INT01 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT02 := INT02 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT03 := INT03 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT04 := INT04 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT05 := INT05 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT06 := INT06 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT07 := INT07 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT08 := INT08 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT09 := INT09 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT10 := INT10 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT11 := INT11 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT12 := INT12 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT13 := INT13 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT14 := INT14 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT15 := INT15 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT16 := INT16 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT17 := INT17 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT18 := INT18 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT19 := INT19 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT20 := INT20 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT21 := INT21 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT22 := INT22 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT23 := INT23 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT24 := INT24 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT25 := INT25 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT26 := INT26 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT27 := INT27 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT28 := INT28 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT29 := INT29 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT30 := INT30 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT31 := INT31 + INTEGER_FACTORS( TASK_INDEX ) ;
  INT32 := INT32 + INTEGER_FACTORS( TASK_INDEX ) 
;
      
  FP01 := FP01 + FP_FACTORS( TASK_INDEX ) ;
  FP02 := FP02 + FP_FACTORS( TASK_INDEX ) ;
  FP03 := FP03 + FP_FACTORS( TASK_INDEX ) ;
  FP04 := FP04 + FP_FACTORS( TASK_INDEX ) ;
  FP05 := FP05 + FP_FACTORS( TASK_INDEX ) ;
  FP06 := FP06 + FP_FACTORS( TASK_INDEX ) ;
  FP07 := FP07 + FP_FACTORS( TASK_INDEX ) ;
  FP08 := FP08 + FP_FACTORS( TASK_INDEX ) ;
  FP09 := FP09 + FP_FACTORS( TASK_INDEX ) ;
  FP10 := FP10 + FP_FACTORS( TASK_INDEX ) ;
  FP11 := FP11 + FP_FACTORS( TASK_INDEX ) ;
  FP12 := FP12 + FP_FACTORS( TASK_INDEX ) ;
  FP13 := FP13 + FP_FACTORS( TASK_INDEX ) ;
  FP14 := FP14 + FP_FACTORS( TASK_INDEX ) ;
  FP15 := FP15 + FP_FACTORS( TASK_INDEX ) ;
  FP16 := FP16 + FP_FACTORS( TASK_INDEX ) ;
  FP17 := FP17 + FP_FACTORS( TASK_INDEX ) ;
  FP18 := FP18 + FP_FACTORS( TASK_INDEX ) ;
  FP19 := FP19 + FP_FACTORS( TASK_INDEX ) ;
  FP20 := FP20 + FP_FACTORS( TASK_INDEX ) ;
  FP21 := FP21 + FP_FACTORS( TASK_INDEX ) ;
  FP22 := FP22 + FP_FACTORS( TASK_INDEX ) ;
  FP23 := FP23 + FP_FACTORS( TASK_INDEX ) ;
  FP24 := FP24 + FP_FACTORS( TASK_INDEX ) ;
  FP25 := FP25 + FP_FACTORS( TASK_INDEX ) ;
  FP26 := FP26 + FP_FACTORS( TASK_INDEX ) ;
  FP27 := FP27 + FP_FACTORS( TASK_INDEX ) ;
  FP28 := FP28 + FP_FACTORS( TASK_INDEX ) ;
  FP29 := FP29 + FP_FACTORS( TASK_INDEX ) ;
  FP30 := FP30 + FP_FACTORS( TASK_INDEX ) ;
  FP31 := FP31 + FP_FACTORS( TASK_INDEX ) ;
  FP32 := FP32 + FP_FACTORS( TASK_INDEX ) 
;
 
      TEST_SUPPORT.PUT_NAME(
         SPTEST.TASK_NAME( TEST_SUPPORT.TASK_NUMBER( TID ) ),
         FALSE
      );
      TEXT_IO.PUT( " - integer base = (" );
      UNSIGNED32_IO.PUT( INTEGER_FACTORS( TASK_INDEX ), BASE => 16 );
      TEXT_IO.PUT_LINE( ")" );
 
      loop

         RTEMS.CLOCK.GET_TOD( TIME, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET_TOD" );

         TEST_SUPPORT.PUT_NAME( 
            SPTEST.TASK_NAME( TEST_SUPPORT.TASK_NUMBER( TID ) ),
            FALSE
         );

         TEST_SUPPORT.PRINT_TIME( " - clock_get - ", TIME, "" );
         TEXT_IO.NEW_LINE;

         
  
  if INT01 /= (1 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT01 wrong -- (" );
     UNSIGNED32_IO.PUT( INT01, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 1, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT02 /= (2 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT02 wrong -- (" );
     UNSIGNED32_IO.PUT( INT02, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 2, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT03 /= (3 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT03 wrong -- (" );
     UNSIGNED32_IO.PUT( INT03, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 3, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT04 /= (4 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT04 wrong -- (" );
     UNSIGNED32_IO.PUT( INT04, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 4, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT05 /= (5 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT05 wrong -- (" );
     UNSIGNED32_IO.PUT( INT05, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 5, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT06 /= (6 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT06 wrong -- (" );
     UNSIGNED32_IO.PUT( INT06, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 6, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT07 /= (7 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT07 wrong -- (" );
     UNSIGNED32_IO.PUT( INT07, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 7, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT08 /= (8 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT08 wrong -- (" );
     UNSIGNED32_IO.PUT( INT08, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 8, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT09 /= (9 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT09 wrong -- (" );
     UNSIGNED32_IO.PUT( INT09, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 9, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT10 /= (10 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT10 wrong -- (" );
     UNSIGNED32_IO.PUT( INT10, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 10, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT11 /= (11 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT11 wrong -- (" );
     UNSIGNED32_IO.PUT( INT11, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 11, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT12 /= (12 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT12 wrong -- (" );
     UNSIGNED32_IO.PUT( INT12, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 12, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT13 /= (13 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT13 wrong -- (" );
     UNSIGNED32_IO.PUT( INT13, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 13, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT14 /= (14 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT14 wrong -- (" );
     UNSIGNED32_IO.PUT( INT14, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 14, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT15 /= (15 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT15 wrong -- (" );
     UNSIGNED32_IO.PUT( INT15, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 15, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT16 /= (16 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT16 wrong -- (" );
     UNSIGNED32_IO.PUT( INT16, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 16, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT17 /= (17 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT17 wrong -- (" );
     UNSIGNED32_IO.PUT( INT17, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 17, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT18 /= (18 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT18 wrong -- (" );
     UNSIGNED32_IO.PUT( INT18, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 18, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT19 /= (19 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT19 wrong -- (" );
     UNSIGNED32_IO.PUT( INT19, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 19, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT20 /= (20 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT20 wrong -- (" );
     UNSIGNED32_IO.PUT( INT20, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 20, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT21 /= (21 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT21 wrong -- (" );
     UNSIGNED32_IO.PUT( INT21, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 21, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT22 /= (22 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT22 wrong -- (" );
     UNSIGNED32_IO.PUT( INT22, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 22, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT23 /= (23 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT23 wrong -- (" );
     UNSIGNED32_IO.PUT( INT23, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 23, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT24 /= (24 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT24 wrong -- (" );
     UNSIGNED32_IO.PUT( INT24, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 24, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT25 /= (25 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT25 wrong -- (" );
     UNSIGNED32_IO.PUT( INT25, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 25, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT26 /= (26 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT26 wrong -- (" );
     UNSIGNED32_IO.PUT( INT26, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 26, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT27 /= (27 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT27 wrong -- (" );
     UNSIGNED32_IO.PUT( INT27, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 27, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT28 /= (28 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT28 wrong -- (" );
     UNSIGNED32_IO.PUT( INT28, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 28, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT29 /= (29 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT29 wrong -- (" );
     UNSIGNED32_IO.PUT( INT29, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 29, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT30 /= (30 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT30 wrong -- (" );
     UNSIGNED32_IO.PUT( INT30, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 30, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT31 /= (31 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT31 wrong -- (" );
     UNSIGNED32_IO.PUT( INT31, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 31, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if INT32 /= (32 + INTEGER_FACTORS( TASK_INDEX )  ) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": INT32 wrong -- (" );
     UNSIGNED32_IO.PUT( INT32, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( 32, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if



;
         
  
  if ( FP01 - ( 1.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP01 - ( 1.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP01 wrong -- (" );
     FLOAT_IO.PUT( FP01 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 1.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP02 - ( 2.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP02 - ( 2.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP02 wrong -- (" );
     FLOAT_IO.PUT( FP02 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 2.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP03 - ( 3.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP03 - ( 3.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP03 wrong -- (" );
     FLOAT_IO.PUT( FP03 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 3.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP04 - ( 4.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP04 - ( 4.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP04 wrong -- (" );
     FLOAT_IO.PUT( FP04 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 4.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP05 - ( 5.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP05 - ( 5.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP05 wrong -- (" );
     FLOAT_IO.PUT( FP05 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 5.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP06 - ( 6.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP06 - ( 6.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP06 wrong -- (" );
     FLOAT_IO.PUT( FP06 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 6.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP07 - ( 7.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP07 - ( 7.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP07 wrong -- (" );
     FLOAT_IO.PUT( FP07 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 7.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP08 - ( 8.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP08 - ( 8.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP08 wrong -- (" );
     FLOAT_IO.PUT( FP08 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 8.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP09 - ( 9.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP09 - ( 9.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP09 wrong -- (" );
     FLOAT_IO.PUT( FP09 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 9.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP10 - ( 10.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP10 - ( 10.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP10 wrong -- (" );
     FLOAT_IO.PUT( FP10 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 10.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP11 - ( 11.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP11 - ( 11.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP11 wrong -- (" );
     FLOAT_IO.PUT( FP11 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 11.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP12 - ( 12.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP12 - ( 12.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP12 wrong -- (" );
     FLOAT_IO.PUT( FP12 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 12.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP13 - ( 13.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP13 - ( 13.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP13 wrong -- (" );
     FLOAT_IO.PUT( FP13 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 13.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP14 - ( 14.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP14 - ( 14.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP14 wrong -- (" );
     FLOAT_IO.PUT( FP14 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 14.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP15 - ( 15.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP15 - ( 15.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP15 wrong -- (" );
     FLOAT_IO.PUT( FP15 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 15.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP16 - ( 16.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP16 - ( 16.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP16 wrong -- (" );
     FLOAT_IO.PUT( FP16 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 16.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP17 - ( 17.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP17 - ( 17.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP17 wrong -- (" );
     FLOAT_IO.PUT( FP17 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 17.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP18 - ( 18.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP18 - ( 18.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP18 wrong -- (" );
     FLOAT_IO.PUT( FP18 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 18.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP19 - ( 19.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP19 - ( 19.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP19 wrong -- (" );
     FLOAT_IO.PUT( FP19 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 19.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP20 - ( 20.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP20 - ( 20.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP20 wrong -- (" );
     FLOAT_IO.PUT( FP20 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 20.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP21 - ( 21.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP21 - ( 21.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP21 wrong -- (" );
     FLOAT_IO.PUT( FP21 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 21.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP22 - ( 22.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP22 - ( 22.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP22 wrong -- (" );
     FLOAT_IO.PUT( FP22 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 22.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP23 - ( 23.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP23 - ( 23.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP23 wrong -- (" );
     FLOAT_IO.PUT( FP23 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 23.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP24 - ( 24.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP24 - ( 24.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP24 wrong -- (" );
     FLOAT_IO.PUT( FP24 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 24.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP25 - ( 25.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP25 - ( 25.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP25 wrong -- (" );
     FLOAT_IO.PUT( FP25 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 25.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP26 - ( 26.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP26 - ( 26.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP26 wrong -- (" );
     FLOAT_IO.PUT( FP26 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 26.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP27 - ( 27.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP27 - ( 27.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP27 wrong -- (" );
     FLOAT_IO.PUT( FP27 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 27.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP28 - ( 28.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP28 - ( 28.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP28 wrong -- (" );
     FLOAT_IO.PUT( FP28 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 28.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP29 - ( 29.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP29 - ( 29.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP29 wrong -- (" );
     FLOAT_IO.PUT( FP29 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 29.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP30 - ( 30.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP30 - ( 30.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP30 wrong -- (" );
     FLOAT_IO.PUT( FP30 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 30.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP31 - ( 31.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP31 - ( 31.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP31 wrong -- (" );
     FLOAT_IO.PUT( FP31 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 31.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if

;
  
  if ( FP32 - ( 32.0 + FP_FACTORS( TASK_INDEX )   ) ) > 0.0005 or else ( FP32 - ( 32.0 + FP_FACTORS( TASK_INDEX )   ) ) < - 0.0005 then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": FP32 wrong -- (" );
     FLOAT_IO.PUT( FP32 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( 32.0 + FP_FACTORS( TASK_INDEX )   );
     TEXT_IO.PUT_LINE( ")" );
  end if



;

         RTEMS.TASKS.WAKE_AFTER( 
            TEST_SUPPORT.TASK_NUMBER( TID ) * 5 * 
              TEST_SUPPORT.TICKS_PER_SECOND, 
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
          
      end loop;
   
   end TASK_1;

end SPTEST;
