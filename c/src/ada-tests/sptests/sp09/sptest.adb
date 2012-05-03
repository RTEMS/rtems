--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 9 of the RTEMS
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
with RTEMS.CLOCK;
with RTEMS.DEBUG;
with RTEMS.EVENT;
with RTEMS.MESSAGE_QUEUE;
with RTEMS.PARTITION;
with RTEMS.PORT;
with RTEMS.RATE_MONOTONIC;
with RTEMS.REGION;
with RTEMS.SEMAPHORE;
with RTEMS.TIMER;

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
      TEXT_IO.PUT_LINE( "*** TEST 9 ***" );

      SPTEST.TASK_NAME( 1 )   := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TASK_NAME( 2 )   := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );
      SPTEST.TASK_NAME( 3 )   := RTEMS.BUILD_NAME(  'T', 'A', '3', ' ' );
      SPTEST.TASK_NAME( 4 )   := RTEMS.BUILD_NAME(  'T', 'A', '4', ' ' );
      SPTEST.TASK_NAME( 5 )   := RTEMS.BUILD_NAME(  'T', 'A', '5', ' ' );
      SPTEST.TASK_NAME( 6 )   := RTEMS.BUILD_NAME(  'T', 'A', '6', ' ' );
      SPTEST.TASK_NAME( 7 )   := RTEMS.BUILD_NAME(  'T', 'A', '7', ' ' );
      SPTEST.TASK_NAME( 8 )   := RTEMS.BUILD_NAME(  'T', 'A', '8', ' ' );
      SPTEST.TASK_NAME( 9 )   := RTEMS.BUILD_NAME(  'T', 'A', '9', ' ' );
      SPTEST.TASK_NAME( 10 )  := RTEMS.BUILD_NAME(  'T', 'A', 'A', ' ' );

      SPTEST.TIMER_NAME( 1 )   := RTEMS.BUILD_NAME(  'T', 'M', '1', ' ' );

      SPTEST.SEMAPHORE_NAME( 1 ) := RTEMS.BUILD_NAME( 'S', 'M', '1', ' ' );
      SPTEST.SEMAPHORE_NAME( 2 ) := RTEMS.BUILD_NAME( 'S', 'M', '2', ' ' );
      SPTEST.SEMAPHORE_NAME( 3 ) := RTEMS.BUILD_NAME( 'S', 'M', '3', ' ' );

      SPTEST.QUEUE_NAME( 1 )     := RTEMS.BUILD_NAME( 'M', 'Q', '1', ' ' );
      SPTEST.QUEUE_NAME( 2 )     := RTEMS.BUILD_NAME( 'M', 'Q', '2', ' ' );

      SPTEST.PARTITION_NAME( 1 ) := RTEMS.BUILD_NAME( 'P', 'T', '1', ' ' );

      SPTEST.REGION_NAME( 1 )    := RTEMS.BUILD_NAME( 'R', 'N', '1', ' ' );

      SPTEST.PORT_NAME( 1 )      := RTEMS.BUILD_NAME( 'D', 'P', '1', ' ' );

      SPTEST.PERIOD_NAME( 1 )    := RTEMS.BUILD_NAME( 'T', 'M', '1', ' ' );

      RTEMS.TASKS.CREATE(
         SPTEST.TASK_NAME( 1 ),
         0,
         2048,
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_PRIORITY,
         "TASK_CREATE WITH ILLEGAL PRIORITY"
      );
      TEXT_IO.PUT_LINE( "INIT - task_create - INVALID_PRIORITY" );

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

      RTEMS.TASKS.RESTART(
         SPTEST.TASK_ID( 1 ),
         0,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INCORRECT_STATE,
         "TASK_RESTART OF DORMANT TASK"
      );
      TEXT_IO.PUT_LINE( "INIT - task_restart - INCORRECT_STATE" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 1 ),
         SPTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  DELAYED_SUBPROGRAM
--

   procedure DELAYED_SUBPROGRAM (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   ) is
   begin

      NULL;

   end DELAYED_SUBPROGRAM;

-- 
--  SCREEN_1
--

   procedure SCREEN_1
   is
      NOTEPAD_VALUE     : RTEMS.UNSIGNED32 := 0;
      SELF_ID           : RTEMS.ID;
      PREVIOUS_PRIORITY : RTEMS.TASKS.PRIORITY;
      STATUS            : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASKS.DELETE( 100, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TASK_DELETE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - task_delete - INVALID_ID" );

      begin
        RTEMS.TASKS.GET_NOTE( RTEMS.SELF, 
                             RTEMS.NOTEPAD_INDEX'LAST + 10, 
                             NOTEPAD_VALUE, 
                             STATUS 
        );
        TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
           STATUS,
           RTEMS.INVALID_NUMBER,
           "TASK_GET_NOTE WITH ILLEGAL NOTEPAD"
        );
        TEXT_IO.PUT_LINE( "TA1 - task_get_note - INVALID_NUMBER" );
      exception
         when others =>
            TEXT_IO.PUT_LINE(
               "TA1 - task_get_note - INVALID_NUMBER -- constraint error"
            );
      end;

      RTEMS.TASKS.GET_NOTE( 
         100, 
         RTEMS.NOTEPAD_INDEX'LAST, 
         NOTEPAD_VALUE, 
         STATUS 
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TASK_GET_NOTE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - task_get_note - INVALID_ID" );

      RTEMS.TASKS.IDENT(
         RTEMS.SELF,
         RTEMS.SEARCH_ALL_NODES, 
         SELF_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF SELF" );
      if SELF_ID /= SPTEST.TASK_ID( 1 ) then
         TEXT_IO.PUT_LINE( "ERROR - task_ident - incorrect ID returned!" );
         RTEMS.SHUTDOWN_EXECUTIVE( 0 );
      end if;
      TEXT_IO.PUT_LINE(
         "TA1 - task_ident - current task SUCCESSFUL"
      );

      RTEMS.TASKS.IDENT(
         100,
         RTEMS.SEARCH_ALL_NODES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "TASK_IDENT WITH ILLEGAL NAME (local)"
      );
      TEXT_IO.PUT_LINE( "TA1 - task_ident - global INVALID_NAME" );

      RTEMS.TASKS.IDENT( 100, 1, SPTEST.JUNK_ID, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "TASK_IDENT WITH ILLEGAL NAME (global)"
      );
      TEXT_IO.PUT_LINE( "TA1 - task_ident - local INVALID_NAME" );

      RTEMS.TASKS.IDENT( 100, 2, SPTEST.JUNK_ID, STATUS );
      if TEST_SUPPORT.Is_Configured_Multiprocessing then
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.INVALID_NODE,
            "TASK_IDENT WITH ILLEGAL NODE"
         );
      else
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.INVALID_NAME,
            "TASK_IDENT WITH ILLEGAL NODE"
         );
      end if;
      TEXT_IO.PUT_LINE( "TA1 - task_ident - INVALID_NODE" );

      RTEMS.TASKS.RESTART( 100, 0, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TASK_RESTART WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - task_restart - INVALID_ID" );

      RTEMS.TASKS.RESUME( 100, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TASK_RESUME WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - task_resume - INVALID_ID" );

      RTEMS.TASKS.RESUME( RTEMS.SELF, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INCORRECT_STATE,
         "TASK_RESUME OF READY TASK"
      );
      TEXT_IO.PUT_LINE( "TA1 - task_resume - INCORRECT_STATE" );

      begin
        RTEMS.TASKS.SET_PRIORITY( RTEMS.SELF, 512, PREVIOUS_PRIORITY, STATUS );
        TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
           STATUS,
           RTEMS.INVALID_PRIORITY,
           "TASK_SET_PRIORITY WITH ILLEGAL PRIORITY"
        );
        TEXT_IO.PUT_LINE(
           "TA1 - task_set_priority - INVALID_PRIORITY"
        );
      exception
         when others =>
            TEXT_IO.PUT_LINE(
               "TA1 - task_set_priority - INVALID_PRIORITY -- constraint error"
            );
      end;

      RTEMS.TASKS.SET_PRIORITY( 100, 8, PREVIOUS_PRIORITY, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TASK_SET_PRIORITY WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - task_set_priority - INVALID_ID" );

      begin
         RTEMS.TASKS.SET_NOTE( RTEMS.SELF, 
                              RTEMS.NOTEPAD_INDEX'LAST + 10, 
                              NOTEPAD_VALUE, 
                              STATUS 
         );
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.INVALID_NUMBER,
            "TASK_SET_NOTE WITH ILLEGAL NOTEPAD"
         );
         TEXT_IO.PUT_LINE( "TA1 - task_set_note - INVALID_NUMBER" );
      exception
         when others =>
            TEXT_IO.PUT_LINE(
               "TA1 - task_set_note - INVALID_NUMBER -- constraint error"
            );
      end;

      RTEMS.TASKS.SET_NOTE( 
         100, 
         RTEMS.NOTEPAD_INDEX'LAST, 
         NOTEPAD_VALUE, 
         STATUS 
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TASK_SET_NOTE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - task_set_note - INVALID_ID" );

      RTEMS.TASKS.START( 100, SPTEST.TASK_1'ACCESS, 0, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TASK_START WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - task_start - INVALID_ID" );

      RTEMS.TASKS.START( RTEMS.SELF, SPTEST.TASK_1'ACCESS, 0, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INCORRECT_STATE,
         "TASK_START OF READY TASK"
      );
      TEXT_IO.PUT_LINE( "TA1 - task_start - INCORRECT_STATE" );

      RTEMS.TASKS.SUSPEND( 100, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TASK_SUSPEND WITH INVALID ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - task_suspend - INVALID_ID" );

   end SCREEN_1;

-- 
--  SCREEN_2
--

   procedure SCREEN_2
   is
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin

-- errors before clock is set

      RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
     
      if RTEMS.IS_STATUS_SUCCESSFUL( STATUS ) then
         TEXT_IO.PUT_LINE(
            "TA1 - clock_get - NOT_DEFINED -- DID THE BSP SET THE TIME OF DAY?"
         );
      else
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.NOT_DEFINED,
            "CLOCK_GET BEFORE CLOCK IS SET"
         );
         TEXT_IO.PUT_LINE( "TA1 - clock_get - NOT_DEFINED" );
      end if;

      RTEMS.TASKS.WAKE_WHEN( TIME, STATUS );
      if RTEMS.IS_STATUS_SUCCESSFUL( STATUS ) then
         TEXT_IO.PUT( "TA1 - task_wake_when - NOT_DEFINED -- " );
         TEXT_IO.PUT_LINE( "DID THE BSP SET THE TIME OF DAY?" );
      else
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.NOT_DEFINED,
            "TASK_WAKE_WHEN BEFORE CLOCK IS SET"
         );
         TEXT_IO.PUT_LINE( "TA1 - task_wake_when - NOT_DEFINED" );
      end if;

      RTEMS.TIMER.FIRE_WHEN(
         0,
         TIME,
         SPTEST.DELAYED_SUBPROGRAM'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );

      if RTEMS.IS_STATUS_SUCCESSFUL( STATUS ) then
         TEXT_IO.PUT( "TA1 - task_fire_when - NOT_DEFINED -- " );
         TEXT_IO.PUT_LINE( "DID THE BSP SET THE TIME OF DAY?" );
      else

         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.NOT_DEFINED,
            "TIMER_FIRE_WHEN BEFORE CLOCK IS SET"
         );
         TEXT_IO.PUT_LINE( "TA1 - timer_fire_when - NOT_DEFINED" );

      end if;

-- invalid time of day field

      TIME := ( 1987, 2, 5, 8, 30, 45, 0 );
      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_set - ", TIME, "" );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_CLOCK,
         "CLOCK_SET WITH INVALID YEAR"
      );
      TEXT_IO.PUT_LINE( " - INVALID_CLOCK" );

      TIME := ( 1988, 15, 5, 8, 30, 45, 0 );
      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_set - ", TIME, "" );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_CLOCK,
         "CLOCK_SET WITH INVALID MONTH"
      );
      TEXT_IO.PUT_LINE( " - INVALID_CLOCK" );

      TIME := ( 1988, 2, 32, 8, 30, 45, 0 );
      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_set - ", TIME, "" );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_CLOCK,
         "CLOCK_SET WITH INVALID DAY"
      );
      TEXT_IO.PUT_LINE( " - INVALID_CLOCK" );

      TIME := ( 1988, 2, 5, 25, 30, 45, 0 );
      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_set - ", TIME, "" );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_CLOCK,
         "CLOCK_SET WITH INVALID HOUR"
      );
      TEXT_IO.PUT_LINE( " - INVALID_CLOCK" );

      TIME := ( 1988, 2, 5, 8, 61, 45, 0 );
      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_set - ", TIME, "" );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_CLOCK,
         "CLOCK_SET WITH INVALID MINUTE"
      );
      TEXT_IO.PUT_LINE( " - INVALID_CLOCK" );

      TIME := ( 1988, 2, 5, 8, 30, 61, 0 );
      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_set - ", TIME, "" );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_CLOCK,
         "CLOCK_SET WITH INVALID SECOND"
      );
      TEXT_IO.PUT_LINE( " - INVALID_CLOCK" );

      TIME := ( 1988, 2, 5, 8, 30, 45, TEST_SUPPORT.TICKS_PER_SECOND + 1 );
      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_set - ", TIME, "" );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_CLOCK,
         "CLOCK_SET WITH INVALID TICKS PER SECOND"
      );
      TEXT_IO.PUT_LINE( " - INVALID_CLOCK" );

      TIME := ( 1988, 2, 5, 8, 30, 45, 0 );
      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_set - ", TIME, "" );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET SUCCESSFUL" );
      TEXT_IO.PUT_LINE( " - SUCCESSFUL" );

