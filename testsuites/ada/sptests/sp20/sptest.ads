-- SPDX-License-Identifier: BSD-2-Clause

--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 20 of the RTEMS
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

with RTEMS;
with RTEMS.TASKS;

package SPTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( 1 .. 6 ) of RTEMS.ID;
   TASK_NAME : array ( 1 .. 6 ) of RTEMS.NAME;

--
--  These arrays contain the parameters which define the execution
--  characteristics and periods of each instantiation of the 
--  copies of the RTEMS task TASKS_1_THROUGH_6.
--

   PERIODS : constant array ( 1 .. 6 ) 
     of RTEMS.UNSIGNED32 := ( 2, 2, 2, 2, 100, 0 );

   ITERATIONS : constant array ( 1 .. 6 ) 
     of RTEMS.UNSIGNED32 := ( 50, 50, 50, 50, 1, 10 );

   PRIORITIES : constant array ( 1 .. 6 ) 
     of RTEMS.UNSIGNED32 := ( 1, 1, 3, 4, 5, 1 );

--
--  The following type defines the array used to manage the 
--  execution counts of each task's period.
--

   type COUNT_ARRAY is array ( 1 .. 6 ) of RTEMS.UNSIGNED32;

--
--  These arrays contains the number of periods successfully completed
--  by each of the tasks.  At each of its periods, the fifth task
--  will copy the contents of the COUNT array to TEMPORARY_COUNT,
--  and clear the COUNT array.  The copy is performed to insure
--  that no periods fire while it is verifying the correctness
--  of the test's execution.
--

   COUNT           : SPTEST.COUNT_ARRAY;
   TEMPORARY_COUNT : SPTEST.COUNT_ARRAY;

--
--  INIT
--
--  DESCRIPTION:
--
--  This RTEMS task initializes the application.
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, INIT);

--
--  TASK_1_THROUGH_6
--
--  DESCRIPTION:
--
--  These RTEMS tasks test the Rate Monotonic Manager.
--

   procedure TASK_1_THROUGH_6 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_1_THROUGH_6);

--
--  GET_ALL_COUNTERS
--
--  DESCRIPTION:
--
--  This subprogram atomically copies the contents of COUNTER to
--  TEMPORARY_COUNTER.
--

   procedure GET_ALL_COUNTERS;

end SPTEST;
