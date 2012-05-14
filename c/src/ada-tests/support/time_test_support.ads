--
--  TIME_TEST_SUPPORT / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package provides routines which aid the individual tests in
--  the Timing Test Suite and simplify their design and operation.
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

with RTEMS;

package TIME_TEST_SUPPORT is

--
--  The following constants define the number of times a directive
--  will be performed in the Timing Suite.
--

   OPERATION_COUNT : constant RTEMS.UNSIGNED32 := 100;
   ITERATION_COUNT : constant RTEMS.UNSIGNED32 := 100;

--
--  PUT_TIME
--
--  DESCRIPTION:
--
--  This subprogram prints the MESSAGE followed by the length of
--  time which each individual operation took.  All times are
--  in microseconds.
--

   procedure PUT_TIME (
      MESSAGE          : in     STRING;
      TOTAL_TIME       : in     RTEMS.UNSIGNED32;
      ITERATIONS       : in     RTEMS.UNSIGNED32;
      LOOP_OVERHEAD    : in     RTEMS.UNSIGNED32;
      CALLING_OVERHEAD : in     RTEMS.UNSIGNED32
   );

end TIME_TEST_SUPPORT;
