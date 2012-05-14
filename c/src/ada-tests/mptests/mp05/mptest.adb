--
--  MPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation for Test 5 of the RTEMS
--  Multiprocessor Test Suite.
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
with RTEMS;
with RTEMS.FATAL;
with RTEMS.SIGNAL;
with RTEMS.TASKS;
with RTEMS.TIMER;
with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;

package body MPTEST is

--
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT( "*** TEST 5 -- NODE " );
      UNSIGNED32_IO.PUT(
         TEST_SUPPORT.NODE,
         WIDTH => 1
      );
      TEXT_IO.PUT_LINE( " ***" );

      MPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  '1', '1', '1', ' ' );
      MPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  '2', '2', '2', ' ' );

      TEXT_IO.PUT_LINE( "Creating Test_task (Global)" );

      RTEMS.TASKS.CREATE( 
         MPTEST.TASK_NAME( TEST_SUPPORT.NODE ), 
         1,
         2048, 
         RTEMS.TIMESLICE,
         RTEMS.GLOBAL,
         MPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

      TEXT_IO.PUT_LINE( "Starting Test_task (Global)" );

      RTEMS.TASKS.START(
         MPTEST.TASK_ID( 1 ),
         MPTEST.TEST_TASK'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START" );

      MPTEST.TIMER_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'M', '1', ' ' );
      MPTEST.TIMER_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'M', '2', ' ' );

      RTEMS.TIMER.CREATE(
         MPTEST.TIMER_NAME( TEST_SUPPORT.NODE ),
         MPTEST.TIMER_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE" );

      TEXT_IO.PUT_LINE( "Deleting initialization task" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--
--  PROCESS_ASR
--

   procedure PROCESS_ASR (
      SIGNAL : in     RTEMS.SIGNAL_SET
   )
   is 
   begin

      if SIGNAL /= MPTEST.EXPECTED_SIGNAL then

         TEXT_IO.PUT( "ERROR: I was expecting signal 0x" );
         UNSIGNED32_IO.PUT( EXPECTED_SIGNAL, BASE => 16 );
         TEXT_IO.PUT( " got 0x" );
         UNSIGNED32_IO.PUT( SIGNAL, BASE => 16 );
         TEXT_IO.NEW_LINE;

         RTEMS.FATAL.ERROR_OCCURRED( 16#000F_0000# );

      end if;

      MPTEST.SIGNAL_CAUGHT := TRUE;

   end PROCESS_ASR;

--
--  STOP_TEST_TSR
--

   procedure STOP_TEST_TSR (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   ) is
   begin

      MPTEST.STOP_TEST := TRUE;

   end STOP_TEST_TSR;

--
--  TEST_TASK
--

   procedure TEST_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      STATUS  : RTEMS.STATUS_CODES;
   begin

      MPTEST.STOP_TEST := FALSE;

      MPTEST.SIGNAL_CAUGHT := FALSE;
      MPTEST.SIGNAL_COUNT  := 0;

      TEXT_IO.PUT_LINE( "signal_catch: initializing signal catcher" );
      RTEMS.SIGNAL.CATCH( 
         MPTEST.PROCESS_ASR'ACCESS, 
         RTEMS.NO_ASR + RTEMS.NO_PREEMPT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SIGNAL_CATCH" );

      if TEST_SUPPORT.NODE = 1 then
         MPTEST.REMOTE_NODE     := 2;
         MPTEST.REMOTE_SIGNAL   := RTEMS.SIGNAL_18;
         MPTEST.EXPECTED_SIGNAL := RTEMS.SIGNAL_17;
      else
         MPTEST.REMOTE_NODE     := 1;
         MPTEST.REMOTE_SIGNAL   := RTEMS.SIGNAL_17;
         MPTEST.EXPECTED_SIGNAL := RTEMS.SIGNAL_18;
      end if;

      TEXT_IO.PUT( "Remote task's name is : " );
      TEST_SUPPORT.PUT_NAME( MPTEST.TASK_NAME( MPTEST.REMOTE_NODE ), TRUE );

      TEXT_IO.PUT_LINE( "Getting TID of remote task" );
      loop

         RTEMS.TASKS.IDENT( 
            MPTEST.TASK_NAME( MPTEST.REMOTE_NODE ),
            RTEMS.SEARCH_ALL_NODES,
            MPTEST.REMOTE_TID,
            STATUS
         );

         exit when RTEMS.IS_STATUS_SUCCESSFUL( STATUS );

      end loop;

      RTEMS.TIMER.FIRE_AFTER(
         MPTEST.TIMER_ID( 1 ),
         3 * TEST_SUPPORT.TICKS_PER_SECOND,
         MPTEST.STOP_TEST_TSR'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );

      if TEST_SUPPORT.NODE = 1 then

         TEXT_IO.PUT_LINE( "Sending signal to remote task" );
         loop
            RTEMS.SIGNAL.SEND(
               MPTEST.REMOTE_TID,
               MPTEST.REMOTE_SIGNAL,
               STATUS
            );

            exit when RTEMS.IS_STATUS_SUCCESSFUL( STATUS );

            exit when not RTEMS.ARE_STATUSES_EQUAL(STATUS, RTEMS.NOT_DEFINED);
   
         end loop;
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SIGNAL_SEND" );

      end if;

      loop
 
         exit when MPTEST.STOP_TEST = TRUE;

         if MPTEST.SIGNAL_CAUGHT = TRUE then

            MPTEST.SIGNAL_CAUGHT := FALSE;
            MPTEST.SIGNAL_COUNT  := MPTEST.SIGNAL_COUNT + 1;

            if MPTEST.SIGNAL_COUNT >= MPTEST.SIGNALS_PER_DOT then

               MPTEST.SIGNAL_COUNT := 0;

               TEST_SUPPORT.PUT_DOT( "." );

            end if;

            RTEMS.SIGNAL.SEND(
               MPTEST.REMOTE_TID,
               MPTEST.REMOTE_SIGNAL,
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SIGNAL_SEND" );

         end if;

      end loop;

      TEXT_IO.NEW_LINE;
      TEXT_IO.PUT_LINE( "*** END OF TEST 5 ***" );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TEST_TASK;

end MPTEST;
