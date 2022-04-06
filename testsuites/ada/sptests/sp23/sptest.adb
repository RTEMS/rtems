-- SPDX-License-Identifier: BSD-2-Clause

--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 23 of the RTEMS
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

with ADDRESS_IO;
with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;
with RTEMS.PORT;

package body SPTEST is

-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      INTERNAL_AREA : RTEMS.ADDRESS;
      EXTERNAL_AREA : RTEMS.ADDRESS;
      STATUS        : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEST_SUPPORT.ADA_TEST_BEGIN;

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         1, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 1 ),
         SPTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      SPTEST.PORT_NAME( 1 ) := RTEMS.BUILD_NAME(  'D', 'P', '1', ' ' );

      INTERNAL_AREA := SPTEST.INTERNAL_PORT_AREA( 0 )'ADDRESS;
      EXTERNAL_AREA := SPTEST.EXTERNAL_PORT_AREA( 0 )'ADDRESS;
      RTEMS.PORT.CREATE(
         SPTEST.PORT_NAME( 1 ), 
         INTERNAL_AREA,
         EXTERNAL_AREA,
         SPTEST.INTERNAL_PORT_AREA'LENGTH,
         SPTEST.PORT_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PORT_CREATE OF DP1" );
      TEXT_IO.PUT( "INIT - port_create - DP1 - internal = " );
      ADDRESS_IO.PUT( INTERNAL_AREA, WIDTH => 8, BASE => 16 );
      TEXT_IO.PUT( " external =  " );
      ADDRESS_IO.PUT( EXTERNAL_AREA, WIDTH => 8, BASE => 16 );
      TEXT_IO.NEW_LINE;
   
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
      DPID            : RTEMS.ID;
      TO_BE_CONVERTED : RTEMS.ADDRESS; 
      CONVERTED       : RTEMS.ADDRESS; 
      STATUS          : RTEMS.STATUS_CODES;
   begin

      RTEMS.PORT.IDENT( SPTEST.PORT_NAME( 1 ), DPID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PORT_IDENT" );
      TEXT_IO.PUT( "TA1 - port_ident - " );
      UNSIGNED32_IO.PUT( DPID, WIDTH => 8, BASE => 16 );
      TEXT_IO.NEW_LINE;

      TO_BE_CONVERTED :=  SPTEST.EXTERNAL_PORT_AREA( 16#E# )'ADDRESS;
      RTEMS.PORT.EXTERNAL_TO_INTERNAL(
         SPTEST.PORT_ID( 1 ),
         TO_BE_CONVERTED,
         CONVERTED,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PORT_EXTERNAL_TO_INTERNAL" );
      TEXT_IO.PUT( "TA1 - port_external_to_internal - external: " );
      ADDRESS_IO.PUT( TO_BE_CONVERTED, WIDTH => 8, BASE => 16 );
      TEXT_IO.PUT( " => internal: " );
      ADDRESS_IO.PUT( CONVERTED, WIDTH => 8, BASE => 16 );
      TEXT_IO.NEW_LINE;
   
      TO_BE_CONVERTED :=  SPTEST.INTERNAL_PORT_AREA( 16#E# )'ADDRESS;
      RTEMS.PORT.INTERNAL_TO_EXTERNAL(
         SPTEST.PORT_ID( 1 ),
         TO_BE_CONVERTED,
         CONVERTED,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PORT_INTERNAL_TO_EXTERNAL" );
      TEXT_IO.PUT( "TA1 - port_internal_to_external - internal: " );
      ADDRESS_IO.PUT( TO_BE_CONVERTED, WIDTH => 8, BASE => 16 );
      TEXT_IO.PUT( " => external: " );
      ADDRESS_IO.PUT( CONVERTED, WIDTH => 8, BASE => 16 );
      TEXT_IO.NEW_LINE;
   
      TO_BE_CONVERTED :=  SPTEST.ABOVE_PORT_AREA( 16#E# )'ADDRESS;
      RTEMS.PORT.EXTERNAL_TO_INTERNAL(
         SPTEST.PORT_ID( 1 ),
         TO_BE_CONVERTED,
         CONVERTED,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PORT_EXTERNAL_TO_INTERNAL" );
      TEXT_IO.PUT( "TA1 - port_external_to_internal - external: " );
      ADDRESS_IO.PUT( TO_BE_CONVERTED, WIDTH => 8, BASE => 16 );
      TEXT_IO.PUT( " => internal: " );
      ADDRESS_IO.PUT( CONVERTED, WIDTH => 8, BASE => 16 );
      TEXT_IO.NEW_LINE;
   
      TO_BE_CONVERTED :=  SPTEST.BELOW_PORT_AREA( 16#E# )'ADDRESS;
      RTEMS.PORT.INTERNAL_TO_EXTERNAL(
         SPTEST.PORT_ID( 1 ),
         TO_BE_CONVERTED,
         CONVERTED,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PORT_INTERNAL_TO_EXTERNAL" );
      TEXT_IO.PUT( "TA1 - port_internal_to_external - internal: " );
      ADDRESS_IO.PUT( TO_BE_CONVERTED, WIDTH => 8, BASE => 16 );
      TEXT_IO.PUT( " => external: " );
      ADDRESS_IO.PUT( CONVERTED, WIDTH => 8, BASE => 16 );
      TEXT_IO.NEW_LINE;
   
      TEXT_IO.PUT_LINE( "TA1 - port_delete - DP1" );
      RTEMS.PORT.DELETE( SPTEST.PORT_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "PORT_DELETE" );

      TEST_SUPPORT.ADA_TEST_END;
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TASK_1;

end SPTEST;
