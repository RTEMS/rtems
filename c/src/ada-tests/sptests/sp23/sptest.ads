--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 23 of the RTEMS
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
with SYSTEM;
with System.Storage_Elements;

package SPTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS ports created
--  by this test.
--

   PORT_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.ID;
   PORT_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.NAME;

--
--  The following area defines a memory area to be used as the
--  internal address space of the port.
-- 

   INTERNAL_PORT_AREA_ADDRESS : constant System.Address :=
       System.Storage_Elements.To_Address(16#00001000#);

   INTERNAL_PORT_AREA : array ( RTEMS.UNSIGNED32 range 0 .. 255 )
      of RTEMS.UNSIGNED8;
   for INTERNAL_PORT_AREA use at INTERNAL_PORT_AREA_ADDRESS;

--
--  The following area defines a memory area to be used as the
--  external address space of the port.
-- 

   EXTERNAL_PORT_AREA_ADDRESS : constant System.Address :=
       System.Storage_Elements.To_Address(16#00002000#);

   EXTERNAL_PORT_AREA : array ( RTEMS.UNSIGNED32 range 0 .. 255 )
      of RTEMS.UNSIGNED8;
   for EXTERNAL_PORT_AREA use at EXTERNAL_PORT_AREA_ADDRESS;

--
--  The following area defines a memory area to be used for 
--  addresses which are below the address space of the port.
-- 

   BELOW_PORT_AREA_ADDRESS : constant System.Address :=
       System.Storage_Elements.To_Address(16#00000500#);

   BELOW_PORT_AREA : array ( RTEMS.UNSIGNED32 range 0 .. 255 )
      of RTEMS.UNSIGNED8;
   for BELOW_PORT_AREA use at BELOW_PORT_AREA_ADDRESS;

--
--  The following area defines a memory area to be used for 
--  addresses which are above the address space of the port.
-- 

   ABOVE_PORT_AREA_ADDRESS : constant System.Address :=
       System.Storage_Elements.To_Address(16#00003000#);

   ABOVE_PORT_AREA : array ( RTEMS.UNSIGNED32 range 0 .. 255 )
      of RTEMS.UNSIGNED8;
   for ABOVE_PORT_AREA use at ABOVE_PORT_AREA_ADDRESS;

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
--  This RTEMS task tests the Dual Ported Memory Manager.
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

end SPTEST;
