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
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with TIME_TEST_SUPPORT;
with RTEMS;
with RTEMS.TASKS;

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
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, INIT);

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
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_1);

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
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_2);

end TMTEST;
