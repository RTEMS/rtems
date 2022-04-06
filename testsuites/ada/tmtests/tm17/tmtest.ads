-- SPDX-License-Identifier: BSD-2-Clause

--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 17 of the RTEMS
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

with RTEMS;
with RTEMS.TASKS;
with TIME_TEST_SUPPORT;

package TMTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 
      range 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 
      range 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT ) of RTEMS.NAME;

--
--  These variables are used to index the TASK_ID array and change
--  the priority of each successive task such that the TASK_SET_PRIORITY
--  results in a preemption.
--

   TASK_COUNT     : RTEMS.UNSIGNED32;
   TASK_PRIORITY  : RTEMS.TASKS.PRIORITY;

--
--  The following variable is set to the execution time returned
--  by the timer.
--

   END_TIME       : RTEMS.UNSIGNED32;

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
--  FIRST_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is the first task to execute in the system.  It
--  starts the timer and updates the TASK_COUNT and TASK_PRIORITY 
--  variables so that a preemptive TASK_SET_PRIORITY can be performed.
--

   procedure FIRST_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, FIRST_TASK);

--
--  MIDDLE_TASK
--
--  DESCRIPTION:
--
--  These RTEMS tasks update the TASK_COUNT and TASK_PRIORITY variables
--  so that a preemptive TASK_SET_PRIORITY can be performed.
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, MIDDLE_TASKS);

--
--  LAST_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is the last task to execute in the system.  It
--  stops the timer and reports the execution time of a preemptive
--  TASK_SET_PRIORITY.
--

   procedure LAST_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, LAST_TASK);

end TMTEST;