-- task_wake_when

      TIME := ( 1988, 2, 5, 8, 30, 48, TEST_SUPPORT.TICKS_PER_SECOND + 1 );
      TEXT_IO.PUT( "TA1 - task_wake_when - TICK INVALID - " );
      TEXT_IO.PUT_LINE( "sleep about 3 seconds" );
      RTEMS.TASKS.WAKE_WHEN( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED(
         STATUS,
         "TASK_WAKE_WHEN WITH INVALID TICKS PER SECOND"
      );
      TEXT_IO.PUT( "TA1 - task_wake_when - TICK INVALID - " );
      TEXT_IO.PUT_LINE( "woke up SUCCESSFUL" );

      TIME := ( 1961, 2, 5, 8, 30, 48, 0 );
      TEST_SUPPORT.PRINT_TIME( "TA1 - task_wake_when - ", TIME, "" );
      RTEMS.TASKS.WAKE_WHEN( TIME, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_CLOCK,
         "TASK_WAKE_WHEN WITH INVALID YEAR"
      );
      TEXT_IO.PUT_LINE( " - INVALID_CLOCK" );

      TIME := ( 1988, 2, 5, 25, 30, 48, 0 );
      TEST_SUPPORT.PRINT_TIME( "TA1 - task_wake_when - ", TIME, "" );
      RTEMS.TASKS.WAKE_WHEN( TIME, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_CLOCK,
         "TASK_WAKE_WHEN WITH INVALID HOUR"
      );
      TEXT_IO.PUT_LINE( " - INVALID_CLOCK" );

      RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET SUCCESSFUL" );
      TEST_SUPPORT.PRINT_TIME( "TA1 - current time - ", TIME, "" );
      TEXT_IO.NEW_LINE;

      TIME.MONTH := 1;
      TEST_SUPPORT.PRINT_TIME( "TA1 - task_wake_when - ", TIME, "" );
      RTEMS.TASKS.WAKE_WHEN( TIME, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_CLOCK,
         "TASK_WAKE_WHEN BEFORE CURRENT TIME"
      );
      TEXT_IO.PUT_LINE( " - INVALID_CLOCK" );

   end SCREEN_2;

-- 
--  SCREEN_3
--

   procedure SCREEN_3
   is
      TASK_NAME : RTEMS.NAME;
      STATUS    : RTEMS.STATUS_CODES;
   begin

      TASK_NAME := 1;

      RTEMS.TASKS.CREATE(
         0,
         1,
         2048,
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "TASK_CREATE WITH ILLEGAL NAME"
      );
      TEXT_IO.PUT_LINE( 
         "TA1 - task_create - INVALID_NAME"
      );

      RTEMS.TASKS.CREATE(
         TASK_NAME,
         1,
         TEST_SUPPORT.WORK_SPACE_SIZE,
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.UNSATISFIED,
         "TASK_CREATE WITH A STACK SIZE LARGER THAN THE WORKSPACE"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - task_create - stack size - UNSATISFIED"
      );

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
      TEXT_IO.PUT_LINE(
         "TA1 - task_create - TA2 created - SUCCESSFUL"
      );

      RTEMS.TASKS.SUSPEND( SPTEST.TASK_ID( 2 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND OF TA2" );
      TEXT_IO.PUT_LINE(
         "TA1 - task_suspend - suspend TA2 - SUCCESSFUL"
      );

      RTEMS.TASKS.SUSPEND( SPTEST.TASK_ID( 2 ), STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.ALREADY_SUSPENDED,
         "TASK_SUSPEND ON SUSPENDED TA2"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - task_suspend - suspend TA2 - ALREADY_SUSPENDED"
      );

      RTEMS.TASKS.RESUME( SPTEST.TASK_ID( 2 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESUME OF TA2" );
      TEXT_IO.PUT_LINE(
         "TA1 - task_resume - TA2 resumed - SUCCESSFUL"
      );

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
      TEXT_IO.PUT_LINE(
         "TA1 - task_create - TA3 created - SUCCESSFUL"
      );

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
      TEXT_IO.PUT_LINE(
         "TA1 - task_create - 4 created - SUCCESSFUL"
      );

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
      TEXT_IO.PUT_LINE(
         "TA1 - task_create - 5 created - SUCCESSFUL"
      );

      RTEMS.TASKS.CREATE(
         SPTEST.TASK_NAME( 6 ),
         4,
         2048,
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 6 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA6" );
      TEXT_IO.PUT_LINE(
         "TA1 - task_create - 6 created - SUCCESSFUL"
      );

      RTEMS.TASKS.CREATE(
         SPTEST.TASK_NAME( 7 ),
         4,
         2048,
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 7 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA7" );
      TEXT_IO.PUT_LINE(
         "TA1 - task_create - 7 created - SUCCESSFUL"
      );

      RTEMS.TASKS.CREATE(
         SPTEST.TASK_NAME( 8 ),
         4,
         2048,
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 8 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA8" );
      TEXT_IO.PUT_LINE(
         "TA1 - task_create - 8 created - SUCCESSFUL"
      );

      RTEMS.TASKS.CREATE(
         SPTEST.TASK_NAME( 9 ),
         4,
         2048,
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 9 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA9" );
      TEXT_IO.PUT_LINE(
         "TA1 - task_create - 9 created - SUCCESSFUL"
      );

      RTEMS.TASKS.CREATE(
         SPTEST.TASK_NAME( 10 ),
         4,
         2048,
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 10 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA10" );
      TEXT_IO.PUT_LINE(
         "TA1 - task_create - 10 created - SUCCESSFUL"
      );

      RTEMS.TASKS.CREATE(
         TASK_NAME,
         4,
         2048,
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TOO_MANY,
         "TASK_CREATE FOR TOO MANY TASKS"
      );
      TEXT_IO.PUT_LINE( "TA1 - task_create - 11 - TOO_MANY" );

      if TEST_SUPPORT.Is_Configured_Multiprocessing then
         RTEMS.TASKS.CREATE(
            TASK_NAME,
            4,
            2048,
            RTEMS.DEFAULT_MODES,
            RTEMS.GLOBAL,
            SPTEST.JUNK_ID,
            STATUS
         );
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.MP_NOT_CONFIGURED,
            "TASK_CREATE OF GLOBAL TASK IN SINGLE CPU SYSTEM"
         );
      end if;
      TEXT_IO.PUT_LINE( "TA1 - task_create - MP_NOT_CONFIGURED" );

   end SCREEN_3;

-- 
--  SCREEN_4
--

   procedure SCREEN_4
   is
      EVENT_OUT : RTEMS.EVENT_SET;
      TIME      : RTEMS.TIME_OF_DAY;
      STATUS    : RTEMS.STATUS_CODES;
   begin

      RTEMS.EVENT.RECEIVE(
         RTEMS.EVENT_16,
         RTEMS.NO_WAIT,
         RTEMS.NO_TIMEOUT,
         EVENT_OUT,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.UNSATISFIED,
         "EVENT_RECEIVE UNSATISFIED (ALL)"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - event_receive - UNSATISFIED ( all conditions )"
      );

      RTEMS.EVENT.RECEIVE(
         RTEMS.EVENT_16,
         RTEMS.NO_WAIT + RTEMS.EVENT_ANY,
         RTEMS.NO_TIMEOUT,
         EVENT_OUT,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.UNSATISFIED,
         "EVENT_RECEIVE UNSATISFIED (ANY)"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - event_receive - UNSATISFIED ( any conditions )"
      );

      TEXT_IO.PUT_LINE( "TA1 - event_receive - timeout in 3 seconds" );
      RTEMS.EVENT.RECEIVE(
         RTEMS.EVENT_16,
         RTEMS.DEFAULT_OPTIONS,
         3 * TEST_SUPPORT.TICKS_PER_SECOND,
         EVENT_OUT,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TIMEOUT,
         "EVENT_RECEIVE AFTER 3 SECOND TIMEOUT"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - event_receive - woke with TIMEOUT"
      );

      RTEMS.EVENT.SEND(
         100,
         RTEMS.EVENT_16,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "EVENT_SEND WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - event_send - INVALID_ID"
      );

      TEXT_IO.PUT_LINE( 
         "TA1 - task_wake_after - sleep 1 second - SUCCESSFUL" 
      );
      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER (1 SECOND)" );

      TIME := ( 1988, 2, 5, 8, 30, 45, 0 );
      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_set - ", TIME, "" );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET SUCCESSFUL" );
      TEXT_IO.PUT_LINE( " - SUCCESSFUL" );

   end SCREEN_4;

-- 
--  SCREEN_5
--

   procedure SCREEN_5
   is
      STATUS         : RTEMS.STATUS_CODES;
   begin

      RTEMS.SEMAPHORE.CREATE(
         0,
         1,
         RTEMS.DEFAULT_ATTRIBUTES,
         RTEMS.TASKS.NO_PRIORITY,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "SEMAPHORE_CREATE WITH ILLEGAL NAME"
      );
      TEXT_IO.PUT_LINE( "TA1 - semaphore_create - INVALID_NAME" );

      RTEMS.SEMAPHORE.CREATE(
         SPTEST.SEMAPHORE_NAME( 1 ),
         1,
         RTEMS.DEFAULT_ATTRIBUTES,
         RTEMS.TASKS.NO_PRIORITY,
         SPTEST.SEMAPHORE_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED(
         STATUS,
         "SEMAPHORE_CREATE SUCCESSFUL"
      );
      TEXT_IO.PUT_LINE( "TA1 - semaphore_create - 1 - SUCCESSFUL" );

      RTEMS.SEMAPHORE.CREATE(
         SPTEST.SEMAPHORE_NAME( 2 ),
         1,
         (RTEMS.BINARY_SEMAPHORE or RTEMS.PRIORITY or RTEMS.INHERIT_PRIORITY),
         RTEMS.TASKS.NO_PRIORITY,
         SPTEST.SEMAPHORE_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED(
         STATUS,
         "SEMAPHORE_CREATE SUCCESSFUL"
      );
      TEXT_IO.PUT_LINE( "TA1 - semaphore_create - 2 - SUCCESSFUL" );

      loop
         RTEMS.SEMAPHORE.CREATE(
            SPTEST.SEMAPHORE_NAME( 3 ),
            1,
            RTEMS.DEFAULT_ATTRIBUTES,
            RTEMS.TASKS.NO_PRIORITY,
            SPTEST.JUNK_ID,
            STATUS
         );

         exit when not RTEMS.ARE_STATUSES_EQUAL( STATUS, RTEMS.SUCCESSFUL );
      end loop;

      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TOO_MANY,
         "SEMAPHORE_CREATE OF TOO MANY"
      );
      TEXT_IO.PUT_LINE( "TA1 - semaphore_create - 3 - TOO_MANY" );

      RTEMS.SEMAPHORE.CREATE(
         SPTEST.SEMAPHORE_NAME( 1 ),
         1,
         RTEMS.INHERIT_PRIORITY + RTEMS.BINARY_SEMAPHORE + RTEMS.FIFO,
         RTEMS.TASKS.NO_PRIORITY,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.NOT_DEFINED,
         "SEMAPHORE_CREATE OF FIFO INHERIT PRIORITY"
      );
      TEXT_IO.PUT_LINE("TA1 - semaphore_create - NOT_DEFINED");

      RTEMS.SEMAPHORE.CREATE(
         SPTEST.SEMAPHORE_NAME( 1 ),
         1,
         RTEMS.INHERIT_PRIORITY + RTEMS.COUNTING_SEMAPHORE + RTEMS.PRIORITY,
         RTEMS.TASKS.NO_PRIORITY,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.NOT_DEFINED,
         "SEMAPHORE_CREATE OF COUNTING_SEMAPHORE INHERIT PRIORITY"
      );
      TEXT_IO.PUT_LINE("TA1 - semaphore_create - NOT_DEFINED");

      RTEMS.SEMAPHORE.CREATE(
         SPTEST.SEMAPHORE_NAME( 1 ),
         2,
         RTEMS.BINARY_SEMAPHORE,
         RTEMS.TASKS.NO_PRIORITY,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NUMBER,
         "SEMAPHORE_CREATE OF BINARY_SEMAPHORE with COUNT > 1"
      );
      TEXT_IO.PUT_LINE("TA1 - semaphore_create - INVALID_NUMBER");

      if TEST_SUPPORT.Is_Configured_Multiprocessing then
         RTEMS.SEMAPHORE.CREATE(
            SPTEST.SEMAPHORE_NAME( 3 ),
            1,
            RTEMS.GLOBAL,
            RTEMS.TASKS.NO_PRIORITY,
            SPTEST.JUNK_ID,
            STATUS
         );
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.MP_NOT_CONFIGURED,
            "SEMAPHORE_CREATE OF MP_NOT_CONFIGURED"
         );
      end if;
      TEXT_IO.PUT_LINE("TA1 - semaphore_create - MP_NOT_CONFIGURED");

      RTEMS.SEMAPHORE.DELETE( 100, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "SEMAPHORE_DELETE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - semaphore_delete - unknown INVALID_ID"
      );

      RTEMS.SEMAPHORE.DELETE( 16#10100#, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "SEMAPHORE_DELETE WITH LOCAL ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - semaphore_delete - local INVALID_ID"
      );

      RTEMS.SEMAPHORE.IDENT(
         100,
         RTEMS.SEARCH_ALL_NODES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "SEMAPHORE_IDENT WITH ILLEGAL NAME (local)"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - semaphore_ident - global INVALID_NAME"
      );

      RTEMS.SEMAPHORE.IDENT( 100, 1, SPTEST.JUNK_ID, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "SEMAPHORE_IDENT WITH ILLEGAL NAME (global)"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - semaphore_ident - local INVALID_NAME"
      );

  end SCREEN_5;

-- 
--  SCREEN_6
--

   procedure SCREEN_6
   is
      STATUS         : RTEMS.STATUS_CODES;
   begin

      RTEMS.SEMAPHORE.OBTAIN(
         100,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "SEMAPHORE_OBTAIN WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - semaphore_obtain - INVALID_ID" );

      RTEMS.SEMAPHORE.OBTAIN(
         SPTEST.SEMAPHORE_ID( 1 ),
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED(STATUS, "SEMAPHORE_OBTAIN SUCCESSFUL");
      TEXT_IO.PUT_LINE( 
         "TA1 - semaphore_obtain - got sem 1 - SUCCESSFUL"
      );

      RTEMS.SEMAPHORE.OBTAIN(
         SPTEST.SEMAPHORE_ID( 1 ),
         RTEMS.NO_WAIT,
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.UNSATISFIED,
         "SEMAPHORE_OBTAIN NOT AVAILABLE"
      );
      TEXT_IO.PUT_LINE( "TA1 - semaphore_obtain - UNSATISFIED" );

      TEXT_IO.PUT_LINE( "TA1 - semaphore_obtain - timeout in 3 seconds" );
      RTEMS.SEMAPHORE.OBTAIN(
         SPTEST.SEMAPHORE_ID( 1 ),
         RTEMS.DEFAULT_OPTIONS,
         3 * TEST_SUPPORT.TICKS_PER_SECOND,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TIMEOUT,
         "SEMAPHORE_OBTAIN NOT AVAILABLE"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - semaphore_obtain - woke with TIMEOUT"
      );

      RTEMS.SEMAPHORE.RELEASE( SPTEST.SEMAPHORE_ID( 2 ), STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.NOT_OWNER_OF_RESOURCE,
         "SEMAPHORE_RELEASE AND NOT OWNER"
      );
      TEXT_IO.PUT_LINE( 
         "TA1 - semaphore_release - NOT_OWNER_OF_RESOURCE"
      );

      RTEMS.SEMAPHORE.RELEASE( 100, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "SEMAPHORE_RELEASE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - semaphore_release - INVALID_ID" );

      TEXT_IO.PUT_LINE(
         "TA1 - task_start - start TA2 - SUCCESSFUL"
     );
      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 2 ),
         SPTEST.TASK_2'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA2" );

      TEXT_IO.PUT_LINE( 
         "TA1 - task_wake_after - yield processor - SUCCESSFUL"
      );
      RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER (yield)" );

      TEXT_IO.PUT_LINE( 
         "TA1 - semaphore_delete - delete sem 1 - SUCCESSFUL"
      );
      RTEMS.SEMAPHORE.DELETE( SPTEST.SEMAPHORE_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_DELETE OF SM1" );

      TEXT_IO.PUT_LINE( "TA1 - semaphore_obtain - binary semaphore" );
      RTEMS.SEMAPHORE.OBTAIN(
         SPTEST.SEMAPHORE_ID( 2 ),
         RTEMS.NO_WAIT,
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN" );

      TEXT_IO.PUT_LINE( 
         "TA1 - semaphore_delete - delete sem 2 - RESOURCE_IN_USE"
      );
      RTEMS.SEMAPHORE.DELETE( SPTEST.SEMAPHORE_ID( 2 ), STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS( 
         STATUS, 
         RTEMS.RESOURCE_IN_USE,
         "SEMAPHORE_DELETE OF SM2" 
      );

      TEXT_IO.PUT_LINE( 
         "TA1 - task_wake_after - yield processor - SUCCESSFUL"
      );
      RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER (yield)" );

      RTEMS.TASKS.DELETE( SPTEST.TASK_ID( 2 ), STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TASK_DELETE AFTER THE TASK HAS BEEN DELETED"
      );
      TEXT_IO.PUT_LINE( 
         "TA1 - task_delete TA2 - already deleted INVALID_ID"
      );

   end SCREEN_6;

-- 
--  SCREEN_7
--

   procedure SCREEN_7
   is
      BUFFER         : SPTEST.BUFFER;
      BUFFER_POINTER : RTEMS.ADDRESS;
      COUNT          : RTEMS.UNSIGNED32;
      MESSAGE_SIZE   : RTEMS.UNSIGNED32 := 0;
      STATUS         : RTEMS.STATUS_CODES;
   begin

      BUFFER_POINTER := BUFFER'ADDRESS;

      RTEMS.MESSAGE_QUEUE.BROADCAST(
         100,
         BUFFER_POINTER,
         16,
         COUNT,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "MESSAGE_QUEUE_BROADCAST WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_broadcast - INVALID_ID"
      );

      RTEMS.MESSAGE_QUEUE.CREATE(
         0,
         3,
         16,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "MESSAGE_QUEUE_CREATE WITH ILLEGAL NAME"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_create - Q 1 - INVALID_NAME"
      );

      if TEST_SUPPORT.Is_Configured_Multiprocessing then
         RTEMS.MESSAGE_QUEUE.CREATE(
            SPTEST.QUEUE_NAME( 1 ),
            1,
            16,
            RTEMS.GLOBAL,
            SPTEST.JUNK_ID,
            STATUS
         );
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.MP_NOT_CONFIGURED,
            "MESSAGE_QUEUE_CREATE OF MP NOT CONFIGURED"
         );
      end if;
      TEXT_IO.PUT_LINE(
             "TA1 - message_queue_create - Q 1 - MP_NOT_CONFIGURED");

      RTEMS.MESSAGE_QUEUE.CREATE(
         SPTEST.QUEUE_NAME( 1 ),
         2,
         16,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.QUEUE_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED(
         STATUS,
         "MESSAGE_QUEUE_CREATE SUCCESSFUL"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_create - Q 1 - 2 DEEP - SUCCESSFUL"
      );

      RTEMS.MESSAGE_QUEUE.CREATE(
         SPTEST.QUEUE_NAME( 2 ),
         1,
         16,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TOO_MANY,
         "MESSAGE_QUEUE_CREATE OF TOO MANY"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_create - Q 2 - TOO_MANY"
      );

      RTEMS.MESSAGE_QUEUE.DELETE( 100, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "MESSAGE_QUEUE_DELETE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_delete - unknown INVALID_ID"
      );

      RTEMS.MESSAGE_QUEUE.DELETE( 16#10100#, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "MESSAGE_QUEUE_DELETE WITH LOCAL ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_delete - local INVALID_ID"
      );

      RTEMS.MESSAGE_QUEUE.IDENT(
         100,
         RTEMS.SEARCH_ALL_NODES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "MESSAGE_QUEUE_IDENT WITH ILLEGAL NAME"
      );
      TEXT_IO.PUT_LINE( "TA1 - message_queue_ident - INVALID_NAME" );

      RTEMS.MESSAGE_QUEUE.GET_NUMBER_PENDING( 100, COUNT, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
        STATUS,
        RTEMS.INVALID_ID,
        "MESSAGE_QUEUE_GET_NUMBER_PENDING WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE("TA1 - message_queue_get_number_pending - INVALID_ID");

      RTEMS.MESSAGE_QUEUE.FLUSH( 100, COUNT, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "MESSAGE_QUEUE_FLUSH WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - message_queue_flush - INVALID_ID" );

      RTEMS.MESSAGE_QUEUE.RECEIVE(
         100,
         BUFFER_POINTER,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         MESSAGE_SIZE,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "MESSAGE_QUEUE_RECEIVE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - message_queue_receive - INVALID_ID" );

      RTEMS.MESSAGE_QUEUE.RECEIVE(
         SPTEST.QUEUE_ID( 1 ),
         BUFFER_POINTER,
         RTEMS.NO_WAIT,
         RTEMS.NO_TIMEOUT,
         MESSAGE_SIZE,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.UNSATISFIED,
         "MESSAGE_QUEUE_RECEIVE UNSATISFIED"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_receive - Q 1 - UNSATISFIED"
      );

      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_receive - Q 1 - timeout in 3 seconds"
      );
      RTEMS.MESSAGE_QUEUE.RECEIVE(
         SPTEST.QUEUE_ID( 1 ),
         BUFFER_POINTER,
         RTEMS.DEFAULT_OPTIONS,
         3 * TEST_SUPPORT.TICKS_PER_SECOND,
         MESSAGE_SIZE,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TIMEOUT,
         "MESSAGE_QUEUE_RECEIVE 3 SECOND TIMEOUT"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_receive - Q 1 - woke up with TIMEOUT"
      );

      RTEMS.MESSAGE_QUEUE.SEND( 100, BUFFER_POINTER, 16, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "MESSAGE_QUEUE_SEND WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - message_queue_send - INVALID_ID" );

      RTEMS.MESSAGE_QUEUE.SEND(
         SPTEST.QUEUE_ID( 1 ),
         BUFFER_POINTER,
         16,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_SEND" );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_send - BUFFER 1 TO Q 1 - SUCCESSFUL"
      );

      RTEMS.MESSAGE_QUEUE.SEND(
         SPTEST.QUEUE_ID( 1 ),
         BUFFER_POINTER,
         16,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_SEND" );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_send - BUFFER 2 TO Q 1 - SUCCESSFUL"
      );

      RTEMS.MESSAGE_QUEUE.SEND(
         SPTEST.QUEUE_ID( 1 ),
         BUFFER_POINTER,
         16,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TOO_MANY,
         "MESSAGE_QUEUE_SEND TOO MANY TO LIMITED QUEUE"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_send - BUFFER 3 TO Q 1 - TOO_MANY"
      );

   end SCREEN_7;

-- 
--  SCREEN_8
--

   procedure SCREEN_8
   is
      BUFFER         : SPTEST.BUFFER;
      BUFFER_POINTER : RTEMS.ADDRESS;
      STATUS         : RTEMS.STATUS_CODES;
   begin

      BUFFER_POINTER := BUFFER'ADDRESS;

      RTEMS.MESSAGE_QUEUE.DELETE( SPTEST.QUEUE_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED(
         STATUS,
         "MESSAGE_QUEUE_DELETE SUCCESSFUL"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_delete - Q 1 - SUCCESSFUL"
      );

      RTEMS.MESSAGE_QUEUE.CREATE(
         SPTEST.QUEUE_NAME( 1 ),
         2,
         16,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.QUEUE_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED(
         STATUS,
         "MESSAGE_QUEUE_CREATE SUCCESSFUL"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_create - Q 1 - 2 DEEP - SUCCESSFUL"
      );

      RTEMS.MESSAGE_QUEUE.SEND(
         SPTEST.QUEUE_ID( 1 ),
         BUFFER_POINTER,
         16,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_SEND" );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_send - BUFFER 1 TO Q 1 - SUCCESSFUL"
      );

      RTEMS.MESSAGE_QUEUE.SEND(
         SPTEST.QUEUE_ID( 1 ),
         BUFFER_POINTER,
         16,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_SEND" );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_send - BUFFER 2 TO Q 1 - SUCCESSFUL"
      );

      RTEMS.MESSAGE_QUEUE.SEND(
         SPTEST.QUEUE_ID( 1 ),
         BUFFER_POINTER,
         16,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TOO_MANY,
         "MESSAGE_QUEUE_SEND TOO MANY TO LIMITED QUEUE"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_send - BUFFER 3 TO Q 1 - TOO_MANY"
      );

      RTEMS.MESSAGE_QUEUE.DELETE( SPTEST.QUEUE_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED(
         STATUS,
         "MESSAGE_QUEUE_DELETE SUCCESSFUL"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_delete - Q 1 - SUCCESSFUL"
      );

      RTEMS.MESSAGE_QUEUE.CREATE(
         SPTEST.QUEUE_NAME( 1 ),
         3,
         16,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.QUEUE_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED(
         STATUS,
         "MESSAGE_QUEUE_CREATE SUCCESSFUL"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_create - Q 1 - 3 DEEP - SUCCESSFUL"
      );

      RTEMS.MESSAGE_QUEUE.SEND(
         SPTEST.QUEUE_ID( 1 ),
         BUFFER_POINTER,
         16,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_SEND" );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_send - BUFFER 1 TO Q 1 - SUCCESSFUL"
      );

      RTEMS.MESSAGE_QUEUE.SEND(
         SPTEST.QUEUE_ID( 1 ),
         BUFFER_POINTER,
         16,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_SEND" );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_send - BUFFER 2 TO Q 1 - SUCCESSFUL"
      );

      RTEMS.MESSAGE_QUEUE.SEND(
         SPTEST.QUEUE_ID( 1 ),
         BUFFER_POINTER,
         16,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_SEND" );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_send - BUFFER 3 TO Q 1 - SUCCESSFUL"
      );

      RTEMS.MESSAGE_QUEUE.SEND(
         SPTEST.QUEUE_ID( 1 ),
         BUFFER_POINTER,
         16,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TOO_MANY,
         "MESSAGE_QUEUE_SEND TOO MANY TO LIMITED QUEUE"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_send - BUFFER 4 TO Q 1 - TOO_MANY"
      );

      RTEMS.MESSAGE_QUEUE.DELETE( SPTEST.QUEUE_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED(
         STATUS,
         "MESSAGE_QUEUE_DELETE SUCCESSFUL"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_delete - Q 1 - SUCCESSFUL"
      );

      RTEMS.MESSAGE_QUEUE.CREATE(
         SPTEST.QUEUE_NAME( 1 ),
         2,
         16,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.QUEUE_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED(
         STATUS,
         "MESSAGE_QUEUE_CREATE SUCCESSFUL"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_create - Q 1 - 3 DEEP - SUCCESSFUL"
      );

      TEXT_IO.PUT_LINE( 
         "TA1 - task_start - start TA3 - SUCCESSFUL"
      );
      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 3 ),
         SPTEST.TASK_3'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA3" );

      TEXT_IO.PUT_LINE( 
         "TA1 - task_wake_after - yield processor - SUCCESSFUL"
      );
      RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER (yield)" );

      RTEMS.MESSAGE_QUEUE.DELETE( SPTEST.QUEUE_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED(
         STATUS,
         "MESSAGE_QUEUE_DELETE SUCCESSFUL"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - message_queue_delete - delete Q 1 - SUCCESSFUL"
      );

      TEXT_IO.PUT_LINE( 
         "TA1 - task_wake_after - yield processor - SUCCESSFUL"
      );
      RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER (yield)" );

   end SCREEN_8;

-- 
--  SCREEN_9
--

   procedure SCREEN_9
   is
      CONVERTED : RTEMS.ADDRESS;
      STATUS    : RTEMS.STATUS_CODES;
   begin

      RTEMS.PORT.CREATE(
         0,
         SPTEST.INTERNAL_PORT_AREA'ADDRESS,
         SPTEST.EXTERNAL_PORT_AREA'ADDRESS,
         SPTEST.INTERNAL_PORT_AREA'LENGTH,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "PORT_CREATE WITH ILLEGAL NAME"
      );
      TEXT_IO.PUT_LINE( "TA1 - port_create - INVALID_NAME" );

      RTEMS.PORT.CREATE(
         SPTEST.PORT_NAME( 1 ),
         SPTEST.INTERNAL_PORT_AREA( 1 )'ADDRESS,
         SPTEST.EXTERNAL_PORT_AREA'ADDRESS,
         SPTEST.INTERNAL_PORT_AREA'LENGTH,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ADDRESS,
         "PORT_CREATE WITH ILLEGAL ADDRESS"
      );
      TEXT_IO.PUT_LINE( "TA1 - port_create - INVALID_ADDRESS" );

      RTEMS.PORT.CREATE(
         SPTEST.PORT_NAME( 1 ),
         SPTEST.INTERNAL_PORT_AREA'ADDRESS,
         SPTEST.EXTERNAL_PORT_AREA'ADDRESS,
         SPTEST.INTERNAL_PORT_AREA'LENGTH,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TOO_MANY,
         "PORT_CREATE OF TOO MANY"
      );
      TEXT_IO.PUT_LINE( "TA1 - port_create - TOO_MANY" );

      RTEMS.PORT.DELETE( 0, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "PORT_DELETE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - port_delete - INVALID_ID" );

      RTEMS.PORT.IDENT( 0, SPTEST.JUNK_ID, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "PORT_IDENT WITH ILLEGAL NAME"
      );
      TEXT_IO.PUT_LINE( "TA1 - port_ident - INVALID_NAME" );

      RTEMS.PORT.INTERNAL_TO_EXTERNAL(
         100,
         SPTEST.INTERNAL_PORT_AREA'ADDRESS,
         CONVERTED,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "PORT_INTERNAL_TO_EXTERNAL WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - port_internal_to_external - INVALID_ID"
      );

      RTEMS.PORT.EXTERNAL_TO_INTERNAL(
         100,
         SPTEST.EXTERNAL_PORT_AREA'ADDRESS,
         CONVERTED,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "PORT_EXTERNAL_TO_INTERNAL WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - port_external_to_internal - INVALID_ID"
      );

   end SCREEN_9;

-- 
--  SCREEN_10
--

   procedure SCREEN_10
   is
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.RATE_MONOTONIC.CREATE( 0, SPTEST.JUNK_ID, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "RATE_MONOTONIC_CREATE WITH ILLEGAL NAME"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - rate_monotonic_create - INVALID_NAME"
      );

      RTEMS.RATE_MONOTONIC.CREATE(
         SPTEST.PERIOD_NAME( 1 ),
         SPTEST.PERIOD_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "RATE_MONOTONIC_CREATE" );
      TEXT_IO.PUT_LINE(
         "TA1 - rate_monotonic_create - SUCCESSFUL"
      );

      RTEMS.RATE_MONOTONIC.CREATE(
         SPTEST.PERIOD_NAME( 1 ),
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TOO_MANY,
         "RATE_MONOTONIC_CREATE FOR TOO MANY"
      );
      TEXT_IO.PUT_LINE( "TA1 - rate_monotonic_create - TOO_MANY" );

      RTEMS.RATE_MONOTONIC.IDENT( 0, SPTEST.JUNK_ID, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "RATE_MONOTONIC_IDENT WITH ILLEGAL NAME"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - rate_monotonic_ident - INVALID_NAME"
      );

      RTEMS.RATE_MONOTONIC.PERIOD( 100, 5, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "RATE_MONOTONIC_PERIOD WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - rate_monotonic_period - unknown INVALID_ID"
      );

      RTEMS.RATE_MONOTONIC.PERIOD( 16#10100#, 5, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "RATE_MONOTONIC_PERIOD WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - rate_monotonic_period - local INVALID_ID"
      );

      RTEMS.RATE_MONOTONIC.PERIOD(
         SPTEST.PERIOD_ID( 1 ),
         RTEMS.RATE_MONOTONIC_PERIOD_STATUS,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.NOT_DEFINED,
         "RATE_MONOTONIC_PERIOD STATUS NOT DEFINED"
      );
      TEXT_IO.PUT_LINE( 
         "TA1 - rate_monotonic_period( STATUS ) - NOT_DEFINED"
      );

      RTEMS.RATE_MONOTONIC.PERIOD( SPTEST.PERIOD_ID( 1 ), 100, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED(
         STATUS,
         "RATE_MONOTONIC_PERIOD 100 TICKS"
      );
      TEXT_IO.PUT( "TA1 - rate_monotonic_period - 100 ticks - " );
      TEXT_IO.PUT_LINE( "SUCCESSFUL" );

      RTEMS.RATE_MONOTONIC.PERIOD(
         SPTEST.PERIOD_ID( 1 ),
         RTEMS.RATE_MONOTONIC_PERIOD_STATUS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED(
         STATUS,
         "RATE_MONOTONIC_PERIOD STATUS"
      );
      TEXT_IO.PUT( "TA1 - rate_monotonic_period( STATUS ) - " );
      TEXT_IO.PUT_LINE( "SUCCESSFUL" );

      loop

         RTEMS.RATE_MONOTONIC.PERIOD(
            SPTEST.PERIOD_ID( 1 ),
            RTEMS.RATE_MONOTONIC_PERIOD_STATUS,
            STATUS
         );

         exit when RTEMS.ARE_STATUSES_EQUAL( STATUS, RTEMS.TIMEOUT );

         TEST_SUPPORT.DIRECTIVE_FAILED(
            STATUS,
            "RATE_MONOTONIC_PERIOD STATUS WAITING FOR TIMEOUT"
         );
 
      end loop;
      TEXT_IO.PUT( "TA1 - rate_monotonic_period( STATUS ) - " );
      TEXT_IO.PUT_LINE( "TIMEOUT" );

      RTEMS.RATE_MONOTONIC.CANCEL( 100, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "RATE_MONOTONIC_CANCEL WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - rate_monotonic_cancel - unknown INVALID_ID"
      );

      RTEMS.RATE_MONOTONIC.CANCEL( 16#10100#, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "RATE_MONOTONIC_CANCEL WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - rate_monotonic_cancel - local INVALID_ID"
      );

      RTEMS.RATE_MONOTONIC.CANCEL( SPTEST.PERIOD_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "RATE_MONOTONIC_CANCEL" );
      TEXT_IO.PUT_LINE( "TA1 - rate_monotonic_cancel - SUCCESSFUL" );

      RTEMS.RATE_MONOTONIC.PERIOD( SPTEST.PERIOD_ID( 1 ), 5, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED(
         STATUS,
         "RATE_MONOTONIC_PERIOD RESTART"
      );

      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

      RTEMS.RATE_MONOTONIC.PERIOD( SPTEST.PERIOD_ID( 1 ), 5, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TIMEOUT,
         "RATE_MONOTONIC_PERIOD TIMED OUT"
      );
      TEXT_IO.PUT_LINE( 
         "TA1 - rate_monotonic_period - 5 ticks - TIMEOUT" 
      );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 4 ),
         SPTEST.TASK_4'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA4" );

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - yielding to TA4" );
      RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER (yield)" );

      RTEMS.RATE_MONOTONIC.DELETE( 100, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "RATE_MONOTONIC_DELETE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - rate_monotonic_delete - unknown INVALID_ID"
      );

      RTEMS.RATE_MONOTONIC.DELETE( 16#10100#, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "RATE_MONOTONIC_DELETE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - rate_monotonic_delete - local INVALID_ID"
      );

      RTEMS.RATE_MONOTONIC.DELETE( SPTEST.PERIOD_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "RATE_MONOTONIC_DELETE" );
      TEXT_IO.PUT_LINE(
         "TA1 - rate_monotonic_delete - SUCCESSFUL"
      );

   end SCREEN_10;

-- 
--  SCREEN_11
--

   procedure SCREEN_11
   is
      BUFFER_ADDRESS_1 : RTEMS.ADDRESS;
      BUFFER_ADDRESS_2 : RTEMS.ADDRESS;
      BUFFER_ADDRESS_3 : RTEMS.ADDRESS;
      STATUS           : RTEMS.STATUS_CODES;
   begin

      RTEMS.PARTITION.CREATE(
         0,
         SPTEST.PARTITION_GOOD_AREA'ADDRESS,
         128,
         40,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "PARTITION_CREATE WITH ILLEGAL NAME"
      );
      TEXT_IO.PUT_LINE( "TA1 - partition_create - INVALID_NAME" );

      RTEMS.PARTITION.CREATE(
         SPTEST.PARTITION_NAME( 1 ),
         SPTEST.PARTITION_GOOD_AREA'ADDRESS,
         0,
         80,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_SIZE,
         "PARTITION_CREATE WITH ILLEGAL LENGTH"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - partition_create - length - INVALID_SIZE"
      );

      RTEMS.PARTITION.CREATE(
         SPTEST.PARTITION_NAME( 1 ),
         SPTEST.PARTITION_GOOD_AREA'ADDRESS,
         128,
         0,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_SIZE,
         "PARTITION_CREATE WITH ILLEGAL BUFFER_SIZE"
      );
      TEXT_IO.PUT_LINE( 
         "TA1 - partition_create - buffer size - INVALID_SIZE" 
      );

      RTEMS.PARTITION.CREATE(
         SPTEST.PARTITION_NAME( 1 ),
         SPTEST.PARTITION_GOOD_AREA'ADDRESS,
         128,
         256,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_SIZE,
         "PARTITION_CREATE WITH BUFFER_SIZE > LENGTH"
      );
      TEXT_IO.PUT_LINE( 
       "TA1 - partition_create - length < buffer size - INVALID_SIZE"
      );

      if TEST_SUPPORT.Is_Configured_Multiprocessing then
         RTEMS.PARTITION.CREATE(
            SPTEST.PARTITION_NAME( 1 ),
            SPTEST.PARTITION_GOOD_AREA'ADDRESS,
            128,
            64,
            RTEMS.GLOBAL,
            SPTEST.JUNK_ID,
            STATUS
         );
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.MP_NOT_CONFIGURED,
            "PARTITION_CREATE OF GLOBAL"
         );
      end if;
      TEXT_IO.PUT_LINE("TA1 - partition_create - MP_NOT_CONFIGURED");

      RTEMS.PARTITION.CREATE(
         SPTEST.PARTITION_NAME( 1 ),
         SPTEST.PARTITION_BAD_AREA'ADDRESS,
         128,
         64,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ADDRESS,
         "PARTITION_CREATE WITH BAD ADDRESS"
      );
      TEXT_IO.PUT_LINE( "TA1 - partition_create - INVALID_ADDRESS" );

      RTEMS.PARTITION.CREATE(
         SPTEST.PARTITION_NAME( 1 ),
         SPTEST.PARTITION_GOOD_AREA'ADDRESS,
         128,
         34,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_SIZE,
         "PARTITION_CREATE WITH UNALIGNED BUFFER_SIZE"
      );
      TEXT_IO.PUT_LINE( "TA1 - partition_create - INVALID_SIZE" );

      RTEMS.PARTITION.DELETE( 100, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "PARTITION_DELETE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - partition_delete - unknown INVALID_ID"
      );

      RTEMS.PARTITION.DELETE( 16#10100#, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "PARTITION_DELETE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - partition_delete - local INVALID_ID"
      );

      RTEMS.PARTITION.GET_BUFFER( 100, BUFFER_ADDRESS_1, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "PARTITION_GET_BUFFER WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - partition_get_buffer - INVALID_ID" );

      RTEMS.PARTITION.IDENT(
         0,
         RTEMS.SEARCH_ALL_NODES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "PARTITION_IDENT WITH ILLEGAL NAME"
      );
      TEXT_IO.PUT_LINE( "TA1 - partition_ident - INVALID_NAME" );

      RTEMS.PARTITION.RETURN_BUFFER( 100, BUFFER_ADDRESS_1, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "PARTITION_RETURN WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - partition_return_buffer - INVALID_ID"
      );

      RTEMS.PARTITION.CREATE(
         SPTEST.PARTITION_NAME( 1 ),
         SPTEST.PARTITION_GOOD_AREA'ADDRESS,
         128,
         64,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.PARTITION_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_CREATE" );
      TEXT_IO.PUT_LINE( "TA1 - partition_create - SUCCESSFUL" );

      RTEMS.PARTITION.CREATE(
         SPTEST.PARTITION_NAME( 1 ),
         SPTEST.PARTITION_GOOD_AREA'ADDRESS,
         128,
         64,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TOO_MANY,
         "PARTITION_CREATE OF TOO MANY"
      );
      TEXT_IO.PUT_LINE( "TA1 - partition_create - TOO_MANY" );

      RTEMS.PARTITION.GET_BUFFER(
         SPTEST.PARTITION_ID( 1 ),
         BUFFER_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_GET_BUFFER" );
      TEXT_IO.PUT_LINE( "TA1 - partition_get_buffer - SUCCESSFUL" );

      RTEMS.PARTITION.GET_BUFFER(
         SPTEST.PARTITION_ID( 1 ),
         BUFFER_ADDRESS_2,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_GET_BUFFER" );
      TEXT_IO.PUT_LINE( "TA1 - partition_get_buffer - SUCCESSFUL" );

      RTEMS.PARTITION.GET_BUFFER(
         SPTEST.PARTITION_ID( 1 ),
         BUFFER_ADDRESS_3,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.UNSATISFIED,
         "PARTITION_GET_BUFFER UNSATISFIED"
      );
      TEXT_IO.PUT_LINE( "TA1 - partition_get_buffer - UNSATISFIED" );

      RTEMS.PARTITION.DELETE(
         SPTEST.PARTITION_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.RESOURCE_IN_USE,
         "PARTITION_DELETE WITH BUFFERS IN USE"
      );
      TEXT_IO.PUT_LINE( "TA1 - partition_delete - RESOURCE_IN_USE" );

      RTEMS.PARTITION.RETURN_BUFFER(
         SPTEST.PARTITION_ID( 1 ),
         SPTEST.REGION_GOOD_AREA( 0 )'ADDRESS,  -- NOTE: REGION
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ADDRESS,
         "PARTITION_RETURN WITH BUFFER ADDRESS OUT OF PARTITION"
      );
      TEXT_IO.PUT( "TA1 - partition_return_buffer - " );
      TEXT_IO.PUT_LINE( "INVALID_ADDRESS - out of range" );

      RTEMS.PARTITION.RETURN_BUFFER(
         SPTEST.PARTITION_ID( 1 ),
         SPTEST.PARTITION_GOOD_AREA( 7 )'ADDRESS,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ADDRESS,
         "PARTITION_RETURN WITH BUFFER ADDRESS NOT ON BOUNDARY"
      );
      TEXT_IO.PUT( "TA1 - partition_return_buffer - " );
      TEXT_IO.PUT_LINE( "INVALID_ADDRESS - not on boundary" );


   end SCREEN_11;

-- 
--  SCREEN_12
--

   procedure SCREEN_12
   is
      SEGMENT_ADDRESS_1 : RTEMS.ADDRESS;
      SEGMENT_ADDRESS_2 : RTEMS.ADDRESS;
      SEGMENT_ADDRESS_3 : RTEMS.ADDRESS;
--    OFFSET            : RTEMS.UNSIGNED32;
--    GOOD_FRONT_FLAG   : RTEMS.UNSIGNED32;
--    GOOD_BACK_FLAG    : RTEMS.UNSIGNED32;
      STATUS            : RTEMS.STATUS_CODES;
   begin

      RTEMS.REGION.CREATE(
         0,
         SPTEST.REGION_GOOD_AREA'ADDRESS,
         16#40#,
         32,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "REGION_CREATE WITH ILLEGAL NAME"
      );
      TEXT_IO.PUT_LINE( "TA1 - region_create - INVALID_NAME" );

      RTEMS.REGION.CREATE(
         SPTEST.REGION_NAME( 1 ),
         SPTEST.REGION_BAD_AREA'ADDRESS,
         16#40#,
         32,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ADDRESS,
         "REGION_CREATE WITH ILLEGAL ADDRESS"
      );
      TEXT_IO.PUT_LINE( "TA1 - region_create - INVALID_ADDRESS" );

      RTEMS.REGION.CREATE(
         SPTEST.REGION_NAME( 1 ),
         SPTEST.REGION_GOOD_AREA'ADDRESS,
         34,
         34,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_SIZE,
         "REGION_CREATE WITH ILLEGAL SIZE"
      );
      TEXT_IO.PUT_LINE( "TA1 - region_create - INVALID_SIZE" );

      RTEMS.REGION.CREATE(
         SPTEST.REGION_NAME( 1 ),
         SPTEST.REGION_GOOD_AREA( SPTEST.REGION_START_OFFSET )'ADDRESS,
         SPTEST.REGION_LENGTH,
         16#40#,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.REGION_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_CREATE" );
      TEXT_IO.PUT_LINE( "TA1 - region_create - SUCCESSFUL" );

      RTEMS.REGION.CREATE(
         SPTEST.REGION_NAME( 1 ),
         SPTEST.REGION_GOOD_AREA'ADDRESS,
         SPTEST.REGION_LENGTH,
         16#40#,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TOO_MANY,
         "REGION_CREATE FOR TOO MANY"
      );
      TEXT_IO.PUT_LINE( "TA1 - region_create - TOO_MANY" );

      RTEMS.REGION.DELETE( 100, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "REGION_DELETE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - region_delete - unknown INVALID_ID" );

      RTEMS.REGION.DELETE( 16#10100#, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "REGION_DELETE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - region_delete - local INVALID_ID" );

      RTEMS.REGION.IDENT( 0, SPTEST.JUNK_ID, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "REGION_IDENT WITH ILLEGAL NAME"
      );
      TEXT_IO.PUT_LINE( "TA1 - region_ident - INVALID_NAME" );

      RTEMS.REGION.GET_SEGMENT(
         100,
         16#40#,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "REGION_GET_SEGMENT WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - region_get_segment - INVALID_ID" );

      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         (SPTEST.REGION_GOOD_AREA'SIZE / 8) * 2,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_SIZE,
         "REGION_GET_SEGMENT WITH ILLEGAL SIZE"
      );
      TEXT_IO.PUT_LINE( "TA1 - region_get_segment - INVALID_SIZE" );

      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         384,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "REGION_GET_SEGMENT" );
      TEXT_IO.PUT_LINE( "TA1 - region_get_segment - SUCCESSFUL" );

      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         SPTEST.REGION_LENGTH / 2,
         RTEMS.NO_WAIT,
         RTEMS.NO_TIMEOUT,
         SEGMENT_ADDRESS_2,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.UNSATISFIED,
         "REGION_GET_SEGMENT UNSATISFIED"
      );
      TEXT_IO.PUT_LINE( "TA1 - region_get_segment - UNSATISFIED" );

      TEXT_IO.PUT_LINE( "TA1 - region_get_segment - timeout in 3 seconds" );
      RTEMS.REGION.GET_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         128,
         RTEMS.DEFAULT_OPTIONS,
         3 * TEST_SUPPORT.TICKS_PER_SECOND,
         SEGMENT_ADDRESS_3,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TIMEOUT,
         "REGION_GET_SEGMENT TIMEOUT"
      );
      TEXT_IO.PUT_LINE( 
         "TA1 - region_get_segment - woke up with TIMEOUT" 
      );

      RTEMS.REGION.DELETE( SPTEST.REGION_ID( 1 ), STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.RESOURCE_IN_USE,
         "REGION_DELETE WITH BUFFERS IN USE"
      );
      TEXT_IO.PUT_LINE( "TA1 - region_delete - RESOURCE_IN_USE" );

      RTEMS.REGION.RETURN_SEGMENT(
         100,
         SEGMENT_ADDRESS_1,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "REGION_RETURN_SEGMENT WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - region_return_segment - INVALID_ID" );

      RTEMS.REGION.RETURN_SEGMENT(
         SPTEST.REGION_ID( 1 ),
         SPTEST.REGION_GOOD_AREA'ADDRESS,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ADDRESS,
         "REGION_RETURN_SEGMENT WITH ILLEGAL SEGMENT"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - region_return_segment - INVALID_ADDRESS"
      );

      -- internal heap errors, subject to change if heap code changes

      TEXT_IO.PUT_LINE( "TA1 - debug_disable - DEBUG_REGION" );
      RTEMS.DEBUG.DISABLE( RTEMS.DEBUG.REGION );

--    OFFSET := 0;
--    GOOD_BACK_FLAG := 0;
--    GOOD_FRONT_FLAG := 0;

      TEXT_IO.PUT_LINE(
         "TA1 - region_return_segment - INVALID_ADDRESS - SKIPPED"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - region_return_segment - INVALID_ADDRESS - SKIPPED"
      );


--      OFFSET := RTEMS.SUBTRACT( 
--                   SEGMENT_ADDRESS_1, 
--                   SPTEST.REGION_GOOD_AREA'ADDRESS
--                ) / 4;
--
--      
--      -- bad FRONT_FLAG error
--
--      GOOD_FRONT_FLAG := SPTEST.REGION_GOOD_AREA( OFFSET - 1 );
--      SPTEST.REGION_GOOD_AREA( OFFSET - 1 ) := GOOD_FRONT_FLAG + 2;
--
--      RTEMS.REGION.RETURN_SEGMENT(
--         SPTEST.REGION_ID( 1 ),
--         SEGMENT_ADDRESS_1,
--         STATUS
--      );
--      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
--         STATUS,
--         RTEMS.INVALID_ADDRESS,
--         "REGION_RETURN_SEGMENT WITH BACK_FLAG /= FRONT_FLAG"
--      );
--      TEXT_IO.PUT_LINE(
--         "TA1 - region_return_segment - INVALID_ADDRESS"
--      );
--
--      SPTEST.REGION_GOOD_AREA( OFFSET - 1 ) := GOOD_FRONT_FLAG;
--
--      -- bad BACK_FLAG error
--
--      GOOD_BACK_FLAG := SPTEST.REGION_GOOD_AREA( OFFSET - 2 );
--      SPTEST.REGION_GOOD_AREA( OFFSET - 2 ) := 1024;
--
--      RTEMS.REGION.RETURN_SEGMENT(
--         SPTEST.REGION_ID( 1 ),
--         SEGMENT_ADDRESS_1,
--         STATUS
--      );
--      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
--         STATUS,
--         RTEMS.INVALID_ADDRESS,
--         "REGION_RETURN_SEGMENT WITH BACK_FLAG /= FRONT_FLAG"
--      );
--      TEXT_IO.PUT_LINE(
--         "TA1 - region_return_segment - INVALID_ADDRESS"
--      );
--
--      SPTEST.REGION_GOOD_AREA( OFFSET - 2 ) := GOOD_BACK_FLAG;

      TEXT_IO.PUT_LINE( "TA1 - debug_enable - DEBUG_REGION" );
      RTEMS.DEBUG.ENABLE( RTEMS.DEBUG.REGION );

      RTEMS.REGION.EXTEND(
         100,
         SPTEST.REGION_GOOD_AREA'ADDRESS,
         128,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "REGION_EXTEND WITH INVALID_ID"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - region_extend - INVALID_ID"
      );

      RTEMS.REGION.EXTEND(
         SPTEST.REGION_ID( 1 ),
         SPTEST.REGION_GOOD_AREA( SPTEST.REGION_START_OFFSET + 16 )'ADDRESS,
         128,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ADDRESS,
         "REGION_EXTEND WITH INVALID_ADDRESS"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - region_extend - within heap - INVALID_ADDRESS"
      );

      RTEMS.REGION.EXTEND(
         SPTEST.REGION_ID( 1 ),
         SPTEST.REGION_BAD_AREA'ADDRESS,
         128,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.NOT_IMPLEMENTED,
         "REGION_EXTEND WITH NOT_IMPLEMENTED"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - region_extend - non-contiguous lower - NOT_IMPLEMENTED"
      );

      RTEMS.REGION.EXTEND(
         SPTEST.REGION_ID( 1 ),
         SPTEST.REGION_GOOD_AREA( 
            SPTEST.REGION_START_OFFSET - SPTEST.REGION_LENGTH )'ADDRESS,
         128,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.NOT_IMPLEMENTED,
         "REGION_EXTEND WITH NOT_IMPLEMENTED"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - region_extend - contiguous lower - NOT_IMPLEMENTED"
      );

      RTEMS.REGION.EXTEND(
         SPTEST.REGION_ID( 1 ),
         SPTEST.REGION_GOOD_AREA( 
            SPTEST.REGION_START_OFFSET + SPTEST.REGION_LENGTH + 16 )'ADDRESS,
         128,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.NOT_IMPLEMENTED,
         "REGION_EXTEND WITH NOT_IMPLEMENTED"
      );
      TEXT_IO.PUT_LINE(
         "TA1 - region_extend - non-contiguous higher - NOT_IMPLEMENTED"
      );

   end SCREEN_12;

-- 
--  SCREEN_13
--

   procedure SCREEN_13
   is
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin
      TIME := ( 2000, 12, 31, 23, 59, 59, 0 );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET SUCCESSFUL" );
      TEST_SUPPORT.PRINT_TIME(
         "TA1 - clock_set - ",
         TIME,
         " - SUCCESSFUL"
      );
      TEXT_IO.NEW_LINE;
      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
      RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET SUCCESSFUL" );
      TEST_SUPPORT.PRINT_TIME(
         "TA1 - clock_get - ",
         TIME,
         " - SUCCESSFUL"
      );
      TEXT_IO.NEW_LINE;

      TIME := ( 1999, 12, 31, 23, 59, 59, 0 );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET SUCCESSFUL" );
      TEST_SUPPORT.PRINT_TIME(
         "TA1 - clock_set - ",
         TIME,
         " - SUCCESSFUL"
      );
      TEXT_IO.NEW_LINE;
      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
      RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET SUCCESSFUL" );
      TEST_SUPPORT.PRINT_TIME(
         "TA1 - clock_get - ",
         TIME,
         " - SUCCESSFUL"
      );
      TEXT_IO.NEW_LINE;

      TIME := ( 2100, 12, 31, 23, 59, 59, 0 );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET SUCCESSFUL" );
      TEST_SUPPORT.PRINT_TIME(
         "TA1 - clock_set - ",
         TIME,
         " - SUCCESSFUL"
      );
      TEXT_IO.NEW_LINE;
      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
      RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET SUCCESSFUL" );
      TEST_SUPPORT.PRINT_TIME(
         "TA1 - clock_get - ",
         TIME,
         " - SUCCESSFUL"
      );
      TEXT_IO.NEW_LINE;

      TIME := ( 2099, 12, 31, 23, 59, 59, 0 );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET SUCCESSFUL" );
      TEST_SUPPORT.PRINT_TIME(
         "TA1 - clock_set - ",
         TIME,
         " - SUCCESSFUL"
      );
      TEXT_IO.NEW_LINE;
      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
      RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET SUCCESSFUL" );
      TEST_SUPPORT.PRINT_TIME(
         "TA1 - clock_get - ",
         TIME,
         " - SUCCESSFUL"
      );
      TEXT_IO.NEW_LINE;

      TIME := ( 1991, 12, 31, 23, 59, 59, 0 );
      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET SUCCESSFUL" );
      TEST_SUPPORT.PRINT_TIME(
         "TA1 - clock_set - ",
         TIME,
         " - SUCCESSFUL"
      );
      TEXT_IO.NEW_LINE;
      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
      RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET SUCCESSFUL" );
      TEST_SUPPORT.PRINT_TIME(
         "TA1 - clock_get - ",
         TIME,
         " - SUCCESSFUL"
      );
      TEXT_IO.NEW_LINE;

   end SCREEN_13;

-- 
--  SCREEN_14
--

   procedure SCREEN_14
   is
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.TIMER.CREATE( 0, SPTEST.JUNK_ID, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "TIMER_CREATE WITH ILLEGAL NAME"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_create - INVALID_NAME" );

      RTEMS.TIMER.CREATE(
         SPTEST.TIMER_NAME( 1 ),
         SPTEST.TIMER_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE" );
      TEXT_IO.PUT_LINE( "TA1 - timer_create - 1 - SUCCESSFUL" );

      RTEMS.TIMER.CREATE(
         SPTEST.TIMER_NAME( 1 ),
         SPTEST.JUNK_ID,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.TOO_MANY,
         "TIMER_CREATE FOR TOO MANY"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_create - 2 - TOO_MANY" );

      RTEMS.TIMER.DELETE( 100, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TIMER_DELETE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_delete - local INVALID_ID" );

      RTEMS.TIMER.DELETE( 16#10100#, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TIMER_DELETE WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_delete - global INVALID_ID" );

      RTEMS.TIMER.IDENT( 0, SPTEST.JUNK_ID, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NAME,
         "TIMER_IDENT WITH ILLEGAL NAME"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_ident - INVALID_NAME" );

      RTEMS.TIMER.CANCEL( 16#10100#, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TIMER_CANCEL WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_cancel - INVALID_ID" );

      RTEMS.TIMER.RESET( 16#10100#, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TIMER_RESET WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_reset - INVALID_ID" );

      RTEMS.TIMER.RESET( SPTEST.TIMER_ID( 1 ), STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.NOT_DEFINED,
         "TIMER_RESET BEFORE INITIATED"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_reset - NOT_DEFINED" );

      RTEMS.TIMER.FIRE_AFTER(
         16#10100#,
         5 * TEST_SUPPORT.TICKS_PER_SECOND,
         SPTEST.DELAYED_SUBPROGRAM'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TIMER_FIRE_AFTER WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_fire_after - INVALID_ID" );

      TIME := ( 1994, 12, 31, 1, 0, 0, 0 );
      RTEMS.TIMER.FIRE_WHEN(
         16#10100#,
         TIME,
         SPTEST.DELAYED_SUBPROGRAM'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_ID,
         "TIMER_FIRE_WHEN WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_fire_when - INVALID_ID" );

      RTEMS.TIMER.FIRE_AFTER(
         SPTEST.TIMER_ID( 1 ),
         0,
         SPTEST.DELAYED_SUBPROGRAM'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_NUMBER,
         "TIMER_FIRE_AFTER WITH 0 TICKS"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_fire_after - INVALID_NUMBER" );

      TIME := ( 1987, 2, 5, 8, 30, 45, 0 );
      RTEMS.TIMER.FIRE_WHEN(
         SPTEST.TIMER_ID( 1 ),
         TIME,
         SPTEST.DELAYED_SUBPROGRAM'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_CLOCK,
         "TIMER_FIRE_WHEN WITH ILLEGAL TIME"
      );
      TEST_SUPPORT.PRINT_TIME(
         "TA1 - timer_fire_when - ",
         TIME,
         " - INVALID_CLOCK"
      );
      TEXT_IO.NEW_LINE;

      RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET SUCCESSFUL" );
      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_get       - ", TIME, "" );
      TEXT_IO.NEW_LINE;

      TIME := ( 1990, 2, 5, 8, 30, 45, 0 );
      RTEMS.TIMER.FIRE_WHEN(
         SPTEST.TIMER_ID( 1 ),
         TIME,
         SPTEST.DELAYED_SUBPROGRAM'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.INVALID_CLOCK,
         "TIMER_FIRE_WHEN BEFORE CURRENT TIME"
      );
      TEST_SUPPORT.PRINT_TIME(
         "TA1 - timer_fire_when - ",
         TIME,
         " - before INVALID_CLOCK"
      );
      TEXT_IO.NEW_LINE;

      RTEMS.TIMER.SERVER_FIRE_AFTER(
         0, 5, SPTEST.DELAYED_SUBPROGRAM'ACCESS, RTEMS.NULL_ADDRESS, STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
        STATUS,
        RTEMS.INCORRECT_STATE,
        "TIMER_SERVER_FIRE_AFTER INCORRECT STATE"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_server_fire_after - INCORRECT_STATE" );

      RTEMS.TIMER.SERVER_FIRE_WHEN(
         0, TIME, SPTEST.DELAYED_SUBPROGRAM'ACCESS, RTEMS.NULL_ADDRESS, STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
        STATUS,
        RTEMS.INCORRECT_STATE,
        "TIMER_SERVER_FIRE_WHEN INCORRECT STATE"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_server_fire_when - INCORRECT_STATE" );

      RTEMS.TIMER.INITIATE_SERVER( 0, 0, 0, STATUS );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
        STATUS,
        RTEMS.INVALID_PRIORITY,
        "timer_initiate_server invalid priority"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_initiate_server - INVALID_PRIORITY" );

      RTEMS.TIMER.INITIATE_SERVER(
-- XXX ask Joel
--         RTEMS.TIMER.SERVER_DEFAULT_PRIORITY, 0, 0, STATUS
         -1, 0, 0, STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "timer_initiate_server" );
      TEXT_IO.PUT_LINE( "TA1 - timer_initiate_server" );

      RTEMS.TIMER.SERVER_FIRE_AFTER(
        16#010100#,
        5 * TEST_SUPPORT.TICKS_PER_SECOND,
        SPTEST.DELAYED_SUBPROGRAM'ACCESS,
        RTEMS.NULL_ADDRESS,
        STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
        STATUS,
        RTEMS.INVALID_ID,
        "TIMER_SERVER_FIRE_AFTER ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_server_fire_after - INVALID_ID" );

      TIME := ( 1994, 12, 31, 9, 0, 0, 0 );
      RTEMS.TIMER.SERVER_FIRE_WHEN(
         16#010100#,
         TIME,
         SPTEST.DELAYED_SUBPROGRAM'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
        STATUS,
        RTEMS.INVALID_ID,
        "TIMER_SERVER_FIRE_WHEN WITH ILLEGAL ID"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_server_fire_when - INVALID_ID" );

      RTEMS.TIMER.SERVER_FIRE_AFTER(
         SPTEST.TIMER_ID( 1 ),
         0,
         SPTEST.DELAYED_SUBPROGRAM'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
        STATUS,
        RTEMS.INVALID_NUMBER,
        "TIMER_SERVER_FIRE_AFTER WITH 0 TICKS"
      );
      TEXT_IO.PUT_LINE( "TA1 - timer_server_fire_after - INVALID_NUMBER" );

      TIME := ( 1987, 2, 5, 8, 30, 45, 0 );
      RTEMS.TIMER.SERVER_FIRE_WHEN(
         SPTEST.TIMER_ID( 1 ),
         TIME,
         SPTEST.DELAYED_SUBPROGRAM'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
        STATUS,
        RTEMS.INVALID_CLOCK,
        "TIMER_SERVER_FIRE_WHEN WITH ILLEGAL TIME"
      );
      TEST_SUPPORT.PRINT_TIME(
        "TA1 - timer_server_fire_when - ",
        TIME,
        " - INVALID_CLOCK"
      );
      TEXT_IO.NEW_LINE;

      RTEMS.CLOCK.GET( RTEMS.CLOCK.GET_TOD, TIME'ADDRESS, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "clock_set" );
      TEST_SUPPORT.PRINT_TIME( "TA1 - clock_get       - ", TIME, "" );
      TEXT_IO.NEW_LINE;

      TIME := ( 1990, 2, 5, 8, 30, 45, 0 );
      RTEMS.TIMER.SERVER_FIRE_WHEN(
         SPTEST.TIMER_ID( 1 ),
         TIME,
         SPTEST.DELAYED_SUBPROGRAM'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
        STATUS,
        RTEMS.INVALID_CLOCK,
        "TIMER_SERVER_FIRE_WHEN BEFORE CURRENT TIME"
      );
      TEST_SUPPORT.PRINT_TIME(
        "TA1 - timer_server_fire_when - ",
        TIME,
        " - before INVALID_CLOCK"
      );
      TEXT_IO.NEW_LINE;

   end SCREEN_14;

-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
   begin

      SPTEST.SCREEN_1;
      TEST_SUPPORT.PAUSE_AND_SCREEN_NUMBER( 2 );

      SPTEST.SCREEN_2;
      TEST_SUPPORT.PAUSE_AND_SCREEN_NUMBER( 3 );

      SPTEST.SCREEN_3;
      TEST_SUPPORT.PAUSE_AND_SCREEN_NUMBER( 4 );

      SPTEST.SCREEN_4;
      TEST_SUPPORT.PAUSE_AND_SCREEN_NUMBER( 5 );

      SPTEST.SCREEN_5;
      TEST_SUPPORT.PAUSE_AND_SCREEN_NUMBER( 6 );

      SPTEST.SCREEN_6;
      TEST_SUPPORT.PAUSE_AND_SCREEN_NUMBER( 7 );

      SPTEST.SCREEN_7;
      TEST_SUPPORT.PAUSE_AND_SCREEN_NUMBER( 8 );

      SPTEST.SCREEN_8;
      TEST_SUPPORT.PAUSE_AND_SCREEN_NUMBER( 9 );

      SPTEST.SCREEN_9;
      TEST_SUPPORT.PAUSE_AND_SCREEN_NUMBER( 10 );

      SPTEST.SCREEN_10;
      TEST_SUPPORT.PAUSE_AND_SCREEN_NUMBER( 11 );

      SPTEST.SCREEN_11;
      TEST_SUPPORT.PAUSE_AND_SCREEN_NUMBER( 12 );

      SPTEST.SCREEN_12;
      TEST_SUPPORT.PAUSE_AND_SCREEN_NUMBER( 13 );

      SPTEST.SCREEN_13;
      TEST_SUPPORT.PAUSE_AND_SCREEN_NUMBER( 14 );

      SPTEST.SCREEN_14;

      TEXT_IO.PUT_LINE( "*** END OF TEST 9 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TASK_1;

-- 
--  TASK_2
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT_LINE( "TA2 - semaphore_obtain - sem 1 - WAIT FOREVER" );
      RTEMS.SEMAPHORE.OBTAIN(
         SPTEST.SEMAPHORE_ID( 1 ),
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.OBJECT_WAS_DELETED,
         "SEMAPHORE_OBTAIN WAITING TO BE DELETED"
      );
      TEXT_IO.PUT_LINE(
         "TA2 - semaphore_obtain - woke up with OBJECT_WAS_DELETED"
      );

      TEXT_IO.PUT_LINE( 
         "TA2 - task_delete - delete self - SUCCESSFUL" 
      );
      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA2" );

   end TASK_2;

-- 
--  TASK_3
--

   procedure TASK_3 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      BUFFER         : SPTEST.BUFFER;
      BUFFER_POINTER : RTEMS.ADDRESS;
      MESSAGE_SIZE   : RTEMS.UNSIGNED32 := 0;
      STATUS         : RTEMS.STATUS_CODES;
   begin

      BUFFER_POINTER := BUFFER'ADDRESS;

      TEXT_IO.PUT_LINE(
         "TA3 - message_queue_receive - Q 1 - WAIT FOREVER"
      );
      RTEMS.MESSAGE_QUEUE.RECEIVE(
         SPTEST.QUEUE_ID( 1 ),
         BUFFER_POINTER,
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         MESSAGE_SIZE,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.OBJECT_WAS_DELETED,
         "MESSAGE_QUEUE_RECEIVE WAITING TO BE DELETED"
      );
      TEXT_IO.PUT( "TA3 - message_queue_receive - woke up " );
      TEXT_IO.PUT_LINE( "with OBJECT_WAS_DELETED" );

      TEXT_IO.PUT_LINE( 
         "TA3 - task_delete - delete self - SUCCESSFUL" 
      );
      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA3" );

   end TASK_3;

-- 
--  TASK_4
--

   procedure TASK_4 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.RATE_MONOTONIC.CANCEL(
         SPTEST.PERIOD_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.NOT_OWNER_OF_RESOURCE,
         "RATE_MONOTONIC_CANCEL NOT THE OWNER"
      );
      TEXT_IO.PUT_LINE(
         "TA4 - rate_monotonic_cancel - NOT_OWNER_OF_RESOURCE"
       );

      RTEMS.RATE_MONOTONIC.PERIOD(
         SPTEST.PERIOD_ID( 1 ),
         5,
         STATUS
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
         STATUS,
         RTEMS.NOT_OWNER_OF_RESOURCE,
         "RATE_MONOTONIC_PERIOD NOT THE OWNER"
      );
      TEXT_IO.PUT_LINE(
         "TA4 - rate_monotonic_period - NOT_OWNER_OF_RESOURCE"
      );

      TEXT_IO.PUT_LINE(
         "TA4 - task_delete - delete self - SUCCESSFUL" 
      );
      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA4" );


   end TASK_4;

end SPTEST;
