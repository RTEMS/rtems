-- SPDX-License-Identifier: BSD-2-Clause

--
--  MPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation for Test 6 of the RTEMS
--  Multiprocessor Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  Redistribution and use in source and binary forms, with or without
--  modification, are permitted provided that the following conditions
--  are met:
--  1. Redistributions of source code must retain the above copyright
--     notice, this list of conditions and the following disclaimer.
--  2. Redistributions in binary form must reproduce the above copyright
--     notice, this list of conditions and the following disclaimer in the
--     documentation and/or other materials provided with the distribution.
--
--  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
--  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
--  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
--  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
--  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
--  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
--  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
--  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
--  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
--  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
--  POSSIBILITY OF SUCH DAMAGE.
--

with INTERFACES; use INTERFACES;
with RTEMS.EVENT;
with RTEMS.TIMER;
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
      pragma Unreferenced(ARGUMENT);
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEST_SUPPORT.ADA_TEST_BEGIN;
      TEXT_IO.PUT( "*** NODE " );
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

      MPTEST.TIMER_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'M', '1', ' ' );
      MPTEST.TIMER_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'M', '2', ' ' );

      RTEMS.TIMER.CREATE(
         MPTEST.TIMER_NAME( TEST_SUPPORT.NODE ),
         MPTEST.TIMER_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE" );

      TEXT_IO.PUT_LINE( "Deleting initialization task" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--
--  STOP_TEST_TSR
--
 
   procedure STOP_TEST_TSR (
      IGNORED1 : in     RTEMS.ID;
      IGNORED2 : in     RTEMS.ADDRESS
   ) is
   begin
 
      MPTEST.STOP_TEST := TRUE;
 
   end STOP_TEST_TSR;
 
--
--  TEST_TASK
--

   procedure TEST_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      COUNT                    : RTEMS.UNSIGNED32;
      EVENT_OUT                : RTEMS.EVENT_SET;
      EVENT_FOR_THIS_ITERATION : RTEMS.EVENT_SET;
      STATUS                   : RTEMS.STATUS_CODES;
   begin

      MPTEST.STOP_TEST := FALSE;

      if TEST_SUPPORT.NODE = 1 then
         MPTEST.REMOTE_NODE     := 2;
      else
         MPTEST.REMOTE_NODE     := 1;
      end if;

      TEXT_IO.PUT( "Remote task's name is : " );
      TEST_SUPPORT.PUT_NAME( MPTEST.TASK_NAME( MPTEST.REMOTE_NODE ), TRUE );

      TEXT_IO.PUT_LINE( "Getting TID of remote task" );

      loop

         RTEMS.TASKS.IDENT( 
            MPTEST.TASK_NAME( MPTEST.REMOTE_NODE ),
            RTEMS.SEARCH_ALL_NODES,
            MPTEST.REMOTE_TID,
            STATUS
         );

         exit when RTEMS.IS_STATUS_SUCCESSFUL( STATUS );

      end loop;

      if TEST_SUPPORT.NODE = 1 then
         TEXT_IO.PUT_LINE( "Sending events to remote task" );
      else
         TEXT_IO.PUT_LINE( "Receiving events from remote task" );
      end if;

      RTEMS.TIMER.FIRE_AFTER(
         MPTEST.TIMER_ID( 1 ),
         5 * TEST_SUPPORT.TICKS_PER_SECOND,
         MPTEST.STOP_TEST_TSR'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );
 
      COUNT := 0;

      loop
 
         exit when MPTEST.STOP_TEST;

         EVENT_FOR_THIS_ITERATION := 
            MPTEST.EVENT_SET_TABLE( 
               INTEGER( COUNT ) mod MPTEST.EVENT_SET_TABLE'LAST + 1
            );

         if TEST_SUPPORT.NODE = 1 then

            RTEMS.EVENT.SEND( 
               MPTEST.REMOTE_TID,
               EVENT_FOR_THIS_ITERATION,
               STATUS
            );
            TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_SEND" );

         else

            RTEMS.EVENT.RECEIVE( 
               EVENT_FOR_THIS_ITERATION,
               RTEMS.DEFAULT_OPTIONS,
               1 * TEST_SUPPORT.TICKS_PER_SECOND,
               EVENT_OUT,
               STATUS
            );

            if RTEMS.ARE_STATUSES_EQUAL( STATUS, RTEMS.TIMEOUT ) then
               TEXT_IO.NEW_LINE( 1 );

               if TEST_SUPPORT.NODE = 2 then
                  TEXT_IO.PUT_LINE(
                     "Correct behavior if the other node exitted."
                  );
               else
                  TEXT_IO.PUT_LINE(
                     "ERROR... node 1 died"
                  );
               end if;

               exit;

            else

               TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "EVENT_RECEIVE" );

            end if;

         end if;

         if (COUNT mod MPTEST.MAXIMUM_DOTS) = 0 then

            TEST_SUPPORT.PUT_DOT( "." );

         end if;

         COUNT := COUNT + 1;

      end loop;

      TEXT_IO.NEW_LINE;

      if TEST_SUPPORT.NODE = 2 then

         RTEMS.EVENT.RECEIVE( 
            RTEMS.EVENT_16,
            RTEMS.DEFAULT_OPTIONS,
            1 * TEST_SUPPORT.TICKS_PER_SECOND,
            EVENT_OUT,
            STATUS
         );
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS( 
            RTEMS.TIMEOUT,
            STATUS,
            "EVENT_RECEIVE"
         );

         TEXT_IO.NEW_LINE;
         TEXT_IO.PUT_LINE( "event_receive - correctly returned TIMEOUT" );

      end if;

      TEST_SUPPORT.ADA_TEST_END;

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TEST_TASK;

end MPTEST;
