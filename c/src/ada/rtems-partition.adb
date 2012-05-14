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

package body RTEMS.Partition is

   --
   -- Partition Manager
   --

   procedure Create
     (Name             : in RTEMS.Name;
      Starting_Address : in RTEMS.Address;
      Length           : in RTEMS.Unsigned32;
      Buffer_Size      : in RTEMS.Unsigned32;
      Attribute_Set    : in RTEMS.Attribute;
      ID               : out RTEMS.ID;
      Result           : out RTEMS.Status_Codes)
   is
      function Create_Base
        (Name             : RTEMS.Name;
         Starting_Address : RTEMS.Address;
         Length           : RTEMS.Unsigned32;
         Buffer_Size      : RTEMS.Unsigned32;
         Attribute_Set    : RTEMS.Attribute;
         ID               : access RTEMS.Event_Set)
         return             RTEMS.Status_Codes;
      pragma Import (C, Create_Base, "rtems_partition_create");
      ID_Base : aliased RTEMS.ID;
   begin

      Result :=
         Create_Base
           (Name,
            Starting_Address,
            Length,
            Buffer_Size,
            Attribute_Set,
            ID_Base'Access);
      ID     := ID_Base;

   end Create;

   procedure Ident
     (Name   : in RTEMS.Name;
      Node   : in RTEMS.Unsigned32;
      ID     : out RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Ident_Base
        (Name : RTEMS.Name;
         Node : RTEMS.Unsigned32;
         ID   : access RTEMS.Event_Set)
         return RTEMS.Status_Codes;
      pragma Import (C, Ident_Base, "rtems_partition_ident");
      ID_Base : aliased RTEMS.ID;
   begin

      Result := Ident_Base (Name, Node, ID_Base'Access);
      ID     := ID_Base;

   end Ident;

   procedure Delete
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Delete_Base
        (ID   : RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import (C, Delete_Base, "rtems_partition_delete");
   begin

      Result := Delete_Base (ID);

   end Delete;

   procedure Get_Buffer
     (ID     : in RTEMS.ID;
      Buffer : out RTEMS.Address;
      Result : out RTEMS.Status_Codes)
   is
      function Get_Buffer_Base
        (ID     : RTEMS.ID;
         Buffer : access RTEMS.Address)
         return   RTEMS.Status_Codes;
      pragma Import
        (C,
         Get_Buffer_Base,
         "rtems_partition_get_buffer");
      Buffer_Base : aliased RTEMS.Address;
   begin

      Result := Get_Buffer_Base (ID, Buffer_Base'Access);
      Buffer := Buffer_Base;

   end Get_Buffer;

   procedure Return_Buffer
     (ID     : in RTEMS.ID;
      Buffer : in RTEMS.Address;
      Result : out RTEMS.Status_Codes)
   is
      function Return_Buffer_Base
        (ID     : RTEMS.Name;
         Buffer : RTEMS.Address)
         return   RTEMS.Status_Codes;
      pragma Import
        (C,
         Return_Buffer_Base,
         "rtems_partition_return_buffer");
   begin

      Result := Return_Buffer_Base (ID, Buffer);

   end Return_Buffer;

end RTEMS.Partition;
