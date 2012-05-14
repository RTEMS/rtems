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
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with RTEMS;
with RTEMS.TASKS;

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
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, INIT);

--
--  FIRST_FP_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task tests the restart and deletion of floating point tasks.
--

   procedure FIRST_FP_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, FIRST_FP_TASK);

--
--  FP_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task tests the basic capabilities of a floating point
--  task.
--

   procedure FP_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, FP_TASK);

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
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_1);

--
--  Add_Float
--
--  DESCRIPTION:
--
--  This method ensures the compilers thinks we are using the variables.
--

   function Add_Float(
     n      : in Float;
     factor : in Float
   ) return Float;
   pragma Interface (C, Add_Float);
   pragma Interface_Name (Add_Float, "add_float");

end SPTEST;
