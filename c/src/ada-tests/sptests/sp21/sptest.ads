--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 21 of the RTEMS
--  Single Processor Test Suite.
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

package SPTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;

--
--  The following constants define the device major numbers
--  utilized by this test.
--

   NO_DRIVER_MAJOR      : constant RTEMS.DEVICE_MAJOR_NUMBER := 3; 
   INVALID_DRIVER_MAJOR : constant RTEMS.DEVICE_MAJOR_NUMBER := 5;
   STUB_DRIVER_MAJOR    : constant RTEMS.DEVICE_MAJOR_NUMBER := 2;

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
--  This RTEMS task tests the Input Output Manager.
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

end SPTEST;
