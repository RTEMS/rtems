--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 16 of the RTEMS
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
--  sptest.ads,v 1.3 1995/07/12 19:42:11 joel Exp
--

with CLOCK_DRIVER;
with RTEMS;

package SPTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 5 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 5 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS regions created
--  by this test.
--

   REGION_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   REGION_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;

--
--  The following constant defines the priority of most of the 
--  RTEMS tasks in this test.  This allows one of the tasks to
--  easily set itself to a higher priority than the rest.
--

   BASE_PRIORITY : constant RTEMS.TASK_PRIORITY := 140;

--
--  These arrays define the memory areas used for the regions in
--  this test.
--

   AREA_1 : array ( RTEMS.UNSIGNED32 range 0 .. 4095 ) of RTEMS.UNSIGNED8;
   for AREA_1'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

   AREA_2 : array ( RTEMS.UNSIGNED32 range 0 .. 4095 ) of RTEMS.UNSIGNED8;
   for AREA_2'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

   AREA_3 : array ( RTEMS.UNSIGNED32 range 0 .. 4095 ) of RTEMS.UNSIGNED8;
   for AREA_3'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

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
--  PUT_ADDRESS_FROM_AREA_1
--
--  DESCRIPTION:
--
--  This subprogram prints the offset of the address TO_BE_PRINTED 
--  from the first byte of AREA_1.
--
--  NOTE:
--
--  This subprogram is used because the actual address of AREA_1
--  varies based upon the size of the executable, the target board,
--  and the target processor.
--

   procedure PUT_ADDRESS_FROM_AREA_1 (
      TO_BE_PRINTED : in     RTEMS.ADDRESS
   );

--
--  PUT_ADDRESS_FROM_AREA_2
--
--  DESCRIPTION:
--
--  This subprogram prints the offset of the address TO_BE_PRINTED 
--  from the first byte of AREA_2.
--
--  NOTE:
--
--  This subprogram is used because the actual address of AREA_2
--  varies based upon the size of the executable, the target board,
--  and the target processor.
--

   procedure PUT_ADDRESS_FROM_AREA_2 (
      TO_BE_PRINTED : in     RTEMS.ADDRESS
   );

--
--  PUT_ADDRESS_FROM_AREA_3
--
--  DESCRIPTION:
--
--  This subprogram prints the offset of the address TO_BE_PRINTED 
--  from the first byte of AREA_3.
--
--  NOTE:
--
--  This subprogram is used because the actual address of AREA_3
--  varies based upon the size of the executable, the target board,
--  and the target processor.
--

   procedure PUT_ADDRESS_FROM_AREA_3 (
      TO_BE_PRINTED : in     RTEMS.ADDRESS
   );

--
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Region Manager.
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASK_2
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Region Manager.
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASK_3
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Region Manager.
--

   procedure TASK_3 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASK_4
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Region Manager.
--

   procedure TASK_4 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASK_5
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Region Manager.
--

   procedure TASK_5 (
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
      0,                         -- maximum # timers
      0,                         -- maximum # semaphores
      0,                         -- maximum # message queues
      0,                         -- maximum # messages
      0,                         -- maximum # partitions
      3,                         -- maximum # regions
      0,                         -- maximum # dp memory areas
      0,                         -- maximum # periods
      0,                         -- maximum # user extensions
      RTEMS.MILLISECONDS_TO_MICROSECONDS(10), -- # us in a tick
      100                        -- # ticks in a timeslice
  );

end SPTEST;
