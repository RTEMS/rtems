-- SPDX-License-Identifier: BSD-2-Clause

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
with TEST_SUPPORT;
with TEXT_IO;
with RTEMS.CLOCK;
with RTEMS.TIMER;

package body SPTEST is

-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEST_SUPPORT.ADA_TEST_BEGIN;

      TIME := ( 1988, 12, 31, 9, 0, 0, 0 );

      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );
      SPTEST.TASK_NAME( 3 ) := RTEMS.BUILD_NAME(  'T', 'A', '3', ' ' );

      SPTEST.TIMER_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'M', '1', ' ' );
      SPTEST.TIMER_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'M', '2', ' ' );
      SPTEST.TIMER_NAME( 3 ) := RTEMS.BUILD_NAME(  'T', 'M', '3', ' ' );

      for INDEX in 1 .. 3
      loop

         RTEMS.TASKS.CREATE( 
            SPTEST.TASK_NAME( INDEX ), 
            1, 
            2048, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            SPTEST.TASK_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         RTEMS.TIMER.CREATE(
            SPTEST.TIMER_NAME( INDEX ), 
            SPTEST.TIMER_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE LOOP" );

      end loop;

      for INDEX in 1 .. 3
      loop

         RTEMS.TASKS.START(
            SPTEST.TASK_ID( INDEX ),
            SPTEST.TASK_1_THROUGH_3'ACCESS,
            RTEMS.TASKS.ARGUMENT( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  RESUME_TASK
--

   procedure RESUME_TASK (
      TIMER_ID        : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   ) is
      pragma Unreferenced(IGNORED_ADDRESS);
      TASK_TO_RESUME : RTEMS.ID;
      STATUS         : RTEMS.STATUS_CODES;
   begin

      TASK_TO_RESUME := SPTEST.TASK_ID(INTEGER( RTEMS.GET_INDEX( TIMER_ID ) ));
      RTEMS.TASKS.RESUME( TASK_TO_RESUME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESUME" );

   end RESUME_TASK;

-- 
--  TASK_1_THROUGH_3
--

   procedure TASK_1_THROUGH_3 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      TID    : RTEMS.ID;
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASKS.IDENT( RTEMS.SELF, RTEMS.SEARCH_ALL_NODES, TID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF SELF" );
   
      loop

         RTEMS.TIMER.FIRE_AFTER( 
            SPTEST.TIMER_ID( INTEGER( ARGUMENT ) ), 
            TEST_SUPPORT.TASK_NUMBER( TID ) * 5 * 
              TEST_SUPPORT.TICKS_PER_SECOND, 
            SPTEST.RESUME_TASK'ACCESS,
            RTEMS.NULL_ADDRESS,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );

         RTEMS.CLOCK.GET_TOD( TIME, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET_TOD" );

         if TIME.SECOND >= 35 then
            TEST_SUPPORT.ADA_TEST_END;
            RTEMS.SHUTDOWN_EXECUTIVE( 0 );
         end if;

         TEST_SUPPORT.PUT_NAME( 
            SPTEST.TASK_NAME( INTEGER( TEST_SUPPORT.TASK_NUMBER( TID ) ) ),
            FALSE
         );

         TEST_SUPPORT.PRINT_TIME( " - clock_get - ", TIME, "" );
         TEXT_IO.NEW_LINE;

         RTEMS.TASKS.SUSPEND( RTEMS.SELF, STATUS ); 
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );
          
      end loop;
   
   end TASK_1_THROUGH_3;

end SPTEST;
