-- SPDX-License-Identifier: BSD-2-Clause

--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 1 of the RTEMS
--  Timing Test Suite.
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
with RTEMS_CALLING_OVERHEAD;
with TEST_SUPPORT;
with TEXT_IO;
with TIME_TEST_SUPPORT;
with TIMER_DRIVER;
with RTEMS.SEMAPHORE;

package body TMTEST is

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

      TMTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      TMTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  'T', 'A', '2', ' ' );
      TMTEST.TASK_NAME( 3 ) := RTEMS.BUILD_NAME(  'T', 'A', '3', ' ' );

      RTEMS.TASKS.CREATE( 
         TMTEST.TASK_NAME( 1 ), 
         1, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TMTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASKS.START(
         TMTEST.TASK_ID( 1 ),
         TMTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

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
      SEMAPHORE_OBTAIN_TIME         : RTEMS.UNSIGNED32;
      SEMAPHORE_RELEASE_TIME        : RTEMS.UNSIGNED32;
      SEMAPHORE_OBTAIN_NO_WAIT_TIME : RTEMS.UNSIGNED32;
      SEMAPHORE_OBTAIN_LOOP_TIME    : RTEMS.UNSIGNED32;
      SEMAPHORE_RELEASE_LOOP_TIME   : RTEMS.UNSIGNED32;
      NAME                          : RTEMS.NAME;
      SMID                          : RTEMS.ID;
      STATUS                        : RTEMS.STATUS_CODES;
   begin

      NAME := RTEMS.BUILD_NAME(  'S', 'M', '1', ' ' );
     
      SEMAPHORE_OBTAIN_TIME         := 0;
      SEMAPHORE_RELEASE_TIME        := 0;
      SEMAPHORE_OBTAIN_NO_WAIT_TIME := 0;
      SEMAPHORE_OBTAIN_LOOP_TIME    := 0;
      SEMAPHORE_RELEASE_LOOP_TIME   := 0;
    
-- Time one invocation of SEMAPHORE_CREATE

      TIMER_DRIVER.INITIALIZE;

         RTEMS.SEMAPHORE.CREATE(
            NAME,
            TIME_TEST_SUPPORT.OPERATION_COUNT,
            RTEMS.DEFAULT_ATTRIBUTES,
            RTEMS.TASKS.NO_PRIORITY,
            SMID,
            STATUS
         );
     
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      
      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_CREATE", 
         TMTEST.END_TIME, 
         1, 
         0,
         RTEMS_CALLING_OVERHEAD.SEMAPHORE_CREATE 
      );

-- Time one invocation of SEMAPHORE_DELETE

      TIMER_DRIVER.INITIALIZE;

         RTEMS.SEMAPHORE.DELETE( SMID, STATUS );
     
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      
      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_DELETE", 
         TMTEST.END_TIME, 
         1, 
         0,
         RTEMS_CALLING_OVERHEAD.SEMAPHORE_DELETE 
      );

-- Create semaphore for repeated operations.

      RTEMS.SEMAPHORE.CREATE(
         NAME,
         TIME_TEST_SUPPORT.OPERATION_COUNT,
         RTEMS.DEFAULT_ATTRIBUTES,
         RTEMS.TASKS.NO_PRIORITY,
         SMID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE" );

-- Perform repeated SEMAPHORE OBTAIN's and RELEASE's 
     
      for ITERATIONS in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         TIMER_DRIVER.INITIALIZE;
            for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
            loop
               TIMER_DRIVER.EMPTY_FUNCTION;
            end loop;
         TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

         SEMAPHORE_OBTAIN_LOOP_TIME  := SEMAPHORE_OBTAIN_LOOP_TIME + 
                                          TMTEST.END_TIME;
         SEMAPHORE_RELEASE_LOOP_TIME := SEMAPHORE_RELEASE_LOOP_TIME + 
                                          TMTEST.END_TIME;

         -- Semaphore_obtain (available loop)

         TIMER_DRIVER.INITIALIZE;
            for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
            loop

               RTEMS.SEMAPHORE.OBTAIN( 
                  SMID, 
                  RTEMS.DEFAULT_OPTIONS, 
                  RTEMS.NO_TIMEOUT, 
                  STATUS 
               );
    
            end loop;

         TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

         SEMAPHORE_OBTAIN_TIME := SEMAPHORE_OBTAIN_TIME + TMTEST.END_TIME;

         -- Semaphore_release

            TIMER_DRIVER.INITIALIZE;
     
               for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
               loop

                  RTEMS.SEMAPHORE.RELEASE( SMID, STATUS );
     
               end loop;

            TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

            SEMAPHORE_RELEASE_TIME := SEMAPHORE_RELEASE_TIME + 
                                        TMTEST.END_TIME;

         -- Semaphore_p (NO_WAIT)

         TIMER_DRIVER.INITIALIZE;

            for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
            loop
     
               RTEMS.SEMAPHORE.OBTAIN( 
                  SMID, 
                  RTEMS.NO_WAIT, 
                  RTEMS.NO_TIMEOUT, 
                  STATUS 
               );

            end loop;
     
         TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

         SEMAPHORE_OBTAIN_NO_WAIT_TIME := 
            SEMAPHORE_OBTAIN_NO_WAIT_TIME + TMTEST.END_TIME;

         -- Semaphore_v 

            TIMER_DRIVER.INITIALIZE;
     
               for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
               loop

                  RTEMS.SEMAPHORE.RELEASE( SMID, STATUS );
     
               end loop;

            TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

            SEMAPHORE_RELEASE_TIME := SEMAPHORE_RELEASE_TIME + 
                                        TMTEST.END_TIME;

      end loop;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_OBTAIN (semaphore available)", 
         SEMAPHORE_OBTAIN_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT *
           TIME_TEST_SUPPORT.OPERATION_COUNT,
         SEMAPHORE_OBTAIN_LOOP_TIME,
         RTEMS_CALLING_OVERHEAD.SEMAPHORE_OBTAIN 
      );

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_OBTAIN (NO_WAIT)", 
         SEMAPHORE_OBTAIN_NO_WAIT_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT *
           TIME_TEST_SUPPORT.OPERATION_COUNT,
         SEMAPHORE_OBTAIN_LOOP_TIME,
         RTEMS_CALLING_OVERHEAD.SEMAPHORE_OBTAIN 
      );

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_RELEASE (no task waiting)", 
         SEMAPHORE_RELEASE_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT *
           TIME_TEST_SUPPORT.OPERATION_COUNT * 2,
         SEMAPHORE_RELEASE_LOOP_TIME * 2,
         RTEMS_CALLING_OVERHEAD.SEMAPHORE_RELEASE 
      );

      TEST_SUPPORT.ADA_TEST_END;
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );
   
   end TASK_1;

end TMTEST;
