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
--  COPYRIGHT (c) 1997-2008.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

package body RTEMS.Tasks is

   --
   --  Task Manager
   --

   procedure Create
     (Name             : in RTEMS.Name;
      Initial_Priority : in Priority;
      Stack_Size       : in RTEMS.Unsigned32;
      Initial_Modes    : in RTEMS.Mode;
      Attribute_Set    : in RTEMS.Attribute;
      ID               : out RTEMS.ID;
      Result           : out RTEMS.Status_Codes)
   is
      function Create_Base
        (Name             : RTEMS.Name;
         Initial_Priority : Priority;
         Stack_Size       : RTEMS.Unsigned32;
         Initial_Modes    : RTEMS.Mode;
         Attribute_Set    : RTEMS.Attribute;
         ID               : access RTEMS.ID)
         return             RTEMS.Status_Codes;
      pragma Import (C, Create_Base, "rtems_task_create");
      ID_Base : aliased RTEMS.ID;
   begin
      Result :=
         Create_Base
           (Name,
            Initial_Priority,
            Stack_Size,
            Initial_Modes,
            Attribute_Set,
            ID_Base'Access);
      ID     := ID_Base;
   end Create;

   procedure Ident
     (Name   : in RTEMS.Name;
      Node   : in RTEMS.Node;
      ID     : out RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is

      function Ident_Base
        (Name : RTEMS.Name;
         Node : RTEMS.Node;
         ID   : access RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import (C, Ident_Base, "rtems_task_ident");
      ID_Base : aliased RTEMS.ID;

   begin

      Result := Ident_Base (Name, Node, ID_Base'Access);
      ID     := ID_Base;

   end Ident;

   procedure Start
     (ID          : in RTEMS.ID;
      Entry_Point : in RTEMS.Tasks.Entry_Point;
      Argument    : in RTEMS.Tasks.Argument;
      Result      : out RTEMS.Status_Codes)
   is
      function Start_Base
        (ID          : RTEMS.ID;
         Entry_Point : RTEMS.Tasks.Entry_Point;
         Argument    : RTEMS.Tasks.Argument)
         return        RTEMS.Status_Codes;
      pragma Import (C, Start_Base, "rtems_task_start");
   begin

      Result := Start_Base (ID, Entry_Point, Argument);

   end Start;

   procedure Restart
     (ID       : in RTEMS.ID;
      Argument : in RTEMS.Tasks.Argument;
      Result   : out RTEMS.Status_Codes)
   is
      function Restart_Base
        (ID       : RTEMS.ID;
         Argument : RTEMS.Tasks.Argument)
         return     RTEMS.Status_Codes;
      pragma Import (C, Restart_Base, "rtems_task_restart");
   begin

      Result := Restart_Base (ID, Argument);

   end Restart;

   procedure Delete
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Delete_Base (ID : RTEMS.ID) return RTEMS.Status_Codes;
      pragma Import (C, Delete_Base, "rtems_task_delete");
   begin

      Result := Delete_Base (ID);

   end Delete;

   procedure Suspend
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Suspend_Base (ID : RTEMS.ID) return RTEMS.Status_Codes;
      pragma Import (C, Suspend_Base, "rtems_task_suspend");
   begin

      Result := Suspend_Base (ID);

   end Suspend;

   procedure Resume
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Resume_Base (ID : RTEMS.ID) return RTEMS.Status_Codes;
      pragma Import (C, Resume_Base, "rtems_task_resume");
   begin

      Result := Resume_Base (ID);

   end Resume;

   procedure Is_Suspended
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Is_Suspended_Base
        (ID   : RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import (C, Is_Suspended_Base, "rtems_task_is_suspended");
   begin

      Result := Is_Suspended_Base (ID);

   end Is_Suspended;

   procedure Set_Priority
     (ID           : in RTEMS.ID;
      New_Priority : in Priority;
      Old_Priority : out Priority;
      Result       : out RTEMS.Status_Codes)
   is
      function Set_Priority_Base
        (ID           : RTEMS.ID;
         New_Priority : Priority;
         Old_Priority : access Priority)
         return         RTEMS.Status_Codes;
      pragma Import (C, Set_Priority_Base, "rtems_task_set_priority");
      Old_Priority_Base : aliased Priority;
   begin

      Result       :=
         Set_Priority_Base (ID, New_Priority, Old_Priority_Base'Access);
      Old_Priority := Old_Priority_Base;

   end Set_Priority;

   procedure Mode
     (Mode_Set          : in RTEMS.Mode;
      Mask              : in RTEMS.Mode;
      Previous_Mode_Set : out RTEMS.Mode;
      Result            : out RTEMS.Status_Codes)
   is
      function Mode_Base
        (Mode_Set          : RTEMS.Mode;
         Mask              : RTEMS.Mode;
         Previous_Mode_Set : access RTEMS.Mode)
         return              RTEMS.Status_Codes;
      pragma Import (C, Mode_Base, "rtems_task_mode");
      Previous_Mode_Set_Base : aliased RTEMS.Mode;
   begin

      Result            :=
         Mode_Base (Mode_Set, Mask, Previous_Mode_Set_Base'Access);
      Previous_Mode_Set := Previous_Mode_Set_Base;

   end Mode;

   procedure Get_Note
     (ID      : in RTEMS.ID;
      Notepad : in RTEMS.Notepad_Index;
      Note    : out RTEMS.Unsigned32;
      Result  : out RTEMS.Status_Codes)
   is
      function Get_Note_Base
        (ID      : RTEMS.ID;
         Notepad : RTEMS.Notepad_Index;
         Note    : access RTEMS.Unsigned32)
         return    RTEMS.Status_Codes;
      pragma Import (C, Get_Note_Base, "rtems_task_get_note");
      Note_Base : aliased RTEMS.Unsigned32;
   begin

      Result := Get_Note_Base (ID, Notepad, Note_Base'Access);
      Note   := Note_Base;

   end Get_Note;

   procedure Set_Note
     (ID      : in RTEMS.ID;
      Notepad : in RTEMS.Notepad_Index;
      Note    : in RTEMS.Unsigned32;
      Result  : out RTEMS.Status_Codes)
   is
      function Set_Note_Base
        (ID      : RTEMS.ID;
         Notepad : RTEMS.Notepad_Index;
         Note    : RTEMS.Unsigned32)
         return    RTEMS.Status_Codes;
      pragma Import (C, Set_Note_Base, "rtems_task_set_note");
   begin

      Result := Set_Note_Base (ID, Notepad, Note);

   end Set_Note;

   procedure Variable_Add
     (ID            : in RTEMS.ID;
      Task_Variable : in RTEMS.Address;
      Dtor          : in Variable_Dtor;
      Result        : out RTEMS.Status_Codes)
   is
      function Variable_Add_Base
        (ID            : RTEMS.ID;
         Task_Variable : RTEMS.Address;
         Dtor          : Variable_Dtor)
         return          RTEMS.Status_Codes;
      pragma Import (C, Variable_Add_Base, "rtems_task_variable_add");
   begin

      Result := Variable_Add_Base (ID, Task_Variable, Dtor);

   end Variable_Add;

   procedure Variable_Get
     (ID                  : in RTEMS.ID;
      Task_Variable       : out RTEMS.Address;
      Task_Variable_Value : out RTEMS.Address;
      Result              : out RTEMS.Status_Codes)
   is
      function Variable_Get_Base
        (ID                  : RTEMS.ID;
         Task_Variable       : access RTEMS.Address;
         Task_Variable_Value : access RTEMS.Address)
         return                RTEMS.Status_Codes;
      pragma Import (C, Variable_Get_Base, "rtems_task_variable_get");
      Task_Variable_Base       : aliased RTEMS.Address;
      Task_Variable_Value_Base : aliased RTEMS.Address;
   begin

      Result              :=
         Variable_Get_Base
           (ID,
            Task_Variable_Base'Access,
            Task_Variable_Value_Base'Access);
      Task_Variable       := Task_Variable_Base;
      Task_Variable_Value := Task_Variable_Value_Base;

   end Variable_Get;

   procedure Variable_Delete
     (ID            : in RTEMS.ID;
      Task_Variable : out RTEMS.Address;
      Result        : out RTEMS.Status_Codes)
   is
      function Variable_Delete_Base
        (ID            : RTEMS.ID;
         Task_Variable : access RTEMS.Address)
         return          RTEMS.Status_Codes;
      pragma Import
        (C,
         Variable_Delete_Base,
         "rtems_task_variable_delete");
      Task_Variable_Base : aliased RTEMS.Address;
   begin

      Result        := Variable_Delete_Base (ID, Task_Variable_Base'Access);
      Task_Variable := Task_Variable_Base;

   end Variable_Delete;

   procedure Wake_When
     (Time_Buffer : in RTEMS.Time_Of_Day;
      Result      : out RTEMS.Status_Codes)
   is
      function Wake_When_Base
        (Time_Buffer : RTEMS.Time_Of_Day)
         return        RTEMS.Status_Codes;
      pragma Import (C, Wake_When_Base, "rtems_task_wake_when");
   begin

      Result := Wake_When_Base (Time_Buffer);

   end Wake_When;

   procedure Wake_After
     (Ticks  : in RTEMS.Interval;
      Result : out RTEMS.Status_Codes)
   is
      function Wake_After_Base
        (Ticks : RTEMS.Interval)
         return  RTEMS.Status_Codes;
      pragma Import (C, Wake_After_Base, "rtems_task_wake_after");
   begin

      Result := Wake_After_Base (Ticks);

   end Wake_After;

end RTEMS.Tasks;
