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
--  COPYRIGHT (c) 1989-2009.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with INTERFACES; use INTERFACES;
with TEXT_IO;
with UNSIGNED32_IO;

package body TIME_TEST_SUPPORT is

--PAGE
--
--  PUT_TIME
--

   procedure PUT_TIME (
      MESSAGE          : in     STRING;
      TOTAL_TIME       : in     RTEMS.UNSIGNED32;
      ITERATIONS       : in     RTEMS.UNSIGNED32;
      LOOP_OVERHEAD    : in     RTEMS.UNSIGNED32;
      CALLING_OVERHEAD : in     RTEMS.UNSIGNED32
   ) is
      PER_ITERATION : RTEMS.UNSIGNED32;
   begin

      PER_ITERATION := (TOTAL_TIME - LOOP_OVERHEAD) / ITERATIONS;
      PER_ITERATION := PER_ITERATION - CALLING_OVERHEAD;

      if PER_ITERATION = 0 then

         TEXT_IO.PUT( "TOTAL_TIME " );
         UNSIGNED32_IO.PUT( TOTAL_TIME );
         TEXT_IO.NEW_LINE; 

         TEXT_IO.PUT( "ITERATIONS " );
         UNSIGNED32_IO.PUT( ITERATIONS );
         TEXT_IO.NEW_LINE; 

         TEXT_IO.PUT( "LOOP_OVERHEAD " );
         UNSIGNED32_IO.PUT( LOOP_OVERHEAD );
         TEXT_IO.NEW_LINE; 

         TEXT_IO.PUT( "CALLING_OVERHEAD " );
         UNSIGNED32_IO.PUT( CALLING_OVERHEAD );
         TEXT_IO.NEW_LINE; 

      end if;

      TEXT_IO.PUT( MESSAGE );
      TEXT_IO.PUT( " " );
      UNSIGNED32_IO.PUT( PER_ITERATION );
      TEXT_IO.NEW_LINE; 
      TEXT_IO.FLUSH; 

   end PUT_TIME;

end TIME_TEST_SUPPORT;
