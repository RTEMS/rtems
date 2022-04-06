-- SPDX-License-Identifier: BSD-2-Clause

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
