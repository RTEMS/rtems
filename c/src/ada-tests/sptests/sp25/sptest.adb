--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 25 of the RTEMS
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

with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;
with RTEMS.REGION;

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
      TEXT_IO.PUT_LINE( "*** TEST 25 ***" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         SPTEST.BASE_PRIORITY, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 1 ),
         SPTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      SPTEST.REGION_NAME( 1 ) := RTEMS.BUILD_NAME(  'R', 'N', '1', ' ' );

      RTEMS.REGION.CREATE(
         SPTEST.REGION_NAME( 1 ), 
         SPTEST.AREA_1'ADDRESS,
         64000, 
         32,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.REGION_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_CREATE OF RN1" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  PUT_ADDRESS_FROM_AREA_1
--

   procedure PUT_ADDRESS_FROM_AREA_1 (
      TO_BE_PRINTED : in     RTEMS.ADDRESS
   ) is

   begin

      UNSIGNED32_IO.PUT(
         RTEMS.SUBTRACT( TO_BE_PRINTED, SPTEST.AREA_1'ADDRESS ),
         WIDTH => 8,
         BASE => 16
      );

   end PUT_ADDRESS_FROM_AREA_1;

-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      RNID              : RTEMS.ID;
      SEGMENT_ADDRESS_1 : RTEMS.ADDRESS;
      SEGMENT_ADDRESS_2 : RTEMS.ADDRESS;
      SEGMENT_ADDRESS_3 : RTEMS.ADDRESS;
      SEGMENT_ADDRESS_4 : RTEMS.ADDRESS;
      SEGMENT_ADDRESS_5 : RTEMS.ADDRESS;
      SEGMENT_ADDRESS_6 : RTEMS.ADDRESS;
      SEGMENT_ADDRESS_7 : RTEMS.ADDRESS;
      SEGMENT_ADDRESS_8 : RTEMS.ADDRESS;
      STATUS            : RTEMS.STATUS_CODES;
   begin

      RTEMS.REGION.IDENT( SPTEST.REGION_NAME( 1 ), RNID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_IDENT OF RN1" );
      TEXT_IO.PUT( "TA1 - region_ident - rnid => " );
      UNSIGNED32_IO.PUT( RNID, WIDTH => 8, BASE => 16 );
      TEXT_IO.NEW_LINE;
       
      TEXT_IO.PUT( "TA1 - region_get_segment - wait on " );
      TEXT_IO.PUT_LINE( "64 byte segment from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         64,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA1 - got segment from region 1 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_1 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT( "TA1 - region_get_segment - wait on " );
      TEXT_IO.PUT_LINE( "128 byte segment from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         128,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_2,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA1 - got segment from region 1 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_2 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT( "TA1 - region_get_segment - wait on " );
      TEXT_IO.PUT_LINE( "256 byte segment from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         256,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_3,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA1 - got segment from region 1 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_3 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT( "TA1 - region_get_segment - wait on " );
      TEXT_IO.PUT_LINE( "512 byte segment from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         512,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_4,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA1 - got segment from region 1 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_4 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT( "TA1 - region_get_segment - wait on " );
      TEXT_IO.PUT_LINE( "1024 byte segment from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         1024,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_5,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA1 - got segment from region 1 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_5 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT( "TA1 - region_get_segment - wait on " );
      TEXT_IO.PUT_LINE( "2048 byte segment from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         2048,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_6,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA1 - got segment from region 1 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_6 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT( "TA1 - region_get_segment - wait on " );
      TEXT_IO.PUT_LINE( "4096 byte segment from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         4096,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_7,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA1 - got segment from region 1 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_7 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT( "TA1 - region_get_segment - wait on " );
      TEXT_IO.PUT_LINE( "8192 byte segment from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         8192,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_8,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA1 - got segment from region 1 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_8 );
      TEXT_IO.NEW_LINE;

TEST_SUPPORT.PAUSE;

      TEXT_IO.PUT( 
         "TA1 - region_return_segment - return segment to region 1 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_3 );
      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         SEGMENT_ADDRESS_3,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );
      TEXT_IO.NEW_LINE;
      -- SEGMENT_ADDRESS_3 := RTEMS.NULL_ADDRESS;

      TEXT_IO.PUT( 
         "TA1 - region_return_segment - return segment to region 1 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_4 );
      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         SEGMENT_ADDRESS_4,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );
      TEXT_IO.NEW_LINE;
      -- SEGMENT_ADDRESS_4 := RTEMS.NULL_ADDRESS;

      TEXT_IO.PUT( 
         "TA1 - region_return_segment - return segment to region 1 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_1 );
      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );
      TEXT_IO.NEW_LINE;
      -- SEGMENT_ADDRESS_1 := RTEMS.NULL_ADDRESS;

      TEXT_IO.PUT( 
         "TA1 - region_return_segment - return segment to region 1 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_2 );
      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         SEGMENT_ADDRESS_2,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );
      TEXT_IO.NEW_LINE;
      -- SEGMENT_ADDRESS_2 := RTEMS.NULL_ADDRESS;

      TEXT_IO.PUT( 
         "TA1 - region_return_segment - return segment to region 1 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_7 );
      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         SEGMENT_ADDRESS_7,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );
      TEXT_IO.NEW_LINE;
      -- SEGMENT_ADDRESS_7 := RTEMS.NULL_ADDRESS;

      TEXT_IO.PUT( 
         "TA1 - region_return_segment - return segment to region 1 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_6 );
      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         SEGMENT_ADDRESS_6,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );
      TEXT_IO.NEW_LINE;
      -- SEGMENT_ADDRESS_6 := RTEMS.NULL_ADDRESS;

      TEXT_IO.PUT( 
         "TA1 - region_return_segment - return segment to region 1 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_8 );
      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         SEGMENT_ADDRESS_8,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );
      TEXT_IO.NEW_LINE;
      -- SEGMENT_ADDRESS_8 := RTEMS.NULL_ADDRESS;

      TEXT_IO.PUT( 
         "TA1 - region_return_segment - return segment to region 1 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_5 );
      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         SEGMENT_ADDRESS_5,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );
      TEXT_IO.NEW_LINE;
      -- SEGMENT_ADDRESS_5 := RTEMS.NULL_ADDRESS;

      TEXT_IO.PUT_LINE(
         "TA1 - region_delete - walks heap if debug enabled"
      );
      RTEMS.REGION.DELETE( SPTEST.REGION_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_DELETE OF RN1" );

      TEXT_IO.PUT_LINE( "*** END OF TEST 25 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TASK_1;

end SPTEST;
