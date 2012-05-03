--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 16 of the RTEMS
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
      TEXT_IO.PUT_LINE( "*** TEST 16 ***" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );
      SPTEST.TASK_NAME( 3 ) := RTEMS.BUILD_NAME(  'T', 'A', '3', ' ' );
      SPTEST.TASK_NAME( 4 ) := RTEMS.BUILD_NAME(  'T', 'A', '4', ' ' );
      SPTEST.TASK_NAME( 5 ) := RTEMS.BUILD_NAME(  'T', 'A', '5', ' ' );

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

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 2 ), 
         SPTEST.BASE_PRIORITY, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA2" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 3 ), 
         SPTEST.BASE_PRIORITY, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 3 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA3" );

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

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 3 ),
         SPTEST.TASK_3'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA3" );

      SPTEST.REGION_NAME( 1 ) := RTEMS.BUILD_NAME(  'R', 'N', '1', ' ' );
      SPTEST.REGION_NAME( 2 ) := RTEMS.BUILD_NAME(  'R', 'N', '2', ' ' );
      SPTEST.REGION_NAME( 3 ) := RTEMS.BUILD_NAME(  'R', 'N', '3', ' ' );
      SPTEST.REGION_NAME( 4 ) := RTEMS.BUILD_NAME(  'R', 'N', '4', ' ' );

      RTEMS.REGION.CREATE(
         SPTEST.REGION_NAME( 1 ), 
         SPTEST.AREA_1'ADDRESS,
         4096, 
         128,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.REGION_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_CREATE OF RN1" );

      RTEMS.REGION.CREATE(
         SPTEST.REGION_NAME( 2 ), 
         SPTEST.AREA_2'ADDRESS,
         4096, 
         128,
         RTEMS.PRIORITY,
         SPTEST.REGION_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_CREATE OF RN2" );

      RTEMS.REGION.CREATE(
         SPTEST.REGION_NAME( 3 ), 
         SPTEST.AREA_3'ADDRESS,
         4096, 
         128,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.REGION_ID( 3 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_CREATE OF RN3" );

      RTEMS.REGION.CREATE(
         SPTEST.REGION_NAME( 4 ), 
         SPTEST.AREA_4'ADDRESS,
         4096, 
         128,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.REGION_ID( 4 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_CREATE OF RN4" );

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
--  PUT_ADDRESS_FROM_AREA_2
--

   procedure PUT_ADDRESS_FROM_AREA_2 (
      TO_BE_PRINTED : in     RTEMS.ADDRESS
   ) is
   begin

      UNSIGNED32_IO.PUT(
         RTEMS.SUBTRACT( TO_BE_PRINTED, SPTEST.AREA_2'ADDRESS ),
         WIDTH => 8,
         BASE => 16
      );

   end PUT_ADDRESS_FROM_AREA_2;

-- 
--  PUT_ADDRESS_FROM_AREA_3
--

   procedure PUT_ADDRESS_FROM_AREA_3 (
      TO_BE_PRINTED : in     RTEMS.ADDRESS
   ) is
   begin

      UNSIGNED32_IO.PUT(
         RTEMS.SUBTRACT( TO_BE_PRINTED, SPTEST.AREA_3'ADDRESS ),
         WIDTH => 8,
         BASE => 16
      );

   end PUT_ADDRESS_FROM_AREA_3;

-- 
--  PUT_ADDRESS_FROM_AREA_4
--

   procedure PUT_ADDRESS_FROM_AREA_4 (
      TO_BE_PRINTED : in     RTEMS.ADDRESS
   ) is
   begin

      UNSIGNED32_IO.PUT(
         RTEMS.SUBTRACT( TO_BE_PRINTED, SPTEST.AREA_4'ADDRESS ),
         WIDTH => 8,
         BASE => 16
      );

   end PUT_ADDRESS_FROM_AREA_4;

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
      STATUS            : RTEMS.STATUS_CODES;
   begin

      RTEMS.REGION.IDENT( SPTEST.REGION_NAME( 1 ), RNID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_IDENT OF RN1" );
      TEXT_IO.PUT( "TA1 - region_ident - rnid => " );
      UNSIGNED32_IO.PUT( RNID, WIDTH => 8, BASE => 16 );
      TEXT_IO.NEW_LINE;
       
      TEXT_IO.PUT( "TA1 - region_get_segment - wait on " );
      TEXT_IO.PUT_LINE( "100 byte segment from region 2" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 2 ),
         100,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA1 - got segment from region 2 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_2( SEGMENT_ADDRESS_1 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT( "TA1 - region_get_segment - wait on " );
      TEXT_IO.PUT_LINE( "3K segment from region 3" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 3 ),
         3072,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_2,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA1 - got segment from region 3 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_3( SEGMENT_ADDRESS_2 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT( "TA1 - region_get_segment - get 3080 byte segment " );
      TEXT_IO.PUT_LINE( "from region 1 - NO_WAIT" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         3080,
         RTEMS.NO_WAIT,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_3,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA1 - got segment from region 1 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_3 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - yield processor" );
      RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
   
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

      TEXT_IO.PUT( "TA1 - region_get_segment - wait 10 seconds for 3K " );
      TEXT_IO.PUT_LINE( "segment from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         3072,
         RTEMS.DEFAULT_OPTIONS,
         10 * TEST_SUPPORT.TICKS_PER_SECOND,
         SEGMENT_ADDRESS_4,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA1 - got segment from region 1 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_4 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT( 
         "TA1 - region_return_segment - return segment to region 2 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_2( SEGMENT_ADDRESS_1 );
      TEXT_IO.NEW_LINE;
      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 2 ),
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - yield processor" );
      RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
   
      TEXT_IO.PUT_LINE( "TA1 - task_delete - delete TA3" );
      RTEMS.TASKS.DELETE( SPTEST.TASK_ID( 3 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA3" );

TEST_SUPPORT.PAUSE;

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 4 ), 
         SPTEST.BASE_PRIORITY, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 4 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA4" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 5 ), 
         SPTEST.BASE_PRIORITY, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 5 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA5" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 4 ),
         SPTEST.TASK_4'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA4" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 5 ),
         SPTEST.TASK_5'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA5" );

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - yield processor" );
      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
          
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

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - yield processor" );
      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
   
      TEXT_IO.PUT( "TA1 - region_get_segment - wait 10 seconds for 3K " );
      TEXT_IO.PUT_LINE( "segment from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         3072,
         RTEMS.DEFAULT_OPTIONS,
         10 * TEST_SUPPORT.TICKS_PER_SECOND,
         SEGMENT_ADDRESS_4,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA1 - got segment from region 1 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_4 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - yield processor" );
      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
   
      TEXT_IO.PUT_LINE( "TA1 - task_delete - delete TA4" );
      RTEMS.TASKS.DELETE( SPTEST.TASK_ID( 4 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA4" );

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

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - yield processor" );
      RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
   
      TEXT_IO.PUT_LINE( "TA1 - region_delete - delete region 1" );
      RTEMS.REGION.DELETE( SPTEST.REGION_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_DELETE OF RN1" );
--XXX
      TEXT_IO.PUT_LINE(
         "TA1 - region_get_segment - get 3K segment from region 4"
      );
      RTEMS.REGION.GET_SEGMENT(
        SPTEST.REGION_ID( 4 ),
        3072,
        RTEMS.DEFAULT_OPTIONS,
        RTEMS.NO_TIMEOUT,
        segment_address_1,
        STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "region_get_segment" );
      TEXT_IO.PUT( "TA1 - got segment from region 4 - " );
      PUT_ADDRESS_FROM_AREA_4( segment_address_1 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE(
       "TA1 - region_get_segment - attempt to get 3K segment from region 4"
      );
      RTEMS.REGION.GET_SEGMENT(
        SPTEST.REGION_ID( 4 ),
        3072,
        RTEMS.NO_WAIT,
        RTEMS.NO_TIMEOUT,
        SEGMENT_ADDRESS_2,
        STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
        STATUS,
        RTEMS.UNSATISFIED,
        "task_get_segment with no memory left"
      );
      TEXT_IO.PUT_LINE( "TA1 - task_get_note - UNSATISFIED" );

      TEXT_IO.PUT_LINE( "TA1 - region_extend - extend region 4 by 4K" );
      RTEMS.REGION.EXTEND(
        SPTEST.REGION_ID( 4 ),
        SPTEST.AREA_4( 4096 )'ADDRESS,
        4096,
        STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( status, "region_extend" );

      TEXT_IO.PUT_LINE(
       "TA1 - region_get_segment - attempt to get 3K segment from region 4"
      );
      RTEMS.REGION.GET_SEGMENT(
        SPTEST.REGION_ID( 4 ),
        3072,
        RTEMS.NO_WAIT,
        RTEMS.NO_TIMEOUT,
        segment_address_3,
        STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( status, "region_get_segment" );
      TEXT_IO.PUT( "TA1 - got 3K segment from region 4 - " );
      PUT_ADDRESS_FROM_AREA_4( segment_address_3 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT(
        "TA1 - return_segment - return segment to region 4 - "
      );
      PUT_ADDRESS_FROM_AREA_4( segment_address_1 );
      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 4 ), segment_address_1, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( status, "region_return_segment" );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT(
        "TA1 - region_return_segment - return segment to region 4 - "
      );
      PUT_ADDRESS_FROM_AREA_4( segment_address_3 );
      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 4 ), segment_address_3, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( status, "region_return_segment" );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( "TA1 - region_delete - delete region 4" );
      RTEMS.REGION.DELETE( SPTEST.REGION_ID( 4 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( status, "region_delete" );

      TEXT_IO.PUT_LINE( "*** END OF TEST 16 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TASK_1;

-- 
--  TASK_2
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      PREVIOUS_PRIORITY : RTEMS.TASKS.PRIORITY;
      SEGMENT_ADDRESS_1 : RTEMS.ADDRESS;
      SEGMENT_ADDRESS_2 : RTEMS.ADDRESS;
      STATUS            : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT( "TA2 - region_get_segment - wait on 2K segment " );
      TEXT_IO.PUT_LINE( "from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         2048,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA2 - got segment from region 1 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_1 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT( 
         "TA2 - region_return_segment - return segment to region 1 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_1 );
      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT( "TA2 - task_set_priority - make self " );
      TEXT_IO.PUT_LINE( "highest priority task" );
      RTEMS.TASKS.SET_PRIORITY(
         RTEMS.SELF,
         SPTEST.BASE_PRIORITY - 1,
         PREVIOUS_PRIORITY,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY" );

      TEXT_IO.PUT( 
         "TA2 - region_get_segment - wait on 3950 byte segment "
      );
      TEXT_IO.PUT_LINE( "from region 2" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 2 ),
         3950,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_2,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA2 - got segment from region 2 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_2( SEGMENT_ADDRESS_2 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT( 
         "TA2 - region_return_segment - return segment to region 2 - " 
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_2( SEGMENT_ADDRESS_2 );
      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 2 ),
         SEGMENT_ADDRESS_2,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( "TA2 - task_delete - delete self" );
      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA2" );

   end TASK_2;

-- 
--  TASK_3
--

   procedure TASK_3 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      SEGMENT_ADDRESS_1 : RTEMS.ADDRESS;
      SEGMENT_ADDRESS_2 : RTEMS.ADDRESS;
      STATUS            : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT( "TA3 - region_get_segment - wait on 3950 byte segment " );
      TEXT_IO.PUT_LINE( "from region 2" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 2 ),
         3950,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA3 - got segment from region 2 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_2( SEGMENT_ADDRESS_1 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT( "TA3 - region_get_segment - wait on 2K segment " );
      TEXT_IO.PUT_LINE( "from region 3" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 3 ),
         2048,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_2,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );

   end TASK_3;

-- 
--  TASK_4
--

   procedure TASK_4 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      SEGMENT_ADDRESS_1 : RTEMS.ADDRESS;
      SEGMENT_ADDRESS_2 : RTEMS.ADDRESS;
      STATUS            : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT( "TA4 - region_get_segment - wait on 1.5K segment " );
      TEXT_IO.PUT_LINE( "from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         1536,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );

      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );
      TEXT_IO.PUT( "TA4 - got and returned " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_1 );
      TEXT_IO.NEW_LINE;

      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

      TEXT_IO.PUT( "TA4 - region_get_segment - wait on 3K segment " );
      TEXT_IO.PUT_LINE( "from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         3072,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_2,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );

   end TASK_4;

-- 
--  TASK_5
--

   procedure TASK_5 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      SEGMENT_ADDRESS_1 : RTEMS.ADDRESS;
      SEGMENT_ADDRESS_2 : RTEMS.ADDRESS;
      STATUS            : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT( "TA5 - region_get_segment - wait on 1.5K segment " );
      TEXT_IO.PUT_LINE( "from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         1536,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );

      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );
      TEXT_IO.PUT( "TA5 - got and returned " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_1 );
      TEXT_IO.NEW_LINE;

      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

      TEXT_IO.PUT( "TA5 - region_get_segment - wait on 3K segment " );
      TEXT_IO.PUT_LINE( "from region 1" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         3072,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_2,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT( "TA5 - got segment from region 1 - " );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_2 );
      TEXT_IO.NEW_LINE;

      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         SEGMENT_ADDRESS_2,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );
      TEXT_IO.PUT( 
         "TA5 - region_return_segment - return segment to region 1 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( SEGMENT_ADDRESS_2 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( "TA5 - task_delete - delete self" );
      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA5" );

   end TASK_5;

end SPTEST;
