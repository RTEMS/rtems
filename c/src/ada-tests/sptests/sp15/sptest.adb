--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 15 of the RTEMS
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
with RTEMS.PARTITION;

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
      TEXT_IO.PUT_LINE( "*** TEST 15 ***" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );

      SPTEST.PARTITION_NAME( 1 ) := RTEMS.BUILD_NAME(  'P', 'T', '1', ' ' );
      SPTEST.PARTITION_NAME( 2 ) := RTEMS.BUILD_NAME(  'P', 'T', '2', ' ' );

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

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 1 ),
         SPTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      TEXT_IO.PUT_LINE( "INIT - partition_create - partition 1" );
      RTEMS.PARTITION.CREATE(
         SPTEST.PARTITION_NAME( 1 ),
         AREA_1'ADDRESS,
         4096,
         512,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.PARTITION_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_CREATE OF PT1" );

      TEXT_IO.PUT_LINE( "INIT - partition_create - partition 2" );
      RTEMS.PARTITION.CREATE(
         SPTEST.PARTITION_NAME( 2 ),
         AREA_2'ADDRESS,
         274,
         128,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.PARTITION_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_CREATE OF PT2" );

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
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      PTID_1           : RTEMS.ID;
      PTID_2           : RTEMS.ID;
      BUFFER_ADDRESS_1 : RTEMS.ADDRESS;
      BUFFER_ADDRESS_2 : RTEMS.ADDRESS;
      BUFFER_ADDRESS_3 : RTEMS.ADDRESS;
      BUFFER_ADDRESS_4 : RTEMS.ADDRESS;
      STATUS           : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT( "TA1 - partition_ident - partition 1 id = " );
      RTEMS.PARTITION.IDENT( 
         SPTEST.PARTITION_NAME( 1 ), 
         RTEMS.SEARCH_ALL_NODES, 
         PTID_1, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_IDENT OF PT1" );
      UNSIGNED32_IO.PUT( PTID_1, WIDTH => 8, BASE => 16 );
      TEXT_IO.NEW_LINE;
   
      TEXT_IO.PUT( "TA1 - partition_ident - partition 2 id = " );
      RTEMS.PARTITION.IDENT( 
         SPTEST.PARTITION_NAME( 2 ), 
         RTEMS.SEARCH_ALL_NODES, 
         PTID_2, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_IDENT OF PT2" );
      UNSIGNED32_IO.PUT( PTID_2, WIDTH => 8, BASE => 16 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT(
         "TA1 - partition_get_buffer - get buffer 1 from partition 1 - "
      );
      RTEMS.PARTITION.GET_BUFFER( 
         SPTEST.PARTITION_ID( 1 ), 
         BUFFER_ADDRESS_1,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_GET_BUFFER" );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( BUFFER_ADDRESS_1 );
      TEXT_IO.NEW_LINE;
      
      TEXT_IO.PUT(
         "TA1 - partition_get_buffer - get buffer 2 from partition 1 - "
      );
      RTEMS.PARTITION.GET_BUFFER( 
         SPTEST.PARTITION_ID( 1 ), 
         BUFFER_ADDRESS_2,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_GET_BUFFER" );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( BUFFER_ADDRESS_2 );
      TEXT_IO.NEW_LINE;
      
      TEXT_IO.PUT(
         "TA1 - partition_get_buffer - get buffer 1 from partition 2 - "
      );
      RTEMS.PARTITION.GET_BUFFER( 
         SPTEST.PARTITION_ID( 2 ), 
         BUFFER_ADDRESS_3,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_GET_BUFFER" );
      SPTEST.PUT_ADDRESS_FROM_AREA_2( BUFFER_ADDRESS_3 );
      TEXT_IO.NEW_LINE;
      
      TEXT_IO.PUT(
         "TA1 - partition_get_buffer - get buffer 2 from partition 2 - "
      );
      RTEMS.PARTITION.GET_BUFFER( 
         SPTEST.PARTITION_ID( 2 ), 
         BUFFER_ADDRESS_4,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_GET_BUFFER" );
      SPTEST.PUT_ADDRESS_FROM_AREA_2( BUFFER_ADDRESS_4 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT(
         "TA1 - partition_return_buffer - return buffer 1 to partition 1 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( BUFFER_ADDRESS_1 );
      TEXT_IO.NEW_LINE;
      RTEMS.PARTITION.RETURN_BUFFER( 
         SPTEST.PARTITION_ID( 1 ), 
         BUFFER_ADDRESS_1,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_RETURN_BUFFER" );
 
      TEXT_IO.PUT(
         "TA1 - partition_return_buffer - return buffer 2 to partition 1 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_1( BUFFER_ADDRESS_2 );
      TEXT_IO.NEW_LINE;
      RTEMS.PARTITION.RETURN_BUFFER( 
         SPTEST.PARTITION_ID( 1 ), 
         BUFFER_ADDRESS_2,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_RETURN_BUFFER" );
 
      TEXT_IO.PUT(
         "TA1 - partition_return_buffer - return buffer 1 to partition 2 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_2( BUFFER_ADDRESS_3 );
      TEXT_IO.NEW_LINE;
      RTEMS.PARTITION.RETURN_BUFFER( 
         SPTEST.PARTITION_ID( 2 ), 
         BUFFER_ADDRESS_3,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_RETURN_BUFFER" );
 
      TEXT_IO.PUT(
         "TA1 - partition_return_buffer - return buffer 2 to partition 2 - "
      );
      SPTEST.PUT_ADDRESS_FROM_AREA_2( BUFFER_ADDRESS_4 );
      TEXT_IO.NEW_LINE;
      RTEMS.PARTITION.RETURN_BUFFER( 
         SPTEST.PARTITION_ID( 2 ), 
         BUFFER_ADDRESS_4,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_RETURN_BUFFER" );
 
      TEXT_IO.PUT_LINE( "TA1 - partition_delete - delete partition 1" );
      RTEMS.PARTITION.DELETE( SPTEST.PARTITION_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_DELETE OF SELF" );

      TEXT_IO.PUT_LINE( "TA1 - partition_delete - delete partition 2" );
      RTEMS.PARTITION.DELETE( SPTEST.PARTITION_ID( 2 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_DELETE OF SELF" );

      TEXT_IO.PUT_LINE( "*** END OF TEST 15 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );
   
   end TASK_1;

end SPTEST;
