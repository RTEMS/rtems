--
--  MPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation for Test 2 of the RTEMS
--  Multiprocessor Test Suite.
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
with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;

package body MPTEST is

   package body PER_NODE_CONFIGURATION is separate;

--PAGE
--
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT( "*** TEST 2 -- NODE " );
      UNSIGNED32_IO.PUT(
         MPTEST.MULTIPROCESSING_CONFIGURATION.NODE,
         WIDTH => 1
      );
      TEXT_IO.PUT_LINE( " ***" );

      MPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  '1', '1', '1', ' ' );
      MPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  '2', '2', '2', ' ' );

      TEXT_IO.PUT_LINE( "Creating test task (Global)" );

      RTEMS.TASK_CREATE( 
         MPTEST.TASK_NAME( MPTEST.MULTIPROCESSING_CONFIGURATION.NODE ),
         1, 
         2048, 
         RTEMS.NO_PREEMPT,
         RTEMS.GLOBAL,
         MPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

      RTEMS.TASK_START(
         MPTEST.TASK_ID( 1 ),
         MPTEST.TEST_TASK'ACCESS,
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
      TID         : RTEMS.ID;
      TEST_TID    : RTEMS.ID;
      REMOTE_TID  : RTEMS.ID;
      REMOTE_NODE : RTEMS.UNSIGNED32;
      NOTE        : RTEMS.UNSIGNED32;
      STATUS      : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASK_IDENT( RTEMS.SELF, RTEMS.SEARCH_ALL_NODES, TID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF SELF" );
   
      if MPTEST.MULTIPROCESSING_CONFIGURATION.NODE = 1 then
         REMOTE_NODE := 2;
      else
         REMOTE_NODE := 1;
      end if;
    
      TEXT_IO.PUT_LINE( "Getting TID of remote task (all nodes)" );

      loop

         RTEMS.TASK_IDENT( 
            MPTEST.TASK_NAME( REMOTE_NODE ),
            RTEMS.SEARCH_ALL_NODES,
            REMOTE_TID,
            STATUS
         );

         exit when RTEMS.IS_STATUS_SUCCESSFUL( STATUS );

      end loop;

      --
      --  We just got this ID above so looping is not necessary.
      --

      TEXT_IO.PUT_LINE( "Getting TID of remote task (1 node)" );
      RTEMS.TASK_IDENT( 
         MPTEST.TASK_NAME( REMOTE_NODE ),
         REMOTE_NODE,
         TEST_TID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT" );

      if TEST_TID /= REMOTE_TID then
         TEXT_IO.PUT_LINE( "task_ident tid's do not match!!" );
         RTEMS.SHUTDOWN_EXECUTIVE( 0 );
      end if;

      RTEMS.TASK_DELETE( REMOTE_TID, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.ILLEGAL_ON_REMOTE_OBJECT,
         "task_delete of remote task"
      );
      TEXT_IO.PUT_LINE( 
         "task_delete of remote task returned the correct error" 
      );
          
      RTEMS.TASK_START( REMOTE_TID, MPTEST.TEST_TASK'ACCESS, 0, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.ILLEGAL_ON_REMOTE_OBJECT,
         "task_start of remote task"
      );
      TEXT_IO.PUT_LINE( 
         "task_start of remote task returned the correct error" 
      );
          
      RTEMS.TASK_RESTART( REMOTE_TID, 0, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.ILLEGAL_ON_REMOTE_OBJECT,
         "task_restart of remote task"
      );
      TEXT_IO.PUT_LINE( 
         "task_restart of remote task returned the correct error" 
      );
          
   
      TEXT_IO.PUT( "Setting notepad " );
      UNSIGNED32_IO.PUT( RTEMS.GET_NODE( TID ), WIDTH=>1 );
      TEXT_IO.PUT( " of the remote task to " );
      UNSIGNED32_IO.PUT( RTEMS.GET_NODE( TID ), WIDTH=>1 );
      TEXT_IO.NEW_LINE;
      RTEMS.TASK_SET_NOTE( 
         REMOTE_TID, 
         RTEMS.GET_NODE( TID ),
         RTEMS.GET_NODE( TID ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_NOTE" );

      TEXT_IO.PUT_LINE( "Getting a notepad of the remote task" );
      RTEMS.TASK_GET_NOTE( 
         REMOTE_TID, 
         RTEMS.GET_NODE( TID ),
         NOTE,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_GET_NOTE" );

      if NOTE = RTEMS.GET_NODE( TID ) then
         TEXT_IO.PUT_LINE( "Remote notepad set and read correctly" );
      else
         TEXT_IO.PUT(
            "FAILURE!!! Remote notepad was not set and read correctly ("
         );
         UNSIGNED32_IO.PUT( NOTE );
         TEXT_IO.PUT( ", " );
         UNSIGNED32_IO.PUT( RTEMS.GET_NODE( TID ) );
         TEXT_IO.PUT_LINE( ")" );

      end if;

      RTEMS.TASK_DELETE( REMOTE_TID, STATUS );
      TEXT_IO.PUT_LINE( "*** END OF TEST 2 ***" );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TEST_TASK;

end MPTEST;
