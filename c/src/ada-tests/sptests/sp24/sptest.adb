--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 24 of the RTEMS
--  Single Processor Test Suite.
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

package body SPTEST is

--PAGE
-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      TIME   : RTEMS.TIME_OF_DAY;
      INDEX  : RTEMS.UNSIGNED32;
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TEST 24 ***" );

      TIME := ( 1988, 12, 31, 9, 0, 0, 0 );

      RTEMS.CLOCK_SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );
      SPTEST.TASK_NAME( 3 ) := RTEMS.BUILD_NAME(  'T', 'A', '3', ' ' );

      SPTEST.TIMER_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'M', '1', ' ' );
      SPTEST.TIMER_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'M', '2', ' ' );
      SPTEST.TIMER_NAME( 3 ) := RTEMS.BUILD_NAME(  'T', 'M', '3', ' ' );

      for INDEX in 1 .. 3
      loop

         RTEMS.TASK_CREATE( 
            SPTEST.TASK_NAME( INDEX ), 
            1, 
            2048, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            SPTEST.TASK_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         RTEMS.TIMER_CREATE(
            SPTEST.TIMER_NAME( INDEX ), 
            SPTEST.TIMER_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE LOOP" );

      end loop;

      for INDEX in 1 .. 3
      loop

         RTEMS.TASK_START(
            SPTEST.TASK_ID( INDEX ),
            SPTEST.TASK_1_THROUGH_3'ACCESS,
            RTEMS.TASK_ARGUMENT( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  RESUME_TASK
--

   procedure RESUME_TASK (
      TIMER_ID        : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   ) is
      TASK_TO_RESUME : RTEMS.ID;
      STATUS         : RTEMS.STATUS_CODES;
   begin

      TASK_TO_RESUME := SPTEST.TASK_ID(INTEGER( RTEMS.GET_INDEX( TIMER_ID ) ));
      RTEMS.TASK_RESUME( TASK_TO_RESUME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESUME" );

   end RESUME_TASK;

--PAGE
-- 
--  TASK_1_THROUGH_3
--

   procedure TASK_1_THROUGH_3 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      TID    : RTEMS.ID;
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASK_IDENT( RTEMS.SELF, RTEMS.SEARCH_ALL_NODES, TID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF SELF" );
   
      loop

         RTEMS.TIMER_FIRE_AFTER( 
            SPTEST.TIMER_ID( INTEGER( ARGUMENT ) ), 
            TEST_SUPPORT.TASK_NUMBER( TID ) * 5 * 
              TEST_SUPPORT.TICKS_PER_SECOND, 
            SPTEST.RESUME_TASK'ACCESS,
            RTEMS.NULL_ADDRESS,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );

         RTEMS.CLOCK_GET( RTEMS.CLOCK_GET_TOD, TIME'ADDRESS, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

         if TIME.SECOND >= 35 then
            TEXT_IO.PUT_LINE( "*** END OF TEST 24 ***" );
            RTEMS.SHUTDOWN_EXECUTIVE( 0 );
         end if;

         TEST_SUPPORT.PUT_NAME( 
            SPTEST.TASK_NAME( INTEGER( TEST_SUPPORT.TASK_NUMBER( TID ) ) ),
            FALSE
         );

         TEST_SUPPORT.PRINT_TIME( " - clock_get - ", TIME, "" );
         TEXT_IO.NEW_LINE;

         RTEMS.TASK_SUSPEND( RTEMS.SELF, STATUS ); 
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );
          
      end loop;
   
   end TASK_1_THROUGH_3;

end SPTEST;
