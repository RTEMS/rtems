--
--  MPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 14 of the RTEMS
--  Multiprocessor Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--
--  $Id$
--

with RTEMS;

package MPTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test for passing event sets.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.NAME;
--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test for passing event sets.
--

   EVENT_TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.ID;
   EVENT_TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test for manipulating semaphores.
--

   SEMAPHORE_TASK_ID   : 
      array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.ID;
   SEMAPHORE_TASK_NAME : 
      array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test for passing messages.
--

   QUEUE_TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.ID;
   QUEUE_TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test for manipulating the global partitions.
--

   PARTITION_TASK_ID   : 
      array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.ID;
   PARTITION_TASK_NAME : 
      array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS partitions
--  created by this test.
--

   PARTITION_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.ID;
   PARTITION_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS semaphores
--  created by this test.
--

   SEMAPHORE_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.ID;
   SEMAPHORE_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS message_queues
--  created by this test.
--

   QUEUE_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.ID;
   QUEUE_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.NAME;

--
--
--  These arrays contain the IDs and NAMEs of all RTEMS timers
--  created by this test.
--

   TIMER_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.ID;
   TIMER_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.NAME;

--
--  The following are message buffers used to contain the test messages
--  and pointers to those buffers.
--

   BUFFER_AREAS : array (  RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.BUFFER;
   BUFFERS      : 
      array (  RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.BUFFER_POINTER;

--
--  This is the area used for the partition.
--

   PARTITION_AREA : 
      array ( RTEMS.UNSIGNED32 range 0 .. 16#7FFF# ) of RTEMS.UNSIGNED8;
   for PARTITION_AREA'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

--
--  The following constants control the flow of "dot" indicators
--  from the various test componenents.
--

   EVENT_TASK_DOT_COUNT    : constant RTEMS.UNSIGNED32 := 100;
   EVENT_SEND_DOT_COUNT    : constant RTEMS.UNSIGNED32 := 100;
   DELAYED_EVENT_DOT_COUNT : constant RTEMS.UNSIGNED32 := 1000;
   MESSAGE_DOT_COUNT       : constant RTEMS.UNSIGNED32 := 200;
   PARTITION_DOT_COUNT     : constant RTEMS.UNSIGNED32 := 200;
   SEMAPHORE_DOT_COUNT     : constant RTEMS.UNSIGNED32 := 200;

--
--  These contain the IDs and NAMEs of the RTEMS timers used
--  by this test to stop.
--
 
   STOP_TIMER_ID   : RTEMS.ID;
   STOP_TIMER_NAME : RTEMS.NAME;
 
--
--  This variable is set when the test should stop executing.
--

   STOP_TEST : RTEMS.BOOLEAN;
 
--
--  EXIT_TEST
--
--  DESCRIPTION:
--
--  This subprogram is invoked to stop this test.
--
 
   procedure EXIT_TEST;

--
--  DELAYED_SEND_EVENT
--
--  DESCRIPTION:
--
--  This subprogram is a timer service routine which sends an
--  event set to a waiting task.
--
 
   procedure DELAYED_SEND_EVENT (
      TIMER_ID        : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS 
   );

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
--  TEST_TASK
--
--  DESCRIPTION:
--
--  This is one of the test tasks.
--

   procedure TEST_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  DELAYED_EVENTS_TASK
--
--  DESCRIPTION:
--
--  This is one of the test tasks.
--
 
   procedure DELAYED_EVENTS_TASK ( 
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  MESSAGE_QUEUE_TASK
--
--  DESCRIPTION:
--
--  This is one of the test tasks.
--
 
   procedure MESSAGE_QUEUE_TASK ( 
      INDEX : in     RTEMS.TASK_ARGUMENT
   );

--
--  PARTITION_TASK
--
--  DESCRIPTION:
--
--  This is one of the test tasks.
--
 
   procedure PARTITION_TASK ( 
      IGNORED : in     RTEMS.TASK_ARGUMENT
   );

--
--  SEMAPHORE_TASK
--
--  DESCRIPTION:
--
--  This is one of the test tasks.
--
 
   procedure SEMAPHORE_TASK ( 
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
       RTEMS.TIMESLICE,                              -- initial mode
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
      32,                        -- maximum # global objects
      32                         -- maximum # proxies
    );

--
--  This is the Configuration Table for this test.
--

   CONFIGURATION : aliased RTEMS.CONFIGURATION_TABLE := (
      RTEMS.NULL_ADDRESS,        -- will be replaced by BSP
      64 * 1024,                 -- executive RAM size
      10,                        -- maximum # tasks
      12,                        -- maximum # timers
      1,                         -- maximum # semaphores
      1,                         -- maximum # message queues
      1,                         -- maximum # messages
      1,                         -- maximum # partitions
      0,                         -- maximum # regions
      0,                         -- maximum # dp memory areas
      0,                         -- maximum # periods
      0,                         -- maximum # user extensions
      RTEMS.MILLISECONDS_TO_MICROSECONDS(10), -- # us in a tick
      1                          -- # ticks in a timeslice
  );

end MPTEST;
