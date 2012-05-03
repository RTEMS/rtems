--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 9 of the RTEMS
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
with SYSTEM;
with System.Storage_Elements;

package SPTEST is

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

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 11 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 11 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS timers created
--  by this test.
--

   TIMER_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.ID;
   TIMER_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS semaphores created
--  by this test.
--

   SEMAPHORE_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   SEMAPHORE_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS message queues 
--  created by this test.
--

   QUEUE_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;
   QUEUE_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS partitions created
--  by this test.
--

   PARTITION_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.ID;
   PARTITION_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS regions created
--  by this test.
--

   REGION_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.ID;
   REGION_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS ports created
--  by this test.
--

   PORT_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.ID;
   PORT_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS periods created
--  by this test.
--

   PERIOD_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.ID;
   PERIOD_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.NAME;

--
--  This variable is used as the output ID on directives which return
--  an ID but are invoked in a manner which returns a non-SUCCESSFUL
--  status code.
--

   JUNK_ID : RTEMS.ID;

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
--  memory space for a partition.
-- 

   PARTITION_GOOD_AREA : array ( RTEMS.UNSIGNED32 range 0 .. 255 )
      of RTEMS.UNSIGNED8;
   for PARTITION_GOOD_AREA'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

--
--  The following area defines a memory area to be used as the
--  memory space for a partition which starts at an invalid address.
-- 

   PARTITION_BAD_AREA_ADDRESS : constant System.Address :=
       System.Storage_Elements.To_Address(16#00000006#);

   PARTITION_BAD_AREA : array ( RTEMS.UNSIGNED32 range 0 .. 255 )
      of RTEMS.UNSIGNED8;
   for PARTITION_BAD_AREA'Address use PARTITION_BAD_AREA_ADDRESS;

--
--  The following area defines a memory area to be used as the
--  memory space for a region and constants used to define the regions
--  starting area and length.
-- 

   REGION_GOOD_AREA : array ( RTEMS.UNSIGNED32 range 0 .. 4095 )
      of RTEMS.UNSIGNED32;
   for REGION_GOOD_AREA'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

   REGION_START_OFFSET : constant RTEMS.UNSIGNED32 := 1024;
   REGION_LENGTH       : constant RTEMS.UNSIGNED32 := 512;

--
--  The following area defines a memory area to be used as the
--  memory space for a region which starts at an invalid address.
-- 

   REGION_BAD_AREA_ADDRESS : constant System.Address :=
       System.Storage_Elements.To_Address(16#00000006#);

   REGION_BAD_AREA : array ( RTEMS.UNSIGNED32 range 0 .. 255 )
      of RTEMS.UNSIGNED8;
   for REGION_BAD_AREA'Address use REGION_BAD_AREA_ADDRESS;

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
--  DELAYED_SUBPROGRAM
--
--  DESCRIPTION:
--
--  This subprogram is scheduled as a timer service routine.
--  It performs no actions if it fires.
--

   procedure DELAYED_SUBPROGRAM (
      IGNORED_ID      : in     RTEMS.ID;  
      IGNORED_ADDRESS : in     RTEMS.ADDRESS 
   );
   pragma Convention (C, DELAYED_SUBPROGRAM);

--
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task tests numerous error conditions.
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_1);

--
--  TASK_2
--
--  DESCRIPTION:
--
--  This RTEMS task tests numerous error conditions.
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_2);

--
--  TASK_3
--
--  DESCRIPTION:
--
--  This RTEMS task tests numerous error conditions.
--

   procedure TASK_3 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_3);

--
--  TASK_4
--
--  DESCRIPTION:
--
--  This RTEMS task tests numerous error conditions.
--

   procedure TASK_4 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_4);

--
--  SCREEN_1
--
--  DESCRIPTION:
--
--  This subprogram processes the a single output screen of this test.
--

   procedure SCREEN_1;

--
--  SCREEN_2
--
--  DESCRIPTION:
--
--  This subprogram processes the a single output screen of this test.
--

   procedure SCREEN_2;

--
--  SCREEN_3
--
--  DESCRIPTION:
--
--  This subprogram processes the a single output screen of this test.
--

   procedure SCREEN_3;

--
--  SCREEN_4
--
--  DESCRIPTION:
--
--  This subprogram processes the a single output screen of this test.
--

   procedure SCREEN_4;

--
--  SCREEN_5
--
--  DESCRIPTION:
--
--  This subprogram processes the a single output screen of this test.
--

   procedure SCREEN_5;

--
--  SCREEN_6
--
--  DESCRIPTION:
--
--  This subprogram processes the a single output screen of this test.
--

   procedure SCREEN_6;

--
--  SCREEN_7
--
--  DESCRIPTION:
--
--  This subprogram processes the a single output screen of this test.
--

   procedure SCREEN_7;

--
--  SCREEN_8
--
--  DESCRIPTION:
--
--  This subprogram processes the a single output screen of this test.
--

   procedure SCREEN_8;

--
--  SCREEN_9
--
--  DESCRIPTION:
--
--  This subprogram processes the a single output screen of this test.
--

   procedure SCREEN_9;

--
--  SCREEN_10
--
--  DESCRIPTION:
--
--  This subprogram processes the a single output screen of this test.
--

   procedure SCREEN_10;

--
--  SCREEN_11
--
--  DESCRIPTION:
--
--  This subprogram processes the a single output screen of this test.
--

   procedure SCREEN_11;

--
--  SCREEN_12
--
--  DESCRIPTION:
--
--  This subprogram processes the a single output screen of this test.
--

   procedure SCREEN_12;

--
--  SCREEN_13
--
--  DESCRIPTION:
--
--  This subprogram processes the a single output screen of this test.
--

   procedure SCREEN_13;

end SPTEST;
