--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 17 of the RTEMS
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
--  The following is set to TRUE by TASK_2 when it preempts TASK_1.
--

   TASK_2_PREEMPTED : STANDARD.BOOLEAN;

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
--  PROCESS_ASR
--
--  DESCRIPTION:
--
--  This subprogram is the ASR for TASK_1.
--

   procedure PROCESS_ASR (
      SIGNALS : in     RTEMS.SIGNAL_SET
   );
   pragma Convention (C, PROCESS_ASR);

--
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task tests that when the transfer from the task
--  to the ASR (and vice-versa) is performed, that the new mode 
--  is taken into account.  If this occurs, then TASK_2 will be 
--  able to preempt upon completion of the ASR of TASK_1.
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
--  This RTEMS task is designed to preempt TASK_1.
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_2);

end SPTEST;
