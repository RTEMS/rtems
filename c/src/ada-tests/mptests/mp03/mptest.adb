--
--  This package is the implementation for Test 3 of the RTEMS
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
with RTEMS.EVENT;
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
      TEXT_IO.PUT( "*** TEST 3 -- NODE " );
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
         RTEMS.NO_PREEMPT,
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

      RTEMS.TIMER.CREATE( 
         MPTEST.TIMER_NAME( 1 ), 
         MPTEST.TIMER_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE" );

      TEXT_IO.PUT_LINE( "Deleting initialization task" );
      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--
--  DELAYED_SEND_EVENT
--

   procedure DELAYED_SEND_EVENT (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   ) is
      STATUS  : RTEMS.STATUS_CODES;
   begin

      RTEMS.EVENT.SEND( MPTEST.TASK_ID( 1 ), RTEMS.EVENT_16, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND" );

   end DELAYED_SEND_EVENT;

--
--  TEST_TASK
--

   procedure TEST_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      TID         : RTEMS.ID;
      STATUS      : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASKS.IDENT( RTEMS.SELF, RTEMS.SEARCH_ALL_NODES, TID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF SELF" );
   
      TEXT_IO.PUT_LINE( "Getting TID of remote task" );
      if TEST_SUPPORT.NODE = 1 then
         MPTEST.REMOTE_NODE := 2;
      else
         MPTEST.REMOTE_NODE := 1;
      end if;

      TEXT_IO.PUT( "Remote task's name is : " );
      TEST_SUPPORT.PUT_NAME( MPTEST.TASK_NAME( MPTEST.REMOTE_NODE ), TRUE );

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
         10 * TEST_SUPPORT.TICKS_PER_SECOND, 
         MPTEST.DELAYED_SEND_EVENT'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );

      MPTEST.TEST_TASK_SUPPORT( 1 );

      RTEMS.TIMER.FIRE_AFTER( 
         MPTEST.TIMER_ID( 1 ), 
         11 * TEST_SUPPORT.TICKS_PER_SECOND, 
         MPTEST.DELAYED_SEND_EVENT'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );

      if TEST_SUPPORT.NODE = 2 then
         
         RTEMS.TASKS.WAKE_AFTER( 
            2 * TEST_SUPPORT.TICKS_PER_SECOND,
            STATUS
         );
        TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

      end if;

      MPTEST.TEST_TASK_SUPPORT( 2 );

      TEXT_IO.PUT_LINE( "*** END OF TEST 3 ***" );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TEST_TASK;

-- 
--  TEST_TASK_SUPPORT
--


   procedure TEST_TASK_SUPPORT (
      NODE : in    RTEMS.UNSIGNED32
   ) is
      EVENTS : RTEMS.EVENT_SET;
      STATUS : RTEMS.STATUS_CODES;
   begin

      if TEST_SUPPORT.NODE = NODE then

         loop

            RTEMS.EVENT.RECEIVE( 
               RTEMS.EVENT_16,
               RTEMS.NO_WAIT,
               RTEMS.NO_TIMEOUT,
               EVENTS,
               STATUS
            );

            exit when RTEMS.ARE_STATUSES_EQUAL( RTEMS.SUCCESSFUL, STATUS );

            TEST_SUPPORT.FATAL_DIRECTIVE_STATUS( 
               STATUS,
               RTEMS.UNSATISFIED,
               "EVENT_RECEIVE"
            );

            RTEMS.TASKS.WAKE_AFTER( 
               2 * TEST_SUPPORT.TICKS_PER_SECOND, 
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

            TEST_SUPPORT.PUT_NAME( MPTEST.TASK_NAME( NODE ), FALSE );
            TEXT_IO.PUT_LINE( " - Suspending remote task" );
            RTEMS.TASKS.SUSPEND( MPTEST.REMOTE_TID, STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );

            RTEMS.TASKS.WAKE_AFTER( 
               2 * TEST_SUPPORT.TICKS_PER_SECOND, 
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

            TEST_SUPPORT.PUT_NAME( MPTEST.TASK_NAME( NODE ), FALSE );
            TEXT_IO.PUT_LINE( " - Resuming remote task" );

            RTEMS.TASKS.RESUME( MPTEST.REMOTE_TID, STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESUME" );

         end loop;

      else

         loop

            RTEMS.EVENT.RECEIVE( 
               RTEMS.EVENT_16,
               RTEMS.NO_WAIT,
               RTEMS.NO_TIMEOUT,
               EVENTS,
               STATUS
            );

            exit when RTEMS.ARE_STATUSES_EQUAL( RTEMS.SUCCESSFUL, STATUS );

            TEST_SUPPORT.FATAL_DIRECTIVE_STATUS( 
               STATUS,
               RTEMS.UNSATISFIED,
               "EVENT_RECEIVE"
            );

            TEST_SUPPORT.PUT_NAME( MPTEST.TASK_NAME( REMOTE_NODE ), FALSE );
            TEXT_IO.PUT_LINE( " - have I been suspended???" ); 
            RTEMS.TASKS.WAKE_AFTER( 
               TEST_SUPPORT.TICKS_PER_SECOND / 2,
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

         end loop;

      end if;

   end TEST_TASK_SUPPORT;

end MPTEST;
