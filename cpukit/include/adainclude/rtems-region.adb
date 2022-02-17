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
--  COPYRIGHT (c) 1997-2011.
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

package body RTEMS.Region is

   --
   -- Region Manager
   --

   procedure Create
     (Name             : in RTEMS.Name;
      Starting_Address : in RTEMS.Address;
      Length           : in RTEMS.Size;
      Page_Size        : in RTEMS.Size;
      Attribute_Set    : in RTEMS.Attribute;
      ID               : out RTEMS.ID;
      Result           : out RTEMS.Status_Codes)
   is
      function Create_Base
        (Name             : RTEMS.Name;
         Starting_Address : RTEMS.Address;
         Length           : RTEMS.Size;
         Page_Size        : RTEMS.Size;
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
      Length           : in RTEMS.Size;
      Result           : out RTEMS.Status_Codes)
   is
      function Extend_Base
        (ID               : RTEMS.ID;
         Starting_Address : RTEMS.Address;
         Length           : RTEMS.Size)
         return             RTEMS.Status_Codes;
      pragma Import (C, Extend_Base, "rtems_region_extend");
   begin

      Result := Extend_Base (ID, Starting_Address, Length);

   end Extend;

   procedure Get_Segment
     (ID         : in RTEMS.ID;
      Size       : in RTEMS.Size;
      Option_Set : in RTEMS.Option;
      Timeout    : in RTEMS.Interval;
      Segment    : out RTEMS.Address;
      Result     : out RTEMS.Status_Codes)
   is
      function Get_Segment_Base
        (ID         : RTEMS.ID;
         Size       : RTEMS.Size;
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
      Size    : out RTEMS.Size;
      Result  : out RTEMS.Status_Codes)
   is
      function Get_Segment_Size_Base
        (ID      : RTEMS.ID;
         Segment : RTEMS.Address;
         Size    : access RTEMS.Size)
         return    RTEMS.Status_Codes;
      pragma Import
        (C,
         Get_Segment_Size_Base,
         "rtems_region_get_segment_size");
      Size_Base : aliased RTEMS.Size;
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
      Size     : in RTEMS.Size;
      Old_Size : out RTEMS.Size;
      Result   : out RTEMS.Status_Codes)
   is
      function Resize_Segment_Base
        (ID       : RTEMS.ID;
         Segment  : RTEMS.Address;
         Size     : RTEMS.Size;
         Old_Size : access RTEMS.Size)
         return     RTEMS.Status_Codes;
      pragma Import
        (C,
         Resize_Segment_Base,
         "rtems_region_resize_segment");
      Old_Size_Base : aliased RTEMS.Size;
   begin

      Result   :=
         Resize_Segment_Base (ID, Segment, Size, Old_Size_Base'Access);
      Old_Size := Old_Size_Base;

   end Resize_Segment;

end RTEMS.Region;
