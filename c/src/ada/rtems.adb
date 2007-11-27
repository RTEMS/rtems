--
--  RTEMS / Body
--
--  DESCRIPTION:
--
--  This package provides the interface to the RTEMS API.
--  
--
--  DEPENDENCIES:
--
--
--
--  COPYRIGHT (c) 1997-2007.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--
--  $Id$
--

with Ada;
with Ada.Unchecked_Conversion;
with System;
with Interfaces; use Interfaces;
with Interfaces.C;

package body RTEMS is

   --
   --  Utility Functions
   --
 
   function From_Ada_Boolean (
      Ada_Boolean : Standard.Boolean
   ) return RTEMS.Boolean is
   begin

      if Ada_Boolean = Standard.True then
         return RTEMS.True;
      end if;

      return RTEMS.False;

   end From_Ada_Boolean;
 
   function To_Ada_Boolean (
      RTEMS_Boolean : RTEMS.Boolean
   ) return Standard.Boolean is
   begin

      if RTEMS_Boolean = RTEMS.True then
         return Standard.True;
      end if;

      return Standard.False;

   end To_Ada_Boolean;

   function Milliseconds_To_Microseconds (
      Milliseconds : RTEMS.Unsigned32
   ) return RTEMS.Unsigned32 is
   begin

      return Milliseconds * 1000;

   end Milliseconds_To_Microseconds;

   function Microseconds_To_Ticks (
      Microseconds : RTEMS.Unsigned32
   ) return RTEMS.Interval is
      Microseconds_Per_Tick : RTEMS.Interval;
      pragma Import (C, Microseconds_Per_Tick, "_TOD_Microseconds_per_tick");
   begin

      return Microseconds / Microseconds_Per_Tick; 

   end Microseconds_To_Ticks;

   function Milliseconds_To_Ticks (
      Milliseconds : RTEMS.Unsigned32
   ) return RTEMS.Interval is
   begin

      return Microseconds_To_Ticks(Milliseconds_To_Microseconds(Milliseconds));

   end Milliseconds_To_Ticks;

   function Build_Name (
      C1 : in     Character;
      C2 : in     Character;
      C3 : in     Character;
      C4 : in     Character
   ) return RTEMS.Name is
      C1_Value : RTEMS.Unsigned32;
      C2_Value : RTEMS.Unsigned32;
      C3_Value : RTEMS.Unsigned32;
      C4_Value : RTEMS.Unsigned32;
   begin

     C1_Value := Character'Pos( C1 );
     C2_Value := Character'Pos( C2 );
     C3_Value := Character'Pos( C3 );
     C4_Value := Character'Pos( C4 );

     return Interfaces.Shift_Left( C1_Value, 24 ) or
            Interfaces.Shift_Left( C2_Value, 16 ) or
            Interfaces.Shift_Left( C3_Value, 8 )  or
            C4_Value;

   end Build_Name;

   procedure Name_To_Characters (
      Name : in     RTEMS.Name;
      C1   :    out Character;
      C2   :    out Character;
      C3   :    out Character;
      C4   :    out Character
   ) is
      C1_Value : RTEMS.Unsigned32;
      C2_Value : RTEMS.Unsigned32;
      C3_Value : RTEMS.Unsigned32;
      C4_Value : RTEMS.Unsigned32;
   begin

     C1_Value := Interfaces.Shift_Right( Name, 24 );
     C2_Value := Interfaces.Shift_Right( Name, 16 );
     C3_Value := Interfaces.Shift_Right( Name, 8 );
     C4_Value := Name;

     C1_Value := C1_Value and 16#00FF#;
     C2_Value := C2_Value and 16#00FF#;
     C3_Value := C3_Value and 16#00FF#;
     C4_Value := C4_Value and 16#00FF#;

     C1 := Character'Val( C1_Value );
     C2 := Character'Val( C2_Value );
     C3 := Character'Val( C3_Value );
     C4 := Character'Val( C4_Value );

   end Name_To_Characters;

   function Get_Node (
      ID : in     RTEMS.ID
   ) return RTEMS.Unsigned32 is
   begin

      -- May not be right
      return Interfaces.Shift_Right( ID, 16 );

   end Get_Node;

   function Get_Index (
      ID : in     RTEMS.ID
   ) return RTEMS.Unsigned32 is
   begin

      -- May not be right
      return ID and 16#FFFF#;

   end Get_Index;

   function Are_Statuses_Equal (
      Status  : in     RTEMS.Status_Codes;
      Desired : in     RTEMS.Status_Codes
   ) return Standard.Boolean is
   begin

      if Status = Desired then
         return Standard.True;
      end if;

      return Standard.False;

   end Are_Statuses_Equal;

   function Is_Status_Successful (
      Status  : in     RTEMS.Status_Codes
   ) return Standard.Boolean is
   begin

      if Status = RTEMS.Successful then
         return Standard.True;
      end if;

      return Standard.False;

   end Is_Status_Successful;

   function Subtract (
      Left   : in     RTEMS.Address;
      Right  : in     RTEMS.Address
   ) return RTEMS.Unsigned32 is
      function To_Unsigned32 is
         new Ada.Unchecked_Conversion (System.Address, RTEMS.Unsigned32);

   begin
      return To_Unsigned32(Left) - To_Unsigned32(Right);
   end Subtract;

   function Are_Equal (
      Left   : in     RTEMS.Address;
      Right  : in     RTEMS.Address
   ) return Standard.Boolean is
      function To_Unsigned32 is
         new Ada.Unchecked_Conversion (System.Address, RTEMS.Unsigned32);

   begin
      return (To_Unsigned32(Left) = To_Unsigned32(Right));
   end Are_Equal;

   --
   --
   --  RTEMS API
   --

   --
   --  Initialization Manager -- Shutdown Only
   --
   procedure Shutdown_Executive (
      Status           : in     RTEMS.Unsigned32
   ) is
      procedure Shutdown_Executive_Base (
         Status : RTEMS.Unsigned32
      );
      pragma Import (C, Shutdown_Executive_Base, "rtems_shutdown_executive");
   begin
      Shutdown_Executive_Base (Status);
   end Shutdown_Executive;
  

   --
   --  Task Manager
   --

   procedure Task_Create (
      Name             : in     RTEMS.Name;
      Initial_Priority : in     RTEMS.Task_Priority;
      Stack_Size       : in     RTEMS.Unsigned32;
      Initial_Modes    : in     RTEMS.Mode;
      Attribute_Set    : in     RTEMS.Attribute;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   ) is
      function Task_Create_Base (
         Name             : RTEMS.Name;
         Initial_Priority : RTEMS.Task_Priority;
         Stack_Size       : RTEMS.Unsigned32;
         Initial_Modes    : RTEMS.Mode;
         Attribute_Set    : RTEMS.Attribute;
         ID               : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Create_Base, "rtems_task_create");
      ID_Base : aliased RTEMS.ID;
   begin
      Result := Task_Create_Base (
        Name,
        Initial_Priority,
        Stack_Size,
        Initial_Modes,
        Attribute_Set,
        ID_Base'Unchecked_Access
      );
      ID := ID_Base;
   end Task_Create;

   procedure Task_Ident (
      Name             : in     RTEMS.Name;
      Node             : in     RTEMS.Node;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   ) is

      function Task_Ident_Base (
         Name             : RTEMS.Name;
         Node             : RTEMS.Node;
         ID               : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Ident_Base, "rtems_task_ident");
      ID_Base     : aliased RTEMS.ID;

   begin

      Result := Task_Ident_Base ( Name, Node, ID_Base'Unchecked_Access ); 
      ID := ID_Base;

   end Task_Ident;

   procedure Task_Start (
      ID          : in     RTEMS.ID;
      Entry_Point : in     RTEMS.Task_Entry;
      Argument    : in     RTEMS.Task_Argument;
      Result      :    out RTEMS.Status_Codes
   ) is
      function Task_Start_Base (
         ID          : RTEMS.ID;
         Entry_Point : RTEMS.Task_Entry;
         Argument    : RTEMS.Task_Argument
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Start_Base, "rtems_task_start");
   begin

      Result := Task_Start_Base ( ID, Entry_Point, Argument );

   end Task_Start;

   procedure Task_Restart (
      ID       : in     RTEMS.ID;
      Argument : in     RTEMS.Task_Argument;
      Result   :    out RTEMS.Status_Codes
   ) is
      function Task_Restart_Base (
         ID       : RTEMS.ID;
         Argument : RTEMS.Task_Argument
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Restart_Base, "rtems_task_restart");
   begin

      Result := Task_Restart_Base ( ID, Argument );
 
   end Task_Restart;
 
   procedure Task_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Task_Delete_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Delete_Base, "rtems_task_delete");
   begin

      Result := Task_Delete_Base ( ID );
 
   end Task_Delete;
 
   procedure Task_Suspend (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Task_Suspend_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Suspend_Base, "rtems_task_suspend");
   begin

      Result := Task_Suspend_Base ( ID );
 
   end Task_Suspend;
 
   procedure Task_Resume (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Task_Resume_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Resume_Base, "rtems_task_resume");
   begin

      Result := Task_Resume_Base ( ID );

   end Task_Resume;
 
   procedure Task_Is_Suspended (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Task_Is_Suspended_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Is_Suspended_Base, "rtems_task_is_suspended");
   begin

      Result := Task_Is_Suspended_Base ( ID );

   end Task_Is_Suspended;
 
   procedure Task_Set_Priority (
      ID           : in     RTEMS.ID;
      New_Priority : in     RTEMS.Task_Priority;
      Old_Priority :    out RTEMS.Task_Priority;
      Result       :    out RTEMS.Status_Codes
   ) is
      function Task_Set_Priority_Base (
         ID           : RTEMS.ID;
         New_Priority : RTEMS.Task_Priority;
         Old_Priority : access RTEMS.Task_Priority
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Set_Priority_Base, "rtems_task_set_priority");
      Old_Priority_Base : aliased RTEMS.Task_Priority;
   begin
 
      Result := Task_Set_Priority_Base (
         ID,
         New_Priority,
         Old_Priority_Base'Unchecked_Access
      );
      Old_Priority := Old_Priority_Base;

   end Task_Set_Priority;
 
   procedure Task_Mode (
      Mode_Set          : in     RTEMS.Mode;
      Mask              : in     RTEMS.Mode;
      Previous_Mode_Set :    out RTEMS.Mode;
      Result            :    out RTEMS.Status_Codes
   ) is
      function Task_Mode_Base (
         Mode_Set          : RTEMS.Mode;
         Mask              : RTEMS.Mode;
         Previous_Mode_Set : access RTEMS.Mode
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Mode_Base, "rtems_task_mode");
      Previous_Mode_Set_Base : aliased RTEMS.Mode;
   begin

      Result := Task_Mode_Base (
         Mode_Set,
         Mask,
         Previous_Mode_Set_Base'Unchecked_Access
      );
      Previous_Mode_Set := Previous_Mode_Set_Base;

   end Task_Mode;
 
   procedure Task_Get_Note (
      ID      : in     RTEMS.ID;
      Notepad : in     RTEMS.Notepad_Index;
      Note    :    out RTEMS.Unsigned32;
      Result  :    out RTEMS.Status_Codes
   ) is
      function Task_Get_Note_Base (
         ID      : RTEMS.ID;
         Notepad : RTEMS.Notepad_Index;
         Note    : access RTEMS.Unsigned32
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Get_Note_Base, "rtems_task_get_note");
      Note_Base : aliased RTEMS.Unsigned32;
   begin

      Result := Task_Get_Note_Base ( ID, Notepad, Note_Base'Unchecked_Access );
      Note := NOTE_Base;

   end Task_Get_Note;
 
   procedure Task_Set_Note (
      ID      : in     RTEMS.ID;
      Notepad : in     RTEMS.Notepad_Index;
      Note    : in     RTEMS.Unsigned32;
      Result  :    out RTEMS.Status_Codes
   ) is
      function Task_Set_Note_Base (
         ID      : RTEMS.ID;
         Notepad : RTEMS.Notepad_Index;
         Note    : RTEMS.Unsigned32
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Set_Note_Base, "rtems_task_set_note");
   begin

      Result := Task_Set_Note_Base ( ID, Notepad, Note );

   end Task_Set_Note;
 
   procedure Task_Variable_Add (
      ID            : in     RTEMS.ID;
      Task_Variable : in     RTEMS.Address;
      Dtor          : in     RTEMS.Task_Variable_Dtor;
      Result        :    out RTEMS.Status_Codes
   ) is
      function Task_Variable_Add_Base (
         ID            : RTEMS.ID;
         Task_Variable : RTEMS.Address;
         Dtor          : RTEMS.Task_Variable_Dtor
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Variable_Add_Base, "rtems_task_variable_add");
   begin

      Result := Task_Variable_Add_Base ( ID, Task_Variable, Dtor );

   end Task_Variable_Add;

   procedure Task_Variable_Get (
      ID                  : in     RTEMS.ID;
      Task_Variable       :    out RTEMS.Address;
      Task_Variable_Value :    out RTEMS.Address;
      Result              :    out RTEMS.Status_Codes
   ) is
      function Task_Variable_Get_Base (
         ID                  : RTEMS.ID;
         Task_Variable       : access RTEMS.Address;
         Task_Variable_Value : access RTEMS.Address
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Variable_Get_Base, "rtems_task_variable_get");
      Task_Variable_Base       : aliased RTEMS.Address;
      Task_Variable_Value_Base : aliased RTEMS.Address;
   begin

      Result := Task_Variable_Get_Base (
         ID,
         Task_Variable_Base'Unchecked_Access,
         Task_Variable_Value_Base'Unchecked_Access
      );
      Task_Variable := Task_Variable_Base;
      Task_Variable_Value := Task_Variable_Value_Base;

   end Task_Variable_Get;

   procedure Task_Variable_Delete (
      ID                  : in     RTEMS.ID;
      Task_Variable       :    out RTEMS.Address;
      Result              :    out RTEMS.Status_Codes
   ) is
      function Task_Variable_Delete_Base (
         ID                  : RTEMS.ID;
         Task_Variable       : access RTEMS.Address
      )  return RTEMS.Status_Codes;
      pragma Import (
         C, Task_Variable_Delete_Base, "rtems_task_variable_delete"
      );
      Task_Variable_Base : aliased RTEMS.Address;
   begin

      Result := Task_Variable_Delete_Base (
         ID, Task_Variable_Base'Unchecked_Access
      );
      Task_Variable := Task_Variable_Base;

   end Task_Variable_Delete;

   procedure Task_Wake_When (
      Time_Buffer : in     RTEMS.Time_Of_Day;
      Result      :    out RTEMS.Status_Codes
   ) is
      function Task_Wake_When_Base (
         Time_Buffer : RTEMS.Time_Of_Day
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Wake_When_Base, "rtems_task_wake_when");
   begin

      Result := Task_Wake_When_Base ( Time_Buffer );

   end Task_Wake_When;
 
   procedure Task_Wake_After (
      Ticks  : in     RTEMS.Interval;
      Result :    out RTEMS.Status_Codes
   ) is
      function Task_Wake_After_Base (
         Ticks : RTEMS.Interval
      )  return RTEMS.Status_Codes;
      pragma Import (C, Task_Wake_After_Base, "rtems_task_wake_after");
   begin

      Result := Task_Wake_After_Base ( Ticks );

   end Task_Wake_After;
 
   --
   -- Interrupt Manager
   --

   -- Interrupt_Disable is interfaced in the specification
   -- Interrupt_Enable is interfaced in the specification
   -- Interrupt_Flash is interfaced in the specification
   -- Interrupt_Is_In_Progress is interfaced in the specification

   --
   -- Clock Manager
   -- 
 
   procedure Clock_Set (
      Time_Buffer : in     RTEMS.Time_Of_Day;
      Result      :    out RTEMS.Status_Codes
   ) is
      function Clock_Set_Base (
         Time_Buffer : RTEMS.Time_Of_Day
      )  return RTEMS.Status_Codes;
      pragma Import (C, Clock_Set_Base, "rtems_clock_set");
   begin
 
      Result := Clock_Set_Base ( Time_Buffer );

   end Clock_Set;
 
   procedure Clock_Get (
      Option      : in     RTEMS.Clock_Get_Options;
      Time_Buffer : in     RTEMS.Address;
      Result      :    out RTEMS.Status_Codes
   ) is
      function Clock_Get_Base (
         Option      : RTEMS.Clock_Get_Options;
         Time_Buffer : RTEMS.Address
      )  return RTEMS.Status_Codes;
      pragma Import (C, Clock_Get_Base, "rtems_clock_get");
   begin

      Result := Clock_Get_Base ( Option, Time_Buffer );

   end Clock_Get;
 
   procedure Clock_Get_Uptime (
      Uptime :    out RTEMS.Timespec;
      Result :    out RTEMS.Status_Codes
   ) is
      function Clock_Get_Uptime_Base (
         Uptime : access RTEMS.Timespec
      )  return RTEMS.Status_Codes;
      pragma Import (C, Clock_Get_Uptime_Base, "rtems_clock_get_uptime");
      Uptime_Base : aliased RTEMS.Timespec;
   begin

      Result := Clock_Get_Uptime_Base (
         Uptime_Base'Unchecked_Access
      );
      Uptime := Uptime_Base;

   end Clock_Get_Uptime;
 
   procedure Clock_Tick (
      Result :    out RTEMS.Status_Codes
   ) is
      function Clock_Tick_Base return RTEMS.Status_Codes;
      pragma Import (C, Clock_Tick_Base, "rtems_clock_tick");
   begin

      Result := Clock_Tick_Base;

   end Clock_Tick;

   --
   -- Extension Manager
   --
 
   procedure Extension_Create (
      Name   : in     RTEMS.Name;
      Table  : in     RTEMS.Extensions_Table_Pointer;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Extension_Create_Base (
         Name   : RTEMS.Name;
         Table  : RTEMS.Extensions_Table_Pointer;
         ID     : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Extension_Create_Base, "rtems_extension_create");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Extension_Create_Base ( Name, Table, ID_Base'Unchecked_Access );
      ID := ID_Base;

   end Extension_Create;
 
   procedure Extension_Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Extension_Ident_Base (
         Name   : RTEMS.Name;
         ID     : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Extension_Ident_Base, "rtems_extension_ident");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Extension_Ident_Base ( Name, ID_Base'Unchecked_Access ); 
      ID := ID_Base;

   end Extension_Ident;
 
   procedure Extension_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Extension_Delete_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Extension_Delete_Base, "rtems_extension_delete");
   begin
 
      Result := Extension_Delete_Base ( ID );

   end Extension_Delete;
 
   --
   -- Timer Manager
   --
 
   procedure Timer_Create (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Timer_Create_Base (
         Name   : RTEMS.Name;
         ID     : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Timer_Create_Base, "rtems_timer_create");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Timer_Create_Base ( Name, ID_Base'Unchecked_Access );
      ID := ID_Base;

   end Timer_Create;
 
   procedure Timer_Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Timer_Ident_Base (
         Name   : RTEMS.Name;
         ID     : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Timer_Ident_Base, "rtems_timer_ident");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Timer_Ident_Base ( Name, ID_Base'Unchecked_Access );
      ID := ID_Base;

   end Timer_Ident;
 
   procedure Timer_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Timer_Delete_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Timer_Delete_Base, "rtems_timer_delete");
   begin
 
      Result := Timer_Delete_Base ( ID );

   end Timer_Delete;
 
   procedure Timer_Fire_After (
      ID        : in     RTEMS.ID;
      Ticks     : in     RTEMS.Interval;
      Routine   : in     RTEMS.Timer_Service_Routine;
      User_Data : in     RTEMS.Address;
      Result    :    out RTEMS.Status_Codes
   ) is
      function Timer_Fire_After_Base (
         ID        : RTEMS.ID;
         Ticks     : RTEMS.Interval;
         Routine   : RTEMS.Timer_Service_Routine;
         User_Data : RTEMS.Address
      )  return RTEMS.Status_Codes;
      pragma Import (C, Timer_Fire_After_Base, "rtems_timer_fire_after");
   begin
 
      Result := Timer_Fire_After_Base ( ID, Ticks, Routine, User_Data );

   end Timer_Fire_After;
 
   procedure Timer_Server_Fire_After (
      ID        : in     RTEMS.ID;
      Ticks     : in     RTEMS.Interval;
      Routine   : in     RTEMS.Timer_Service_Routine;
      User_Data : in     RTEMS.Address;
      Result    :    out RTEMS.Status_Codes
   ) is
      function Timer_Server_Fire_After_Base (
         ID        : RTEMS.ID;
         Ticks     : RTEMS.Interval;
         Routine   : RTEMS.Timer_Service_Routine;
         User_Data : RTEMS.Address
      )  return RTEMS.Status_Codes;
      pragma Import (
        C,
        Timer_Server_Fire_After_Base,
        "rtems_timer_server_fire_after"
      );
   begin
 
      Result := Timer_Server_Fire_After_Base ( ID, Ticks, Routine, User_Data ); 

   end Timer_Server_Fire_After;
 
   procedure Timer_Fire_When (
      ID        : in     RTEMS.ID;
      Wall_Time : in     RTEMS.Time_Of_Day;
      Routine   : in     RTEMS.Timer_Service_Routine;
      User_Data : in     RTEMS.Address;
      Result    :    out RTEMS.Status_Codes
   ) is
      function Timer_Fire_When_Base (
         ID        : RTEMS.ID;
         Wall_Time : RTEMS.Time_Of_Day;
         Routine   : RTEMS.Timer_Service_Routine;
         User_Data : RTEMS.Address
      )  return RTEMS.Status_Codes;
      pragma Import (C, Timer_Fire_When_Base, "rtems_timer_fire_when");
   begin
 
      Result := Timer_Fire_When_Base ( ID, Wall_Time, Routine, User_Data );

   end Timer_Fire_When;
 
   procedure Timer_Server_Fire_When (
      ID        : in     RTEMS.ID;
      Wall_Time : in     RTEMS.Time_Of_Day;
      Routine   : in     RTEMS.Timer_Service_Routine;
      User_Data : in     RTEMS.Address;
      Result    :    out RTEMS.Status_Codes
   ) is
      function Timer_Server_Fire_When_Base (
         ID        : RTEMS.ID;
         Wall_Time : RTEMS.Time_Of_Day;
         Routine   : RTEMS.Timer_Service_Routine;
         User_Data : RTEMS.Address
      )  return RTEMS.Status_Codes;
      pragma Import (
         C,
         Timer_Server_Fire_When_Base,
         "rtems_timer_server_fire_when"
      );
   begin
 
      Result :=
         Timer_Server_Fire_When_Base ( ID, Wall_Time, Routine, User_Data ); 
   end Timer_Server_Fire_When;
 
   procedure Timer_Reset (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Timer_Reset_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Timer_Reset_Base, "rtems_timer_reset");
   begin
 
      Result := Timer_Reset_Base ( ID );

   end Timer_Reset;
 
   procedure Timer_Cancel (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Timer_Cancel_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Timer_Cancel_Base, "rtems_timer_cancel");
   begin
 
      Result := Timer_Cancel_Base ( ID );

   end Timer_Cancel;
 
   procedure Timer_Initiate_Server (
      Server_Priority : in     RTEMS.Task_Priority;
      Stack_Size      : in     RTEMS.Unsigned32;
      Attribute_Set   : in     RTEMS.Attribute;
      Result          :    out RTEMS.Status_Codes
   ) is
      function Timer_Initiate_Server_Base (
         Server_Priority : RTEMS.Task_Priority;
         Stack_Size      : RTEMS.Unsigned32;
         Attribute_Set   : RTEMS.Attribute
      )  return RTEMS.Status_Codes;
      pragma Import (
         C,
         Timer_Initiate_Server_Base,
         "rtems_timer_initiate_server"
      );
   begin
      Result := Timer_Initiate_Server_Base (
         Server_Priority,
         Stack_Size,
         Attribute_Set
      );
   end Timer_Initiate_Server;

   --
   -- Semaphore Manager
   --
 
   procedure Semaphore_Create (
      Name             : in     RTEMS.Name;
      Count            : in     RTEMS.Unsigned32;
      Attribute_Set    : in     RTEMS.Attribute;
      Priority_Ceiling : in     RTEMS.Task_Priority;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   ) is
      function Semaphore_Create_Base (
         Name             : RTEMS.Name;
         Count            : RTEMS.Unsigned32;
         Attribute_Set    : RTEMS.Attribute;
         Priority_Ceiling : RTEMS.Task_Priority;
         ID               : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Semaphore_Create_Base, "rtems_semaphore_create");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Semaphore_Create_Base (
         Name,
         Count,
         Attribute_Set,
         Priority_Ceiling,
         ID_Base'Unchecked_Access
      );
      ID := ID_Base;

   end Semaphore_Create;
 
   procedure Semaphore_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Semaphore_Delete_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Semaphore_Delete_Base, "rtems_semaphore_delete");
   begin
 
      Result := Semaphore_Delete_Base ( ID );

   end Semaphore_Delete;
 
   procedure Semaphore_Ident (
      Name   : in     RTEMS.Name;
      Node   : in     RTEMS.Unsigned32;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Semaphore_Ident_Base (
         Name : RTEMS.Name;
         Node : RTEMS.Unsigned32;
         ID   : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Semaphore_Ident_Base, "rtems_semaphore_ident");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Semaphore_Ident_Base ( Name, Node, ID_Base'Unchecked_Access );
      ID := ID_Base;

   end Semaphore_Ident;
 
   procedure Semaphore_Obtain (
      ID         : in     RTEMS.ID;
      Option_Set : in     RTEMS.Option;
      Timeout    : in     RTEMS.Interval;
      Result     :    out RTEMS.Status_Codes
   ) is
      function Semaphore_Obtain_Base (
         ID         : RTEMS.ID;
         Option_Set : RTEMS.Option;
         Timeout    : RTEMS.Interval
      )  return RTEMS.Status_Codes;
      pragma Import (C, Semaphore_Obtain_Base, "rtems_semaphore_obtain");
   begin
 
      Result := Semaphore_Obtain_Base ( ID, Option_Set, Timeout );

   end Semaphore_Obtain;
 
   procedure Semaphore_Release (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Semaphore_Release_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Semaphore_Release_Base, "rtems_semaphore_release");
   begin
 
      Result := Semaphore_Release_Base ( ID );

   end Semaphore_Release;
 
   procedure Semaphore_Flush (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Semaphore_Flush_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Semaphore_Flush_Base, "rtems_semaphore_flush");
   begin
 
      Result := Semaphore_Flush_Base ( ID );

   end Semaphore_Flush;
 
   --
   -- Message Queue Manager
   --
 
   procedure Message_Queue_Create (
      Name             : in     RTEMS.Name;
      Count            : in     RTEMS.Unsigned32;
      Max_Message_Size : in     RTEMS.Unsigned32;
      Attribute_Set    : in     RTEMS.Attribute;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   ) is
      --  XXX broken
      function Message_Queue_Create_Base (
         Name             : RTEMS.Name;
         Count            : RTEMS.Unsigned32;
         Max_Message_Size : RTEMS.Unsigned32;
         Attribute_Set    : RTEMS.Attribute;
         ID               : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C,
        Message_Queue_Create_Base, "rtems_message_queue_create");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Message_Queue_Create_Base (
         Name,
         Count,
         Max_Message_Size,
         Attribute_Set,
         ID_Base'Unchecked_Access
      );
      ID := ID_Base;

   end Message_Queue_Create;
 
   procedure Message_Queue_Ident (
      Name   : in     RTEMS.Name;
      Node   : in     RTEMS.Unsigned32;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Message_Queue_Ident_Base (
         Name : RTEMS.Name;
         Node : RTEMS.Unsigned32;
         ID   : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Message_Queue_Ident_Base, "rtems_message_queue_ident");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result :=
         Message_Queue_Ident_Base ( Name, Node, ID_Base'Unchecked_Access );
      ID := ID_Base;

   end Message_Queue_Ident;
 
   procedure Message_Queue_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Message_Queue_Delete_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (
         C, Message_Queue_Delete_Base, "rtems_message_queue_delete");
   begin
 
      Result := Message_Queue_Delete_Base ( ID );

   end Message_Queue_Delete;
 
   procedure Message_Queue_Send (
      ID     : in     RTEMS.ID;
      Buffer : in     RTEMS.Address;
      Size   : in     RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   ) is
      function Message_Queue_Send_Base (
         ID     : RTEMS.ID;
         Buffer : RTEMS.Address;
         Size   : RTEMS.Unsigned32
      )  return RTEMS.Status_Codes;
      pragma Import (C, Message_Queue_Send_Base, "rtems_message_queue_send");
   begin
 
      Result := Message_Queue_Send_Base ( ID, Buffer, Size );

   end Message_Queue_Send;
 
   procedure Message_Queue_Urgent (
      ID     : in     RTEMS.ID;
      Buffer : in     RTEMS.Address;
      Size   : in     RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   ) is
      function Message_Queue_Urgent_Base (
         ID     : RTEMS.ID;
         Buffer : RTEMS.Address;
         Size   : RTEMS.Unsigned32
      )  return RTEMS.Status_Codes;
      pragma Import (C, Message_Queue_Urgent_Base,
         "rtems_message_queue_urgent");
   begin
 
      Result := Message_Queue_Urgent_Base ( ID, Buffer, Size );

   end Message_Queue_Urgent;
 
   procedure Message_Queue_Broadcast (
      ID     : in     RTEMS.ID;
      Buffer : in     RTEMS.Address;
      Size   : in     RTEMS.Unsigned32;
      Count  :    out RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   ) is
      function Message_Queue_Broadcast_Base (
         ID     : RTEMS.ID;
         Buffer : RTEMS.Address;
         Size   : RTEMS.Unsigned32;
         Count  : access RTEMS.Unsigned32 
      )  return RTEMS.Status_Codes;
      pragma Import (C, Message_Queue_Broadcast_Base,
         "rtems_message_queue_broadcast");
      Count_Base : aliased RTEMS.Unsigned32;
   begin
 
      Result := Message_Queue_Broadcast_Base ( 
         ID, 
         Buffer, 
         Size,
         Count_Base'Unchecked_Access
      );
      Count := Count_Base;

   end Message_Queue_Broadcast;
 
   procedure Message_Queue_Receive (
      ID         : in     RTEMS.ID;
      Buffer     : in     RTEMS.Address;
      Option_Set : in     RTEMS.Option;
      Timeout    : in     RTEMS.Interval;
      Size       :    out RTEMS.Unsigned32;
      Result     :    out RTEMS.Status_Codes
   ) is
      function Message_Queue_Receive_Base (
         ID         : RTEMS.ID;
         Buffer     : RTEMS.Address;
         Size       : access RTEMS.Unsigned32;
         Option_Set : RTEMS.Option;
         Timeout    : RTEMS.Interval
      )  return RTEMS.Status_Codes;
      pragma Import (C, Message_Queue_Receive_Base,
         "rtems_message_queue_receive");
      Size_Base : aliased RTEMS.Unsigned32;
   begin
 
      Result := Message_Queue_Receive_Base ( 
         ID,
         Buffer, 
         Size_Base'Unchecked_Access,
         Option_Set, 
         Timeout 
      );
      Size := Size_Base;

   end Message_Queue_Receive;
 
   procedure Message_Queue_Get_Number_Pending (
      ID     : in     RTEMS.ID;
      Count  :    out RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   ) is
      function Message_Queue_Get_Number_Pending_Base (
         ID    : RTEMS.ID;
         Count : access RTEMS.Unsigned32
      )  return RTEMS.Status_Codes;
      pragma Import (
         C,
         Message_Queue_Get_Number_Pending_Base,
         "rtems_message_queue_get_number_pending"
      );
      COUNT_Base : aliased RTEMS.Unsigned32;
   begin
 
      Result := Message_Queue_Get_Number_Pending_Base (
         ID, COUNT_Base'Unchecked_Access
      );
      Count := COUNT_Base;

   end Message_Queue_Get_Number_Pending;
 
   procedure Message_Queue_Flush (
      ID     : in     RTEMS.ID;
      Count  :    out RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   ) is
      function Message_Queue_Flush_Base (
         ID    : RTEMS.ID;
         Count : access RTEMS.Unsigned32
      )  return RTEMS.Status_Codes;
      pragma Import (C, Message_Queue_Flush_Base, "rtems_message_queue_flush");
      COUNT_Base : aliased RTEMS.Unsigned32;
   begin
 
      Result := Message_Queue_Flush_Base ( ID, COUNT_Base'Unchecked_Access );
      Count := COUNT_Base;

   end Message_Queue_Flush;
 
   --
   -- Event Manager
   --

   procedure Event_Send (
      ID       : in     RTEMS.ID;
      Event_In : in     RTEMS.Event_Set;
      Result   :    out RTEMS.Status_Codes
   ) is
      function Event_Send_Base (
         ID       : RTEMS.ID;
         Event_In : RTEMS.Event_Set
      )  return RTEMS.Status_Codes;
      pragma Import (C, Event_Send_Base, "rtems_event_send");
   begin

      Result := Event_Send_Base ( ID, Event_In );

   end Event_Send;

   procedure Event_Receive (
      Event_In   : in     RTEMS.Event_Set;
      Option_Set : in     RTEMS.Option;
      Ticks      : in     RTEMS.Interval;
      Event_Out  :    out RTEMS.Event_Set;
      Result     :    out RTEMS.Status_Codes
   ) is
      function Event_Receive_Base (
         Event_In   : RTEMS.Event_Set;
         Option_Set : RTEMS.Option;
         Ticks      : RTEMS.Interval;
         Event_Out  : access RTEMS.Event_Set
      )  return RTEMS.Status_Codes;
      pragma Import (C, Event_Receive_Base, "rtems_event_receive");
      Event_Out_Base : aliased RTEMS.Event_Set;
   begin

      Result := Event_Receive_Base (
         Event_In,
         Option_Set,
         Ticks,
         Event_Out_Base'Access
      ); 
      Event_Out := Event_Out_Base;

   end Event_Receive;

   --
   -- Signal Manager
   --
 
   procedure Signal_Catch (
      ASR_Handler : in     RTEMS.ASR_Handler;
      Mode_Set    : in     RTEMS.Mode;
      Result      :    out RTEMS.Status_Codes
   ) is
      function Signal_Catch_Base (
         ASR_Handler : RTEMS.ASR_Handler;
         Mode_Set    : RTEMS.Mode
      )  return RTEMS.Status_Codes;
      pragma Import (C, Signal_Catch_Base, "rtems_signal_catch");
   begin

      Result := Signal_Catch_Base ( ASR_Handler, Mode_Set );

   end Signal_Catch;
 
   procedure Signal_Send (
      ID         : in     RTEMS.ID;
      Signal_Set : in     RTEMS.Signal_Set;
      Result     :    out RTEMS.Status_Codes
   ) is
      function Signal_Send_Base (
         ID         : RTEMS.ID;
         Signal_Set : RTEMS.Signal_Set
      )  return RTEMS.Status_Codes;
      pragma Import (C, Signal_Send_Base, "rtems_signal_send");
   begin
 
      Result := Signal_Send_Base ( ID, Signal_Set );

   end Signal_Send;
 
 
   --
   -- Partition Manager
   --
 
   procedure Partition_Create (
      Name             : in     RTEMS.Name;
      Starting_Address : in     RTEMS.Address;
      Length           : in     RTEMS.Unsigned32;
      Buffer_Size      : in     RTEMS.Unsigned32;
      Attribute_Set    : in     RTEMS.Attribute;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   ) is
      function Partition_Create_Base (
         Name             : RTEMS.Name;
         Starting_Address : RTEMS.Address;
         Length           : RTEMS.Unsigned32;
         Buffer_Size      : RTEMS.Unsigned32;
         Attribute_Set    : RTEMS.Attribute;
         ID               : access RTEMS.Event_Set
      )  return RTEMS.Status_Codes;
      pragma Import (C, Partition_Create_Base, "rtems_partition_create");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Partition_Create_Base (
         Name,
         Starting_Address,
         Length,
         Buffer_Size,
         Attribute_Set,
         ID_Base'Unchecked_Access
      );
      ID := ID_Base;
 
   end Partition_Create;
 
   procedure Partition_Ident (
      Name   : in     RTEMS.Name;
      Node   : in     RTEMS.Unsigned32;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Partition_Ident_Base (
         Name : RTEMS.Name;
         Node : RTEMS.Unsigned32;
         ID   : access RTEMS.Event_Set
      )  return RTEMS.Status_Codes;
      pragma Import (C, Partition_Ident_Base, "rtems_partition_ident");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Partition_Ident_Base ( Name, Node, ID_Base'Unchecked_Access );
      ID := ID_Base;

   end Partition_Ident;
 
   procedure Partition_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Partition_Delete_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Partition_Delete_Base, "rtems_partition_delete");
   begin
 
      Result := Partition_Delete_Base ( ID );

   end Partition_Delete;
 
   procedure Partition_Get_Buffer (
      ID     : in     RTEMS.ID;
      Buffer :    out RTEMS.Address;
      Result :    out RTEMS.Status_Codes
   ) is
      function Partition_Get_Buffer_Base (
         ID     : RTEMS.ID;
         Buffer : access RTEMS.Address
      )  return RTEMS.Status_Codes;
      pragma Import (C, Partition_Get_Buffer_Base,
         "rtems_partition_get_buffer");
      Buffer_Base : aliased RTEMS.Address;
   begin
 
      Result := Partition_Get_Buffer_Base ( ID, Buffer_Base'Unchecked_Access );
      Buffer := Buffer_Base;

   end Partition_Get_Buffer;
 
   procedure Partition_Return_Buffer (
      ID     : in     RTEMS.ID;
      Buffer : in     RTEMS.Address;
      Result :    out RTEMS.Status_Codes
   ) is
      function Partition_Return_Buffer_Base (
         ID     : RTEMS.Name;
         Buffer : RTEMS.Address
      )  return RTEMS.Status_Codes;
      pragma Import (C, Partition_Return_Buffer_Base,
         "rtems_partition_return_buffer");
   begin
 
      Result := Partition_Return_Buffer_Base ( ID, Buffer );

   end Partition_Return_Buffer;

   --
   -- Region Manager
   --
 
   procedure Region_Create (
      Name             : in     RTEMS.Name;
      Starting_Address : in     RTEMS.Address;
      Length           : in     RTEMS.Unsigned32;
      Page_Size        : in     RTEMS.Unsigned32;
      Attribute_Set    : in     RTEMS.Attribute;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   ) is
      function Region_Create_Base (
         Name             : RTEMS.Name;
         Starting_Address : RTEMS.Address;
         Length           : RTEMS.Unsigned32;
         Page_Size        : RTEMS.Unsigned32;
         Attribute_Set    : RTEMS.Attribute;
         ID               : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Region_Create_Base, "rtems_region_create");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Region_Create_Base (
         Name,
         Starting_Address,
         Length,
         Page_Size,
         Attribute_Set,
         ID_Base'Unchecked_Access
      );
      ID := ID_Base;

   end Region_Create;
 
   procedure Region_Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Region_Ident_Base (
         Name   : RTEMS.Name;
         ID     : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Region_Ident_Base, "rtems_region_ident");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Region_Ident_Base ( Name, ID_Base'Unchecked_Access );
      ID := ID_Base;

   end Region_Ident;
 
   procedure Region_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Region_Delete_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Region_Delete_Base, "rtems_region_delete");
   begin
 
      Result := Region_Delete_Base ( ID );

   end Region_Delete;
 
   procedure Region_Extend (
      ID               : in     RTEMS.ID;
      Starting_Address : in     RTEMS.Address;
      Length           : in     RTEMS.Unsigned32;
      Result           :    out RTEMS.Status_Codes
   ) is
      function Region_Extend_Base (
         ID               : RTEMS.ID;
         Starting_Address : RTEMS.Address;
         Length           : RTEMS.Unsigned32
      )  return RTEMS.Status_Codes;
      pragma Import (C, Region_Extend_Base, "rtems_region_extend");
   begin
 
      Result := Region_Extend_Base ( ID, Starting_Address, Length );

   end Region_Extend;
 
   procedure Region_Get_Segment (
      ID         : in     RTEMS.ID;
      Size       : in     RTEMS.Unsigned32;
      Option_Set : in     RTEMS.Option;
      Timeout    : in     RTEMS.Interval;
      Segment    :    out RTEMS.Address;
      Result     :    out RTEMS.Status_Codes
   ) is
      function Region_Get_Segment_Base (
         ID         : RTEMS.ID;
         Size       : RTEMS.Unsigned32;
         Option_Set : RTEMS.Option;
         Timeout    : RTEMS.Interval;
         Segment    : access RTEMS.Address
      )  return RTEMS.Status_Codes;
      pragma Import (C, Region_Get_Segment_Base, "rtems_region_get_segment");
      Segment_Base : aliased RTEMS.Address;
   begin
 
      Result := Region_Get_Segment_Base (
         ID,
         Size,
         Option_Set,
         Timeout,
         Segment_Base'Unchecked_Access
      );
      Segment := SEGMENT_Base;

   end Region_Get_Segment;
 
   procedure Region_Get_Segment_Size (
      ID      : in     RTEMS.ID;
      Segment : in     RTEMS.Address;
      Size    :    out RTEMS.Unsigned32;
      Result  :    out RTEMS.Status_Codes
   ) is
      function Region_Get_Segment_Size_Base (
         ID      : RTEMS.ID;
         Segment : RTEMS.Address;
         Size    : access RTEMS.Unsigned32
      )  return RTEMS.Status_Codes;
      pragma Import (C, Region_Get_Segment_Size_Base,
         "rtems_region_get_segment_size");
      Size_Base : aliased RTEMS.Unsigned32;
   begin
 
      Result := Region_Get_Segment_Size_Base (
         ID,
         Segment,
         Size_Base'Unchecked_Access
      );
      Size := SIZE_Base;

   end Region_Get_Segment_Size;
 
   procedure Region_Return_Segment (
      ID      : in     RTEMS.ID;
      Segment : in     RTEMS.Address;
      Result  :    out RTEMS.Status_Codes
   ) is
      function Region_Return_Segment_Base (
         ID      : RTEMS.ID;
         Segment : RTEMS.Address
      )  return RTEMS.Status_Codes;
      pragma Import (C, Region_Return_Segment_Base,
         "rtems_region_return_segment");
   begin
 
      Result := Region_Return_Segment_Base ( ID, Segment );

   end Region_Return_Segment;
 
   procedure Region_Resize_Segment (
      ID         : in     RTEMS.ID;
      Segment    : in     RTEMS.Address;
      Size       : in     RTEMS.Unsigned32;
      Old_Size   :    out RTEMS.Unsigned32;
      Result     :    out RTEMS.Status_Codes
   ) is
      function Region_Resize_Segment_Base (
         ID       : RTEMS.ID;
         Segment  : RTEMS.Address;
         Size     : RTEMS.Unsigned32;
         Old_Size : access RTEMS.Unsigned32
      )  return RTEMS.Status_Codes;
      pragma Import (C, Region_Resize_Segment_Base,
         "rtems_region_resize_segment");
      Old_Size_Base : aliased RTEMS.Unsigned32;
   begin
 
      Result := Region_Resize_Segment_Base (
         ID,
         Segment,
         Size,
         Old_Size_Base'Unchecked_Access
      );
      Old_Size := Old_Size_Base;

   end Region_Resize_Segment;

   --
   -- Dual Ported Memory Manager
   --
 
   procedure Port_Create (
      Name           : in     RTEMS.Name;
      Internal_Start : in     RTEMS.Address;
      External_Start : in     RTEMS.Address;
      Length         : in     RTEMS.Unsigned32;
      ID             :    out RTEMS.ID;
      Result         :    out RTEMS.Status_Codes
   ) is
      function Port_Create_Base (
         Name           : RTEMS.Name;
         Internal_Start : RTEMS.Address;
         External_Start : RTEMS.Address;
         Length         : RTEMS.Unsigned32;
         ID             : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Port_Create_Base, "rtems_port_create");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Port_Create_Base (
         Name,
         Internal_Start,
         External_Start,
         Length,
         ID_Base'Unchecked_Access
      );
      ID := ID_Base;

   end Port_Create;
 
   procedure Port_Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Port_Ident_Base (
         Name : RTEMS.Name;
         ID   : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Port_Ident_Base, "rtems_port_ident");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Port_Ident_Base ( Name, ID_Base'Unchecked_Access );
      ID := ID_Base;

   end Port_Ident;
 
   procedure Port_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Port_Delete_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Port_Delete_Base, "rtems_port_delete");
   begin
 
      Result := Port_Delete_Base ( ID );

   end Port_Delete;
 
   procedure Port_External_To_Internal (
      ID       : in     RTEMS.ID;
      External : in     RTEMS.Address;
      Internal :    out RTEMS.Address;
      Result   :    out RTEMS.Status_Codes
   ) is
      function Port_External_To_Internal_Base (
         ID       : RTEMS.ID;
         External : RTEMS.Address;
         Internal : access RTEMS.Address
      )  return RTEMS.Status_Codes;
      pragma Import (C, Port_External_To_Internal_Base,
         "rtems_port_external_to_internal");
      Internal_Base : aliased RTEMS.Address;
   begin
 
      Result := Port_External_To_Internal_Base (
         ID,
         External,
         Internal_Base'Unchecked_Access
      );
      Internal := INTERNAL_Base;

   end Port_External_To_Internal;
 
   procedure Port_Internal_To_External (
      ID       : in     RTEMS.ID;
      Internal : in     RTEMS.Address;
      External :    out RTEMS.Address;
      Result   :    out RTEMS.Status_Codes
   ) is
      function Port_Internal_To_External_Base (
         ID       : RTEMS.ID;
         Internal : RTEMS.Address;
         External : access RTEMS.Address
      )  return RTEMS.Status_Codes;
      pragma Import (C, Port_Internal_To_External_Base,
         "rtems_port_internal_to_external");
      External_Base : aliased RTEMS.Address;
   begin
 
      Result := Port_Internal_To_External_Base (
         ID,
         Internal,
         External_Base'Unchecked_Access
      );
      External := EXTERNAL_Base;

   end Port_Internal_To_External;
 
 
   --
   -- Fatal Error Manager
   --
 
   procedure Fatal_Error_Occurred (
      The_Error : in     RTEMS.Unsigned32
   ) is
      procedure Fatal_Error_Occurred_Base (
         The_Error : RTEMS.Unsigned32
      );
   pragma Import (C, Fatal_Error_Occurred_Base, "rtems_fatal_error_occurred");
   begin
 
      Fatal_Error_Occurred_Base ( The_Error );

   end Fatal_Error_Occurred;


   --
   -- Rate Monotonic Manager
   --
 
   procedure Rate_Monotonic_Create (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Rate_Monotonic_Create_Base (
         Name   : RTEMS.Name;
         ID     : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Rate_Monotonic_Create_Base, "rtems_rate_monotonic_create");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Rate_Monotonic_Create_Base ( Name, ID_Base'Unchecked_Access );
      ID := ID_Base;

   end Rate_Monotonic_Create;
 
   procedure Rate_Monotonic_Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Rate_Monotonic_Ident_Base (
         Name   : RTEMS.Name;
         ID     : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Rate_Monotonic_Ident_Base, "rtems_rate_monotonic_ident");
      ID_Base : aliased RTEMS.ID;
   begin
 
      Result := Rate_Monotonic_Ident_Base ( Name, ID_Base'Unchecked_Access );
 
      ID := ID_Base;

   end Rate_Monotonic_Ident;
 
   procedure Rate_Monotonic_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Rate_Monotonic_Delete_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Rate_Monotonic_Delete_Base,
         "rtems_rate_monotonic_delete");
   begin
 
      Result := Rate_Monotonic_Delete_Base ( ID );

   end Rate_Monotonic_Delete;
 
   procedure Rate_Monotonic_Cancel (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Rate_Monotonic_Cancel_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Rate_Monotonic_Cancel_Base,
         "rtems_rate_monotonic_cancel");
   begin
 
      Result := Rate_Monotonic_Cancel_Base ( ID );

   end Rate_Monotonic_Cancel;
 
   procedure Rate_Monotonic_Period (
      ID      : in     RTEMS.ID;
      Length  : in     RTEMS.Interval;
      Result  :    out RTEMS.Status_Codes
   ) is
      function Rate_Monotonic_Period_Base (
         ID     : RTEMS.ID;
         Length : RTEMS.Interval
      )  return RTEMS.Status_Codes;
      pragma Import (C, Rate_Monotonic_Period_Base,
         "rtems_rate_monotonic_period");
   begin
 
      Result := Rate_Monotonic_Period_Base ( ID, Length );

   end Rate_Monotonic_Period;
 
   procedure Rate_Monotonic_Get_Status (
      ID      : in     RTEMS.ID;
      Status  :    out RTEMS.Rate_Monotonic_Period_Status;
      Result  :    out RTEMS.Status_Codes
   ) is
      function Rate_Monotonic_Get_Status_Base (
         ID      : RTEMS.ID;
         Status  : access RTEMS.Rate_Monotonic_Period_Status
      )  return RTEMS.Status_Codes;
      pragma Import (C, Rate_Monotonic_Get_Status_Base,
         "rtems_rate_monotonic_get_status");

      Status_Base : aliased RTEMS.Rate_Monotonic_Period_Status;
   begin

      Result := Rate_Monotonic_Get_Status_Base (
         ID,
         Status_Base'Unchecked_Access
      );

      Status := Status_Base;


   end Rate_Monotonic_Get_Status;

   procedure Rate_Monotonic_Reset_Statistics (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Rate_Monotonic_Reset_Statistics_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Rate_Monotonic_Reset_Statistics_Base,
         "rtems_rate_monotonic_reset_statistics");
   begin

      Result := Rate_Monotonic_Reset_Statistics_Base ( ID );

   end Rate_Monotonic_Reset_Statistics;


   --
   -- Barrier Manager
   --

   procedure Barrier_Create (
      Name            : in     RTEMS.Name;
      Attribute_Set   : in     RTEMS.Attribute;
      Maximum_Waiters : in     RTEMS.Unsigned32;
      ID              :    out RTEMS.ID;
      Result          :    out RTEMS.Status_Codes
   ) is
      function Barrier_Create_Base (
         Name            : RTEMS.Name;
         Attribute_Set   : RTEMS.Attribute;
         Maximum_Waiters : RTEMS.Unsigned32;
         ID              : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Barrier_Create_Base, "rtems_barrier_create");
      ID_Base : aliased RTEMS.ID;
   begin

      Result := Barrier_Create_Base (
         Name,
         Attribute_Set,
         Maximum_Waiters,
         ID_Base'Unchecked_Access
      );
      ID := ID_Base;

   end Barrier_Create;

   procedure Barrier_Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Barrier_Ident_Base (
         Name : RTEMS.Name;
         ID   : access RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Barrier_Ident_Base, "rtems_barrier_ident");
      ID_Base : aliased RTEMS.ID;
   begin

      Result := Barrier_Ident_Base ( Name, ID_Base'Unchecked_Access );
      ID := ID_Base;

   end Barrier_Ident;

   procedure Barrier_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   ) is
      function Barrier_Delete_Base (
         ID : RTEMS.ID
      )  return RTEMS.Status_Codes;
      pragma Import (C, Barrier_Delete_Base, "rtems_barrier_delete");
   begin

      Result := Barrier_Delete_Base ( ID );

   end Barrier_Delete;

   procedure Barrier_Wait (
      ID         : in     RTEMS.ID;
      Timeout    : in     RTEMS.Interval;
      Result     :    out RTEMS.Status_Codes
   ) is
      function Barrier_Wait_Base (
         ID         : RTEMS.ID;
         Timeout    : RTEMS.Interval
      )  return RTEMS.Status_Codes;
      pragma Import (C, Barrier_Wait_Base, "rtems_barrier_wait");
   begin

      Result := Barrier_Wait_Base ( ID, Timeout );

   end Barrier_Wait;

   procedure Barrier_Release (
      ID       : in     RTEMS.ID;
      Released :    out RTEMS.Unsigned32;
      Result   :    out RTEMS.Status_Codes
   ) is
      function Barrier_Release_Base (
         ID       : RTEMS.ID
         Released : access RTEMS.Unsigned32
      )  return RTEMS.Status_Codes;
      pragma Import (C, Barrier_Release_Base, "rtems_barrier_release");
      Released_Base : aliased RTEMS.Unsigned32;
   begin

      Result := Barrier_Release_Base ( ID, Released_Base'Unchecked_Access );
      Released := Released_Base;

   end Barrier_Release;

 
   --
   -- Debug Manager
   --
 
   procedure Debug_Enable (
      To_Be_Enabled : in     RTEMS.Debug_Set
   ) is
      procedure Debug_Enable_Base (
         To_Be_Enabled : RTEMS.Debug_Set
      );
   pragma Import (C, Debug_Enable_Base, "rtems_debug_enable");
   begin
 
      Debug_Enable_Base ( To_Be_Enabled );

   end Debug_Enable;
 
   procedure Debug_Disable (
      To_Be_Disabled : in     RTEMS.Debug_Set
   ) is
      procedure Debug_Disable_Base (
         To_Be_Disabled : RTEMS.Debug_Set
      );
   pragma Import (C, Debug_Disable_Base, "rtems_debug_disable");
   begin
 
      Debug_Disable_Base ( To_Be_Disabled );

   end Debug_Disable;
 
   function Debug_Is_Enabled (
      Level : in     RTEMS.Debug_Set
   ) return RTEMS.Boolean is
      function Debug_Is_Enabled_Base (
         Level : RTEMS.Debug_Set
      )  return RTEMS.Boolean;
      pragma Import (C, Debug_Is_Enabled_Base, "_Debug_Is_enabled");
   begin
 
      return Debug_Is_Enabled_Base ( Level );

   end Debug_Is_Enabled;

end RTEMS;
