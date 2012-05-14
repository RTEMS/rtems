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

package body RTEMS.Timer is

   --
   -- Timer Manager
   --

   procedure Create
     (Name   : in RTEMS.Name;
      ID     : out RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Create_Base
        (Name : RTEMS.Name;
         ID   : access RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import (C, Create_Base, "rtems_timer_create");
      ID_Base : aliased RTEMS.ID;
   begin

      Result := Create_Base (Name, ID_Base'Access);
      ID     := ID_Base;

   end Create;

   procedure Ident
     (Name   : in RTEMS.Name;
      ID     : out RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Ident_Base
        (Name : RTEMS.Name;
         ID   : access RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import (C, Ident_Base, "rtems_timer_ident");
      ID_Base : aliased RTEMS.ID;
   begin

      Result := Ident_Base (Name, ID_Base'Access);
      ID     := ID_Base;

   end Ident;

   procedure Delete
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Delete_Base (ID : RTEMS.ID) return RTEMS.Status_Codes;
      pragma Import (C, Delete_Base, "rtems_timer_delete");
   begin

      Result := Delete_Base (ID);

   end Delete;

   procedure Fire_After
     (ID        : in RTEMS.ID;
      Ticks     : in RTEMS.Interval;
      Routine   : in RTEMS.Timer.Service_Routine;
      User_Data : in RTEMS.Address;
      Result    : out RTEMS.Status_Codes)
   is
      function Fire_After_Base
        (ID        : RTEMS.ID;
         Ticks     : RTEMS.Interval;
         Routine   : RTEMS.Timer.Service_Routine;
         User_Data : RTEMS.Address)
         return      RTEMS.Status_Codes;
      pragma Import (C, Fire_After_Base, "rtems_timer_fire_after");
   begin

      Result := Fire_After_Base (ID, Ticks, Routine, User_Data);

   end Fire_After;

   procedure Server_Fire_After
     (ID        : in RTEMS.ID;
      Ticks     : in RTEMS.Interval;
      Routine   : in RTEMS.Timer.Service_Routine;
      User_Data : in RTEMS.Address;
      Result    : out RTEMS.Status_Codes)
   is
      function Server_Fire_After_Base
        (ID        : RTEMS.ID;
         Ticks     : RTEMS.Interval;
         Routine   : RTEMS.Timer.Service_Routine;
         User_Data : RTEMS.Address)
         return      RTEMS.Status_Codes;
      pragma Import
        (C,
         Server_Fire_After_Base,
         "rtems_timer_server_fire_after");
   begin

      Result := Server_Fire_After_Base (ID, Ticks, Routine, User_Data);

   end Server_Fire_After;

   procedure Fire_When
     (ID        : in RTEMS.ID;
      Wall_Time : in RTEMS.Time_Of_Day;
      Routine   : in RTEMS.Timer.Service_Routine;
      User_Data : in RTEMS.Address;
      Result    : out RTEMS.Status_Codes)
   is
      function Fire_When_Base
        (ID        : RTEMS.ID;
         Wall_Time : RTEMS.Time_Of_Day;
         Routine   : RTEMS.Timer.Service_Routine;
         User_Data : RTEMS.Address)
         return      RTEMS.Status_Codes;
      pragma Import (C, Fire_When_Base, "rtems_timer_fire_when");
   begin

      Result := Fire_When_Base (ID, Wall_Time, Routine, User_Data);

   end Fire_When;

   procedure Server_Fire_When
     (ID        : in RTEMS.ID;
      Wall_Time : in RTEMS.Time_Of_Day;
      Routine   : in RTEMS.Timer.Service_Routine;
      User_Data : in RTEMS.Address;
      Result    : out RTEMS.Status_Codes)
   is
      function Server_Fire_When_Base
        (ID        : RTEMS.ID;
         Wall_Time : RTEMS.Time_Of_Day;
         Routine   : RTEMS.Timer.Service_Routine;
         User_Data : RTEMS.Address)
         return      RTEMS.Status_Codes;
      pragma Import
        (C,
         Server_Fire_When_Base,
         "rtems_timer_server_fire_when");
   begin

      Result :=
         Server_Fire_When_Base (ID, Wall_Time, Routine, User_Data);
   end Server_Fire_When;

   procedure Reset
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Reset_Base (ID : RTEMS.ID) return RTEMS.Status_Codes;
      pragma Import (C, Reset_Base, "rtems_timer_reset");
   begin

      Result := Reset_Base (ID);

   end Reset;

   procedure Cancel
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Cancel_Base (ID : RTEMS.ID) return RTEMS.Status_Codes;
      pragma Import (C, Cancel_Base, "rtems_timer_cancel");
   begin

      Result := Cancel_Base (ID);

   end Cancel;

   procedure Initiate_Server
     (Server_Priority : in RTEMS.Tasks.Priority;
      Stack_Size      : in RTEMS.Unsigned32;
      Attribute_Set   : in RTEMS.Attribute;
      Result          : out RTEMS.Status_Codes)
   is
      function Initiate_Server_Base
        (Server_Priority : RTEMS.Tasks.Priority;
         Stack_Size      : RTEMS.Unsigned32;
         Attribute_Set   : RTEMS.Attribute)
         return            RTEMS.Status_Codes;
      pragma Import
        (C,
         Initiate_Server_Base,
         "rtems_timer_initiate_server");
   begin
      Result :=
         Initiate_Server_Base
           (Server_Priority,
            Stack_Size,
            Attribute_Set);
   end Initiate_Server;

end RTEMS.Timer;
