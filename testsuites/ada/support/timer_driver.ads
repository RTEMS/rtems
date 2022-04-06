-- SPDX-License-Identifier: BSD-2-Clause

--
--  Timer_Driver / Specification
--
--  Description:
--
--  This package is the specification for the Timer Driver.
--
--  Dependencies: 
--
--  
--
--  COPYRIGHT (c) 1989-1997.
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

package Timer_Driver is

--
--  Initialize
--
--  Description:
--
--  This subprogram initializes the hardware timer to start it.
--

   procedure Initialize;
   pragma Import (C, Initialize, "benchmark_timer_initialize");

--
--  Read_Timer
--
--  Description:
--
--  This subprogram stops the timer, calculates the length of time
--  in microseconds since the timer was started, and returns that
--  value.
--

   function Read_Timer 
   return RTEMS.Unsigned32;
   pragma Import (C, Read_Timer, "benchmark_timer_read");

--
--  Empty_Function
--
--  Description:
--
--  This subprogram is an empty subprogram.  It is used to
--  insure that a loop will be included in the final executable
--  so that loop overhead can be subtracted from the directive
--  times reported.
--

   procedure Empty_Function;

--
--  Set_Find_Average_Overhead
--
--  Description:
--
--  This subprogram sets the Find_Average_Overhead flag to the
--  the value passed.
--
 
   procedure Set_Find_Average_Overhead (
      Find_Flag : in     Standard.Boolean
   );

private

end Timer_Driver;
