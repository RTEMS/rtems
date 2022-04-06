-- SPDX-License-Identifier: BSD-2-Clause

--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of the Nanosecond test of the
--  Sample Test Suite.
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

with Ada.Integer_Text_IO;
with Interfaces.C;
with RTEMS;
with RTEMS.Clock;
with Text_IO;
with TEST_SUPPORT;
use type Interfaces.C.Long;
use type RTEMS.Time_T;

package body SPTEST is

   Dummy_Variable : Natural := 0;

   procedure Simple_Procedure is
   begin
      Dummy_Variable := Dummy_Variable + 1;
   end Simple_Procedure;

   procedure Subtract_Em (
      Start  : in     RTEMS.Timespec;
      Stop   : in     RTEMS.Timespec;
      Result :    out RTEMS.Timespec
   ) is
      Nanoseconds_Per_Second : constant := 1000000000;
   begin
      if (Stop.TV_Nsec < Start.TV_Nsec) then
         Result.TV_Sec  := Stop.TV_Sec - Start.TV_Sec - 1;
         Result.TV_Nsec :=
           (Nanoseconds_Per_Second - Start.TV_Nsec) + Stop.TV_Nsec;
      else
         Result.TV_Sec  := Stop.TV_Sec - Start.TV_Sec;
         Result.TV_Nsec := Stop.TV_Nsec - Start.TV_Nsec;
      end if;
   end Subtract_Em;


-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      Status : RTEMS.Status_Codes;
      Start  : RTEMS.Timespec;
      Stop   : RTEMS.Timespec;
      Diff   : RTEMS.Timespec;
      Max    : Integer;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEST_SUPPORT.ADA_TEST_BEGIN;

      --
      --  Iterate 10 times showing difference in TOD
      --

      TEXT_IO.PUT_LINE( "10 iterations of getting TOD NOT tested in Ada" );

      --
      --  Iterate 10 times showing difference in Uptime
      --

      TEXT_IO.NEW_LINE;
      TEXT_IO.PUT_LINE( "10 iterations of getting Uptime" );

      for Index in 1 .. 10 loop

         RTEMS.Clock.Get_Uptime( Start, Status );
         RTEMS.Clock.Get_Uptime( Stop, Status );

         Subtract_Em( Start, Stop, Diff );

         Ada.Integer_Text_IO.Put( Integer( Start.TV_Sec ), 1 );
         Text_IO.Put( ":" );
         Ada.Integer_Text_IO.Put( Integer( Start.TV_Nsec ), 9 );
         Text_IO.Put( " " );
         Ada.Integer_Text_IO.Put( Integer( Stop.TV_Sec ), 1 );
         Text_IO.Put( ":" );
         Ada.Integer_Text_IO.Put( Integer( Stop.TV_Nsec ), 9 );
         Text_IO.Put( " --> " );
         Ada.Integer_Text_IO.Put( Integer( Diff.TV_Sec ), 1 );
         Text_IO.Put( ":" );
         Ada.Integer_Text_IO.Put( Integer( Diff.TV_Nsec ), 9 );
         Text_IO.New_Line;
      end loop;

      --
      --  Iterate 10 times showing difference in Uptime with different counts
      --

      TEXT_IO.NEW_LINE;
      TEXT_IO.PUT_LINE(
         "10 iterations of getting Uptime with different loop values"
      );

      for Index in 1 .. 10 loop
         Max := (Index * 10000);
         RTEMS.Clock.Get_Uptime( Start, Status );
         for j in 1 .. Max loop
            Simple_Procedure;
         end loop;
         RTEMS.Clock.Get_Uptime( Stop, Status );

         Subtract_Em( Start, Stop, Diff );

         Text_IO.Put( "loop of " );
         Ada.Integer_Text_IO.Put( Max, 6 );
         Text_IO.Put( " " );
         Ada.Integer_Text_IO.Put( Integer( Start.TV_Sec ), 1 );
         Text_IO.Put( ":" );
         Ada.Integer_Text_IO.Put( Integer( Start.TV_Nsec ), 9 );
         Text_IO.Put( " " );
         Ada.Integer_Text_IO.Put( Integer( Stop.TV_Sec ), 1 );
         Text_IO.Put( ":" );
         Ada.Integer_Text_IO.Put( Integer( Stop.TV_Nsec ), 9 );
         Text_IO.Put( " --> " );
         Ada.Integer_Text_IO.Put( Integer( Diff.TV_Sec ), 1 );
         Text_IO.Put( ":" );
         Ada.Integer_Text_IO.Put( Integer( Diff.TV_Nsec ), 9 );
         Text_IO.New_Line;

      end loop;

      delay( 1.0 );

      TEST_SUPPORT.ADA_TEST_END;

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end INIT;

end SPTEST;
