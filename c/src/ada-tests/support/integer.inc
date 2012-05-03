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
--  http://www.rtems.com/license/LICENSE.

--
-- private definitions for macro use
--

-- macro usage INTEGER_CHECK_ONE( value, base, factor )
define(`INTEGER_CHECK_ONE',`
  if $1 /= ($2 + $3) then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": $1 wrong -- (" );
     UNSIGNED32_IO.PUT( $1, BASE => 16);
     TEXT_IO.PUT( " not " );
     UNSIGNED32_IO.PUT( $2, BASE => 16 );
     TEXT_IO.PUT_LINE( ")" );
  end if
'
)

-- macro usage INTEGER_DECLARE
define(`INTEGER_DECLARE', `
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
'
)


-- macro usage INTEGER_LOAD( factor )
define(`INTEGER_LOAD',`
  INT01 := INT01 + $1;
  INT02 := INT02 + $1;
  INT03 := INT03 + $1;
  INT04 := INT04 + $1;
  INT05 := INT05 + $1;
  INT06 := INT06 + $1;
  INT07 := INT07 + $1;
  INT08 := INT08 + $1;
  INT09 := INT09 + $1;
  INT10 := INT10 + $1;
  INT11 := INT11 + $1;
  INT12 := INT12 + $1;
  INT13 := INT13 + $1;
  INT14 := INT14 + $1;
  INT15 := INT15 + $1;
  INT16 := INT16 + $1;
  INT17 := INT17 + $1;
  INT18 := INT18 + $1;
  INT19 := INT19 + $1;
  INT20 := INT20 + $1;
  INT21 := INT21 + $1;
  INT22 := INT22 + $1;
  INT23 := INT23 + $1;
  INT24 := INT24 + $1;
  INT25 := INT25 + $1;
  INT26 := INT26 + $1;
  INT27 := INT27 + $1;
  INT28 := INT28 + $1;
  INT29 := INT29 + $1;
  INT30 := INT30 + $1;
  INT31 := INT31 + $1;
  INT32 := INT32 + $1
')

-- macro usage INTEGER_CHECK( factor )
define(`INTEGER_CHECK',`
  INTEGER_CHECK_ONE( INT01,  1, $1 );
  INTEGER_CHECK_ONE( INT02,  2, $1 );
  INTEGER_CHECK_ONE( INT03,  3, $1 );
  INTEGER_CHECK_ONE( INT04,  4, $1 );
  INTEGER_CHECK_ONE( INT05,  5, $1 );
  INTEGER_CHECK_ONE( INT06,  6, $1 );
  INTEGER_CHECK_ONE( INT07,  7, $1 );
  INTEGER_CHECK_ONE( INT08,  8, $1 );
  INTEGER_CHECK_ONE( INT09,  9, $1 );
  INTEGER_CHECK_ONE( INT10, 10, $1 );
  INTEGER_CHECK_ONE( INT11, 11, $1 );
  INTEGER_CHECK_ONE( INT12, 12, $1 );
  INTEGER_CHECK_ONE( INT13, 13, $1 );
  INTEGER_CHECK_ONE( INT14, 14, $1 );
  INTEGER_CHECK_ONE( INT15, 15, $1 );
  INTEGER_CHECK_ONE( INT16, 16, $1 );
  INTEGER_CHECK_ONE( INT17, 17, $1 );
  INTEGER_CHECK_ONE( INT18, 18, $1 );
  INTEGER_CHECK_ONE( INT19, 19, $1 );
  INTEGER_CHECK_ONE( INT20, 20, $1 );
  INTEGER_CHECK_ONE( INT21, 21, $1 );
  INTEGER_CHECK_ONE( INT22, 22, $1 );
  INTEGER_CHECK_ONE( INT23, 23, $1 );
  INTEGER_CHECK_ONE( INT24, 24, $1 );
  INTEGER_CHECK_ONE( INT25, 25, $1 );
  INTEGER_CHECK_ONE( INT26, 26, $1 );
  INTEGER_CHECK_ONE( INT27, 27, $1 );
  INTEGER_CHECK_ONE( INT28, 28, $1 );
  INTEGER_CHECK_ONE( INT29, 29, $1 );
  INTEGER_CHECK_ONE( INT30, 30, $1 );
  INTEGER_CHECK_ONE( INT31, 31, $1 );
  INTEGER_CHECK_ONE( INT32, 32, $1 )
'
)
