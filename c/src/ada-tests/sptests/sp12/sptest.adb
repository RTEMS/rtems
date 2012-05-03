--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 12 of the RTEMS
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
with RTEMS.SEMAPHORE;

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
      TEXT_IO.PUT_LINE( "*** TEST 12 ***" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );
      SPTEST.TASK_NAME( 3 ) := RTEMS.BUILD_NAME(  'T', 'A', '3', ' ' );
      SPTEST.TASK_NAME( 4 ) := RTEMS.BUILD_NAME(  'T', 'A', '4', ' ' );
      SPTEST.TASK_NAME( 5 ) := RTEMS.BUILD_NAME(  'T', 'A', '5', ' ' );

      SPTEST.PRIORITY_TASK_NAME( 1 ) := 
         RTEMS.BUILD_NAME(  'P', 'R', 'I', '1' );
      SPTEST.PRIORITY_TASK_NAME( 2 ) := 
         RTEMS.BUILD_NAME(  'P', 'R', 'I', '2' );
      SPTEST.PRIORITY_TASK_NAME( 3 ) := 
         RTEMS.BUILD_NAME(  'P', 'R', 'I', '3' );
      SPTEST.PRIORITY_TASK_NAME( 4 ) := 
         RTEMS.BUILD_NAME(  'P', 'R', 'I', '4' );
      SPTEST.PRIORITY_TASK_NAME( 5 ) := 
         RTEMS.BUILD_NAME(  'P', 'R', 'I', '5' );

      SPTEST.SEMAPHORE_NAME( 1 ) := RTEMS.BUILD_NAME(  'S', 'M', '1', ' ' );
      SPTEST.SEMAPHORE_NAME( 2 ) := RTEMS.BUILD_NAME(  'S', 'M', '2', ' ' );
      SPTEST.SEMAPHORE_NAME( 3 ) := RTEMS.BUILD_NAME(  'S', 'M', '3', ' ' );

      RTEMS.SEMAPHORE.CREATE( 
         SPTEST.SEMAPHORE_NAME( 1 ), 
         1,
         RTEMS.DEFAULT_ATTRIBUTES,
         RTEMS.TASKS.NO_PRIORITY,
         SPTEST.SEMAPHORE_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE OF SM1" );

      RTEMS.SEMAPHORE.CREATE( 
         SPTEST.SEMAPHORE_NAME( 2 ), 
         0,
         RTEMS.PRIORITY,
         RTEMS.TASKS.NO_PRIORITY,
         SPTEST.SEMAPHORE_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE OF SM2" );

      RTEMS.SEMAPHORE.CREATE( 
         SPTEST.SEMAPHORE_NAME( 3 ), 
         1,
         RTEMS.DEFAULT_ATTRIBUTES,
         RTEMS.TASKS.NO_PRIORITY,
         SPTEST.SEMAPHORE_ID( 3 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE OF SM3" );

      TEXT_IO.PUT_LINE( "INIT - Forward priority queue test" );
      SPTEST.PRIORITY_TEST_DRIVER( 0 );

      TEXT_IO.PUT_LINE( "INIT - Backward priority queue test" );
      SPTEST.PRIORITY_TEST_DRIVER( 32 );

TEST_SUPPORT.PAUSE;

      TEXT_IO.PUT_LINE( 
        "INIT - Binary Semaphore and Priority Inheritance Test" 
      );

      RTEMS.SEMAPHORE.DELETE( SPTEST.SEMAPHORE_ID( 2 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_DELETE OF SM2" );

      TEXT_IO.PUT_LINE( 
        "INIT - semaphore_create - allocated binary semaphore"
      );
      RTEMS.SEMAPHORE.CREATE( 
         SPTEST.SEMAPHORE_NAME( 2 ), 
         0,
         RTEMS.BINARY_SEMAPHORE + RTEMS.PRIORITY + RTEMS.INHERIT_PRIORITY,
         RTEMS.TASKS.NO_PRIORITY,
         SPTEST.SEMAPHORE_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE OF SM2" );

      TEXT_IO.PUT_LINE( 
        "INIT - semaphore_release - allocated binary semaphore"
      );
      RTEMS.SEMAPHORE.RELEASE( SPTEST.SEMAPHORE_ID( 2 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE OF SM2" );

      TEXT_IO.PUT_LINE( 
        "INIT - semaphore_delete - allocated binary semaphore"
      );
      RTEMS.SEMAPHORE.DELETE( SPTEST.SEMAPHORE_ID( 2 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_DELETE OF SM2" );

      RTEMS.SEMAPHORE.CREATE( 
         SPTEST.SEMAPHORE_NAME( 2 ), 
         1,
         RTEMS.BINARY_SEMAPHORE + RTEMS.PRIORITY + RTEMS.INHERIT_PRIORITY,
         RTEMS.TASKS.NO_PRIORITY,
         SPTEST.SEMAPHORE_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE OF SM2" );

      SPTEST.PRIORITY_TEST_DRIVER( 64 );

TEST_SUPPORT.PAUSE;

      RTEMS.SEMAPHORE.DELETE( SPTEST.SEMAPHORE_ID( 2 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_DELETE OF SM2" );

      RTEMS.SEMAPHORE.CREATE( 
         SPTEST.SEMAPHORE_NAME( 2 ), 
         0,
         RTEMS.PRIORITY,
         RTEMS.TASKS.NO_PRIORITY,
         SPTEST.SEMAPHORE_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE OF SM2" );

      RTEMS.SEMAPHORE.RELEASE( SPTEST.SEMAPHORE_ID( 2 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE OF SM2" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         4, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 2 ), 
         4, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA2" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 3 ), 
         4, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 3 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA3" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 1 ),
         SPTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 2 ),
         SPTEST.TASK_2'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA2" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 3 ),
         SPTEST.TASK_3'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA3" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  PRIORITY_TEST_DRIVER
--

   procedure PRIORITY_TEST_DRIVER (
      PRIORITY_BASE : in     RTEMS.UNSIGNED32
   ) is 
      PREVIOUS_PRIORITY : RTEMS.TASKS.PRIORITY;
      STATUS            : RTEMS.STATUS_CODES;
   begin

      for INDEX in 1 .. 5
      loop

         case INDEX is
            when 1 | 2 | 3 => 
               SPTEST.TASK_PRIORITY( INDEX )  := 
                  PRIORITY_BASE + RTEMS.TASKS.PRIORITY( INDEX );
            when others    => 
               SPTEST.TASK_PRIORITY( INDEX )  := PRIORITY_BASE + 3;
         end case;

         RTEMS.TASKS.CREATE( 
            SPTEST.PRIORITY_TASK_NAME( INDEX ), 
            SPTEST.TASK_PRIORITY( INDEX ), 
            2048, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            SPTEST.PRIORITY_TASK_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

      end loop;

      if PRIORITY_BASE = 0 then

         for INDEX in 1 .. 5
         loop

            RTEMS.TASKS.START( 
               SPTEST.PRIORITY_TASK_ID( INDEX ),
               SPTEST.PRIORITY_TASK'ACCESS,
               RTEMS.TASKS.ARGUMENT( INDEX ), 
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );
      
         end loop;

      else

         for INDEX in reverse 1 .. 5
         loop

            RTEMS.TASKS.START( 
               SPTEST.PRIORITY_TASK_ID( INDEX ),
               SPTEST.PRIORITY_TASK'ACCESS,
               RTEMS.TASKS.ARGUMENT( INDEX ), 
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );
      
            RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER LOOP" );

            if PRIORITY_BASE = 64 then
               if INDEX = 4 then
                  RTEMS.TASKS.SET_PRIORITY( 
                     SPTEST.PRIORITY_TASK_ID( 5 ),
                     PRIORITY_BASE + 4,
                     PREVIOUS_PRIORITY,
                     STATUS
                  );
                  TEXT_IO.PUT( "PDRV - change priority of PRI5 from " );
                  UNSIGNED32_IO.PUT( PREVIOUS_PRIORITY );
                  TEXT_IO.PUT( " to " );
                  UNSIGNED32_IO.PUT( PRIORITY_BASE + 4 );
                  TEXT_IO.NEW_LINE;
                  TEST_SUPPORT.DIRECTIVE_FAILED( 
                     STATUS, 
                     "PDRV TASK_SET_PRIORITY" 
                  );
               end if;

               RTEMS.TASKS.SET_PRIORITY( 
                  SPTEST.PRIORITY_TASK_ID( 5 ),
                  RTEMS.TASKS.CURRENT_PRIORITY,
                  PREVIOUS_PRIORITY,
                  STATUS
               );
               TEXT_IO.PUT( "PDRV - priority of PRI5 is " );
               UNSIGNED32_IO.PUT( PREVIOUS_PRIORITY );
               TEXT_IO.NEW_LINE;
               TEST_SUPPORT.DIRECTIVE_FAILED( 
                  STATUS, 
                  "PDRV TASK_SET_PRIORITY" 
               );
            end if;

         end loop;

      end if;

      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER AFTER LOOP" );

      if PRIORITY_BASE = 0 then
         for INDEX in 1 .. 5
         loop

            RTEMS.SEMAPHORE.RELEASE( 
               SPTEST.SEMAPHORE_ID( 2 ),
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( 
               STATUS,
               "SEMAPHORE_RELEASE LOOP"
            );

         end loop;
      end if;

      if PRIORITY_BASE = 64 then

         TEXT_IO.PUT_LINE( "PDRV - task_resume - PRI5" );
         RTEMS.TASKS.RESUME( SPTEST.PRIORITY_TASK_ID( 5 ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESUME" );

         RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( 
           STATUS, 
           "TASK_WAKE_AFTER SO PRI5 can run" 
         );

         RTEMS.TASKS.DELETE( SPTEST.PRIORITY_TASK_ID( 5 ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF PRI5" );
      else

         for INDEX in 1 .. 5
         loop

            RTEMS.TASKS.DELETE( 
               SPTEST.PRIORITY_TASK_ID( INDEX ),
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE LOOP" );

         end loop;
      end if;


   end PRIORITY_TEST_DRIVER;

-- 
--  PRIORITY_TASK
--

   procedure PRIORITY_TASK (
      ITS_INDEX : in     RTEMS.TASKS.ARGUMENT
   ) is 
      TIMEOUT          : RTEMS.INTERVAL;
      ITS_PRIORITY     : RTEMS.TASKS.PRIORITY;
      CURRENT_PRIORITY : RTEMS.TASKS.PRIORITY;
      STATUS           : RTEMS.STATUS_CODES;
   begin

      ITS_PRIORITY := SPTEST.TASK_PRIORITY( INTEGER( ITS_INDEX ) );

      if ITS_PRIORITY < 3 then
         TIMEOUT := 5 * TEST_SUPPORT.TICKS_PER_SECOND;
      else
         TIMEOUT := RTEMS.NO_TIMEOUT;
      end if;

      TEST_SUPPORT.PUT_NAME( PRIORITY_TASK_NAME( INTEGER(ITS_INDEX) ), FALSE );
      TEXT_IO.PUT_LINE( " - semaphore_obtain - wait forever on SM2" );
   
      RTEMS.SEMAPHORE.OBTAIN( 
         SPTEST.SEMAPHORE_ID( 2 ), 
         RTEMS.DEFAULT_OPTIONS, 
         TIMEOUT, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN OF SM2" );

      if ITS_PRIORITY < 64 then
         TEXT_IO.PUT( "PRI" );
         UNSIGNED32_IO.PUT( ITS_INDEX );
         TEXT_IO.PUT( " - WHY AM I HERE? (priority = " );
         UNSIGNED32_IO.PUT( ITS_PRIORITY );
         TEXT_IO.PUT_LINE( " )" );
         RTEMS.SHUTDOWN_EXECUTIVE( 0 );
      end if;

      if ITS_INDEX = 5 then
         TEXT_IO.PUT_LINE(
            "PRI5 - task_suspend - until all priority tasks blocked" 
         );
      end if;

      RTEMS.TASKS.SUSPEND( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );

      TEXT_IO.PUT_LINE( "PRI5 - task_delete - all tasks waiting on SM2" );
      for INDEX in 1 .. 4 
      loop

         RTEMS.TASKS.DELETE(  SPTEST.PRIORITY_TASK_ID( INDEX ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE LOOP" );

      end loop;

      TEXT_IO.PUT_LINE( "PRI5 - semaphore_obtain - nested" );
      RTEMS.SEMAPHORE.OBTAIN( 
         SPTEST.SEMAPHORE_ID( 2 ), 
         RTEMS.DEFAULT_OPTIONS, 
         TIMEOUT, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN NESTED" );

      TEXT_IO.PUT_LINE( "PRI5 - semaphore_release - nested" );
      RTEMS.SEMAPHORE.RELEASE( SPTEST.SEMAPHORE_ID( 2 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE NESTED" );

      TEXT_IO.PUT_LINE( "PRI5 - semaphore_release - restore priority" );
      RTEMS.SEMAPHORE.RELEASE( SPTEST.SEMAPHORE_ID( 2 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE" );

      RTEMS.TASKS.SET_PRIORITY( 
         SPTEST.PRIORITY_TASK_ID( 5 ),
         RTEMS.TASKS.CURRENT_PRIORITY,
         CURRENT_PRIORITY,
         STATUS
      );
      TEXT_IO.PUT( "PRI5 - priority of PRI5 is " );
      UNSIGNED32_IO.PUT( CURRENT_PRIORITY );
      TEXT_IO.NEW_LINE;
      TEST_SUPPORT.DIRECTIVE_FAILED( 
         STATUS, 
         "PRI5 TASK_SET_PRIORITY CURRENT" 
      );
      RTEMS.TASKS.SUSPEND( RTEMS.SELF, STATUS );
      -- DOES NOT RETURN.  Following check makes compiler happy
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );

   end PRIORITY_TASK;
   
-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      SMID   : RTEMS.ID;
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.SEMAPHORE.IDENT( 
         SPTEST.SEMAPHORE_NAME( 1 ), 
         RTEMS.SEARCH_ALL_NODES, 
         SMID, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_IDENT OF SM1" );
      TEXT_IO.PUT( "TA1 - semaphore_ident - smid => " );
      UNSIGNED32_IO.PUT( SMID, WIDTH => 8, BASE => 16 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( "TA1 - semaphore_obtain - wait forever on SM2" );
      RTEMS.SEMAPHORE.OBTAIN( 
         SPTEST.SEMAPHORE_ID( 2 ),
         RTEMS.DEFAULT_MODES, 
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN ON SM2" );
     
      TEXT_IO.PUT_LINE( "TA1 - got SM2" );

      TEXT_IO.PUT_LINE( "TA1 - semaphore_obtain - wait forever on SM3" );
      RTEMS.SEMAPHORE.OBTAIN( 
         SPTEST.SEMAPHORE_ID( 3 ),
         RTEMS.DEFAULT_MODES, 
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN ON SM3" );
     
      TEXT_IO.PUT_LINE( "TA1 - got SM3" );
   
      TEXT_IO.PUT_LINE( "TA1 - semaphore_obtain - get SM1 - NO_WAIT" );
      RTEMS.SEMAPHORE.OBTAIN( 
         SPTEST.SEMAPHORE_ID( 1 ),
         RTEMS.NO_WAIT, 
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN ON SM1" );
     
      TEXT_IO.PUT_LINE( "TA1 - got SM1" );

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - sleep 5 seconds" );
      RTEMS.TASKS.WAKE_AFTER( 5 * TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

TEST_SUPPORT.PAUSE;

      TEXT_IO.PUT_LINE( "TA1 - semaphore_release - release SM1" );
      RTEMS.SEMAPHORE.RELEASE( SPTEST.SEMAPHORE_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE ON SM1" );
     
      TEXT_IO.PUT_LINE( 
         "TA1 - semaphore_obtain - waiting for SM1 with 10 second timeout"
      );
      RTEMS.SEMAPHORE.OBTAIN( 
         SPTEST.SEMAPHORE_ID( 1 ),
         RTEMS.DEFAULT_MODES, 
         10 * TEST_SUPPORT.TICKS_PER_SECOND,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN ON SM3" );
     
      TEXT_IO.PUT_LINE( "TA1 - got SM1" );
   
      TEXT_IO.PUT_LINE( "TA1 - semaphore_release - release SM2" );
      RTEMS.SEMAPHORE.RELEASE( 
         SPTEST.SEMAPHORE_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE ON SM2" );

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - sleep 5 seconds" );
      RTEMS.TASKS.WAKE_AFTER( 5 * TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

TEST_SUPPORT.PAUSE;

      TEXT_IO.PUT_LINE( "TA1 - task_delete - delete TA3" );
      RTEMS.TASKS.DELETE( SPTEST.TASK_ID( 3 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA3" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 4 ), 
         4, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 4 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA4" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 5 ), 
         4, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 5 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA5" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 4 ),
         SPTEST.TASK_4'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA4" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 5 ),
         SPTEST.TASK_5'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA5" );

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - sleep 5 seconds" );
      RTEMS.TASKS.WAKE_AFTER( 5 * TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

      TEXT_IO.PUT_LINE( "TA1 - task_delete - delete TA4" );
      RTEMS.TASKS.DELETE( SPTEST.TASK_ID( 4 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA4" );

      TEXT_IO.PUT_LINE( "TA1 - semaphore_release - release SM1" );
      RTEMS.SEMAPHORE.RELEASE( 
         SPTEST.SEMAPHORE_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE ON SM1" );

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - sleep 5 seconds" );
      RTEMS.TASKS.WAKE_AFTER( 5 * TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

      TEXT_IO.PUT_LINE( "TA1 - semaphore_delete - delete SM1" );
      RTEMS.SEMAPHORE.DELETE( SPTEST.SEMAPHORE_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_DELETE OF SM1" );

      TEXT_IO.PUT_LINE( "TA1 - semaphore_delete - delete SM3" );
      RTEMS.SEMAPHORE.DELETE( SPTEST.SEMAPHORE_ID( 3 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_DELETE OF SM3" );

      TEXT_IO.PUT_LINE( "TA1 - task_delete - delete self" );
      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA1" );

   end TASK_1;

-- 
--  TASK_2
--

   procedure TASK_2 (
      ARGUMENT : in    RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS            : RTEMS.STATUS_CODES;
      PREVIOUS_PRIORITY : RTEMS.TASKS.PRIORITY;
   begin
 
      TEXT_IO.PUT_LINE( "TA2 - semaphore_obtain - wait forever on SM1" );
      RTEMS.SEMAPHORE.OBTAIN( 
         SPTEST.SEMAPHORE_ID( 1 ),
         RTEMS.DEFAULT_MODES, 
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN ON SM1" );
     
      TEXT_IO.PUT_LINE( "TA2 - got SM1" );
   
      TEXT_IO.PUT_LINE( "TA2 - semaphore_release - release SM1" );
      RTEMS.SEMAPHORE.RELEASE( 
         SPTEST.SEMAPHORE_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE ON SM1" );

      TEXT_IO.PUT_LINE( 
         "TA2 - task_set_priority - make self highest priority task" 
      );
      RTEMS.TASKS.SET_PRIORITY(
         RTEMS.SELF,
         3,
         PREVIOUS_PRIORITY,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY ON TA2" );
     
      TEXT_IO.PUT_LINE( "TA2 - semaphore_obtain - wait forever on SM2" );
      RTEMS.SEMAPHORE.OBTAIN( 
         SPTEST.SEMAPHORE_ID( 2 ),
         RTEMS.DEFAULT_MODES, 
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN ON SM2" );
     
      TEXT_IO.PUT_LINE( "TA2 - got SM2" );
   
      TEXT_IO.PUT_LINE( "TA2 - semaphore_release - release SM2" );
      RTEMS.SEMAPHORE.RELEASE( 
         SPTEST.SEMAPHORE_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE ON SM2" );
   
      TEXT_IO.PUT_LINE( "TA2 - task_delete - delete self" );
      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA2" );

   end TASK_2;

-- 
--  TASK_3
--

   procedure TASK_3 (
      ARGUMENT : in    RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS : RTEMS.STATUS_CODES;
   begin
 
      TEXT_IO.PUT_LINE( "TA3 - semaphore_obtain - wait forever on SM2" );
      RTEMS.SEMAPHORE.OBTAIN( 
         SPTEST.SEMAPHORE_ID( 2 ),
         RTEMS.DEFAULT_MODES, 
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN ON SM2" );
      TEXT_IO.PUT_LINE( "TA3 - got SM2" );
   
      TEXT_IO.PUT_LINE( "TA3 - semaphore_release - release SM2" );
      RTEMS.SEMAPHORE.RELEASE( 
         SPTEST.SEMAPHORE_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE ON SM2" );
   
      TEXT_IO.PUT_LINE( "TA3 - semaphore_obtain - wait forever on SM3" );
      RTEMS.SEMAPHORE.OBTAIN( 
         SPTEST.SEMAPHORE_ID( 3 ),
         RTEMS.DEFAULT_MODES, 
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN ON SM3" );
     
   end TASK_3;

-- 
--  TASK_4
--

   procedure TASK_4 (
      ARGUMENT : in    RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS : RTEMS.STATUS_CODES;
   begin
 
      TEXT_IO.PUT_LINE( "TA4 - semaphore_obtain - wait forever on SM1" );
      RTEMS.SEMAPHORE.OBTAIN( 
         SPTEST.SEMAPHORE_ID( 1 ),
         RTEMS.DEFAULT_MODES, 
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN ON SM1" );
     
   end TASK_4;

-- 
--  TASK_5
--

   procedure TASK_5 (
      ARGUMENT : in    RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS : RTEMS.STATUS_CODES;
   begin
 
      TEXT_IO.PUT_LINE( "TA5 - semaphore_obtain - wait forever on SM1" );
      RTEMS.SEMAPHORE.OBTAIN( 
         SPTEST.SEMAPHORE_ID( 1 ),
         RTEMS.DEFAULT_MODES, 
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN ON SM1" );
     
      TEXT_IO.PUT_LINE( "TA5 - got SM1" );
   
      TEXT_IO.PUT_LINE( "TA5 - semaphore_obtain - wait forever on SM1" );
      RTEMS.SEMAPHORE.OBTAIN( 
         SPTEST.SEMAPHORE_ID( 1 ),
         RTEMS.DEFAULT_MODES, 
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS( 
         STATUS, 
         RTEMS.OBJECT_WAS_DELETED, 
         "SEMAPHORE_OBTAIN ON SM1" 
      );
      TEXT_IO.PUT_LINE( "TA5 - SM1 deleted by TA1" );
    
      TEXT_IO.PUT_LINE( "*** END OF TEST 12 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );
   
   end TASK_5;

end SPTEST;
