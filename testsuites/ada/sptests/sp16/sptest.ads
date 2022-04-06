-- SPDX-License-Identifier: BSD-2-Clause

--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 16 of the RTEMS
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

package SPTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 5 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 5 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS regions created
--  by this test.
--

   REGION_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.ID;
   REGION_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 4 ) of RTEMS.NAME;

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

   AREA_1 : array ( RTEMS.UNSIGNED32 range 0 .. 4095 ) of RTEMS.UNSIGNED8;
   for AREA_1'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

   AREA_2 : array ( RTEMS.UNSIGNED32 range 0 .. 4095 ) of RTEMS.UNSIGNED8;
   for AREA_2'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

   AREA_3 : array ( RTEMS.UNSIGNED32 range 0 .. 4095 ) of RTEMS.UNSIGNED8;
   for AREA_3'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

   AREA_4 : array ( RTEMS.UNSIGNED32 range 0 .. 8191 ) of RTEMS.UNSIGNED8;
   for AREA_4'ALIGNMENT use RTEMS.STRUCTURE_ALIGNMENT;

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
--  PUT_ADDRESS_FROM_AREA_2
--
--  DESCRIPTION:
--
--  This subprogram prints the offset of the address TO_BE_PRINTED 
--  from the first byte of AREA_2.
--
--  NOTE:
--
--  This subprogram is used because the actual address of AREA_2
--  varies based upon the size of the executable, the target board,
--  and the target processor.
--

   procedure PUT_ADDRESS_FROM_AREA_2 (
      TO_BE_PRINTED : in     RTEMS.ADDRESS
   );

--
--  PUT_ADDRESS_FROM_AREA_3
--
--  DESCRIPTION:
--
--  This subprogram prints the offset of the address TO_BE_PRINTED 
--  from the first byte of AREA_3.
--
--  NOTE:
--
--  This subprogram is used because the actual address of AREA_3
--  varies based upon the size of the executable, the target board,
--  and the target processor.
--

   procedure PUT_ADDRESS_FROM_AREA_3 (
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

--
--  TASK_2
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Region Manager.
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
--  This RTEMS task tests the Region Manager.
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
--  This RTEMS task tests the Region Manager.
--

   procedure TASK_4 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_4);

--
--  TASK_5
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Region Manager.
--

   procedure TASK_5 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASK_5);

end SPTEST;
