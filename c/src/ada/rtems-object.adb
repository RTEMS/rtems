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

with Interfaces;           use Interfaces;
with Interfaces.C.Strings; use Interfaces.C.Strings;

package body RTEMS.Object is

   --
   --  Object Services
   --

   function Build_Name
     (C1   : in Character;
      C2   : in Character;
      C3   : in Character;
      C4   : in Character)
      return RTEMS.Name
   is
      C1_Value : RTEMS.Unsigned32;
      C2_Value : RTEMS.Unsigned32;
      C3_Value : RTEMS.Unsigned32;
      C4_Value : RTEMS.Unsigned32;
   begin

      C1_Value := Character'Pos (C1);
      C2_Value := Character'Pos (C2);
      C3_Value := Character'Pos (C3);
      C4_Value := Character'Pos (C4);

      return Interfaces.Shift_Left (C1_Value, 24) or
             Interfaces.Shift_Left (C2_Value, 16) or
             Interfaces.Shift_Left (C3_Value, 8) or
             C4_Value;

   end Build_Name;

   procedure Get_Classic_Name
     (ID     : in RTEMS.ID;
      Name   : out RTEMS.Name;
      Result : out RTEMS.Status_Codes)
   is
      function Get_Classic_Name_Base
        (ID   : RTEMS.ID;
         Name : access RTEMS.Name)
         return RTEMS.Status_Codes;
      pragma Import
        (C,
         Get_Classic_Name_Base,
         "rtems_object_get_classic_name");
      Tmp_Name : aliased RTEMS.Name;
   begin
      Result := Get_Classic_Name_Base (ID, Tmp_Name'Access);
      Name   := Tmp_Name;
   end Get_Classic_Name;

   procedure Get_Name
     (ID     : in RTEMS.ID;
      Name   : out String;
      Result : out RTEMS.Address)
   is
      function Get_Name_Base
        (ID     : RTEMS.ID;
         Length : RTEMS.Unsigned32;
         Name   : RTEMS.Address)
         return   RTEMS.Address;
      pragma Import (C, Get_Name_Base, "rtems_object_get_name");
   begin
      Name   := (others => ASCII.NUL);
      Result :=
         Get_Name_Base (ID, Name'Length, Name (Name'First)'Address);
   end Get_Name;

   procedure Set_Name
     (ID     : in RTEMS.ID;
      Name   : in String;
      Result : out RTEMS.Status_Codes)
   is
      function Set_Name_Base
        (ID   : RTEMS.ID;
         Name : chars_ptr)
         return RTEMS.Status_Codes;
      pragma Import (C, Set_Name_Base, "rtems_object_set_name");
      NameAsCString : constant chars_ptr := New_String (Name);
   begin
      Result := Set_Name_Base (ID, NameAsCString);
   end Set_Name;

   procedure Id_Get_API
     (ID  : in RTEMS.ID;
      API : out RTEMS.Unsigned32)
   is
      function Id_Get_API_Base
        (ID   : RTEMS.ID)
         return RTEMS.Unsigned32;
      pragma Import (C, Id_Get_API_Base, "rtems_object_id_get_api");
   begin
      API := Id_Get_API_Base (ID);
   end Id_Get_API;

   procedure Id_Get_Class
     (ID        : in RTEMS.ID;
      The_Class : out RTEMS.Unsigned32)
   is
      function Id_Get_Class_Base
        (ID   : RTEMS.ID)
         return RTEMS.Unsigned32;
      pragma Import
        (C,
         Id_Get_Class_Base,
         "rtems_object_id_get_class");
   begin
      The_Class := Id_Get_Class_Base (ID);
   end Id_Get_Class;

   procedure Id_Get_Node
     (ID   : in RTEMS.ID;
      Node : out RTEMS.Unsigned32)
   is
      function Id_Get_Node_Base
        (ID   : RTEMS.ID)
         return RTEMS.Unsigned32;
      pragma Import (C, Id_Get_Node_Base, "rtems_object_id_get_node");
   begin
      Node := Id_Get_Node_Base (ID);
   end Id_Get_Node;

   procedure Id_Get_Index
     (ID    : in RTEMS.ID;
      Index : out RTEMS.Unsigned32)
   is
      function Id_Get_Index_Base
        (ID   : RTEMS.ID)
         return RTEMS.Unsigned32;
      pragma Import
        (C,
         Id_Get_Index_Base,
         "rtems_object_id_get_index");
   begin
      Index := Id_Get_Index_Base (ID);
   end Id_Get_Index;

   function Build_Id
     (The_API   : in RTEMS.Unsigned32;
      The_Class : in RTEMS.Unsigned32;
      The_Node  : in RTEMS.Unsigned32;
      The_Index : in RTEMS.Unsigned32)
      return      RTEMS.ID
   is
      function Build_Id_Base
        (The_API   : RTEMS.Unsigned32;
         The_Class : RTEMS.Unsigned32;
         The_Node  : RTEMS.Unsigned32;
         The_Index : RTEMS.Unsigned32)
         return      RTEMS.ID;
      pragma Import (C, Build_Id_Base, "rtems_build_id");
   begin
      return Build_Id_Base (The_API, The_Class, The_Node, The_Index);
   end Build_Id;

   function Id_API_Minimum return  RTEMS.Unsigned32 is
      function Id_API_Minimum_Base return  RTEMS.Unsigned32;
      pragma Import
        (C,
         Id_API_Minimum_Base,
         "rtems_object_id_api_minimum");
   begin
      return Id_API_Minimum_Base;
   end Id_API_Minimum;

   function Id_API_Maximum return  RTEMS.Unsigned32 is
      function Id_API_Maximum_Base return  RTEMS.Unsigned32;
      pragma Import
        (C,
         Id_API_Maximum_Base,
         "rtems_object_id_api_maximum");
   begin
      return Id_API_Maximum_Base;
   end Id_API_Maximum;

   procedure API_Minimum_Class
     (API     : in RTEMS.Unsigned32;
      Minimum : out RTEMS.Unsigned32)
   is
      function API_Minimum_Class_Base
        (API  : RTEMS.Unsigned32)
         return RTEMS.Unsigned32;
      pragma Import
        (C,
         API_Minimum_Class_Base,
         "rtems_object_api_minimum_class");
   begin
      Minimum := API_Minimum_Class_Base (API);
   end API_Minimum_Class;

   procedure API_Maximum_Class
     (API     : in RTEMS.Unsigned32;
      Maximum : out RTEMS.Unsigned32)
   is
      function API_Maximum_Class_Base
        (API  : RTEMS.Unsigned32)
         return RTEMS.Unsigned32;
      pragma Import
        (C,
         API_Maximum_Class_Base,
         "rtems_object_api_maximum_class");
   begin
      Maximum := API_Maximum_Class_Base (API);
   end API_Maximum_Class;

   -- Translate S from a C-style char* into an Ada String.
   -- If S is Null_Ptr, return "", don't raise an exception.
   -- Copied from Lovelace Tutorial
   function Value_Without_Exception (S : chars_ptr) return String is
   begin
      if S = Null_Ptr then
         return "";
      else
         return Value (S);
      end if;
   end Value_Without_Exception;
   pragma Inline (Value_Without_Exception);

   procedure Get_API_Name
     (API  : in RTEMS.Unsigned32;
      Name : out String)
   is
      function Get_API_Name_Base
        (API  : RTEMS.Unsigned32)
         return chars_ptr;
      pragma Import
        (C,
         Get_API_Name_Base,
         "rtems_object_get_api_name");
      Result  : constant chars_ptr := Get_API_Name_Base (API);
      APIName : constant String    := Value_Without_Exception (Result);
   begin
      Name := APIName;
   end Get_API_Name;

   procedure Get_API_Class_Name
     (The_API   : in RTEMS.Unsigned32;
      The_Class : in RTEMS.Unsigned32;
      Name      : out String)
   is
      function Get_API_Class_Name_Base
        (API   : RTEMS.Unsigned32;
         Class : RTEMS.Unsigned32)
         return  chars_ptr;
      pragma Import
        (C,
         Get_API_Class_Name_Base,
         "rtems_object_get_api_class_name");
      Result    : constant chars_ptr :=
         Get_API_Class_Name_Base (The_API, The_Class);
      ClassName : constant String    := Value_Without_Exception (Result);
   begin
      Name := ClassName;
   end Get_API_Class_Name;

   procedure Get_Class_Information
     (The_API   : in RTEMS.Unsigned32;
      The_Class : in RTEMS.Unsigned32;
      Info      : out API_Class_Information;
      Result    : out RTEMS.Status_Codes)
   is
      function Get_Class_Information_Base
        (The_API   : RTEMS.Unsigned32;
         The_Class : RTEMS.Unsigned32;
         Info      : access API_Class_Information)
         return      RTEMS.Status_Codes;
      pragma Import
        (C,
         Get_Class_Information_Base,
         "rtems_object_get_class_information");
      TmpInfo : aliased API_Class_Information;
   begin
      Result :=
         Get_Class_Information_Base
           (The_API,
            The_Class,
            TmpInfo'Access);
      Info   := TmpInfo;
   end Get_Class_Information;

end RTEMS.Object;
