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
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
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
