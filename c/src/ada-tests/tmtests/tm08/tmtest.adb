--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 8 of the RTEMS
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
      TASK_ID : RTEMS.ID;
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 8 ***" );

      TMTEST.TEST_INIT;

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  TEST_INIT
--

   procedure TEST_INIT
   is
      STATUS     : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASK_CREATE( 
         1,
         128, 
         1024, 
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.DEFAULT_ATTRIBUTES,
         TASK_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

      RTEMS.TASK_START( TASK_ID, TMTEST.TEST_TASK'ACCESS, 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START" );

      RTEMS.TASK_CREATE( 
         1,
         254, 
         1024, 
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.DEFAULT_ATTRIBUTES,
         TASK_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

      RTEMS.TASK_START( TASK_ID, TMTEST.TEST_TASK1'ACCESS, 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START" );

   end TEST_INIT;

--PAGE
-- 
--  TEST_TASK
--

   procedure TEST_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      INDEX        : RTEMS.UNSIGNED32;
      OVERHEAD     : RTEMS.UNSIGNED32;
      OLD_PRIORITY : RTEMS.TASK_PRIORITY;
      OLD_MODE     : RTEMS.MODE;
      OLD_NOTE     : RTEMS.NOTEPAD_INDEX;
      TIME         : RTEMS.TIME_OF_DAY;
      STATUS       : RTEMS.STATUS_CODES;
   begin

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            TIMER_DRIVER.EMPTY_FUNCTION;
         end loop;
      OVERHEAD := TIMER_DRIVER.READ_TIMER;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_SET_PRIORITY( 
               TMTEST.TASK_ID,
               RTEMS.CURRENT_PRIORITY,
               OLD_PRIORITY,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_SET_PRIORITY current priority",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_SET_PRIORITY
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_SET_PRIORITY( 
               TMTEST.TASK_ID,
               253,
               OLD_PRIORITY,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_SET_PRIORITY no preempt",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_SET_PRIORITY
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_MODE( 
               RTEMS.CURRENT_MODE,
               RTEMS.CURRENT_MODE,
               OLD_MODE,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_MODE (current)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_MODE
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_MODE( 
               RTEMS.INTERRUPT_LEVEL( 1 ),
               RTEMS.INTERRUPT_MASK,
               OLD_MODE,
               STATUS
            );
            RTEMS.TASK_MODE( 
               RTEMS.INTERRUPT_LEVEL( 0 ),
               RTEMS.INTERRUPT_MASK,
               OLD_MODE,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_MODE (no reschedule)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT * 2, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_MODE
      );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.TASK_MODE( 
            RTEMS.NO_ASR,
            RTEMS.ASR_MASK,
            OLD_MODE,
            STATUS
         );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_MODE (reschedule)",
         TMTEST.END_TIME, 
         1, 
         0,
         RTEMS_CALLING_OVERHEAD.TASK_MODE
      );

      RTEMS.TASK_MODE( 
         RTEMS.NO_PREEMPT,
         RTEMS.PREEMPT_MASK,
         OLD_MODE,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );

      RTEMS.TASK_SET_PRIORITY( 
         TMTEST.TASK_ID,
         1,
         OLD_PRIORITY,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY" );

      TIMER_DRIVER.INITIALIZE;
      RTEMS.TASK_MODE(          -- preempted by TEST_TASK1
         RTEMS.PREEMPT,
         RTEMS.PREEMPT_MASK,
         OLD_MODE,
         STATUS
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_SET_NOTE( 
               TMTEST.TASK_ID,
               8,
               10,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_SET_NOTE ",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_SET_NOTE
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASK_GET_NOTE( 
               TMTEST.TASK_ID,
               8,
               OLD_NOTE,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_GET_NOTE ",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_GET_NOTE
      );

      TIME := (1988, 1, 1, 0, 0, 0, 0 );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.CLOCK_SET( 
               TIME,
               STATUS
            );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "CLOCK_SET",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.CLOCK_SET
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.CLOCK_GET( RTEMS.CLOCK_GET_TOD, TIME'ADDRESS, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "CLOCK_GET",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.CLOCK_GET
      );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TEST_TASK;

--PAGE
-- 
--  TEST_TASK1
--

   procedure TEST_TASK1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS       : RTEMS.STATUS_CODES;
   begin

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_MODE (preemptive) ",
         TMTEST.END_TIME, 
         1, 
         0,
         RTEMS_CALLING_OVERHEAD.TASK_MODE
      );

      RTEMS.TASK_SUSPEND( RTEMS.SELF, STATUS );

   end TEST_TASK1;

end TMTEST;
