--
--  Test_Support / Specification
--
--  DESCRIPTION:
--
--  This package provides routines which aid the Test Suites
--  and simplify their design and operation.
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

with Interfaces; use Interfaces;
with RTEMS;
with Unsigned32_IO;
with Status_IO;
with Text_IO;

package body Test_Support is

--PAGE
--
--  Fatal_Directive_Status
--

   procedure Fatal_Directive_Status (
      Status  : in     RTEMS.Status_Codes;
      Desired : in     RTEMS.Status_Codes;
      Message : in     String
   ) is
   begin

      if not RTEMS.Are_Statuses_Equal( Status, Desired ) then

         Text_IO.Put( Message );
         Text_IO.Put( " FAILED -- expected " );
         Status_IO.Put( Desired );
         Text_IO.Put( " got " );
         Status_IO.Put( Status );
         Text_IO.New_Line;

         RTEMS.Fatal_Error_Occurred( RTEMS.Status_Codes'Pos( Status ) );

      end if;

   end Fatal_Directive_Status;

--PAGE
--
--  Directive_Failed
--

   procedure Directive_Failed (
      Status  : in     RTEMS.Status_Codes;
      Message : in     String
   ) is
   begin

      Test_Support.Fatal_Directive_Status(
         Status, 
         RTEMS.Successful, 
         Message 
      );

   end Directive_Failed;

--PAGE
--
--  Print_Time
--

   procedure Print_Time (
      Prefix      : in     String;
      Time_Buffer : in     RTEMS.Time_Of_Day;
      Suffix      : in     String
   ) is
   begin

      Text_IO.Put( Prefix );
      Unsigned32_IO.Put( Time_Buffer.Hour, Width=>2 );
      Text_IO.Put( ":" );
      Unsigned32_IO.Put( Time_Buffer.Minute, Width=>2 );
      Text_IO.Put( ":" );
      Unsigned32_IO.Put( Time_Buffer.Second, Width=>2 );
      Text_IO.Put( "   " );
      Unsigned32_IO.Put( Time_Buffer.Month, Width=>2 );
      Text_IO.Put( "/" );
      Unsigned32_IO.Put( Time_Buffer.Day, Width=>2 );
      Text_IO.Put( "/" );
      Unsigned32_IO.Put( Time_Buffer.Year, Width=>2 );
      Text_IO.Put( Suffix );

   end Print_Time;

--PAGE
--
--  Put_Dot
--
 
   procedure Put_Dot (
      Buffer : in     String
   ) is
   begin
      Text_IO.Put( Buffer );
      Text_IO.FLUSH;
   end Put_Dot;

--PAGE
--
--  Pause
--

   procedure Pause is
      --  Ignored_String : String( 1 .. 80 );
      --  Ignored_Last   : Natural;
      
   begin

      -- 
      --  Really should be a "put" followed by a "flush."
      -- 
      Text_IO.Put_Line( "<pause> " );
      -- Text_IO.Get_Line( Ignored_String, Ignored_Last );

   -- exception

   --    when Text_IO.End_Error =>
   --    -- ignore this error.  It happens when redirecting input from /dev/null 
   --    return;

   end Pause;

--PAGE
--
--  Pause_And_Screen_Number
--
 
   procedure Pause_And_Screen_Number (
      SCREEN : in    RTEMS.Unsigned32
   ) is
      --  Ignored_String : String( 1 .. 80 );
      --  Ignored_Last   : Natural;
   begin
 
      --
      --  Really should be a "put" followed by a "flush."
      --
      Text_IO.Put( "<pause - screen  " );
      Unsigned32_IO.Put( SCREEN, Width=>2 );
      Text_IO.Put_Line( "> " );
   --    Text_IO.Get_Line( Ignored_String, Ignored_Last );
 
   -- exception

   --    when Text_IO.End_Error =>
   --    -- ignore this error.  It happens when redirecting input from /dev/null 
   --    return;

   end Pause_And_Screen_Number;

--PAGE
--
--  Put_Name
--

   procedure Put_Name (
      Name     : in     RTEMS.Name;
      New_Line : in     Boolean
   ) is
      C1 : Character;
      C2 : Character;
      C3 : Character;
      C4 : Character;
   begin

      RTEMS.Name_To_Characters( Name, C1, C2, C3, C4 );

      Text_IO.Put( C1 );
      Text_IO.Put( C2 );
      Text_IO.Put( C3 );
      Text_IO.Put( C4 );

      if New_Line = True then
         Text_IO.New_Line;
      end if;

   end Put_Name;
 
--PAGE
--
--  Task_Number
--

   function Task_Number (
      TID : in     RTEMS.ID
   ) return RTEMS.Unsigned32 is
   begin

      return RTEMS.Get_Index( TID ) - 1 -
        RTEMS.Configuration.RTEMS_API_Configuration.Number_Of_Initialization_Tasks;

   end Task_Number;

--PAGE
--
--  Do_Nothing
--

   procedure Do_Nothing is
   begin
      NULL;
   end Do_Nothing;
   

--PAGE
--
--  Milliseconds_Per_Tick
--

   function Milliseconds_Per_Tick 
   return RTEMS.Unsigned32 is
   begin
      return RTEMS.Configuration.Microseconds_Per_Tick / 1000;
   end Milliseconds_Per_Tick;
end Test_Support;
