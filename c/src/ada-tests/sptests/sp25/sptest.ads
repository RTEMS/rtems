--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 25 of the RTEMS
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

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS regions created
--  by this test.
--

   REGION_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;
   REGION_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.NAME;

--
--  The following constant defines the priority of most of the 
--  RTEMS tasks in this test.  This allows one of the tasks to
--  easily set itself to a higher priority than the rest.
--

   BASE_PRIORITY : constant RTEMS.TASKS.PRIORITY := 140;

--
--  These arrays define the memory areas used for the regions in
--  this test.
--

   AREA_1 : array ( RTEMS.UNSIGNED32 range 0 .. 64000 ) of RTEMS.UNSIGNED8;
   for AREA_1'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

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
--  PUT_ADDRESS_FROM_AREA_1
--
--  DESCRIPTION:
--
--  This subprogram prints the offset of the address TO_BE_PRINTED 
--  from the first byte of AREA_1.
--
--  NOTE:
--
--  This subprogram is used because the actual address of AREA_1
--  varies based upon the size of the executable, the target board,
--  and the target processor.
--

   procedure PUT_ADDRESS_FROM_AREA_1 (
      TO_BE_PRINTED : in     RTEMS.ADDRESS
   );

--
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Region Manager.
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_1);

end SPTEST;
