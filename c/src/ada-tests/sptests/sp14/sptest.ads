--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 14 of the RTEMS
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

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS timers created
--  by this test.
--

   TIMER_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;
   TIMER_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.NAME;

--
--  SIGNAL_SENT is set to TRUE to indicate that a signal set has 
--  been sent from an ISR to the executing task.
--

   SIGNAL_SENT : BOOLEAN;
   pragma volatile( SIGNAL_SENT );

--
--  These hold the arguments passed to the timer service routine.
--

   TIMER_GOT_THIS_ID      : RTEMS.ID;
   pragma volatile( TIMER_GOT_THIS_ID );

   TIMER_GOT_THIS_POINTER : RTEMS.ADDRESS;
   pragma volatile( TIMER_GOT_THIS_POINTER );

--
--  ASR_FIRED is set to TRUE to indicate that the ASR has executed
--  and was passed the correct signal set.
--

   ASR_FIRED : BOOLEAN;
   pragma volatile( ASR_FIRED );

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
--  SIGNAL_3_TO_TASK_1
--
--  DESCRIPTION:
--
--  This subprogram sends signal 3 to TASK_1.
--

   procedure SIGNAL_3_TO_TASK_1 (
      ID      : in     RTEMS.ID;
      POINTER : in     RTEMS.ADDRESS
   );
   pragma Convention (C, SIGNAL_3_TO_TASK_1);

--
--  PROCESS_ASR
--
--  DESCRIPTION:
--
--  This subprogram is an ASR for TASK_1.
--

   procedure PROCESS_ASR (
      THE_SIGNAL_SET : in     RTEMS.SIGNAL_SET
   );
   pragma Convention (C, PROCESS_ASR);

--
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Signal Manager.
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
--  This RTEMS task tests the Signal Manager.
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_2);

end SPTEST;
