-- SPDX-License-Identifier: BSD-2-Clause

--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 4 of the RTEMS
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

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS extensions created
--  by this test.
--
 
   EXTENSION_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   EXTENSION_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;
 
--
--  This array contains the number of times each of the test tasks has
--  been context switched TO.
--

   RUN_COUNT : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.UNSIGNED32;
   pragma Volatile (RUN_COUNT);

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
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task tests some of the capabilities of the Task Manager.
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_1);

--
--  TASK_2
--
--  DESCRIPTION:
--
--  This RTEMS task is used as a background task to suspend, resume,
--  and context switch to.
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_2);

--
--  TASK_3
--
--  DESCRIPTION:
--
--  This RTEMS task is used as a background task to suspend, resume,
--  and context switch to.
--

   procedure TASK_3 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_3);

--
--  TASK_SWITCH
--
--  DESCRIPTION:
--
--  This subprogram is the TASK_SWITCH extension for this test.
--  It is invoked at each context switch and counts the number
--  of times each application task has been context switched to.
--

   procedure TASK_SWITCH (
      UNUSED : in     RTEMS.TCB_POINTER;
      HEIR   : in     RTEMS.TCB_POINTER
   );
   pragma Convention (C, TASK_SWITCH);

--
--  This is the User Extension Table for this test.
--

   EXTENSIONS : aliased RTEMS.EXTENSIONS_TABLE := 
   (
        NULL,                                        -- Task create
        NULL,                                        -- Task start
        NULL,                                        -- Task restart
        NULL,                                        -- Task delete
        SPTEST.TASK_SWITCH'ACCESS,                   -- Task switch
        NULL,                                        -- Task post switch
        NULL,                                        -- Task begin
        NULL,                                        -- Task exitted
        NULL                                         -- Fatal error
   );

end SPTEST;
