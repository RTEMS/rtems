-- SPDX-License-Identifier: BSD-2-Clause

--
--  MPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation for Test 12 of the RTEMS
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
with RTEMS.PARTITION;
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
      BUFFER_ADDRESS : RTEMS.ADDRESS;
      STATUS         : RTEMS.STATUS_CODES;
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

      MPTEST.PARTITION_NAME( 1 ) := RTEMS.BUILD_NAME(  'P', 'A', 'R', ' ' );

      TEXT_IO.PUT_LINE( "Got to the initialization task" );

      if TEST_SUPPORT.NODE = 2 then

         RTEMS.TASKS.WAKE_AFTER( 1 * TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

         TEXT_IO.PUT_LINE( "Getting ID of remote Partition (Global)" );

         loop

            RTEMS.PARTITION.IDENT(
               MPTEST.PARTITION_NAME( 1 ),
               RTEMS.SEARCH_ALL_NODES,
               MPTEST.PARTITION_ID( 1 ),
               STATUS
            );

            exit when RTEMS.IS_STATUS_SUCCESSFUL( STATUS );

         end loop;

         TEXT_IO.PUT_LINE( 
            "Attempting to delete remote Partition (Global)"
         );

         RTEMS.PARTITION.DELETE( MPTEST.PARTITION_ID( 1 ), STATUS ); 
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS( 
            STATUS, 
            RTEMS.ILLEGAL_ON_REMOTE_OBJECT,
            "PARTITION_DELETE" 
         );

         TEXT_IO.PUT_LINE( 
            "partition_delete correctly returned ILLEGAL_ON_REMOTE_OBJECT!!"
         );

         TEXT_IO.PUT_LINE( "Obtaining a buffer from the global partition" );

         RTEMS.PARTITION.GET_BUFFER(
            MPTEST.PARTITION_ID( 1 ),
            BUFFER_ADDRESS,
            STATUS
         ); 
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_GET_BUFFER" );
         TEXT_IO.PUT( "Address returned was : " );
         UNSIGNED32_IO.PUT(
            RTEMS.SUBTRACT( BUFFER_ADDRESS, RTEMS.NULL_ADDRESS ),
            WIDTH => 8,
            BASE => 16
         );
         TEXT_IO.NEW_LINE;

         RTEMS.PARTITION.RETURN_BUFFER(
            MPTEST.PARTITION_ID( 1 ),
            BUFFER_ADDRESS,
            STATUS
         ); 
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_RETURN_BUFFER" );

         RTEMS.TASKS.WAKE_AFTER( 2 * TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

      else

         TEXT_IO.PUT_LINE( "Creating Partition (Global)" );
         RTEMS.PARTITION.CREATE(
            MPTEST.PARTITION_NAME( 1 ),
            MPTEST.PARTITION_AREA( 0 )'ADDRESS,
            128,
            64,
            RTEMS.GLOBAL,
            MPTEST.PARTITION_ID( 1 ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_CREATE" );

         TEXT_IO.PUT_LINE( "Sleeping for three seconds" );
         RTEMS.TASKS.WAKE_AFTER( 3 * TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );

         TEXT_IO.PUT_LINE( "Deleting Partition (Global)" );
         RTEMS.PARTITION.DELETE( MPTEST.PARTITION_ID( 1 ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PARTITION_DELETE" );

      end if;

      TEST_SUPPORT.ADA_TEST_END;

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end INIT;

end MPTEST;
