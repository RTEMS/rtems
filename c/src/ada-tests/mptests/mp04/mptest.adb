--
--  MPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation for Test 4 of the RTEMS
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
      TEXT_IO.PUT( "*** TEST 4 -- NODE " );
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
         TEST_SUPPORT.NODE, 
         2048, 
         RTEMS.DEFAULT_MODES,
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
      TID                 : RTEMS.ID;
      PREVIOUS_PRIORITY   : RTEMS.TASKS.PRIORITY;
      PREVIOUS_PRIORITY_1 : RTEMS.TASKS.PRIORITY;
      STATUS              : RTEMS.STATUS_CODES;
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

      RTEMS.TASKS.SET_PRIORITY(
         MPTEST.REMOTE_TID,
         TEST_SUPPORT.NODE,
         PREVIOUS_PRIORITY,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY" );

      if PREVIOUS_PRIORITY /= MPTEST.REMOTE_NODE then

         TEXT_IO.PUT( "Remote priority (0x" );
         UNSIGNED32_IO.PUT( PREVIOUS_PRIORITY, BASE => 16 );
         TEXT_IO.PUT( "does not match remote node (0x" );
         UNSIGNED32_IO.PUT( MPTEST.REMOTE_NODE, BASE => 16 );
         TEXT_IO.PUT_LINE( ")!!!" );

         RTEMS.SHUTDOWN_EXECUTIVE( 16#F00000# );

      end if;

      loop

         RTEMS.TASKS.SET_PRIORITY(
            RTEMS.SELF,
            RTEMS.TASKS.CURRENT_PRIORITY,
            PREVIOUS_PRIORITY_1,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY" );

         exit when PREVIOUS_PRIORITY_1 = MPTEST.REMOTE_NODE;

      end loop;


      TEXT_IO.PUT_LINE( "Local task priority has been set" );

      TEXT_IO.PUT_LINE( "*** END OF TEST 4 ***" );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TEST_TASK;

end MPTEST;
