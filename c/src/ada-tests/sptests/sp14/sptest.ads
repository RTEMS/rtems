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
--  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
--  On-Line Applications Research Corporation (OAR).
--  All rights assigned to U.S. Government, 1994.
--
--  This material may be reproduced by or for the U.S. Government pursuant
--  to the copyright license under the clause at DFARS 252.227-7013.  This
--  notice must appear in all copies of this file and its derivatives.
--
--  sptest.ads,v 1.3 1995/07/12 19:42:02 joel Exp
--

with CLOCK_DRIVER;
with RTEMS;

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

   SIGNAL_SENT : RTEMS.BOOLEAN;
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

   ASR_FIRED   : RTEMS.BOOLEAN;
   pragma volatile( ASR_FIRED );

--
--  INIT
--
--  DESCRIPTION:
--
--  This RTEMS task initializes the application.
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

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

--
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Signal Manager.
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASK_2
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Signal Manager.
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  This is the Driver Address Table for this test.
--

   DEVICE_DRIVERS : aliased RTEMS.DRIVER_ADDRESS_TABLE( 1 .. 1 ) :=
   (1=>
      (
        CLOCK_DRIVER.INITIALIZE'ACCESS,              -- Initialization
        RTEMS.NO_DRIVER_ENTRY,                       -- Open
        RTEMS.NO_DRIVER_ENTRY,                       -- Close
        RTEMS.NO_DRIVER_ENTRY,                       -- Read
        RTEMS.NO_DRIVER_ENTRY,                       -- Write
        RTEMS.NO_DRIVER_ENTRY                        -- Control
      )
   );

--
--  This is the Initialization Tasks Table for this test.
--

   INITIALIZATION_TASKS : aliased RTEMS.INITIALIZATION_TASKS_TABLE( 1 .. 1 ) := 
   (1=>
     (
       RTEMS.BUILD_NAME( 'U', 'I', '1', ' ' ),        -- task name
       2048,                                          -- stack size
       1,                                             -- priority
       RTEMS.DEFAULT_ATTRIBUTES,                      -- attributes
       SPTEST.INIT'ACCESS,                            -- entry point
       RTEMS.NO_PREEMPT,                              -- initial mode
       0                                              -- argument list
     )
   );

--
--  This is the Configuration Table for this test.
--

   CONFIGURATION : aliased RTEMS.CONFIGURATION_TABLE := (
      RTEMS.NULL_ADDRESS,        -- will be replaced by BSP
      64 * 1024,                 -- executive RAM size
      10,                        -- maximum # tasks
      1,                         -- maximum # timers
      0,                         -- maximum # semaphores
      0,                         -- maximum # message queues
      0,                         -- maximum # messages
      0,                         -- maximum # partitions
      0,                         -- maximum # regions
      0,                         -- maximum # dp memory areas
      0,                         -- maximum # periods
      0,                         -- maximum # user extensions
      RTEMS.MILLISECONDS_TO_MICROSECONDS(10), -- # us in a tick
      100                        -- # ticks in a timeslice
  );

end SPTEST;
