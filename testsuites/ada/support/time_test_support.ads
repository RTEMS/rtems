-- SPDX-License-Identifier: BSD-2-Clause

--
--  TIME_TEST_SUPPORT / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package provides routines which aid the individual tests in
--  the Timing Test Suite and simplify their design and operation.
--
--  DEPENDENCIES: 
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

package TIME_TEST_SUPPORT is

--
--  The following constants define the number of times a directive
--  will be performed in the Timing Suite.
--

   OPERATION_COUNT : constant RTEMS.UNSIGNED32 := 100;
   ITERATION_COUNT : constant RTEMS.UNSIGNED32 := 100;

--
--  PUT_TIME
--
--  DESCRIPTION:
--
--  This subprogram prints the MESSAGE followed by the length of
--  time which each individual operation took.  All times are
--  in microseconds.
--

   procedure PUT_TIME (
      MESSAGE          : in     STRING;
      TOTAL_TIME       : in     RTEMS.UNSIGNED32;
      ITERATIONS       : in     RTEMS.UNSIGNED32;
      LOOP_OVERHEAD    : in     RTEMS.UNSIGNED32;
      CALLING_OVERHEAD : in     RTEMS.UNSIGNED32
   );

end TIME_TEST_SUPPORT;
