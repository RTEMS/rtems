--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 28 of the RTEMS
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
--  http://www.OARcorp.com/rtems/license.html.
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
      TASK_ID : RTEMS.ID;
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 28 ***" );

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'T', 'E', 'S', 'T' ),
         128, 
         1024, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TMTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

      RTEMS.TASK_START( 
         TMTEST.TASK_ID( 1 ),
         TMTEST.TEST_TASK'ACCESS, 
         0, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START" );

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  TEST_TASK
--

   procedure TEST_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      NAME      : RTEMS.NAME;
      OVERHEAD  : RTEMS.UNSIGNED32;
      CONVERTED : RTEMS.ADDRESS;
      STATUS    : RTEMS.STATUS_CODES;
   begin

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            TIMER_DRIVER.EMPTY_FUNCTION;
         end loop;
      OVERHEAD := TIMER_DRIVER.READ_TIMER;

      NAME := RTEMS.BUILD_NAME( 'P', 'O', 'R', 'T' );
      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.PORT_CREATE(
               NAME,
               TMTEST.INTERNAL_PORT_AREA'ADDRESS,
               TMTEST.EXTERNAL_PORT_AREA'ADDRESS,
               16#FF#,
               TMTEST.PORT_ID( INDEX ),
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PORT_CREATE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.PORT_CREATE
      );
 
      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.PORT_EXTERNAL_TO_INTERNAL(
               TMTEST.PORT_ID( 1 ),
               TMTEST.EXTERNAL_PORT_AREA( 16#F# )'ADDRESS,
               CONVERTED,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PORT_EXTERNAL_TO_INTERNAL",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.PORT_EXTERNAL_TO_INTERNAL
      );
 
      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.PORT_INTERNAL_TO_EXTERNAL(
               TMTEST.PORT_ID( 1 ),
               TMTEST.INTERNAL_PORT_AREA( 16#F# )'ADDRESS,
               CONVERTED,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PORT_INTERNAL_TO_EXTERNAL",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.PORT_INTERNAL_TO_EXTERNAL
      );
 
      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.PORT_DELETE( TMTEST.PORT_ID( INDEX ), STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "PORT_DELETE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.PORT_DELETE
      );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );
 
   end TEST_TASK;

end TMTEST;
