-- SPDX-License-Identifier: BSD-2-Clause

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
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  Redistribution and use in source and binary forms, with or without
--  modification, are permitted provided that the following conditions
--  are met:
--  1. Redistributions of source code must retain the above copyright
--     notice, this list of conditions and the following disclaimer.
--  2. Redistributions in binary form must reproduce the above copyright
--     notice, this list of conditions and the following disclaimer in the
--     documentation and/or other materials provided with the distribution.
--
--  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
--  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
--  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
--  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
--  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
--  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
--  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
--  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
--  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
--  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
--  POSSIBILITY OF SUCH DAMAGE.
--

with RTEMS;
with RTEMS.TASKS;
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
   for INTERNAL_PORT_AREA'Address use INTERNAL_PORT_AREA_ADDRESS;

--
--  The following area defines a memory area to be used as the
--  external address space of the port.
-- 

   EXTERNAL_PORT_AREA_ADDRESS : constant System.Address :=
       System.Storage_Elements.To_Address(16#00002000#);

   EXTERNAL_PORT_AREA : array ( RTEMS.UNSIGNED32 range 0 .. 255 )
      of RTEMS.UNSIGNED8;
   for EXTERNAL_PORT_AREA'Address use EXTERNAL_PORT_AREA_ADDRESS;

--
--  The following area defines a memory area to be used for 
--  addresses which are below the address space of the port.
-- 

   BELOW_PORT_AREA_ADDRESS : constant System.Address :=
       System.Storage_Elements.To_Address(16#00000500#);

   BELOW_PORT_AREA : array ( RTEMS.UNSIGNED32 range 0 .. 255 )
      of RTEMS.UNSIGNED8;
   for BELOW_PORT_AREA'Address use BELOW_PORT_AREA_ADDRESS;

--
--  The following area defines a memory area to be used for 
--  addresses which are above the address space of the port.
-- 

   ABOVE_PORT_AREA_ADDRESS : constant System.Address :=
       System.Storage_Elements.To_Address(16#00003000#);

   ABOVE_PORT_AREA : array ( RTEMS.UNSIGNED32 range 0 .. 255 )
      of RTEMS.UNSIGNED8;
   for ABOVE_PORT_AREA'Address use ABOVE_PORT_AREA_ADDRESS;

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
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Dual Ported Memory Manager.
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_1);

end SPTEST;
