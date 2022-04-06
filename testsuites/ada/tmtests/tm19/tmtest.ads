-- SPDX-License-Identifier: BSD-2-Clause

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
