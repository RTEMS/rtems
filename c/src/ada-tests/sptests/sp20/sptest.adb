--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 20 of the RTEMS
--  Single Processor Test Suite.
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
with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;
with RTEMS.CLOCK;
with RTEMS.RATE_MONOTONIC;

package body SPTEST is

-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TEST 20 ***" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );
      SPTEST.TASK_NAME( 3 ) := RTEMS.BUILD_NAME(  'T', 'A', '3', ' ' );
      SPTEST.TASK_NAME( 4 ) := RTEMS.BUILD_NAME(  'T', 'A', '4', ' ' );
      SPTEST.TASK_NAME( 5 ) := RTEMS.BUILD_NAME(  'T', 'A', '5', ' ' );
      SPTEST.TASK_NAME( 6 ) := RTEMS.BUILD_NAME(  'T', 'A', '6', ' ' );

      for INDEX in 1 .. 6
      loop

         SPTEST.COUNT( INDEX ) := 0;

         RTEMS.TASKS.CREATE( 
            SPTEST.TASK_NAME( INDEX ), 
            SPTEST.PRIORITIES( INDEX ), 
            4096, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            SPTEST.TASK_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

      end loop;

      for INDEX in 1 .. 6
      loop

         RTEMS.TASKS.START(
            SPTEST.TASK_ID( INDEX ),
            SPTEST.TASK_1_THROUGH_6'ACCESS,
            RTEMS.TASKS.ARGUMENT( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );
 
      end loop;

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  TASK_1_THROUGH_6
--

   procedure TASK_1_THROUGH_6 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      RMID      : RTEMS.ID;
      TEST_RMID : RTEMS.ID;
      PASS      : RTEMS.UNSIGNED32;
      FAILED    : RTEMS.UNSIGNED32;
      STATUS    : RTEMS.STATUS_CODES;
      TIME      : array( 0 .. 10 ) of RTEMS.INTERVAL;
      PERIOD    : RTEMS.INTERVAL;
      MEASURE   : RTEMS.INTERVAL;
   begin

      RTEMS.RATE_MONOTONIC.CREATE( ARGUMENT, RMID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "RATE_MONOTONIC_CREATE" );
      TEST_SUPPORT.PUT_NAME( SPTEST.TASK_NAME( INTEGER( ARGUMENT ) ), FALSE );
      TEXT_IO.PUT( "- rate_monotonic_create id = " );
      UNSIGNED32_IO.PUT( RMID, WIDTH => 8, BASE => 16 );
      TEXT_IO.NEW_LINE;
 
      RTEMS.RATE_MONOTONIC.IDENT( ARGUMENT, TEST_RMID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "RATE_MONOTONIC_IDENT" );
      TEST_SUPPORT.PUT_NAME( SPTEST.TASK_NAME( INTEGER( ARGUMENT ) ), FALSE );
      TEXT_IO.PUT( "- rate_monotonic_ident id = " );
      UNSIGNED32_IO.PUT( RMID, WIDTH => 8, BASE => 16 );
      TEXT_IO.NEW_LINE;
      if TEST_RMID /= RMID then
         TEXT_IO.PUT_LINE( "RMID's DO NOT MATCH!!!" );
         RTEMS.SHUTDOWN_EXECUTIVE( 0 );
      end if; 

      TEST_SUPPORT.PUT_NAME( SPTEST.TASK_NAME( INTEGER( ARGUMENT ) ), FALSE );
      TEXT_IO.PUT( " - (" );
      UNSIGNED32_IO.PUT( RMID, WIDTH => 1, BASE => 16 );
      TEXT_IO.PUT( ") period " );
      UNSIGNED32_IO.PUT( 
         SPTEST.PERIODS( INTEGER( ARGUMENT ) ), 
         WIDTH => 1, 
         BASE => 10 
      );
      TEXT_IO.NEW_LINE;
       
      RTEMS.TASKS.WAKE_AFTER( 2, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
   
      case ARGUMENT is
         when 1 .. 4 =>
            loop
               RTEMS.RATE_MONOTONIC.PERIOD( 
                  RMID, 
                  SPTEST.PERIODS( INTEGER( ARGUMENT ) ), 
                  STATUS
               );

               TEST_SUPPORT.DIRECTIVE_FAILED( 
                  STATUS, 
                  "RATE_MONOTONIC_PERIOD" 
               );

               SPTEST.COUNT( INTEGER( ARGUMENT ) ) := 
                  SPTEST.COUNT( INTEGER( ARGUMENT ) ) + 1;

            end loop;

         when 5 =>

            PASS   := 0;
            FAILED := 0;

            RTEMS.RATE_MONOTONIC.PERIOD( 
               RMID, 
               SPTEST.PERIODS( INTEGER( ARGUMENT ) ),
               STATUS
            );

            TEST_SUPPORT.DIRECTIVE_FAILED( 
               STATUS, 
               "RATE_MONOTONIC_PERIOD 1 OF TA5" 
            );

            SPTEST.GET_ALL_COUNTERS;

            loop

               RTEMS.RATE_MONOTONIC.PERIOD( 
                  RMID, 
                  SPTEST.PERIODS( INTEGER( ARGUMENT ) ),
                  STATUS
               );

               TEST_SUPPORT.DIRECTIVE_FAILED( 
                  STATUS, 
                  "RATE_MONOTONIC_PERIOD 2 OF TA5" 
               );

               SPTEST.GET_ALL_COUNTERS;

               for INDEX in 1 .. 4 loop

                  if SPTEST.TEMPORARY_COUNT( INDEX ) /= 
                        SPTEST.ITERATIONS( INDEX ) then

                     TEXT_IO.PUT( "FAIL -- " );
                     TEST_SUPPORT.PUT_NAME( 
                        SPTEST.TASK_NAME( INDEX ),
                        FALSE
                     );
                     TEXT_IO.PUT( "ACTUAL=" );
                     UNSIGNED32_IO.PUT( 
                        SPTEST.TEMPORARY_COUNT( INDEX ), 
                        WIDTH => 3, 
                        BASE => 10 
                     );
                     TEXT_IO.PUT( " EXPECTED=" );
                     UNSIGNED32_IO.PUT( 
                        SPTEST.ITERATIONS( INDEX ), 
                        WIDTH => 3, 
                        BASE => 10 
                     );
                     TEXT_IO.NEW_LINE;

                     FAILED := FAILED + 1;

                  end if;

               end loop;

               if FAILED = 5 then
                  RTEMS.SHUTDOWN_EXECUTIVE( 0 );
               end if;

               PASS := PASS + 1;
               TEXT_IO.PUT( "TA5 - PERIODS CHECK OK (" );
               UNSIGNED32_IO.PUT( PASS, WIDTH => 1, BASE => 10 );
               TEXT_IO.PUT_LINE( ")" );
 
               if PASS = 10 then
                  TEXT_IO.PUT_LINE( "*** END OF TEST 20 ***" );
                  RTEMS.SHUTDOWN_EXECUTIVE( 0 );
               end if;

            end loop;
 
         when 6 =>
            -- test changing periods
            for INDEX in 0 .. 10 loop
               PERIOD := RTEMS.INTERVAL( ( INDEX + 1 ) * 10 );
               RTEMS.RATE_MONOTONIC.PERIOD( RMID, PERIOD, STATUS);
               TEST_SUPPORT.DIRECTIVE_FAILED(
                  STATUS, "rate_monotonic_period of TA6"
               );

               -- timestamp
               RTEMS.CLOCK.GET(
                  RTEMS.CLOCK.GET_TICKS_SINCE_BOOT,
                  Time( INDEX )'ADDRESS,
                  STATUS
               );
               TEST_SUPPORT.DIRECTIVE_FAILED(
                  STATUS, "clock_get of TA6"
               );
            end loop;

            for INDEX in 1 .. 10 loop
               MEASURE := TIME( INDEX ) - TIME( INDEX - 1 );
               PERIOD := RTEMS.INTERVAL( INDEX * 10 );
               TEXT_IO.PUT( "TA6 - Actual: " );
               UNSIGNED32_IO.PUT( MEASURE, WIDTH => 3, BASE => 10 );
               TEXT_IO.PUT( "  Expected: " );
               UNSIGNED32_IO.PUT( PERIOD, WIDTH => 3, BASE => 10 );
               if PERIOD = MEASURE then
                  TEXT_IO.PUT_LINE( " - OK" );
               else
                  TEXT_IO.PUT_LINE( " - FAILED" );
               end if;
            end loop;

            RTEMS.TASKS.SUSPEND( RTEMS.SELF, STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED(
               STATUS, "task_suspend of TA6"
            );

         when others =>
            NULL;
            
      end case;

   end TASK_1_THROUGH_6;

-- 
--  GET_ALL_COUNTERS
--

   procedure GET_ALL_COUNTERS
   is
      PREVIOUS_MODE : RTEMS.MODE;
      STATUS        : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASKS.MODE(
         RTEMS.NO_PREEMPT,
         RTEMS.PREEMPT_MASK,
         PREVIOUS_MODE,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE TO NO_PREEMPT" );

      SPTEST.TEMPORARY_COUNT := SPTEST.COUNT;

      for INDEX in 1 .. 5
      loop

         SPTEST.COUNT( INDEX ) := 0;

      end loop;
      
      RTEMS.TASKS.MODE(
         RTEMS.PREEMPT,
         RTEMS.PREEMPT_MASK,
         PREVIOUS_MODE,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE TO PREEMPT" );

   end GET_ALL_COUNTERS;

end SPTEST;
