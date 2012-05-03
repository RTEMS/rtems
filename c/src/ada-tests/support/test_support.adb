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
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with Interfaces; use Interfaces;
with Unsigned32_IO;
with Status_IO;
with Text_IO;
with RTEMS.Fatal;

package body Test_Support is

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

         RTEMS.Fatal.Error_Occurred( RTEMS.Status_Codes'Pos( Status ) );

      end if;

   end Fatal_Directive_Status;

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

   end Pause;

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
 
   end Pause_And_Screen_Number;

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

      if New_Line then
         Text_IO.New_Line;
      end if;

   end Put_Name;
 
--
--  Task_Number
--

   function Task_Number (
      TID : in     RTEMS.ID
   ) return RTEMS.Unsigned32 is
   begin

      -- probably OK
      return RTEMS.Get_Index( TID ) - 1;

   end Task_Number;

--
--  Do_Nothing
--

   procedure Do_Nothing is
   begin
      NULL;
   end Do_Nothing;
   

--
--  Milliseconds_Per_Tick
--

   function Milliseconds_Per_Tick 
   return RTEMS.Unsigned32 is
      function Milliseconds_Per_Tick_Base return RTEMS.Unsigned32;
      pragma Import (C, Milliseconds_Per_Tick_Base, "milliseconds_per_tick");
   begin
      return Milliseconds_Per_Tick_Base;
   end Milliseconds_Per_Tick;

--
--  Milliseconds_Per_Tick
--
   function Ticks_Per_Second 
   return RTEMS.Interval is
      function Ticks_Per_Second_Base return RTEMS.Unsigned32;
      pragma Import (C, Ticks_Per_Second_Base, "ticks_per_second");
   begin
      return Ticks_Per_Second_Base;
   end Ticks_Per_Second; 

--
--  Return the size of the RTEMS Workspace
--

   function Work_Space_Size
   return RTEMS.Unsigned32 is 
      function Work_Space_Size_Base return RTEMS.Unsigned32;
      pragma Import (C, Work_Space_Size_Base, "work_space_size");
   begin
      return Work_Space_Size_Base;
   end Work_Space_Size;

--
--  Return an indication of whether multiprocessing is configured
--

   function Is_Configured_Multiprocessing
   return Boolean is
      function Is_Configured_Multiprocessing_Base return RTEMS.Unsigned32;
      pragma Import (
         C, Is_Configured_Multiprocessing_Base, "is_configured_multiprocessing"
      );
   begin
      if Is_Configured_Multiprocessing_Base = 1 then
         return True;
      else
         return False;
      end if;
   end Is_Configured_Multiprocessing;

--
--  Node is the node number in a multiprocessor configuration
--

   function Node 
   return RTEMS.Unsigned32 is
      function Get_Node_Base return RTEMS.Unsigned32;
      pragma Import (C, Get_Node_Base, "get_node");
   begin
      return Get_Node_Base;
   end Node;
end Test_Support;
