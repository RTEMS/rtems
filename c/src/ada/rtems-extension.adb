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
--  COPYRIGHT (c) 1997-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

package body RTEMS.Extension is

   --
   -- Extension Manager
   --

   procedure Create
     (Name   : in RTEMS.Name;
      Table  : in RTEMS.Extensions_Table_Pointer;
      ID     : out RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Create_Base
        (Name  : RTEMS.Name;
         Table : RTEMS.Extensions_Table_Pointer;
         ID    : access RTEMS.ID)
         return  RTEMS.Status_Codes;
      pragma Import (C, Create_Base, "rtems_extension_create");
      ID_Base : aliased RTEMS.ID;
   begin

      Result := Create_Base (Name, Table, ID_Base'Access);
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
      pragma Import (C, Ident_Base, "rtems_extension_ident");
      ID_Base : aliased RTEMS.ID;
   begin

      Result := Ident_Base (Name, ID_Base'Access);
      ID     := ID_Base;

   end Ident;

   procedure Delete
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Delete_Base
        (ID   : RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import (C, Delete_Base, "rtems_extension_delete");
   begin

      Result := Delete_Base (ID);

   end Delete;

end RTEMS.Extension;
