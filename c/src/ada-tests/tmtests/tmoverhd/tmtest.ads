--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Calling Overhead Test of the RTEMS
--  Timing Test Suite.
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

with RTEMS;
with RTEMS.TASKS;
with SYSTEM;
with System.Storage_Elements;

package TMTEST is

--
--  Buffer Record similar to that used by RTEMS 3.2.1.  Using this
--  avoids changes to the test.
--

   type BUFFER is
      record
         FIELD1 : RTEMS.UNSIGNED32;   -- TEMPORARY UNTIL VARIABLE LENGTH
         FIELD2 : RTEMS.UNSIGNED32;
         FIELD3 : RTEMS.UNSIGNED32;
         FIELD4 : RTEMS.UNSIGNED32;
      end record;

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;

--
--  The following variable is set to the execution time returned
--  by the timer.
--

   END_TIME  : RTEMS.UNSIGNED32;

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
--  The following area defines a memory area to be used as the
--  memory space for a partition and later for a region.
-- 

   MEMORY_AREA : array ( RTEMS.UNSIGNED32 range 0 .. 255 ) of RTEMS.UNSIGNED8;
   for MEMORY_AREA'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

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
--  TIMER_HANDLER
--
--  DESCRIPTION:
--
--  This subprogram is a timer service routine.
--

   procedure TIMER_HANDLER (
      IGNORED_ID      : in     RTEMS.ID;  
      IGNORED_ADDRESS : in     RTEMS.ADDRESS 
   );
   pragma Convention (C, TIMER_HANDLER);

--
--  ISR_HANDLER
--
--  DESCRIPTION:
--
--  This subprogram is an interrupt service routine.
--

--   procedure ISR_HANDLER (
--      VECTOR : in     RTEMS.VECTOR_NUMBER
--   );

--
--  ASR_HANDLER
--
--  DESCRIPTION:
--
--  This subprogram is an asynchronous signal routine.
--

   procedure ASR_HANDLER (
      SIGNALS : in     RTEMS.SIGNAL_SET
   );
   pragma Convention (C, ASR_HANDLER);

--
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task is responsible for measuring and 
--  reporting the calling overhead for all RTEMS
--  directives.
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_1);

end TMTEST;
