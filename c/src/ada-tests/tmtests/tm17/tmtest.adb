--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 17 of the RTEMS
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

with INTERFACES; use INTERFACES;
with RTEMS_CALLING_OVERHEAD;
with TEST_SUPPORT;
with TEXT_IO;
with TIMER_DRIVER;

package body TMTEST is

-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      TASK_ENTRY : RTEMS.TASKS.ENTRY_POINT;
      STATUS     : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 17 ***" );

      TMTEST.TASK_PRIORITY := 254;

      for INDEX in 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASKS.CREATE( 
            RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
            TMTEST.TASK_PRIORITY, 
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.TASK_ID( INDEX ), 
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         if INDEX = TIME_TEST_SUPPORT.OPERATION_COUNT then
            TASK_ENTRY := TMTEST.LAST_TASK'ACCESS;
         elsif INDEX = 0 then
            TASK_ENTRY := TMTEST.FIRST_TASK'ACCESS;
         else
            TASK_ENTRY := TMTEST.MIDDLE_TASKS'ACCESS;
         end if;

         RTEMS.TASKS.START( TMTEST.TASK_ID( INDEX ), TASK_ENTRY, 0, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;

      TMTEST.TASK_COUNT := 1;

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  FIRST_TASK
--

   procedure FIRST_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      PREVIOUS_PRIORITY : RTEMS.TASKS.PRIORITY;
      STATUS            : RTEMS.STATUS_CODES;
   begin

      TIMER_DRIVER.INITIALIZE;

      TMTEST.TASK_PRIORITY := TMTEST.TASK_PRIORITY - 1;
      TMTEST.TASK_COUNT    := TMTEST.TASK_COUNT + 1;

      RTEMS.TASKS.SET_PRIORITY(
         TMTEST.TASK_ID( TMTEST.TASK_COUNT ), 
         TMTEST.TASK_PRIORITY,
         PREVIOUS_PRIORITY,
         STATUS 
      );

   end FIRST_TASK;

-- 
--  MIDDLE_TASKS
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      PREVIOUS_PRIORITY : RTEMS.TASKS.PRIORITY;
      STATUS            : RTEMS.STATUS_CODES;
   begin

      TMTEST.TASK_PRIORITY := TMTEST.TASK_PRIORITY - 1;
      TMTEST.TASK_COUNT    := TMTEST.TASK_COUNT + 1;

      RTEMS.TASKS.SET_PRIORITY(
         TMTEST.TASK_ID( TMTEST.TASK_COUNT ), 
         TMTEST.TASK_PRIORITY,
         PREVIOUS_PRIORITY,
         STATUS 
      );

   end MIDDLE_TASKS;

-- 
--  LAST_TASK
--

   procedure LAST_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
   begin

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_SET_PRIORITY (preemptive)", 
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TASK_SET_PRIORITY 
      );

      TEXT_IO.PUT_LINE( "*** END OF TIME TEST 17 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end LAST_TASK;

end TMTEST;
