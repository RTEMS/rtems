--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 19 of the RTEMS
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
--  http://www.OARcorp.com/rtems/license.html.
--
--  $Id$
--

with RTEMS;

package SPTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 6 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 6 ) of RTEMS.NAME;

--
--  This array contains the floating point factors used by the
--  floating point tasks in this test.
--

   FP_FACTORS : array ( RTEMS.UNSIGNED32 range 0 .. 9 ) of FLOAT;

--
--  This array contains the integer factors used by the
--  integer tasks in this test.
--

   INTEGER_FACTORS : array ( RTEMS.UNSIGNED32 range 0 .. 9 ) of 
      RTEMS.UNSIGNED32;

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
--  FIRST_FP_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task tests the restart and deletion of floating point tasks.
--

   procedure FIRST_FP_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  FP_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task tests the basic capabilities of a floating point
--  task.
--

   procedure FP_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task provides a non-floating point task to test
--  that an application can utilize a mixture of floating point
--  and non-floating point tasks.
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

end SPTEST;
