--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 16 of the RTEMS
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
      ID     : RTEMS.ID;
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 16 ***" );

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'T', 'E', 'S', 'T' ),
         251, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TEST INIT" );

      RTEMS.TASK_START( 
         ID, 
         TMTEST.TEST_INIT'ACCESS, 
         0, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TEST INIT" );

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  TEST_INIT
--

   procedure TEST_INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      PRIORITY   : RTEMS.TASK_PRIORITY;
      INDEX      : RTEMS.UNSIGNED32;
      TASK_ENTRY : RTEMS.TASK_ENTRY;
      STATUS     : RTEMS.STATUS_CODES;
   begin

      PRIORITY := 250;

      for INDEX in 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASK_CREATE( 
            RTEMS.BUILD_NAME( 'M', 'I', 'D', ' ' ),
            PRIORITY, 
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.TASK_ID( INDEX ), 
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         if INDEX = TIME_TEST_SUPPORT.OPERATION_COUNT then
            TASK_ENTRY := TMTEST.HIGH_TASK'ACCESS;
         else
            TASK_ENTRY := TMTEST.MIDDLE_TASKS'ACCESS;
         end if;

         RTEMS.TASK_START( 
            TMTEST.TASK_ID( INDEX ), 
            TASK_ENTRY, 
            0, 
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

         PRIORITY := PRIORITY - 1;

      end loop;

      TMTEST.TASK_COUNT := 0;

      TIMER_DRIVER.INITIALIZE;                  -- starts the timer

      RTEMS.EVENT_SEND(                         -- preempts task
         TMTEST.TASK_ID( TMTEST.TASK_COUNT ), 
         RTEMS.EVENT_16, 
         STATUS 
      );
      
   end TEST_INIT;

--PAGE
-- 
--  MIDDLE_TASKS
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      EVENT_OUT : RTEMS.EVENT_SET;
      STATUS    : RTEMS.STATUS_CODES;
   begin

      RTEMS.EVENT_RECEIVE(                      -- task blocks
         RTEMS.EVENT_16, 
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         EVENT_OUT,
         STATUS
      );

      TMTEST.TASK_COUNT := TMTEST.TASK_COUNT + 1;

      RTEMS.EVENT_SEND(                         -- preempts task
         TMTEST.TASK_ID( TMTEST.TASK_COUNT ), 
         RTEMS.EVENT_16, 
         STATUS 
      );
      
   end MIDDLE_TASKS;

--PAGE
-- 
--  HIGH_TASK
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      EVENT_OUT : RTEMS.EVENT_SET;
      STATUS    : RTEMS.STATUS_CODES;
   begin

      RTEMS.EVENT_RECEIVE(                      -- task blocks
         RTEMS.EVENT_16, 
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         EVENT_OUT,
         STATUS
      );

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "EVENT_SEND (preemptive)", 
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.EVENT_SEND 
      );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end HIGH_TASK;

end TMTEST;
