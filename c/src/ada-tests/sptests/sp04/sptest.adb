--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 4 of the RTEMS
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
--  http://www.OARcorp.com/rtems/license.html.
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
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TEST 4 ***" );

      TIME := ( 1988, 12, 31, 9, 15, 0, 0 );

      RTEMS.CLOCK_SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET" );

      SPTEST.EXTENSION_NAME( 1 ) := RTEMS.BUILD_NAME(  'E', 'X', 'T', ' ' );

      RTEMS.EXTENSION_CREATE(
         SPTEST.EXTENSION_NAME( 1 ),
         SPTEST.EXTENSIONS'ACCESS,
         EXTENSION_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EXTENSION_CREATE" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );
      SPTEST.TASK_NAME( 3 ) := RTEMS.BUILD_NAME(  'T', 'A', '3', ' ' );

      SPTEST.RUN_COUNT( 1 ) := 0;
      SPTEST.RUN_COUNT( 2 ) := 0;
      SPTEST.RUN_COUNT( 3 ) := 0;

      RTEMS.TASK_CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         1, 
         2048, 
         RTEMS.TIMESLICE,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASK_CREATE( 
         SPTEST.TASK_NAME( 2 ), 
         1, 
         2048, 
         RTEMS.TIMESLICE,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA2" );

      RTEMS.TASK_CREATE( 
         SPTEST.TASK_NAME( 3 ), 
         1, 
         2048, 
         RTEMS.TIMESLICE,
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
      SECONDS       : RTEMS.UNSIGNED32;
      OLD_SECONDS   : RTEMS.UNSIGNED32;
      STARTED_DELAY : RTEMS.UNSIGNED32;
      PREVIOUS_MODE : RTEMS.MODE;
      TIME          : RTEMS.TIME_OF_DAY;
      START_TIME    : RTEMS.INTERVAL;
      END_TIME      : RTEMS.INTERVAL;
      STATUS        : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT_LINE( "TA1 - task_suspend - on Task 2" );
      RTEMS.TASK_SUSPEND( TASK_ID( 2 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND OF TA2" );
 
      TEXT_IO.PUT_LINE( "TA1 - task_suspend - on Task 3" );
      RTEMS.TASK_SUSPEND( TASK_ID( 3 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND OF TA3" );
 
      TEXT_IO.PUT_LINE( "TA1 - killing time" );

      RTEMS.CLOCK_GET(
         RTEMS.CLOCK_GET_SECONDS_SINCE_EPOCH,
         START_TIME'ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

      loop
         RTEMS.CLOCK_GET(
            RTEMS.CLOCK_GET_SECONDS_SINCE_EPOCH,
            END_TIME'ADDRESS,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

         exit when END_TIME > (START_TIME + 2);
      end loop;

      TEXT_IO.PUT_LINE( "TA1 - task_resume - on Task 2" );
      RTEMS.TASK_RESUME( TASK_ID( 2 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESUME OF TA2" );
 
      TEXT_IO.PUT_LINE( "TA1 - task_resume - on Task 3" );
      RTEMS.TASK_RESUME( TASK_ID( 3 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESUME OF TA3" );
 
      loop

         if SPTEST.RUN_COUNT( 1 ) = 3 then

            TEXT_IO.PUT_LINE( 
               "TA1 - task_mode - change mode to NO PREEMPT"
            );
            RTEMS.TASK_MODE( 
               RTEMS.NO_PREEMPT, 
               RTEMS.PREEMPT_MASK, 
               PREVIOUS_MODE, 
               STATUS 
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );

            RTEMS.CLOCK_GET( RTEMS.CLOCK_GET_TOD, TIME'ADDRESS, STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

            OLD_SECONDS := TIME.SECOND;

            SECONDS := 0;
            loop

               exit when SECONDS >= 6;

               RTEMS.CLOCK_GET( RTEMS.CLOCK_GET_TOD, TIME'ADDRESS, STATUS );
               TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

               if TIME.SECOND /= OLD_SECONDS then
                  OLD_SECONDS := TIME.SECOND;
                  SECONDS := SECONDS + 1;
                  TEST_SUPPORT.PRINT_TIME( "TA1 - ", TIME, "" );
                  TEXT_IO.NEW_LINE;
               end if;

            end loop;

            TEXT_IO.PUT_LINE( 
               "TA1 - task_mode - change mode to PREEMPT"
            );

            RTEMS.TASK_MODE( 
               RTEMS.PREEMPT, 
               RTEMS.PREEMPT_MASK, 
               PREVIOUS_MODE, 
               STATUS 
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );

<<LOOP_LABEL>>
            goto LOOP_LABEL;

         end if;

      end loop;

   end TASK_1;

--PAGE
-- 
--  TASK_2
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
   begin

<<LOOP_LABEL>>
         goto LOOP_LABEL;

   end TASK_2;

--PAGE
-- 
--  TASK_3
--

   procedure TASK_3 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
   begin

<<LOOP_LABEL>>
         goto LOOP_LABEL;

   end TASK_3;

--PAGE
-- 
--  TASK_SWITCH
--

   procedure TASK_SWITCH (
      UNUSED : in     RTEMS.TCB_POINTER;
      HEIR   : in     RTEMS.TCB_POINTER
   ) is
      INDEX  : RTEMS.UNSIGNED32;
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
      function TCB_To_ID (
         TCB : RTEMS.TCB_POINTER
      ) return RTEMS.ID;
      pragma Import (C, TCB_To_ID, "tcb_to_id" );
         
   begin

      --INDEX := TEST_SUPPORT.TASK_NUMBER( HEIR.OBJECT.ID );
      INDEX := TCB_To_ID( HEIR );

      case INDEX is
         when 1  | 2 | 3 =>
            SPTEST.RUN_COUNT( INDEX ) := SPTEST.RUN_COUNT( INDEX ) + 1;

            RTEMS.CLOCK_GET( RTEMS.CLOCK_GET_TOD, TIME'ADDRESS, STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET" );

            TEST_SUPPORT.PUT_NAME( SPTEST.TASK_NAME( INDEX ), FALSE );
            TEST_SUPPORT.PRINT_TIME( "- ", TIME, "" );
            TEXT_IO.NEW_LINE;
           
            if TIME.SECOND >= 16 then
               TEXT_IO.PUT_LINE( "*** END OF TEST 4 ***" );
               RTEMS.SHUTDOWN_EXECUTIVE( 0 );
            end if;

         when others =>
            NULL;
      end case;

   end TASK_SWITCH;

end SPTEST;
