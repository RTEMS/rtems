--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 1 of the RTEMS
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
--  http://www.rtems.org/license/LICENSE.
--

with TEXT_IO;
with TEST_SUPPORT;

package body SPTEST is

-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEST_SUPPORT.ADA_TEST_BEGIN;
      TEXT_IO.PUT_LINE( "Hello World" );
      TEST_SUPPORT.ADA_TEST_END;

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end INIT;

end SPTEST;
