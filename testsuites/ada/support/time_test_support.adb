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

with INTERFACES; use INTERFACES;
with TEXT_IO;
with UNSIGNED32_IO;

package body TIME_TEST_SUPPORT is

--PAGE
--
--  PUT_TIME
--

   procedure PUT_TIME (
      MESSAGE          : in     STRING;
      TOTAL_TIME       : in     RTEMS.UNSIGNED32;
      ITERATIONS       : in     RTEMS.UNSIGNED32;
      LOOP_OVERHEAD    : in     RTEMS.UNSIGNED32;
      CALLING_OVERHEAD : in     RTEMS.UNSIGNED32
   ) is
      PER_ITERATION : RTEMS.UNSIGNED32;
   begin

      PER_ITERATION := (TOTAL_TIME - LOOP_OVERHEAD) / ITERATIONS;
      PER_ITERATION := PER_ITERATION - CALLING_OVERHEAD;

      if PER_ITERATION = 0 then

         TEXT_IO.PUT( "TOTAL_TIME " );
         UNSIGNED32_IO.PUT( TOTAL_TIME );
         TEXT_IO.NEW_LINE; 

         TEXT_IO.PUT( "ITERATIONS " );
         UNSIGNED32_IO.PUT( ITERATIONS );
         TEXT_IO.NEW_LINE; 

         TEXT_IO.PUT( "LOOP_OVERHEAD " );
         UNSIGNED32_IO.PUT( LOOP_OVERHEAD );
         TEXT_IO.NEW_LINE; 

         TEXT_IO.PUT( "CALLING_OVERHEAD " );
         UNSIGNED32_IO.PUT( CALLING_OVERHEAD );
         TEXT_IO.NEW_LINE; 

      end if;

      TEXT_IO.PUT( MESSAGE );
      TEXT_IO.PUT( " " );
      UNSIGNED32_IO.PUT( PER_ITERATION );
      TEXT_IO.NEW_LINE; 
      TEXT_IO.FLUSH; 

   end PUT_TIME;

end TIME_TEST_SUPPORT;
