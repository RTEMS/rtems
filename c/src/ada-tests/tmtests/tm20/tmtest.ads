--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 20 of the RTEMS
--  Timing Test Suite.
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
--  tmtest.ads,v 1.3 1995/07/12 19:44:05 joel Exp
--

with STUB_DRIVER;
with TIME_TEST_SUPPORT;
with TIMER_DRIVER;
with RTEMS;

package TMTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;

--
--  These following are the ID and NAME of the RTEMS partition created
--  by this test.
--

   PARTITION_ID   : RTEMS.ID;
   PARTITION_NAME : RTEMS.NAME;

--
--  These following are the ID and NAME of the RTEMS region created
--  by this test.
--

   REGION_ID   : RTEMS.ID;
   REGION_NAME : RTEMS.NAME;

--
--  The following area defines a memory area to be used as the
--  memory space for a partition.
-- 

   PARTITION_AREA : array ( RTEMS.UNSIGNED32 range 0 .. 2047 )
      of RTEMS.UNSIGNED8;
   for PARTITION_AREA'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

--
--  The following area defines a memory area to be used as the
--  memory space for a region.
-- 

   REGION_AREA : array ( RTEMS.UNSIGNED32 range 0 .. 2047 )
      of RTEMS.UNSIGNED8;

   for REGION_AREA'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

--
--  The following are the buffers allocated from the region.
--

   BUFFER_ADDRESS_1 : RTEMS.ADDRESS;
   BUFFER_ADDRESS_2 : RTEMS.ADDRESS;
   BUFFER_ADDRESS_3 : RTEMS.ADDRESS;
   BUFFER_ADDRESS_4 : RTEMS.ADDRESS;

   BUFFER_ADDRESSES : array ( RTEMS.UNSIGNED32 
      range 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT ) of RTEMS.ADDRESS;

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
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task is responsible for measuring and reporting
--  the following execution times:
--
--    + PARTITION_CREATE
--    + REGION_CREATE
--    + PARTITION_GET_BUFFER with a buffer available
--    + PARTITION_GET_BUFFER with no buffers available
--    + PARTITION_RETURN_BUFFER
--    + PARTITION_DELETE
--    + REGION_GET_SEGMENT with a segment available
--    + REGION_GET_SEGMENT with the NO_WAIT option
--    + REGION_RETURN_SEGMENT with no tasks waiting for memory
--    + REGION_RETURN_SEGMENT preemptive
--    + REGION_DELETE
--    + IO_INITIALIZE
--    + IO_OPEN
--    + IO_CLOSE
--    + IO_READ
--    + IO_WRITE
--    + IO_CONTROL
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASK_2
--
--  DESCRIPTION:
--
--  This RTEMS task is responsible for measuring and reporting
--  the following execution times:
--
--    + blocking REGION_GET_SEGMENT 
--    + REGION_RETURN_SEGMENT which readies a task
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  This is the Driver Address Table for this test.
--

   DEVICE_DRIVERS : aliased RTEMS.DRIVER_ADDRESS_TABLE( 1 .. 1 ) :=
   ( 1 =>
      (
        STUB_DRIVER.INITIALIZE'ACCESS,               -- Initialization
        STUB_DRIVER.OPEN'ACCESS,                     -- Open
        STUB_DRIVER.CLOSE'ACCESS,                    -- Close
        STUB_DRIVER.READ'ACCESS,                     -- Read
        STUB_DRIVER.WRITE'ACCESS,                    -- Write
        STUB_DRIVER.CONTROL'ACCESS                   -- Control
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
       TMTEST.INIT'ACCESS,                            -- entry point
       RTEMS.NO_PREEMPT,                              -- initial mode
       0                                              -- argument list
     )
   );

--
--  This is the Configuration Table for this test.
--

   CONFIGURATION : aliased RTEMS.CONFIGURATION_TABLE := (
      RTEMS.NULL_ADDRESS,        -- will be replaced by BSP
      256 * 1024,                -- executive RAM size
      10,                        -- maximum # tasks
      0,                         -- maximum # timers
      0,                         -- maximum # semaphores
      0,                         -- maximum # message queues
      0,                         -- maximum # messages
      1,                         -- maximum # partitions
      1,                         -- maximum # regions
      0,                         -- maximum # dp memory areas
      0,                         -- maximum # periods
      0,                         -- maximum # user extensions
      RTEMS.MILLISECONDS_TO_MICROSECONDS(10), -- # us in a tick
      0                          -- # ticks in a timeslice
  );

end TMTEST;
