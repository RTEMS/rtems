-- SPDX-License-Identifier: BSD-2-Clause

--
--  Test_Support / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package provides routines which aid the Test Suites
--  and simplify their design and operation.
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

package Test_Support is

   procedure Ada_Test_Begin;
   pragma Import (C, Ada_Test_Begin, "ada_test_begin");

   procedure Ada_Test_End;
   pragma Import (C, Ada_Test_End, "ada_test_end");

   procedure Check_Type(
      t : in Long_Integer;
      s : in Long_Integer;
      a : in Long_Integer
   );
   pragma Import (C, Check_Type, "check_type");

--
--  Fatal_Directive_Status
--
--  DESCRIPTION:
--
--  This subprogram checks if Status is equal to Desired.  If so, it 
--  returns immediately.  Otherwise, it prints the Message along with
--  the Status and Desired status and invokes the Fatal_Error_Occurred
--  directive.
--

   procedure Fatal_Directive_Status (
      Status  : in     RTEMS.Status_Codes;
      Desired : in     RTEMS.Status_Codes;
      Message : in     STRING
   );
   pragma Inline ( Fatal_Directive_Status );

--  Directive_Failed
--
--  DESCRIPTION:
--
--  This subprogram checks if Status is equal to Successful.  If so, it 
--  returns immediately.  Otherwise, it prints the Message along with
--  the Status and Desired status and invokes the Fatal_Error_Occurred
--

   procedure Directive_Failed (
      Status  : in     RTEMS.Status_Codes;
      Message : in     STRING
   );
   pragma Inline ( Directive_Failed );

--
--  Print_Time
--
--  DESCRIPTION:
--
--  This subprogram prints the Prefix string, following by the
--  time of day in Time_Buffer, followed by the Suffix.
--

   procedure Print_Time (
      Prefix      : in     STRING;
      Time_Buffer : in     RTEMS.Time_Of_Day;
      Suffix      : in     STRING
   );
   pragma Inline ( Print_Time );

--
--  Put_Dot
--
--  DESCRIPTION:
--
--  This subprogram prints a single character without a carriage return.
--

   procedure Put_Dot ( 
      Buffer : in     STRING
   );
   pragma Inline ( Put_Dot );

--
--  Pause
--
--  DESCRIPTION:
--
--  This subprogram is used to pause screen output in the Test Suites
--  until the user presses carriage return.
--

   procedure Pause;

--
--  Pause_And_Screen_Number
--
--  DESCRIPTION:
--
--  This subprogram is used to pause screen output  and print the current
--  number in the Test Suites until the user presses carriage return.
--

   procedure Pause_And_Screen_Number (
      SCREEN : in     RTEMS.Unsigned32
   );

--
--  Put_Name
--
--  DESCRIPTION:
--
--  This subprogram prints the RTEMS object Name.  If New_Line is TRUE, 
--  then a carriage return is printed after the Name.
--

   procedure Put_Name (
      Name     : in     RTEMS.Name;
      New_Line : in     Boolean
   );
 
--
--  Task_Number
--
--  DESCRIPTION:
--
--  This function returns the task index which the test should use
--  for TID.
--

   function Task_Number (
      TID : in     RTEMS.ID
   ) return RTEMS.Unsigned32;
   pragma Inline ( Task_Number );

--
--  Do_Nothing
--
--  DESCRIPTION:
--
--  This procedure is called when a test wishes to use a delay
--  loop and insure that the compiler does not optimize it away.
--

   procedure Do_Nothing;

--
--  Ticks_Per_Second is the number of RTEMS clock ticks which
--  occur each second.
--

   function Ticks_Per_Second 
   return RTEMS.Interval; 

--
--  Milliseconds_Per_Tick is the number of milliseconds which
--  occur between each RTEMS clock tick.
--

   function Milliseconds_Per_Tick 
   return RTEMS.Unsigned32;

--
--  Return the size of the RTEMS Workspace
--

   function Work_Space_Size
   return RTEMS.Size;

--
--  Return an indication of whether multiprocessing is configured
--

   function Is_Configured_Multiprocessing 
   return Boolean;

--
--  Node is the node number in a multiprocessor configuration
--

   function Node 
   return RTEMS.Unsigned32;

--
--  Longest time in seconds to run a test
--

    MAXIMUM_LONG_TEST_DURATION : RTEMS.UNSIGNED32;

private

end Test_Support;
