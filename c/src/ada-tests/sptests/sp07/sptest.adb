--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 7 of the RTEMS
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
with RTEMS.EXTENSION;
with RTEMS.FATAL;

package body SPTEST is

      type Task_Event_Type is (
         Created,
         Deleted,
         Restarted,
         Started
      );

      type Task_Event is record
         The_Event : Task_Event_Type;
         Task1     : RTEMS.Unsigned32;
         Task2     : RTEMS.Unsigned32;
      end record;

      Task_Events : array (1 .. 10) of Task_Event;
      Task_Events_Index : Natural := Task_Events'First;

      procedure Log_Task_Event (
         The_Event : in     Task_Event_Type;
         Task1     : in     RTEMS.Unsigned32;
         Task2     : in     RTEMS.Unsigned32
      ) is
      begin
         if Task_Events_Index = Task_Events'Last then
            RTEMS.Fatal.Error_Occurred ( 1 );  -- no other choice
         else
            Task_Events (Task_Events_Index).The_Event := The_Event;
            Task_Events (Task_Events_Index).Task1 := Task1;
            Task_Events (Task_Events_Index).Task2 := Task2;
            Task_Events_Index := Task_Events_Index + 1;
         end if;
      end Log_Task_Event;

      procedure Flush_Task_Event_Log is
      begin
         for I in Task_Events'First .. Task_Events_Index - 1 loop

            case Task_Events (I).The_Event is

               when Created =>
                  TEXT_IO.PUT( "TASKS_CREATE - " );
                  TEST_SUPPORT.PUT_NAME( 
                     SPTEST.TASK_NAME( Task_Events (I).Task1 ),
                     FALSE
                  );
                  TEXT_IO.PUT_LINE( " - created." );

               when Deleted =>
                  TEXT_IO.PUT( "TASKS_DELETE - " );
                  TEST_SUPPORT.PUT_NAME( 
                     SPTEST.TASK_NAME( Task_Events (I).Task1 ),
                     FALSE
                  );

                  TEXT_IO.PUT( " deleting " );
                  TEST_SUPPORT.PUT_NAME( 
                     SPTEST.TASK_NAME( Task_Events (I).Task2 ),
                     TRUE
                  );

               when Restarted =>
                  TEXT_IO.PUT( "TASKS_RESTART - " );
                  TEST_SUPPORT.PUT_NAME( 
                     SPTEST.TASK_NAME( Task_Events (I).Task1 ),
                     FALSE
                  );
                  TEXT_IO.PUT_LINE( " - restarted." );

               when Started =>
                  TEXT_IO.PUT( "TASKS_START - " );
                  TEST_SUPPORT.PUT_NAME( 
                     SPTEST.TASK_NAME( Task_Events (I).Task1 ),
                     FALSE
                  );
                  TEXT_IO.PUT_LINE( " - started." );

            end case;
         end loop;

         -- Reset the events list
         Task_Events_Index := Task_Events'First;

      end Flush_Task_Event_Log;

