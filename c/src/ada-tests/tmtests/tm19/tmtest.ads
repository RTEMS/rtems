--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 19 of the RTEMS
--  Timing Test Suite.
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

package TMTEST is

--
--  This array contains the IDs of all RTEMS tasks created by this test.
--

   TASK_ID : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;

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
--  PROCESS_ASR_FOR_PASS_1
--
--  DESCRIPTION:
--
--  This subprogram is the ASR used to measure and report the 
--  execution time of a SIGNAL_SEND to SELF.  Before it exits
--  it starts the timer so that the execution time required to
--  exit from an ASR back to the same task can be measured.
--

   procedure PROCESS_ASR_FOR_PASS_1 (
      SIGNALS : in     RTEMS.SIGNAL_SET
   );
   pragma Convention (C, PROCESS_ASR_FOR_PASS_1);

--
--  PROCESS_ASR_FOR_PASS_2
--
--  DESCRIPTION:
--
--  This subprogram is the ASR which sets up for a preemptive
--  return from an ASR.  Before it exits it starts the timer 
--  so that the execution time required to exit from an ASR 
--  and transfer control to the another task can be measured.
--

   procedure PROCESS_ASR_FOR_PASS_2 (
      SIGNALS : in     RTEMS.SIGNAL_SET
   );
   pragma Convention (C, PROCESS_ASR_FOR_PASS_2);

--
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task is responsible for measuring and reporting
--  the following execution times:
--
--    + SIGNAL_CATCH
--    + non-preemptive SIGNAL_SEND
--    + non-preemptive return from an ASR 
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_1);

--
--  PROCESS_ASR_FOR_TASK_2
--
--  DESCRIPTION:
--
--  This subprogram is the ASR for TASK_2.  It performs no actions.
--

   procedure PROCESS_ASR_FOR_TASK_2 (
      SIGNALS : in     RTEMS.SIGNAL_SET
   );
   pragma Convention (C, PROCESS_ASR_FOR_TASK_2);

--
--  TASK_2
--
--  DESCRIPTION:
--
--  This RTEMS task is used as the recipient of a SIGNAL_SEND. 
--  After establishing an ASR, it suspends itself to insure
--  that it does not execute again.
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
--  This RTEMS task is used as the recipient of a preemptive 
--  SIGNAL_SEND.  This task suspends itself and is resumed by 
--  an ASR for TASK_1.  Once the task executes, it stops the 
--  timer and reports the execution time for a preemptive 
--  return from an ASR.
--

   procedure TASK_3 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_3);

end TMTEST;
