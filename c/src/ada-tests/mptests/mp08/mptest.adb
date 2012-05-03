--
--  MPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation for Test 8 of the RTEMS
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
with RTEMS.OBJECT;
with RTEMS.SEMAPHORE;
with RTEMS.TASKS;
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
      TEXT_IO.PUT( "*** TEST 8 -- NODE " );
      UNSIGNED32_IO.PUT(
         TEST_SUPPORT.NODE,
         WIDTH => 1
      );
      TEXT_IO.PUT_LINE( " ***" );

      MPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  '1', '1', '1', ' ' );
      MPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  '2', '2', '2', ' ' );

      MPTEST.SEMAPHORE_NAME( 1 ) := RTEMS.BUILD_NAME(  'S', 'E', 'M', ' ' );

      if TEST_SUPPORT.NODE = 1 then

         TEXT_IO.PUT_LINE( "Creating Semaphore(Global)" );

         RTEMS.SEMAPHORE.CREATE(
            MPTEST.SEMAPHORE_NAME( 1 ),
            1,
            RTEMS.GLOBAL,
            MPTEST.SEMAPHORE_ID( 1 ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE" );

      end if;

      TEXT_IO.PUT_LINE( "Creating Test_task (Global)" );
      RTEMS.TASKS.CREATE( 
         MPTEST.TASK_NAME( TEST_SUPPORT.NODE ), 
         TEST_SUPPORT.NODE, 
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

      TEXT_IO.PUT_LINE( "Deleting initialization task" );
      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--
--  TEST_TASK
--

   procedure TEST_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      DOTS                     : RTEMS.UNSIGNED32;
      COUNT                    : RTEMS.UNSIGNED32;
      STATUS                   : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT_LINE( "Getting SMID of semaphore" );
      loop

         RTEMS.SEMAPHORE.IDENT( 
            MPTEST.SEMAPHORE_NAME( 1 ),
            RTEMS.SEARCH_ALL_NODES,
            MPTEST.SEMAPHORE_ID( 1 ),
            STATUS
         );

         exit when RTEMS.IS_STATUS_SUCCESSFUL( STATUS );

      end loop;

      if TEST_SUPPORT.NODE = 2 then

         RTEMS.SEMAPHORE.DELETE(
            MPTEST.SEMAPHORE_ID( 1 ),
            STATUS
         );
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS( 
            STATUS, 
            RTEMS.ILLEGAL_ON_REMOTE_OBJECT,
            "SEMAPHORE_DELETE" 
         );

         TEXT_IO.PUT_LINE(
            "semaphore_delete correctly returned ILLEGAL_ON_REMOTE_OBJECT"
         );

      end if;

      COUNT := 0;

      loop

         TEST_SUPPORT.PUT_DOT( "p" );

         RTEMS.SEMAPHORE.OBTAIN(
            MPTEST.SEMAPHORE_ID( 1 ),
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.NO_TIMEOUT,
            STATUS
         );

         if not RTEMS.IS_STATUS_SUCCESSFUL( STATUS ) then

            TEST_SUPPORT.FATAL_DIRECTIVE_STATUS( 
               STATUS, 
               RTEMS.OBJECT_WAS_DELETED,
               "SEMAPHORE_OBTAIN" 
            );

            TEXT_IO.NEW_LINE;

            TEXT_IO.PUT_LINE( "Global semaphore deleted" );

            TEXT_IO.PUT_LINE( "*** END OF TEST 8 ***" );

            RTEMS.SHUTDOWN_EXECUTIVE( 0 );

         end if;

         COUNT := COUNT + 1;

         if TEST_SUPPORT.NODE = 1 and then
            COUNT >= 1000 then

            RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

            TEXT_IO.NEW_LINE;

            TEXT_IO.PUT_LINE( "Deleting global semaphore" );

            RTEMS.SEMAPHORE.DELETE( MPTEST.SEMAPHORE_ID( 1 ), STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_DELETE" );

            TEXT_IO.PUT_LINE( "*** END OF TEST 8 ***" );

            RTEMS.SHUTDOWN_EXECUTIVE( 0 );

         end if;

         TEST_SUPPORT.PUT_DOT( "v" );

         RTEMS.SEMAPHORE.RELEASE( MPTEST.SEMAPHORE_ID( 1 ), STATUS );

         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE" );

      end loop;

   end TEST_TASK;

end MPTEST;
