--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 1 of the RTEMS
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
--  PUT_SIZE
--
--  DESCRIPTION:
--
--  This procedure outputs the size information about the specified
--  object in a standard format.
--

   procedure PUT_SIZE (
      DESCRIPTION    : in     STRING;
      HAS_FIELD_NAME : in     BOOLEAN;
      FIELD_NAME     : in     STRING             := "";
      REQUIREMENT    : in     RTEMS.UNSIGNED32   := 0
   );

end SPTEST;
