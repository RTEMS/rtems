--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 21 of the RTEMS
--  Timing Test Suite.
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

with RTEMS_CALLING_OVERHEAD;
with TEST_SUPPORT;
with TEXT_IO;
with TIME_TEST_SUPPORT;
with TIMER_DRIVER;
with RTEMS.MESSAGE_QUEUE;
with RTEMS.PARTITION;
with RTEMS.PORT;
with RTEMS.RATE_MONOTONIC;
with RTEMS.REGION;
with RTEMS.SEMAPHORE;
with RTEMS.TIMER;

package body TMTEST is

-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      ID     : RTEMS.ID;
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 21 ***" );

      RTEMS.TASKS.CREATE( 
         RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
         250,
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE TASK_1" );

      RTEMS.TASKS.START( ID, TMTEST.TASK_1'ACCESS, 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START TASK1" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      ID       : RTEMS.ID;
      OVERHEAD : RTEMS.UNSIGNED32;
      STATUS   : RTEMS.STATUS_CODES;
   begin

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASKS.CREATE( 
            INDEX,
            254,
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

         RTEMS.MESSAGE_QUEUE.CREATE( 
            INDEX,
            TIME_TEST_SUPPORT.OPERATION_COUNT,
            16,
            RTEMS.DEFAULT_ATTRIBUTES,
            ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_CREATE" );

         RTEMS.SEMAPHORE.CREATE( 
            INDEX,
            TIME_TEST_SUPPORT.OPERATION_COUNT,
            RTEMS.DEFAULT_ATTRIBUTES,
            RTEMS.TASKS.NO_PRIORITY,
            ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE" );

         RTEMS.REGION.CREATE( 
            INDEX,
            TMTEST.REGION_AREA'ADDRESS,
            2048,
            16,
            RTEMS.DEFAULT_ATTRIBUTES,
            ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_CREATE" );

         RTEMS.PARTITION.CREATE( 
            INDEX,
            TMTEST.PARTITION_AREA'ADDRESS,
            2048,
            128,
            RTEMS.DEFAULT_ATTRIBUTES,
            ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_CREATE" );

         RTEMS.PORT.CREATE( 
            INDEX,
            TMTEST.INTERNAL_PORT_AREA'ADDRESS,
            TMTEST.EXTERNAL_PORT_AREA'ADDRESS,
            16#FF#,
            ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PORT_CREATE" );

         RTEMS.TIMER.CREATE( INDEX, ID, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE" );

         RTEMS.RATE_MONOTONIC.CREATE( INDEX, ID, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "RATE_MONOTONIC_CREATE" );

      end loop;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            TIMER_DRIVER.EMPTY_FUNCTION;
         end loop;
      OVERHEAD := TIMER_DRIVER.READ_TIMER;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASKS.IDENT( INDEX, RTEMS.SEARCH_ALL_NODES, ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_IDENT 
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.MESSAGE_QUEUE.IDENT( 
               INDEX, 
               RTEMS.SEARCH_ALL_NODES, 
               ID, 
               STATUS 
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "MESSAGE_QUEUE_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.MESSAGE_QUEUE_IDENT
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.SEMAPHORE.IDENT( 
               INDEX, 
               RTEMS.SEARCH_ALL_NODES, 
               ID, 
               STATUS 
           );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.SEMAPHORE_IDENT
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.PARTITION.IDENT( 
               INDEX, 
               RTEMS.SEARCH_ALL_NODES, 
               ID, 
               STATUS 
           );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PARTITION_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.PARTITION_IDENT
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.REGION.IDENT( INDEX, ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "REGION_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.REGION_IDENT
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.PORT.IDENT( INDEX, ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PORT_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.PORT_IDENT
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TIMER.IDENT( INDEX, ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TIMER_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TIMER_IDENT
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.RATE_MONOTONIC.IDENT( INDEX, ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "RATE_MONOTONIC_IDENT",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.RATE_MONOTONIC_IDENT
      );

      TEXT_IO.PUT_LINE( "*** END OF TIME TEST 21 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TASK_1;

end TMTEST;
