-- SPDX-License-Identifier: BSD-2-Clause

--
--  RTEMS / Specification
--
--  DESCRIPTION:
--
--  This package provides the interface to the RTEMS API.
--
--  DEPENDENCIES:
--
--  NOTES:
--    RTEMS initialization and configuration are called from
--    the BSP side, therefore should never be called from ADA.
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

package RTEMS.Object is

   --
   --  Object Services
   --

   function Build_Name (
      C1 : in     Character;
      C2 : in     Character;
      C3 : in     Character;
      C4 : in     Character
   ) return RTEMS.Name;

   procedure Get_Classic_Name(
      ID     : in     RTEMS.ID;
      Name   :    out RTEMS.Name;
      Result :    out RTEMS.Status_Codes
   );

   procedure Get_Name(
      ID     : in     RTEMS.ID;
      Name   :    out String;
      Result :    out RTEMS.Address
   );

   procedure Set_Name(
      ID     : in     RTEMS.ID;
      Name   : in     String;
      Result :    out RTEMS.Status_Codes
   );

   procedure Id_Get_API(
      ID  : in     RTEMS.ID;
      API :    out RTEMS.Unsigned32
   );

   procedure Id_Get_Class(
      ID        : in     RTEMS.ID;
      The_Class :    out RTEMS.Unsigned32
   );

   procedure Id_Get_Node(
      ID   : in     RTEMS.ID;
      Node :    out RTEMS.Unsigned32
   );

   procedure Id_Get_Index(
      ID    : in     RTEMS.ID;
      Index :    out RTEMS.Unsigned32
   );

   function Build_Id(
      The_API   : in     RTEMS.Unsigned32;
      The_Class : in     RTEMS.Unsigned32;
      The_Node  : in     RTEMS.Unsigned32;
      The_Index : in     RTEMS.Unsigned32
   ) return RTEMS.Id;

   function Id_API_Minimum return RTEMS.Unsigned32;

   function Id_API_Maximum return RTEMS.Unsigned32;

   procedure API_Minimum_Class(
      API     : in     RTEMS.Unsigned32;
      Minimum :    out RTEMS.Unsigned32
   );

   procedure API_Maximum_Class(
      API     : in     RTEMS.Unsigned32;
      Maximum :    out RTEMS.Unsigned32
   );

   procedure Get_API_Name(
      API  : in     RTEMS.Unsigned32;
      Name :    out String
   );

   procedure Get_API_Class_Name(
      The_API   : in     RTEMS.Unsigned32;
      The_Class : in     RTEMS.Unsigned32;
      Name      :    out String
   );

   type API_Class_Information is
     record
        Minimum_Id    : RTEMS.Id;
        Maximum_Id    : RTEMS.Id;
        Maximum       : RTEMS.Unsigned32;
        AutoExtend    : RTEMS.Boolean;
        Unallocated   : RTEMS.Unsigned32;
     end record;

   procedure Get_Class_Information(
      The_API   : in     RTEMS.Unsigned32;
      The_Class : in     RTEMS.Unsigned32;
      Info      :    out API_Class_Information;
      Result    :    out RTEMS.Status_Codes
   );

end RTEMS.Object;
