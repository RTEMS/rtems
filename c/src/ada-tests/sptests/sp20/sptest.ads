--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 20 of the RTEMS
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
--  sptest.ads,v 1.3 1995/07/12 19:42:25 joel Exp
--

with CLOCK_DRIVER;
with RTEMS;

package SPTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( 1 .. 5 ) of RTEMS.ID;
   TASK_NAME : array ( 1 .. 5 ) of RTEMS.NAME;

--
--  These arrays contain the parameters which define the execution
--  characteristics and periods of each instantiation of the 
--  copies of the RTEMS task TASKS_1_THROUGH_5.
--

   PERIODS : constant array ( 1 .. 5 ) 
     of RTEMS.UNSIGNED32 := ( 2, 2, 2, 2, 100 );

   ITERATIONS : constant array ( 1 .. 5 ) 
     of RTEMS.UNSIGNED32 := ( 50, 50, 50, 50, 1 );

   PRIORITIES : constant array ( 1 .. 5 ) 
     of RTEMS.UNSIGNED32 := ( 1, 1, 3, 4, 5 );

--
--  The following type defines the array used to manage the 
--  execution counts of each task's period.
--

   type COUNT_ARRAY is array ( 1 .. 5 ) of RTEMS.UNSIGNED32;

--
--  These arrays contains the number of periods successfully completed
--  by each of the tasks.  At each of its periods, the fifth task
--  will copy the contents of the COUNT array to TEMPORARY_COUNT,
--  and clear the COUNT array.  The copy is performed to insure
--  that no periods fire while it is verifying the correctness
--  of the test's execution.
--

   COUNT           : SPTEST.COUNT_ARRAY;
   TEMPORARY_COUNT : SPTEST.COUNT_ARRAY;

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
--  TASK_1_THROUGH_5
--
--  DESCRIPTION:
--
--  These RTEMS tasks test the Rate Monotonic Manager.
--

   procedure TASK_1_THROUGH_5 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  GET_ALL_COUNTERS
--
--  DESCRIPTION:
--
--  This subprogram atomically copies the contents of COUNTER to
--  TEMPORARY_COUNTER.
--

   procedure GET_ALL_COUNTERS;

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
       10,                                            -- priority
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
      0,                         -- maximum # timers
      0,                         -- maximum # semaphores
      0,                         -- maximum # message queues
      0,                         -- maximum # messages
      0,                         -- maximum # partitions
      0,                         -- maximum # regions
      0,                         -- maximum # dp memory areas
      10,                        -- maximum # periods
      0,                         -- maximum # user extensions
      RTEMS.MILLISECONDS_TO_MICROSECONDS(10), -- # us in a tick
      50                         -- # ticks in a timeslice
  );

end SPTEST;
