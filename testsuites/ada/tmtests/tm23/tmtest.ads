-- SPDX-License-Identifier: BSD-2-Clause

--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 23 of the RTEMS
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

with TIME_TEST_SUPPORT;
with RTEMS;
with RTEMS.TASKS;

package TMTEST is

--
--  This array contains the IDs of all RTEMS timers created by this test.
--

   TIMER_ID   : array ( RTEMS.UNSIGNED32 
      range 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT ) of RTEMS.ID;

--
--  This data item contains a time of day used by all of the
--  "WHEN" directives timed in this test.
--

   TIME_OF_DAY      : RTEMS.TIME_OF_DAY;

--
--  The following variable is set to the execution time returned
--  by the timer.
--

   END_TIME  : RTEMS.UNSIGNED32;

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
--  NULL_DELAY
--
--  DESCRIPTION:
--
--  This subprogram is a timer service routine.  It performs
--  no actions.
--

   procedure NULL_DELAY (
      IGNORED_ID      : in     RTEMS.ID;  
      IGNORED_ADDRESS : in     RTEMS.ADDRESS 
   );
   pragma Convention (C, NULL_DELAY);

--
--  HIGH_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is responsible for measuring and reporting
--  the following directive execution times:
--
--    + TIMER_CREATE
--    + TIMER_FIRE_AFTER with an inactive timer
--    + TIMER_FIRE_AFTER with an active timer
--    + TIMER_CANCEL of an active timer
--    + TIMER_CANCEL of an inactive timer
--    + TIMER_RESET of an active timer
--    + TIMER_RESET of an inactive timer
--    + TIMER_FIRE_WHEN of an inactive timer
--    + TIMER_FIRE_WHEN of an active timer
--    + TIMER_DELETE of an active timer
--    + TIMER_DELETE of an inactive timer
--
--  After measuring and reporting the above times, this task starts
--  the timer and executes the TASK_WAKE_WHEN directive.
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, HIGH_TASK);

--
--  MIDDLE_TASKS
--
--  DESCRIPTION:
--
--  These RTEMS tasks executes the TASK_WAKE_WHEN directive.
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, MIDDLE_TASKS);

--
--  LOW_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task stops the timer and reports the execution time for
--  the TASK_WAKE_WHEN directive.
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, LOW_TASK);

end TMTEST;
