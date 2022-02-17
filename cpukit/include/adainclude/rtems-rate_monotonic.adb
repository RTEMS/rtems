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

package body RTEMS.Rate_Monotonic is

   --
   -- Rate Monotonic Manager
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
      pragma Import
        (C,
         Create_Base,
         "rtems_rate_monotonic_create");
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
      pragma Import
        (C,
         Ident_Base,
         "rtems_rate_monotonic_ident");
      ID_Base : aliased RTEMS.ID;
   begin

      Result := Ident_Base (Name, ID_Base'Access);

      ID := ID_Base;

   end Ident;

   procedure Delete
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Delete_Base
        (ID   : RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import
        (C,
         Delete_Base,
         "rtems_rate_monotonic_delete");
   begin

      Result := Delete_Base (ID);

   end Delete;

   procedure Cancel
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Cancel_Base
        (ID   : RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import
        (C,
         Cancel_Base,
         "rtems_rate_monotonic_cancel");
   begin

      Result := Cancel_Base (ID);

   end Cancel;

   procedure Period
     (ID     : in RTEMS.ID;
      Length : in RTEMS.Interval;
      Result : out RTEMS.Status_Codes)
   is
      function Period_Base
        (ID     : RTEMS.ID;
         Length : RTEMS.Interval)
         return   RTEMS.Status_Codes;
      pragma Import
        (C,
         Period_Base,
         "rtems_rate_monotonic_period");
   begin

      Result := Period_Base (ID, Length);

   end Period;

   procedure Get_Status
     (ID     : in RTEMS.ID;
      Status : out RTEMS.Rate_Monotonic.Period_Status;
      Result : out RTEMS.Status_Codes)
   is
      function Get_Status_Base
        (ID     : RTEMS.ID;
         Status : access RTEMS.Rate_Monotonic.Period_Status)
         return   RTEMS.Status_Codes;
      pragma Import
        (C,
         Get_Status_Base,
         "rtems_rate_monotonic_get_status");

      Status_Base : aliased RTEMS.Rate_Monotonic.Period_Status;
   begin

      Result := Get_Status_Base (ID, Status_Base'Access);

      Status := Status_Base;

   end Get_Status;

   procedure Reset_Statistics
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Reset_Statistics_Base
        (ID   : RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import
        (C,
         Reset_Statistics_Base,
         "rtems_rate_monotonic_reset_statistics");
   begin

      Result := Reset_Statistics_Base (ID);

   end Reset_Statistics;

end RTEMS.Rate_Monotonic;
