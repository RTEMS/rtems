--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of the Ada Task Name test of the RTEMS
--  Ada Single Processor Test Suite.
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
with TEST_SUPPORT;
with TEXT_IO;
with System; use System;  -- for Null Pointer comparison

package body SPTEST is

   task body AdaTask is
      Status     : RTEMS.Status_Codes;
      Pointer    : RTEMS.Address;
      StringName : String(1 .. 120) := (1 .. 120 => '*' );
      NewName    : constant String  := "Josiah";
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** Ada Task Name TEST ***" );

      RTEMS.Object_Get_Name( RTEMS.Self, StringName, Pointer );
      if Pointer = RTEMS.Null_Address then
         TEXT_IO.PUT_LINE( "Object_Get_Name_Failed" );
      else
         TEXT_IO.PUT_LINE( "My name is (" & StringName & ")" );
      end if;

      TEXT_IO.PUT_LINE( "Setting name to (Josiah)" );
      RTEMS.Object_Set_Name( RTEMS.Self, NewName, Status );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "Object_Set_Name" );

      RTEMS.Object_Get_Name( RTEMS.Self, StringName, Pointer );
      if Pointer = RTEMS.Null_Address then
         TEXT_IO.PUT_LINE( "Object_Get_Name_Failed" );
      else
         TEXT_IO.PUT_LINE( "My name is (" & StringName & ")" );
      end if;

      TEXT_IO.PUT_LINE( "*** END OF Ada Task Name TEST ***" );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end AdaTask;

end SPTEST;
