--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 20 of the RTEMS
--  Timing Test Suite.
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
with RTEMS_CALLING_OVERHEAD;
with TEST_SUPPORT;
with TEXT_IO;
with TIME_TEST_SUPPORT;
with UNSIGNED32_IO;

package body TMTEST is

--PAGE
-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS     : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 20 ***" );

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'T', 'I', 'M', '1' ),
         128,
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TMTEST.TASK_ID( 1 ), 
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TASK1" );

      RTEMS.TASK_START( 
         TMTEST.TASK_ID( 1 ), 
         TMTEST.TASK_1'ACCESS, 
         0, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TASK1" );

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'T', 'I', 'M', '2' ),
         129,
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TMTEST.TASK_ID( 2 ), 
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TASK2" );

      RTEMS.TASK_START( 
         TMTEST.TASK_ID( 2 ), 
         TMTEST.TASK_2'ACCESS, 
         0, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TASK2" );

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
      BUFFER_COUNT      : RTEMS.UNSIGNED32;
      INDEX             : RTEMS.UNSIGNED32;
      PREVIOUS_MODE     : RTEMS.MODE;
      PREVIOUS_PRIORITY : RTEMS.TASK_PRIORITY;
      OVERHEAD          : RTEMS.UNSIGNED32;
      STATUS            : RTEMS.STATUS_CODES;
   begin

      TMTEST.PARTITION_NAME := RTEMS.BUILD_NAME( 'P', 'A', 'R', 'T' );

      TIMER_DRIVER.INITIALIZE;

         RTEMS.PARTITION_CREATE(
            TMTEST.PARTITION_NAME,
            TMTEST.PARTITION_AREA'ADDRESS,
            2048,
            128,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.PARTITION_ID,
            STATUS 
         );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PARTITION_CREATE",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.PARTITION_CREATE 
      );

      TMTEST.REGION_NAME := RTEMS.BUILD_NAME( 'R', 'E', 'G', 'N' );

      TIMER_DRIVER.INITIALIZE;

         RTEMS.REGION_CREATE(
            TMTEST.REGION_NAME,
            TMTEST.REGION_AREA'ADDRESS,
            2048,
            16,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.REGION_ID,
            STATUS 
         );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "REGION_CREATE",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.REGION_CREATE 
      );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.PARTITION_GET_BUFFER(
            TMTEST.PARTITION_ID,
            TMTEST.BUFFER_ADDRESS_1,
            STATUS 
         );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PARTITION_GET_BUFFER (available)",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.PARTITION_GET_BUFFER 
      );

      BUFFER_COUNT := 0;
      loop 

         RTEMS.PARTITION_GET_BUFFER(
            TMTEST.PARTITION_ID,
            TMTEST.BUFFER_ADDRESSES( BUFFER_COUNT ),
            STATUS 
         );

         exit when RTEMS.ARE_STATUSES_EQUAL( STATUS, RTEMS.UNSATISFIED );

         BUFFER_COUNT := BUFFER_COUNT + 1;

      end loop;

      TIMER_DRIVER.INITIALIZE;
         RTEMS.PARTITION_GET_BUFFER(
            TMTEST.PARTITION_ID,
            TMTEST.BUFFER_ADDRESS_2,
            STATUS 
         );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PARTITION_GET_BUFFER (not available)",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.PARTITION_GET_BUFFER 
      );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.PARTITION_RETURN_BUFFER(
            TMTEST.PARTITION_ID,
            TMTEST.BUFFER_ADDRESS_1,
            STATUS 
         );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PARTITION_RETURN_BUFFER",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.PARTITION_RETURN_BUFFER 
      );

      for INDEX in 0 .. BUFFER_COUNT - 1
      loop

         RTEMS.PARTITION_RETURN_BUFFER(
            TMTEST.PARTITION_ID,
            TMTEST.BUFFER_ADDRESSES( INDEX ),
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_RETURN_BUFFER" );

      end loop;

      TIMER_DRIVER.INITIALIZE;
         RTEMS.PARTITION_DELETE( TMTEST.PARTITION_ID, STATUS );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PARTITION_DELETE",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.PARTITION_DELETE 
      );

      RTEMS.REGION_GET_SEGMENT(
         TMTEST.REGION_ID,
         400,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         TMTEST.BUFFER_ADDRESS_2,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.REGION_GET_SEGMENT(
            TMTEST.REGION_ID,
            400,
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.NO_TIMEOUT,
            TMTEST.BUFFER_ADDRESS_3,
            STATUS 
         );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "REGION_GET_SEGMENT (available)",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.REGION_GET_SEGMENT 
      );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.REGION_GET_SEGMENT(
            TMTEST.REGION_ID,
            1998,
            RTEMS.NO_WAIT,
            RTEMS.NO_TIMEOUT,
            TMTEST.BUFFER_ADDRESS_4,
            STATUS 
         );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "REGION_GET_SEGMENT (NO_WAIT)",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.REGION_GET_SEGMENT 
      );

      RTEMS.REGION_RETURN_SEGMENT(
         TMTEST.REGION_ID,
         TMTEST.BUFFER_ADDRESS_3,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.REGION_RETURN_SEGMENT(
            TMTEST.REGION_ID,
            TMTEST.BUFFER_ADDRESS_2,
            STATUS 
         );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "REGION_RETURN_SEGMENT (no tasks waiting)",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.REGION_RETURN_SEGMENT 
      );

      RTEMS.REGION_GET_SEGMENT(
         TMTEST.REGION_ID,
         400,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         TMTEST.BUFFER_ADDRESS_1,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.REGION_GET_SEGMENT(
            TMTEST.REGION_ID,
            1998,
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.NO_TIMEOUT,
            TMTEST.BUFFER_ADDRESS_2,
            STATUS 
         );

      -- execute TASK_2

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "REGION_RETURN_SEGMENT (preempt)",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.REGION_RETURN_SEGMENT 
      );

      RTEMS.REGION_RETURN_SEGMENT(
         TMTEST.REGION_ID,
         TMTEST.BUFFER_ADDRESS_2,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_RETURN_SEGMENT" );

      RTEMS.TASK_MODE(
         RTEMS.NO_PREEMPT,
         RTEMS.PREEMPT_MASK,
         PREVIOUS_MODE,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );

      RTEMS.TASK_SET_PRIORITY( RTEMS.SELF, 254, PREVIOUS_PRIORITY, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY" );

      RTEMS.REGION_GET_SEGMENT(
         TMTEST.REGION_ID,
         400,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         TMTEST.BUFFER_ADDRESS_1,
         STATUS 
      );

      RTEMS.REGION_GET_SEGMENT(
         TMTEST.REGION_ID,
         1998,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         TMTEST.BUFFER_ADDRESS_2,
         STATUS 
      );
 
      -- execute TASK_2

      RTEMS.REGION_RETURN_SEGMENT(
         TMTEST.REGION_ID,
         TMTEST.BUFFER_ADDRESS_2,
         STATUS 
      );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.REGION_DELETE( TMTEST.REGION_ID, STATUS );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "REGION_DELETE",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.REGION_DELETE 
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            TIMER_DRIVER.EMPTY_FUNCTION;
         end loop;
      OVERHEAD := TIMER_DRIVER.READ_TIMER;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.IO_INITIALIZE(
               1, 
               0,
               RTEMS.NULL_ADDRESS,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "IO_INITIALIZE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT,
         0,
         RTEMS_CALLING_OVERHEAD.IO_INITIALIZE 
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.IO_OPEN(
               1, 
               0,
               RTEMS.NULL_ADDRESS,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "IO_OPEN",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT,
         0,
         RTEMS_CALLING_OVERHEAD.IO_OPEN 
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.IO_CLOSE(
               1, 
               0,
               RTEMS.NULL_ADDRESS,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "IO_CLOSE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT,
         0,
         RTEMS_CALLING_OVERHEAD.IO_CLOSE 
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.IO_READ(
               1, 
               0,
               RTEMS.NULL_ADDRESS,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "IO_READ",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT,
         0,
         RTEMS_CALLING_OVERHEAD.IO_READ 
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.IO_WRITE(
               1, 
               0,
               RTEMS.NULL_ADDRESS,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "IO_WRITE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT,
         0,
         RTEMS_CALLING_OVERHEAD.IO_WRITE 
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.IO_CONTROL(
               1, 
               0,
               RTEMS.NULL_ADDRESS,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "IO_CONTROL",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT,
         0,
         RTEMS_CALLING_OVERHEAD.IO_CONTROL 
      );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TASK_1;

--PAGE
-- 
--  TASK_2
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS            : RTEMS.STATUS_CODES;
   begin

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "REGION_GET_SEGMENT (blocking)",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.REGION_GET_SEGMENT 
      );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.REGION_RETURN_SEGMENT(
            TMTEST.REGION_ID,
            TMTEST.BUFFER_ADDRESS_1,
            STATUS 
         );

      -- preempt back to TASK_1

      TIMER_DRIVER.INITIALIZE;
         RTEMS.REGION_RETURN_SEGMENT(
            TMTEST.REGION_ID,
            TMTEST.BUFFER_ADDRESS_1,
            STATUS 
         );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "REGION_RETURN_SEGMENT (ready -- return)",
         TMTEST.END_TIME, 
         1,
         0,
         RTEMS_CALLING_OVERHEAD.REGION_RETURN_SEGMENT 
      );

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end TASK_2;

end TMTEST;
