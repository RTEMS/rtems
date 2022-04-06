-- SPDX-License-Identifier: BSD-2-Clause

--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of the Ada Task Name test of the RTEMS
--  Ada Single Processor Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-2009.
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

with RTEMS;
with RTEMS.OBJECT;
with TEST_SUPPORT;
with TEXT_IO;
with System; use System;  -- for Null Pointer comparison

package body SPTEST is

   task body AdaTask is
      Status     : RTEMS.Status_Codes;
      Pointer    : RTEMS.Address;
      StringName : String(1 .. 120) := (1 .. 120 => '*' );
      NewName    : constant String  := "Josiah";
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEST_SUPPORT.ADA_TEST_BEGIN;

      RTEMS.Object.Get_Name( RTEMS.Self, StringName, Pointer );
      if Pointer = RTEMS.Null_Address then
         TEXT_IO.PUT_LINE( "Object_Get_Name_Failed" );
      else
         TEXT_IO.PUT_LINE( "My name is (" & StringName & ")" );
      end if;

      TEXT_IO.PUT_LINE( "Setting name to (Josiah)" );
      RTEMS.Object.Set_Name( RTEMS.Self, NewName, Status );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "Object_Set_Name" );

      RTEMS.Object.Get_Name( RTEMS.Self, StringName, Pointer );
      if Pointer = RTEMS.Null_Address then
         TEXT_IO.PUT_LINE( "Object_Get_Name_Failed" );
      else
         TEXT_IO.PUT_LINE( "My name is (" & StringName & ")" );
      end if;

      TEST_SUPPORT.ADA_TEST_END;

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end AdaTask;

end SPTEST;
