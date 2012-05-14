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
--  http://www.rtems.com/license/LICENSE.

--
-- private definitions for macro use
--

-- macro usage EPSILON()
define(`EPSILON',`0.0005')

-- macro usage FP_CHECK_ONE( value, base, factor )
define(`FP_CHECK_ONE',`
  if ( $1 - ( $2 + $3 ) ) > EPSILON or else ( $1 - ( $2 + $3 ) ) < - EPSILON then
     UNSIGNED32_IO.PUT( TASK_INDEX );
     TEXT_IO.PUT( ": $1 wrong -- (" );
     FLOAT_IO.PUT( $1 );
     TEXT_IO.PUT( " not " );
     FLOAT_IO.PUT( $2 + $3 );
     TEXT_IO.PUT_LINE( ")" );
  end if
'
)

-- macro usage FP_DECLARE
define(`FP_DECLARE', `
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
'
)


-- macro usage FP_LOAD( factor )
define(`FP_LOAD',`
  FP01 := FP01 + $1;
  FP02 := FP02 + $1;
  FP03 := FP03 + $1;
  FP04 := FP04 + $1;
  FP05 := FP05 + $1;
  FP06 := FP06 + $1;
  FP07 := FP07 + $1;
  FP08 := FP08 + $1;
  FP09 := FP09 + $1;
  FP10 := FP10 + $1;
  FP11 := FP11 + $1;
  FP12 := FP12 + $1;
  FP13 := FP13 + $1;
  FP14 := FP14 + $1;
  FP15 := FP15 + $1;
  FP16 := FP16 + $1;
  FP17 := FP17 + $1;
  FP18 := FP18 + $1;
  FP19 := FP19 + $1;
  FP20 := FP20 + $1;
  FP21 := FP21 + $1;
  FP22 := FP22 + $1;
  FP23 := FP23 + $1;
  FP24 := FP24 + $1;
  FP25 := FP25 + $1;
  FP26 := FP26 + $1;
  FP27 := FP27 + $1;
  FP28 := FP28 + $1;
  FP29 := FP29 + $1;
  FP30 := FP30 + $1;
  FP31 := FP31 + $1;
  FP32 := FP32 + $1
')

-- macro usage FP_CHECK( factor )
define(`FP_CHECK',`
  FP_CHECK_ONE( FP01,  1.0, $1 );
  FP_CHECK_ONE( FP02,  2.0, $1 );
  FP_CHECK_ONE( FP03,  3.0, $1 );
  FP_CHECK_ONE( FP04,  4.0, $1 );
  FP_CHECK_ONE( FP05,  5.0, $1 );
  FP_CHECK_ONE( FP06,  6.0, $1 );
  FP_CHECK_ONE( FP07,  7.0, $1 );
  FP_CHECK_ONE( FP08,  8.0, $1 );
  FP_CHECK_ONE( FP09,  9.0, $1 );
  FP_CHECK_ONE( FP10, 10.0, $1 );
  FP_CHECK_ONE( FP11, 11.0, $1 );
  FP_CHECK_ONE( FP12, 12.0, $1 );
  FP_CHECK_ONE( FP13, 13.0, $1 );
  FP_CHECK_ONE( FP14, 14.0, $1 );
  FP_CHECK_ONE( FP15, 15.0, $1 );
  FP_CHECK_ONE( FP16, 16.0, $1 );
  FP_CHECK_ONE( FP17, 17.0, $1 );
  FP_CHECK_ONE( FP18, 18.0, $1 );
  FP_CHECK_ONE( FP19, 19.0, $1 );
  FP_CHECK_ONE( FP20, 20.0, $1 );
  FP_CHECK_ONE( FP21, 21.0, $1 );
  FP_CHECK_ONE( FP22, 22.0, $1 );
  FP_CHECK_ONE( FP23, 23.0, $1 );
  FP_CHECK_ONE( FP24, 24.0, $1 );
  FP_CHECK_ONE( FP25, 25.0, $1 );
  FP_CHECK_ONE( FP26, 26.0, $1 );
  FP_CHECK_ONE( FP27, 27.0, $1 );
  FP_CHECK_ONE( FP28, 28.0, $1 );
  FP_CHECK_ONE( FP29, 29.0, $1 );
  FP_CHECK_ONE( FP30, 30.0, $1 );
  FP_CHECK_ONE( FP31, 31.0, $1 );
  FP_CHECK_ONE( FP32, 32.0, $1 )
'
)
