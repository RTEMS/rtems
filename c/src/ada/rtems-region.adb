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

package body RTEMS.Region is

   --
   -- Region Manager
   --

   procedure Create
     (Name             : in RTEMS.Name;
      Starting_Address : in RTEMS.Address;
      Length           : in RTEMS.Unsigned32;
      Page_Size        : in RTEMS.Unsigned32;
      Attribute_Set    : in RTEMS.Attribute;
      ID               : out RTEMS.ID;
      Result           : out RTEMS.Status_Codes)
   is
      function Create_Base
        (Name             : RTEMS.Name;
         Starting_Address : RTEMS.Address;
         Length           : RTEMS.Unsigned32;
         Page_Size        : RTEMS.Unsigned32;
         Attribute_Set    : RTEMS.Attribute;
         ID               : access RTEMS.ID)
         return             RTEMS.Status_Codes;
      pragma Import (C, Create_Base, "rtems_region_create");
      ID_Base : aliased RTEMS.ID;
   begin

      Result :=
         Create_Base
           (Name,
            Starting_Address,
            Length,
            Page_Size,
            Attribute_Set,
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
      pragma Import (C, Ident_Base, "rtems_region_ident");
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
      pragma Import (C, Delete_Base, "rtems_region_delete");
   begin

      Result := Delete_Base (ID);

   end Delete;

   procedure Extend
     (ID               : in RTEMS.ID;
      Starting_Address : in RTEMS.Address;
      Length           : in RTEMS.Unsigned32;
      Result           : out RTEMS.Status_Codes)
   is
      function Extend_Base
        (ID               : RTEMS.ID;
         Starting_Address : RTEMS.Address;
         Length           : RTEMS.Unsigned32)
         return             RTEMS.Status_Codes;
      pragma Import (C, Extend_Base, "rtems_region_extend");
   begin

      Result := Extend_Base (ID, Starting_Address, Length);

   end Extend;

   procedure Get_Segment
     (ID         : in RTEMS.ID;
      Size       : in RTEMS.Unsigned32;
      Option_Set : in RTEMS.Option;
      Timeout    : in RTEMS.Interval;
      Segment    : out RTEMS.Address;
      Result     : out RTEMS.Status_Codes)
   is
      function Get_Segment_Base
        (ID         : RTEMS.ID;
         Size       : RTEMS.Unsigned32;
         Option_Set : RTEMS.Option;
         Timeout    : RTEMS.Interval;
         Segment    : access RTEMS.Address)
         return       RTEMS.Status_Codes;
      pragma Import (C, Get_Segment_Base, "rtems_region_get_segment");
      Segment_Base : aliased RTEMS.Address;
   begin

      Result  :=
         Get_Segment_Base
           (ID,
            Size,
            Option_Set,
            Timeout,
            Segment_Base'Access);
      Segment := Segment_Base;

   end Get_Segment;

   procedure Get_Segment_Size
     (ID      : in RTEMS.ID;
      Segment : in RTEMS.Address;
      Size    : out RTEMS.Unsigned32;
      Result  : out RTEMS.Status_Codes)
   is
      function Get_Segment_Size_Base
        (ID      : RTEMS.ID;
         Segment : RTEMS.Address;
         Size    : access RTEMS.Unsigned32)
         return    RTEMS.Status_Codes;
      pragma Import
        (C,
         Get_Segment_Size_Base,
         "rtems_region_get_segment_size");
      Size_Base : aliased RTEMS.Unsigned32;
   begin

      Result := Get_Segment_Size_Base (ID, Segment, Size_Base'Access);
      Size   := Size_Base;

   end Get_Segment_Size;

   procedure Return_Segment
     (ID      : in RTEMS.ID;
      Segment : in RTEMS.Address;
      Result  : out RTEMS.Status_Codes)
   is
      function Return_Segment_Base
        (ID      : RTEMS.ID;
         Segment : RTEMS.Address)
         return    RTEMS.Status_Codes;
      pragma Import
        (C,
         Return_Segment_Base,
         "rtems_region_return_segment");
   begin

      Result := Return_Segment_Base (ID, Segment);

   end Return_Segment;

   procedure Resize_Segment
     (ID       : in RTEMS.ID;
      Segment  : in RTEMS.Address;
      Size     : in RTEMS.Unsigned32;
      Old_Size : out RTEMS.Unsigned32;
      Result   : out RTEMS.Status_Codes)
   is
      function Resize_Segment_Base
        (ID       : RTEMS.ID;
         Segment  : RTEMS.Address;
         Size     : RTEMS.Unsigned32;
         Old_Size : access RTEMS.Unsigned32)
         return     RTEMS.Status_Codes;
      pragma Import
        (C,
         Resize_Segment_Base,
         "rtems_region_resize_segment");
      Old_Size_Base : aliased RTEMS.Unsigned32;
   begin

      Result   :=
         Resize_Segment_Base (ID, Segment, Size, Old_Size_Base'Access);
      Old_Size := Old_Size_Base;

   end Resize_Segment;

end RTEMS.Region;
