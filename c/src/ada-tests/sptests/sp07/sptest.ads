--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 7 of the RTEMS
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
--  sptest.ads,v 1.3 1995/07/12 19:41:33 joel Exp
--

with CLOCK_DRIVER;
with RTEMS;

package SPTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS extensions created
--  by this test.
--
 
   EXTENSION_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   EXTENSION_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;
 
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
--  This RTEMS task tests some of the capabilities of the Task Manager.
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASK_2
--
--  DESCRIPTION:
--
--  This RTEMS task tests some of the capabilities of the Task Manager.
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASK_3
--
--  DESCRIPTION:
--
--  This RTEMS task tests some of the capabilities of the Task Manager.
--

   procedure TASK_3 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASK_4
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Message Queue Manager.
--

   procedure TASK_4 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASK_CREATE_EXTENSION
--
--  DESCRIPTION:
--
--  This subprogram is the TASK_CREATE extension for this test.
--

   procedure TASK_CREATE_EXTENSION (
      UNUSED       : in     RTEMS.TCB_POINTER;
      CREATED_TASK : in     RTEMS.TCB_POINTER
   );

--
--  TASK_DELETE_EXTENSION
--
--  DESCRIPTION:
--
--  This subprogram is the TASK_DELETE extension for this test.
--

   procedure TASK_DELETE_EXTENSION (
      RUNNING_TASK : in     RTEMS.TCB_POINTER;
      DELETED_TASK : in     RTEMS.TCB_POINTER
   );

--
--  TASK_RESTART_EXTENSION
--
--  DESCRIPTION:
--
--  This subprogram is the TASK_RESTART extension for this test.
--

   procedure TASK_RESTART_EXTENSION (
      UNUSED         : in     RTEMS.TCB_POINTER;
      RESTARTED_TASK : in     RTEMS.TCB_POINTER
   );

--
--  TASK_START_EXTENSION
--
--  DESCRIPTION:
--
--  This subprogram is the TASK_START extension for this test.
--

   procedure TASK_START_EXTENSION (
      UNUSED       : in     RTEMS.TCB_POINTER;
      STARTED_TASK : in     RTEMS.TCB_POINTER
   );

--
--  TASK_EXIT_EXTENSION
--
--  DESCRIPTION:
--
--  This subprogram is the TASK_EXIT extension for this test.
--  It is invoked when a RTEMS task exits from its entry point.
--

   procedure TASK_EXIT_EXTENSION (
      RUNNING_TASK : in     RTEMS.TCB_POINTER
   );

--
--  This is the User Extension Table for this test.
--

   EXTENSIONS : aliased RTEMS.EXTENSIONS_TABLE := 
   (
        SPTEST.TASK_CREATE_EXTENSION'ACCESS,         -- Task create
        SPTEST.TASK_START_EXTENSION'ACCESS,          -- Task start
        SPTEST.TASK_RESTART_EXTENSION'ACCESS,        -- Task restart
        SPTEST.TASK_DELETE_EXTENSION'ACCESS,         -- Task delete
        NULL,                                        -- Task switch
        NULL,                                        -- Task begin
        SPTEST.TASK_EXIT_EXTENSION'ACCESS,           -- Task exitted
        NULL                                         -- Fatal error
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
      0,                         -- maximum # regions
      0,                         -- maximum # dp memory areas
      0,                         -- maximum # periods
      1,                         -- maximum # user extensions
      RTEMS.MILLISECONDS_TO_MICROSECONDS(10), -- # us in a tick
      100                        -- # ticks in a timeslice
  );

end SPTEST;
