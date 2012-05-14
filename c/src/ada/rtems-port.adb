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

package body RTEMS.Port is

   --
   -- Dual Ported Memory Manager
   --

   procedure Create
     (Name           : in RTEMS.Name;
      Internal_Start : in RTEMS.Address;
      External_Start : in RTEMS.Address;
      Length         : in RTEMS.Unsigned32;
      ID             : out RTEMS.ID;
      Result         : out RTEMS.Status_Codes)
   is
      function Create_Base
        (Name           : RTEMS.Name;
         Internal_Start : RTEMS.Address;
         External_Start : RTEMS.Address;
         Length         : RTEMS.Unsigned32;
         ID             : access RTEMS.ID)
         return           RTEMS.Status_Codes;
      pragma Import (C, Create_Base, "rtems_port_create");
      ID_Base : aliased RTEMS.ID;
   begin

      Result :=
         Create_Base
           (Name,
            Internal_Start,
            External_Start,
            Length,
            ID_Base'Access);
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
      pragma Import (C, Ident_Base, "rtems_port_ident");
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
      pragma Import (C, Delete_Base, "rtems_port_delete");
   begin

      Result := Delete_Base (ID);

   end Delete;

   procedure External_To_Internal
     (ID       : in RTEMS.ID;
      External : in RTEMS.Address;
      Internal : out RTEMS.Address;
      Result   : out RTEMS.Status_Codes)
   is
      function External_To_Internal_Base
        (ID       : RTEMS.ID;
         External : RTEMS.Address;
         Internal : access RTEMS.Address)
         return     RTEMS.Status_Codes;
      pragma Import
        (C,
         External_To_Internal_Base,
         "rtems_port_external_to_internal");
      Internal_Base : aliased RTEMS.Address;
   begin

      Result   :=
         External_To_Internal_Base (ID, External, Internal_Base'Access);
      Internal := Internal_Base;

   end External_To_Internal;

   procedure Internal_To_External
     (ID       : in RTEMS.ID;
      Internal : in RTEMS.Address;
      External : out RTEMS.Address;
      Result   : out RTEMS.Status_Codes)
   is
      function Internal_To_External_Base
        (ID       : RTEMS.ID;
         Internal : RTEMS.Address;
         External : access RTEMS.Address)
         return     RTEMS.Status_Codes;
      pragma Import
        (C,
         Internal_To_External_Base,
         "rtems_port_internal_to_external");
      External_Base : aliased RTEMS.Address;
   begin

      Result   :=
         Internal_To_External_Base (ID, Internal, External_Base'Access);
      External := External_Base;

   end Internal_To_External;

end RTEMS.Port;
