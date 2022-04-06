-- SPDX-License-Identifier: BSD-2-Clause

--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 22 of the RTEMS
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
--  Buffer Record similar to that used by RTEMS 3.2.1.  Using this
--  avoids changes to the test.
--

   type BUFFER is
      record
         FIELD1 : RTEMS.UNSIGNED32;   -- TEMPORARY UNTIL VARIABLE LENGTH
         FIELD2 : RTEMS.UNSIGNED32;
         FIELD3 : RTEMS.UNSIGNED32;
         FIELD4 : RTEMS.UNSIGNED32;
      end record;

--
--  The following is the ID of the message queue used for timing operations.
--

   MESSAGE_QUEUE_ID : RTEMS.ID;

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
--  HIGH_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is created by LOW_TASK and executes after
--  LOW_TASK has performed a blocking MESSAGE_QUEUE_RECEIVE.
--  This task then executes and performs a readying 
--  MESSAGE_QUEUE_BROADCAST and reports its execution time.
--  Finally, the task suspends itself so LOW_TASK can continue
--  execution.
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, HIGH_TASK);

--
--  LOW_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task performs a blocking MESSAGE_QUEUE_RECEIVE
--  so that when HIGH_TASK performs a MESSAGE_QUEUE_BROADCAST,
--  there will be a task to be readied.  Following this, the 
--  execution time for a MESSAGE_QUEUE_BROADCAST to a message 
--  queue with no waiting tasks is measured and reported.
--  Then this task performs another blocking MESSAGE_QUEUE_RECEIVE
--  so that when PREEMPT_TASK performs a MESSAGE_QUEUE_BROADCAST,
--  PREEMPT_TASK will be preempted by this task.  After control
--  of the processor is transferred back to this task by the
--  preemption, the timer is stopped and the execution time of
--  a preemptive MESSAGE_QUEUE_BROADCAST is reported.
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, LOW_TASK);

--
--  PREEMPT_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task starts the timer and performs a preemptive
--   MESSAGE_QUEUE_BROADCAST.
--

   procedure PREEMPT_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, PREEMPT_TASK);

end TMTEST;
