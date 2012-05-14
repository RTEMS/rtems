--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 13 of the RTEMS
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
with TIME_TEST_SUPPORT;
with TIMER_DRIVER;
with RTEMS.MESSAGE_QUEUE;

package body TMTEST is

-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      TASK_ID : RTEMS.ID;
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 13 ***" );

      RTEMS.TASKS.CREATE( 
         1,
         251, 
         1024, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TASK_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

      RTEMS.TASKS.START( 
         TASK_ID, 
         TMTEST.TEST_INIT'ACCESS, 
         0, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  TEST_INIT
--

   procedure TEST_INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      TASK_ENTRY     : RTEMS.TASKS.ENTRY_POINT;
      PRIORITY       : RTEMS.TASKS.PRIORITY;
      TASK_ID        : RTEMS.ID;
      BUFFER         : TMTEST.BUFFER;
      BUFFER_POINTER : RTEMS.ADDRESS;
      STATUS         : RTEMS.STATUS_CODES;
   begin

      BUFFER_POINTER := BUFFER'ADDRESS;

-- As each task is started, it preempts this task and performs a blocking
-- MESSAGE_QUEUE_RECEIVE.  Upon completion of this loop all created tasks
-- are blocked.

      RTEMS.MESSAGE_QUEUE.CREATE(
         RTEMS.BUILD_NAME( 'M', 'Q', '1', ' ' ),
         TIME_TEST_SUPPORT.OPERATION_COUNT,
         16,
         RTEMS.DEFAULT_OPTIONS,
         TMTEST.QUEUE_ID,
         STATUS
      );

      PRIORITY := 250;

      for INDEX in 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT - 1
      loop

         RTEMS.TASKS.CREATE( 
            RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
            PRIORITY, 
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            TASK_ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         PRIORITY := PRIORITY - 1;

         if INDEX = TIME_TEST_SUPPORT.OPERATION_COUNT - 1 then
            TASK_ENTRY := TMTEST.HIGH_TASK'ACCESS;
         else
            TASK_ENTRY := TMTEST.MIDDLE_TASKS'ACCESS;
         end if;

         RTEMS.TASKS.START( TASK_ID, TASK_ENTRY, 0, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;

      TIMER_DRIVER.INITIALIZE;
      RTEMS.MESSAGE_QUEUE.URGENT( 
         TMTEST.QUEUE_ID,
         BUFFER_POINTER,
         16,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_URGENT" );

   end TEST_INIT;

-- 
--  HIGH_TASK
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      BUFFER         : TMTEST.BUFFER;
      BUFFER_POINTER : RTEMS.ADDRESS;
      MESSAGE_SIZE   : RTEMS.UNSIGNED32 := 0;
      STATUS         : RTEMS.STATUS_CODES;
   begin

      BUFFER_POINTER := BUFFER'ADDRESS;

      RTEMS.MESSAGE_QUEUE.RECEIVE( 
         TMTEST.QUEUE_ID,
         BUFFER_POINTER,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         MESSAGE_SIZE,
         STATUS
      );

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME(
         "MESSAGE_QUEUE_URGENT (preemptive)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.MESSAGE_QUEUE_URGENT 
      );

      TEXT_IO.PUT_LINE( "*** END OF TIME TEST 13 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end HIGH_TASK;

-- 
--  MIDDLE_TASKS
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      BUFFER         : TMTEST.BUFFER;
      BUFFER_POINTER : RTEMS.ADDRESS;
      MESSAGE_SIZE   : RTEMS.UNSIGNED32 := 0;
      STATUS         : RTEMS.STATUS_CODES;
   begin
 
      BUFFER_POINTER := BUFFER'ADDRESS;

      RTEMS.MESSAGE_QUEUE.RECEIVE( 
         TMTEST.QUEUE_ID,
         BUFFER_POINTER,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         MESSAGE_SIZE,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_RECEIVE" );
 
      RTEMS.MESSAGE_QUEUE.URGENT( 
         TMTEST.QUEUE_ID,
         BUFFER_POINTER,
         16,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_URGENT" );

   end MIDDLE_TASKS;

end TMTEST;
