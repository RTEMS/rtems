--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 11 of the RTEMS
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
--  These arrays contain the IDs and NAMEs of all RTEMS timers created
--  by this test.
--

   TIMER_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 6 ) of RTEMS.ID;
   TIMER_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 6 ) of RTEMS.NAME;

--
--  TA1_SEND_18_TO_SELF_5_SECONDS
--
--  DESCRIPTION:
--
--  This subprogram is scheduled to be fired by a timer after 5 seconds
--  and to send EVENT_18 to TASK_1.
--

   procedure TA1_SEND_18_TO_SELF_5_SECONDS (
      IGNORED_ID      : in     RTEMS.ID;  
      IGNORED_ADDRESS : in     RTEMS.ADDRESS 
   ); 
   pragma Convention (C, TA1_SEND_18_TO_SELF_5_SECONDS);

--
--  TA1_SEND_8_TO_SELF_60_SECONDS
--
--  DESCRIPTION:
--
--  This subprogram is scheduled to be fired by a timer after 60 seconds
--  and to send EVENT_8 to TASK_1.
--

   procedure TA1_SEND_8_TO_SELF_60_SECONDS (
      IGNORED_ID      : in     RTEMS.ID;  
      IGNORED_ADDRESS : in     RTEMS.ADDRESS 
   ); 
   pragma Convention (C, TA1_SEND_8_TO_SELF_60_SECONDS);

--
--  TA1_SEND_9_TO_SELF_5_SECONDS
--
--  DESCRIPTION:
--
--  This subprogram is scheduled to be fired by a timer after 60 seconds
--  and to send EVENT_9 to TASK_1.
--

   procedure TA1_SEND_9_TO_SELF_60_SECONDS (
      IGNORED_ID      : in     RTEMS.ID;  
      IGNORED_ADDRESS : in     RTEMS.ADDRESS 
   ); 
   pragma Convention (C, TA1_SEND_9_TO_SELF_60_SECONDS);

--
--  TA1_SEND_10_TO_SELF
--
--  DESCRIPTION:
--
--  This subprogram is scheduled to be fired by a timer after 60 seconds
--  and to send EVENT_10 to TASK_1.
--

   procedure TA1_SEND_10_TO_SELF (
      IGNORED_ID      : in     RTEMS.ID;  
      IGNORED_ADDRESS : in     RTEMS.ADDRESS 
   ); 
   pragma Convention (C, TA1_SEND_10_TO_SELF);

--
--  TA1_SEND_1_TO_SELF_EVERY_SECOND
--
--  DESCRIPTION:
--
--  This subprogram is scheduled to be fired by a timer every second
--  and to send EVENT_1 to TASK_1 until the timer is cancelled.
--

   procedure TA1_SEND_1_TO_SELF_EVERY_SECOND (
      IGNORED_ID      : in     RTEMS.ID;  
      IGNORED_ADDRESS : in     RTEMS.ADDRESS 
   ); 
   pragma Convention (C, TA1_SEND_1_TO_SELF_EVERY_SECOND);

--
--  TA1_SEND_11_TO_SELF
--
--  DESCRIPTION:
--
--  This subprogram is scheduled to be fired by a timer at a
--  specific time of day and to send EVENT_11 to TASK_1.
--

   procedure TA1_SEND_11_TO_SELF (
      IGNORED_ID      : in     RTEMS.ID;  
      IGNORED_ADDRESS : in     RTEMS.ADDRESS 
   ); 
   pragma Convention (C, TA1_SEND_11_TO_SELF);

--
--  TA2_SEND_10_TO_SELF
--
--  DESCRIPTION:
--
--  This subprogram is scheduled to be fired by a timer after 5 seconds
--  and to send EVENT_10 to TASK_2.
--

   procedure TA2_SEND_10_TO_SELF (
      IGNORED_ID      : in     RTEMS.ID;  
      IGNORED_ADDRESS : in     RTEMS.ADDRESS 
   ); 
   pragma Convention (C, TA2_SEND_10_TO_SELF);

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
--  This RTEMS task tests the Event and Timer Managers.
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
--  This RTEMS task tests the Event and Timer Managers.
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_2);

end SPTEST;
