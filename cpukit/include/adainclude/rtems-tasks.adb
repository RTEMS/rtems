-- SPDX-License-Identifier: BSD-2-Clause

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

package body RTEMS.Tasks is

   --
   --  Task Manager
   --

   procedure Create
     (Name             : in RTEMS.Name;
      Initial_Priority : in Priority;
      Stack_Size       : in RTEMS.Size;
      Initial_Modes    : in RTEMS.Mode;
      Attribute_Set    : in RTEMS.Attribute;
      ID               : out RTEMS.ID;
      Result           : out RTEMS.Status_Codes)
   is
      function Create_Base
        (Name             : RTEMS.Name;
         Initial_Priority : Priority;
         Stack_Size       : RTEMS.Size;
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