--
-- TCB_To_ID
--

      function TCB_To_ID (
         TCB : RTEMS.TCB_POINTER
      ) return RTEMS.ID;
      pragma Import (C, TCB_To_ID, "tcb_to_id" );

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
      TEXT_IO.PUT_LINE( "*** TEST 7 ***" );

      SPTEST.EXTENSION_NAME( 1 ) := RTEMS.BUILD_NAME(  'E', 'X', 'T', ' ' );

      RTEMS.EXTENSION.CREATE(
         SPTEST.EXTENSION_NAME( 1 ),
         SPTEST.EXTENSIONS'ACCESS,
         EXTENSION_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EXTENSION_CREATE" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );
      SPTEST.TASK_NAME( 3 ) := RTEMS.BUILD_NAME(  'T', 'A', '3', ' ' );
      SPTEST.TASK_NAME( 4 ) := RTEMS.BUILD_NAME(  'T', 'A', '4', ' ' );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         4, 
         RTEMS.MINIMUM_STACK_SIZE, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 2 ), 
         4, 
         RTEMS.MINIMUM_STACK_SIZE, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA2" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 3 ), 
         250, 
         RTEMS.MINIMUM_STACK_SIZE, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 3 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA3" );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 4 ), 
         254, 
         RTEMS.MINIMUM_STACK_SIZE, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 4 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA4" );

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

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 4 ),
         SPTEST.TASK_4'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA4" );

      RTEMS.TASKS.RESTART( SPTEST.TASK_ID( 3 ), 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESTART OF TA3" );

      Flush_Task_Event_Log;

      RTEMS.TASKS.SET_NOTE( SPTEST.TASK_ID( 1 ), 8, 4, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_NOTE OF TA1" );
      TEXT_IO.PUT( "INIT - task_set_note - set TA1's NOTEPAD_8" ); 
      TEXT_IO.PUT_LINE( " to TA1's initial priority:  4" );
      
      RTEMS.TASKS.SET_NOTE( SPTEST.TASK_ID( 2 ), 8, 4, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_NOTE OF TA2" );
      TEXT_IO.PUT( "INIT - task_set_note - set TA2's NOTEPAD_8" ); 
      TEXT_IO.PUT_LINE( " to TA2's initial priority:  4" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS            : RTEMS.STATUS_CODES;
      THE_PRIORITY      : RTEMS.TASKS.PRIORITY;
      PREVIOUS_PRIORITY : RTEMS.TASKS.PRIORITY;
   begin

      TEST_SUPPORT.PAUSE;

      RTEMS.TASKS.SET_PRIORITY( 
         RTEMS.SELF, 
         RTEMS.TASKS.CURRENT_PRIORITY, 
         THE_PRIORITY, 
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY" );
      TEXT_IO.PUT( "TA1 - task_set_priority - ");
      TEXT_IO.PUT( "get initial priority of self: ");
      UNSIGNED32_IO.PUT( THE_PRIORITY, BASE => 10, WIDTH => 2 );
      TEXT_IO.NEW_LINE;

      loop

         RTEMS.TASKS.GET_NOTE( RTEMS.SELF, 8, THE_PRIORITY, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_GET_NOTE" );
         TEXT_IO.PUT( "TA1 - task_get_note - "); 
         TEXT_IO.PUT( "get NOTEPAD_8 - current priority: " );
         UNSIGNED32_IO.PUT( THE_PRIORITY, BASE => 10, WIDTH => 2 );
         TEXT_IO.NEW_LINE;

         THE_PRIORITY := THE_PRIORITY - 1;

         if THE_PRIORITY = 0 then
            TEXT_IO.PUT_LINE( "TA1 - task_suspend - suspend TA2" ); 
            RTEMS.TASKS.SUSPEND( SPTEST.TASK_ID( 2 ), STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );

            TEXT_IO.PUT( "TA1 - task_set_priority - ");
            TEXT_IO.PUT_LINE( "set priority of TA2 ( blocked )");
            RTEMS.TASKS.SET_PRIORITY( 
               SPTEST.TASK_ID( 2 ), 
               5, 
               PREVIOUS_PRIORITY, 
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY" );

            RTEMS.TASKS.DELETE( SPTEST.TASK_ID( 2 ), STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA2" );

            RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

         end if;

         TEXT_IO.PUT( "TA1 - task_set_note - set TA2's NOTEPAD_8: " );
         UNSIGNED32_IO.PUT( THE_PRIORITY, BASE => 10, WIDTH => 2 );
         TEXT_IO.NEW_LINE;
         RTEMS.TASKS.SET_NOTE( 
            SPTEST.TASK_ID( 2 ), 
            8, 
            THE_PRIORITY, 
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_NOTE" );

         TEXT_IO.PUT("TA1 - task_set_priority - set TA2's priority: ");
         UNSIGNED32_IO.PUT( THE_PRIORITY, BASE => 10, WIDTH => 2 );
         TEXT_IO.NEW_LINE;
         RTEMS.TASKS.SET_PRIORITY( 
            SPTEST.TASK_ID( 2 ), 
            THE_PRIORITY, 
            PREVIOUS_PRIORITY, 
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY" );

      end loop;
   
   end TASK_1;

-- 
--  TASK_2
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS            : RTEMS.STATUS_CODES;
      THE_PRIORITY      : RTEMS.TASKS.PRIORITY;
      PREVIOUS_PRIORITY : RTEMS.TASKS.PRIORITY;
   begin

      loop

         RTEMS.TASKS.GET_NOTE( RTEMS.SELF, 8, THE_PRIORITY, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_GET_NOTE" );
         TEXT_IO.PUT( "TA2 - task_get_note - ");
         TEXT_IO.PUT( "get NOTEPAD_8 - current priority: " );
         UNSIGNED32_IO.PUT( THE_PRIORITY, BASE => 10, WIDTH => 2 );
         TEXT_IO.NEW_LINE;

         THE_PRIORITY := THE_PRIORITY - 1;

         if THE_PRIORITY = 0 then
            TEXT_IO.PUT_LINE( "TA2 - task_suspend - suspend TA1" ); 
            RTEMS.TASKS.SUSPEND( SPTEST.TASK_ID( 1 ), STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );

            TEXT_IO.PUT( "TA2 - task_set_priority - ");
            TEXT_IO.PUT_LINE( "set priority of TA1 ( blocked )");
            RTEMS.TASKS.SET_PRIORITY( 
               SPTEST.TASK_ID( 1 ), 
               5, 
               PREVIOUS_PRIORITY, 
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY" );

            RTEMS.TASKS.DELETE( 
               SPTEST.TASK_ID( 1 ), 
               STATUS 
            );                            -- TA1 is blocked
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA1" );

            RTEMS.TASKS.DELETE( 
               SPTEST.TASK_ID( 3 ), 
               STATUS
            );                            -- TA3 is ready
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF TA3" );

            RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

         else

            TEXT_IO.PUT( "TA2 - task_set_note - set TA1's NOTEPAD_8: " );
            UNSIGNED32_IO.PUT( THE_PRIORITY, BASE => 10, WIDTH => 2 );
            TEXT_IO.NEW_LINE;
            RTEMS.TASKS.SET_NOTE( 
               SPTEST.TASK_ID( 1 ), 
               8, 
               THE_PRIORITY, 
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_NOTE" );

            TEXT_IO.PUT( "TA2 - task_set_priority - "); 
            TEXT_IO.PUT( "set TA1's priority: ");
            UNSIGNED32_IO.PUT( THE_PRIORITY, BASE => 10, WIDTH => 2);
            TEXT_IO.NEW_LINE;
            RTEMS.TASKS.SET_PRIORITY( 
               SPTEST.TASK_ID( 1 ), 
               THE_PRIORITY, 
               PREVIOUS_PRIORITY, 
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY" );

         end if;

      end loop;
   
   end TASK_2;

-- 
--  TASK_3
--

   procedure TASK_3 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS : RTEMS.STATUS_CODES;
   begin
   
      loop

         RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

      end loop;

   end TASK_3;

-- 
--  TASK_4
--

   procedure TASK_4 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
   begin

      Flush_Task_Event_Log;

      TEXT_IO.PUT_LINE( "TA4 - exitting task" );

   end TASK_4;

-- 
--  TASK_EXIT_EXTENSION
--

   procedure TASK_EXIT_EXTENSION (
      RUNNING_TASK : in     RTEMS.TCB_POINTER
   ) is
   begin

      if TEST_SUPPORT.TASK_NUMBER( TCB_To_ID( RUNNING_TASK ) ) > 0 then
         TEXT_IO.PUT_LINE( "TASK_EXITTED - user extension invoked correctly" );
      end if;

      TEXT_IO.PUT_LINE( "*** END OF TEST 7 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TASK_EXIT_EXTENSION;

-- 
--  TASK_CREATE_EXTENSION
--

   function TASK_CREATE_EXTENSION (
      UNUSED       : in     RTEMS.TCB_POINTER;
      CREATED_TASK : in     RTEMS.TCB_POINTER
   ) return RTEMS.Boolean is
      pragma Unreferenced(UNUSED);
   begin

      if TEST_SUPPORT.TASK_NUMBER( TCB_To_ID( CREATED_TASK ) ) > 0 then
         Log_Task_Event ( 
            Created, 
            TEST_SUPPORT.TASK_NUMBER( TCB_To_ID( CREATED_TASK ) ),
            0
         );
      end if;

      return RTEMS.True;

   end TASK_CREATE_EXTENSION;

-- 
--  TASK_DELETE_EXTENSION
--

   procedure TASK_DELETE_EXTENSION (
      RUNNING_TASK : in     RTEMS.TCB_POINTER;
      DELETED_TASK : in     RTEMS.TCB_POINTER
   ) is
   begin

      if TEST_SUPPORT.TASK_NUMBER( TCB_To_ID( RUNNING_TASK ) ) > 0 and
        TEST_SUPPORT.TASK_NUMBER( TCB_To_ID( DELETED_TASK ) ) > 0 then
         Log_Task_Event ( 
            Deleted, 
            TEST_SUPPORT.TASK_NUMBER( TCB_To_ID( RUNNING_TASK ) ),
            TEST_SUPPORT.TASK_NUMBER( TCB_To_ID( DELETED_TASK ) )
         );
      end if;

   end TASK_DELETE_EXTENSION;

-- 
--  TASK_RESTART_EXTENSION
--

   procedure TASK_RESTART_EXTENSION (
      UNUSED         : in     RTEMS.TCB_POINTER;
      RESTARTED_TASK : in     RTEMS.TCB_POINTER
   ) is
      pragma Unreferenced(UNUSED);
   begin

      if TEST_SUPPORT.TASK_NUMBER( TCB_To_ID( RESTARTED_TASK ) ) > 0 then
         Log_Task_Event ( 
            Restarted, 
            TEST_SUPPORT.TASK_NUMBER( TCB_To_ID( RESTARTED_TASK ) ),
            0
         );
      end if;

   end TASK_RESTART_EXTENSION;

-- 
--  TASK_START_EXTENSION
--

   procedure TASK_START_EXTENSION (
      UNUSED       : in     RTEMS.TCB_POINTER;
      STARTED_TASK : in     RTEMS.TCB_POINTER
   ) is
      pragma Unreferenced(UNUSED);
   begin

      if TEST_SUPPORT.TASK_NUMBER( TCB_To_ID( STARTED_TASK ) ) > 0 then
         Log_Task_Event ( 
            Started, 
            TEST_SUPPORT.TASK_NUMBER( TCB_To_ID( STARTED_TASK ) ),
            0
         );
      end if;

   end TASK_START_EXTENSION;

end SPTEST;
