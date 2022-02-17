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
