--
--  MPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation for Test 1 of the RTEMS
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
--  http://www.rtems.com/license/LICENSE.
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
      C      : RTEMS.CHARACTER;
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT( "*** TEST 1 -- NODE " );
      UNSIGNED32_IO.PUT(
         MPTEST.MULTIPROCESSING_CONFIGURATION.NODE,
         WIDTH => 1
      );
      TEXT_IO.PUT_LINE( " ***" );
     
      if MPTEST.MULTIPROCESSING_CONFIGURATION.NODE /= 1 then
         C := 'S';
      else
         C := 'M';
      end if;

      MPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  C, 'A', '1', ' ' );
      MPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  C, 'A', '2', ' ' );
      MPTEST.TASK_NAME( 3 ) := RTEMS.BUILD_NAME(  C, 'A', '3', ' ' );

      TIME := ( 1988, 12, 31, 9, 0, 0, 0 );

      RTEMS.CLOCK_SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET" );

      TEXT_IO.PUT_LINE( "Creating task 1 (Global)" );

      RTEMS.TASK_CREATE( 
         MPTEST.TASK_NAME( 1 ), 
         1, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.GLOBAL,
         MPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      TEXT_IO.PUT_LINE( "Creating task 2 (Global)" );

      RTEMS.TASK_CREATE( 
         MPTEST.TASK_NAME( 2 ), 
         1, 
         2048, 
         RTEMS.TIMESLICE,
         RTEMS.GLOBAL,
         MPTEST.TASK_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA2" );

      TEXT_IO.PUT_LINE( "Creating task 3 (Local)" );

      RTEMS.TASK_CREATE( 
         MPTEST.TASK_NAME( 3 ), 
         1, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         MPTEST.TASK_ID( 3 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA3" );

      RTEMS.TASK_START(
         MPTEST.TASK_ID( 1 ),
         MPTEST.TEST_TASK'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      RTEMS.TASK_START(
         MPTEST.TASK_ID( 2 ),
         MPTEST.TEST_TASK'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA2" );

      RTEMS.TASK_START(
         MPTEST.TASK_ID( 3 ),
         MPTEST.TEST_TASK'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA3" );

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
      TIME   : RTEMS.TIME_OF_DAY;
      TID    : RTEMS.ID;
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASK_IDENT( RTEMS.SELF, RTEMS.SEARCH_ALL_NODES, TID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF SELF" );
   
      RTEMS.CLOCK_GET( RTEMS.CLOCK_GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

      TEST_SUPPORT.PUT_NAME( 
         MPTEST.TASK_NAME( TEST_SUPPORT.TASK_NUMBER( TID ) ),
         FALSE
      );

      TEST_SUPPORT.PRINT_TIME( "- clock_get - ", TIME, "" );
      TEXT_IO.NEW_LINE;

      RTEMS.TASK_WAKE_AFTER( 
         TEST_SUPPORT.TASK_NUMBER( TID ) * 5 * 
           TEST_SUPPORT.TICKS_PER_SECOND, 
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
          
      RTEMS.CLOCK_GET( RTEMS.CLOCK_GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

      TEST_SUPPORT.PUT_NAME( 
         MPTEST.TASK_NAME( TEST_SUPPORT.TASK_NUMBER( TID ) ),
         FALSE
      );

      TEST_SUPPORT.PRINT_TIME( "- clock_get - ", TIME, "" );
      TEXT_IO.NEW_LINE;

      if TEST_SUPPORT.TASK_NUMBER( TID ) = 1 then         -- TASK 1

         TEST_SUPPORT.PUT_NAME( 
            MPTEST.TASK_NAME( TEST_SUPPORT.TASK_NUMBER( TID ) ),
            FALSE
         );

         TEXT_IO.PUT_LINE( " - deleting self" );

         RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

      else if TEST_SUPPORT.TASK_NUMBER( TID ) = 2 then    -- TASK 2

         TEST_SUPPORT.PUT_NAME( MPTEST.TASK_NAME( 2 ), FALSE );
         TEXT_IO.PUT( " - waiting to be deleted by " );
         TEST_SUPPORT.PUT_NAME( MPTEST.TASK_NAME( 3 ), TRUE );

         loop
            TEST_SUPPORT.DO_NOTHING;   -- can't be optimized away
         end loop;

      else                                                -- TASK 3

         TEST_SUPPORT.PUT_NAME( MPTEST.TASK_NAME( 3 ), FALSE );
         TEXT_IO.PUT( " - getting TID of " );
         TEST_SUPPORT.PUT_NAME( MPTEST.TASK_NAME( 2 ), TRUE );
         
         RTEMS.TASK_IDENT( 
            MPTEST.TASK_NAME( 2 ),
            RTEMS.SEARCH_ALL_NODES,
            TID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF TA2" );

         TEST_SUPPORT.PUT_NAME( MPTEST.TASK_NAME( 3 ), FALSE );
         TEXT_IO.PUT( " - deleting " );
         TEST_SUPPORT.PUT_NAME( MPTEST.TASK_NAME( 2 ), TRUE );

         RTEMS.TASK_DELETE( TID, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA2" );

      end if;
      end if;

      TEXT_IO.PUT_LINE( "*** END OF TEST 1 ***" );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TEST_TASK;

end MPTEST;
