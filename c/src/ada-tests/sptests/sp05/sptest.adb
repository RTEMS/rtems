--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 5 of the RTEMS
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
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TEST 5 ***" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );
      SPTEST.TASK_NAME( 3 ) := RTEMS.BUILD_NAME(  'T', 'A', '3', ' ' );

      RTEMS.TASK_CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         1, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASK_CREATE( 
         SPTEST.TASK_NAME( 2 ), 
         1, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA2" );

      RTEMS.TASK_CREATE( 
         SPTEST.TASK_NAME( 3 ), 
         1, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 3 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA3" );

      RTEMS.TASK_START(
         SPTEST.TASK_ID( 1 ),
         SPTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      RTEMS.TASK_START(
         SPTEST.TASK_ID( 2 ),
         SPTEST.TASK_2'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA2" );

      RTEMS.TASK_START(
         SPTEST.TASK_ID( 3 ),
         SPTEST.TASK_3'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA3" );

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      TID2   : RTEMS.ID;
      TID3   : RTEMS.ID;
      PASS   : RTEMS.UNSIGNED32;
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASK_IDENT( 
         SPTEST.TASK_NAME( 2 ), 
         1, 
         TID2, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF TA2" );
   
      RTEMS.TASK_IDENT( 
         SPTEST.TASK_NAME( 3 ), 
         1, 
         TID3, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF TA3" );
   
      for PASS in 1 .. 3
      loop

         TEXT_IO.PUT_LINE( "TA1 - task_wake_after - sleep 5 seconds" );
         RTEMS.TASK_WAKE_AFTER( 5 * TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER OF TA1" );
         
         TEXT_IO.PUT_LINE( "TA1 - task_suspend - suspend TA3" );
         RTEMS.TASK_SUSPEND( TID3, STATUS );
         if PASS = 1 then
           TEST_SUPPORT.FATAL_DIRECTIVE_STATUS( 
              STATUS, 
              RTEMS.ALREADY_SUSPENDED, 
              "TASK_SUSPEND OF TA3" 
           );
         else
           TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND OF TA3" );
         end if;
         
         TEXT_IO.PUT_LINE( "TA1 - task_resume - resume TA2" );
         RTEMS.TASK_RESUME( TID2, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESUME OF TA2" );
         
         TEXT_IO.PUT_LINE( "TA1 - task_wake_after - sleep 5 seconds" );
         RTEMS.TASK_WAKE_AFTER( 5 * TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
         
         TEXT_IO.PUT_LINE( "TA1 - task_suspend - suspend TA2" );
         RTEMS.TASK_SUSPEND( TID2, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND OF TA2" );
         
         TEXT_IO.PUT_LINE( "TA1 - task_resume - resume TA3" );
         RTEMS.TASK_RESUME( TID3, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESUME OF TA3" );
          
      end loop;

      TEXT_IO.PUT_LINE( "*** END OF TEST 5 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TASK_1;

--PAGE
-- 
--  TASK_2
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT_LINE( "TA2 - task_suspend - suspend self" );
      RTEMS.TASK_SUSPEND( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND OF TA2" );

      loop
         TEXT_IO.PUT_LINE( "TA2 - task_wake_after - sleep 1 second" );
         RTEMS.TASK_WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER OF TA2" );
      end loop;
         
   end TASK_2;

--PAGE
-- 
--  TASK_3
--

   procedure TASK_3 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT_LINE( "TA3 - task_suspend - suspend self" );
      RTEMS.TASK_SUSPEND( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND OF TA3" );

      loop
         TEXT_IO.PUT_LINE( "TA3 - task_wake_after - sleep 1 second" );
         RTEMS.TASK_WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER OF TA3" );
      end loop;
         
   end TASK_3;

end SPTEST;
