--
--  MPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 11 of the RTEMS
--  Multiprocessor Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR).
--  Copyright assigned to U.S. Government, 1994.
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.OARcorp.com/rtems/license.html.
--
--  $Id$
--

with RTEMS;

package MPTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS message 
--  queues created by this test.
--

   QUEUE_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;
   QUEUE_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS semaphore
--  created by this test.
--

   SEMAPHORE_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;
   SEMAPHORE_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS partition
--  created by this test.
--

   PARTITION_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;
   PARTITION_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.NAME;

--
--  This is the area used for the partition.
--

   PARTITION_AREA : 
      array ( RTEMS.UNSIGNED32 range 0 .. 1023 ) of RTEMS.UNSIGNED8;
   for PARTITION_AREA'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

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
       MPTEST.INIT'ACCESS,                            -- entry point
       RTEMS.NO_PREEMPT,                              -- initial mode
       0                                              -- argument list
     )
   );

----------------------------------------------------------------------------
----------------------------------------------------------------------------
--                             BEGIN SUBPACKAGE                           --
----------------------------------------------------------------------------
----------------------------------------------------------------------------

   --
   --  MPTEST.PER_NODE_CONFIGURATION / SPECIFICATION
   --
   --  DESCRIPTION:
   --
   --  This package is the specification for the subpackage
   --  which will define the per node configuration parameters.
   --
   
   package PER_NODE_CONFIGURATION is

   --
   --  LOCAL_NODE_NUMBER
   --
   --  DESCRIPTION:
   --
   --  This function returns the node number for this node.
   --

      function LOCAL_NODE_NUMBER 
      return RTEMS.UNSIGNED32;
  
      pragma INLINE ( LOCAL_NODE_NUMBER );

   end PER_NODE_CONFIGURATION;
  
----------------------------------------------------------------------------
----------------------------------------------------------------------------
--                              END SUBPACKAGE                            --
----------------------------------------------------------------------------
----------------------------------------------------------------------------

--
--  This is the Multiprocessor Configuration Table for this test.
--

   MULTIPROCESSING_CONFIGURATION : aliased RTEMS.MULTIPROCESSING_TABLE := (
      MPTEST.PER_NODE_CONFIGURATION.LOCAL_NODE_NUMBER,
      2,                         -- maximum # nodes in system 
      0,                         -- maximum # global objects
      0                          -- maximum # proxies
    );

--
--  This is the Configuration Table for this test.
--

   CONFIGURATION : aliased RTEMS.CONFIGURATION_TABLE := (
      RTEMS.NULL_ADDRESS,        -- will be replaced by BSP
      64 * 1024,                 -- executive RAM size
      10,                        -- maximum # tasks
      0,                         -- maximum # timers
      1,                         -- maximum # semaphores
      1,                         -- maximum # message queues
      0,                         -- maximum # messages
      1,                         -- maximum # partitions
      0,                         -- maximum # regions
      0,                         -- maximum # dp memory areas
      0,                         -- maximum # periods
      0,                         -- maximum # user extensions
      RTEMS.MILLISECONDS_TO_MICROSECONDS(10), -- # us in a tick
      50                         -- # ticks in a timeslice
  );

end MPTEST;
