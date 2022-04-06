-- SPDX-License-Identifier: BSD-2-Clause

--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 7 of the RTEMS
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

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS extensions created
--  by this test.
--
 
   EXTENSION_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   EXTENSION_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;
 
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
--  This RTEMS task tests some of the capabilities of the Task Manager.
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
--  This RTEMS task tests some of the capabilities of the Task Manager.
--

   procedure TASK_3 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_3);

--
--  TASK_4
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Message Queue Manager.
--

   procedure TASK_4 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_4);

--
--  TASK_CREATE_EXTENSION
--
--  DESCRIPTION:
--
--  This subprogram is the TASK_CREATE extension for this test.
--

   function TASK_CREATE_EXTENSION (
      UNUSED       : in     RTEMS.TCB_POINTER;
      CREATED_TASK : in     RTEMS.TCB_POINTER
   ) return RTEMS.Boolean;
   pragma Convention (C, TASK_CREATE_EXTENSION);

--
--  TASK_DELETE_EXTENSION
--
--  DESCRIPTION:
--
--  This subprogram is the TASK_DELETE extension for this test.
--

   procedure TASK_DELETE_EXTENSION (
      RUNNING_TASK : in     RTEMS.TCB_POINTER;
      DELETED_TASK : in     RTEMS.TCB_POINTER
   );
   pragma Convention (C, TASK_DELETE_EXTENSION);

--
--  TASK_RESTART_EXTENSION
--
--  DESCRIPTION:
--
--  This subprogram is the TASK_RESTART extension for this test.
--

   procedure TASK_RESTART_EXTENSION (
      UNUSED         : in     RTEMS.TCB_POINTER;
      RESTARTED_TASK : in     RTEMS.TCB_POINTER
   );
   pragma Convention (C, TASK_RESTART_EXTENSION);

--
--  TASK_START_EXTENSION
--
--  DESCRIPTION:
--
--  This subprogram is the TASK_START extension for this test.
--

   procedure TASK_START_EXTENSION (
      UNUSED       : in     RTEMS.TCB_POINTER;
      STARTED_TASK : in     RTEMS.TCB_POINTER
   );
   pragma Convention (C, TASK_START_EXTENSION);

--
--  TASK_EXIT_EXTENSION
--
--  DESCRIPTION:
--
--  This subprogram is the TASK_EXIT extension for this test.
--  It is invoked when a RTEMS task exits from its entry point.
--

   procedure TASK_EXIT_EXTENSION (
      RUNNING_TASK : in     RTEMS.TCB_POINTER
   );
   pragma Convention (C, TASK_EXIT_EXTENSION);

--
--  This is the User Extension Table for this test.
--

   EXTENSIONS : aliased RTEMS.EXTENSIONS_TABLE :=
   (
        SPTEST.TASK_CREATE_EXTENSION'ACCESS,         -- Task create
        SPTEST.TASK_START_EXTENSION'ACCESS,          -- Task start
        SPTEST.TASK_RESTART_EXTENSION'ACCESS,        -- Task restart
        SPTEST.TASK_DELETE_EXTENSION'ACCESS,         -- Task delete
        NULL,                                        -- Task switch
        NULL,                                        -- Task begin
        SPTEST.TASK_EXIT_EXTENSION'ACCESS,           -- Task exitted
        NULL,                                        -- Fatal error
        NULL                                         -- Task terminate
   );

end SPTEST;
