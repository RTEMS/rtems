--
--  MPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 6 of the RTEMS
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
--  These arrays contain the IDs and NAMEs of all RTEMS timers created
--  by this test.
--
 
   TIMER_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;
   TIMER_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.NAME;
 
--
--  This variable is set when the test should stop executing.
--
   STOP_TEST : RTEMS.BOOLEAN;

--
--  This variable contains the ID of the remote task with which this
--  test interacts.
--

   REMOTE_TID  : RTEMS.ID;

--
--  This variable contains the node on which the remote task with which 
--  this test interacts resides.
--

   REMOTE_NODE : RTEMS.UNSIGNED32;

--
--  The number of signals to process per dot printed out.
--

   MAXIMUM_DOTS : constant RTEMS.UNSIGNED32 := 25;

--
--  The following is a table of the event sets which consist of
--  a single event.  This test cycles through all of these
--  events.
-- 

   EVENT_SET_TABLE : constant array ( 0 .. 30 ) of RTEMS.EVENT_SET := (
      RTEMS.EVENT_0,
      RTEMS.EVENT_1,
      RTEMS.EVENT_2,
      RTEMS.EVENT_3,
      RTEMS.EVENT_4,
      RTEMS.EVENT_5,
      RTEMS.EVENT_6,
      RTEMS.EVENT_7,
      RTEMS.EVENT_8,
      RTEMS.EVENT_9,
      RTEMS.EVENT_10,
      RTEMS.EVENT_11,
      RTEMS.EVENT_12,
      RTEMS.EVENT_13,
      RTEMS.EVENT_14,
      RTEMS.EVENT_15,
      RTEMS.EVENT_16,
      RTEMS.EVENT_17,
      RTEMS.EVENT_18,
      RTEMS.EVENT_19,
      RTEMS.EVENT_20,
      RTEMS.EVENT_21,
      RTEMS.EVENT_22,
      RTEMS.EVENT_23,
      RTEMS.EVENT_24,
      RTEMS.EVENT_25,
      RTEMS.EVENT_26,
      RTEMS.EVENT_27,
      RTEMS.EVENT_28,
      RTEMS.EVENT_29,
      RTEMS.EVENT_30
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
--  STOP_TEST_TSR
--
--  DESCRIPTION:
--
--  This subprogram is a TSR which sets the "stop test" flag.
--
 
   procedure STOP_TEST_TSR (
      IGNORED1 : in     RTEMS.ID;
      IGNORED2 : in     RTEMS.ADDRESS
   );

--
--  TEST_TASK
--
--  DESCRIPTION:
--
--  This is the body of the RTEMS tasks which constitute this test.
--

   procedure TEST_TASK (
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
      1,                         -- maximum # timers
      2,                         -- maximum # semaphores
      0,                         -- maximum # message queues
      0,                         -- maximum # messages
      0,                         -- maximum # partitions
      0,                         -- maximum # regions
      0,                         -- maximum # dp memory areas
      0,                         -- maximum # periods
      0,                         -- maximum # user extensions
      RTEMS.MILLISECONDS_TO_MICROSECONDS(10), -- # us in a tick
      50                         -- # ticks in a timeslice
  );

end MPTEST;
