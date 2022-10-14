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

with Ada;
with Ada.Unchecked_Conversion;
with Interfaces;               use Interfaces;
with Interfaces.C;             use Interfaces.C;
with Interfaces.C.Strings;     use Interfaces.C.Strings;

package body RTEMS.IO is

   --
   -- Input/Output Manager
   --

   procedure Register_Name
     (Name   : in String;
      Major  : in RTEMS.Device_Major_Number;
      Minor  : in RTEMS.Device_Minor_Number;
      Result : out RTEMS.Status_Codes)
   is
      function Register_Name_Base
        (Name  : Interfaces.C.char_array;
         Major : RTEMS.Device_Major_Number;
         Minor : RTEMS.Device_Minor_Number)
         return  RTEMS.Status_Codes;
      pragma Import (C, Register_Name_Base, "rtems_io_register_name");
   begin

      Result := Register_Name_Base (Interfaces.C.To_C (Name), Major, Minor);

   end Register_Name;

   procedure Open
     (Major    : in RTEMS.Device_Major_Number;
      Minor    : in RTEMS.Device_Minor_Number;
      Argument : in RTEMS.Address;
      Result   : out RTEMS.Status_Codes)
   is
      function Open_Base
        (Major    : RTEMS.Device_Major_Number;
         Minor    : RTEMS.Device_Minor_Number;
         Argument : RTEMS.Address)
         return     RTEMS.Status_Codes;
      pragma Import (C, Open_Base, "rtems_io_open");
   begin

      Result := Open_Base (Major, Minor, Argument);

   end Open;
   pragma Inline (Open);

   procedure Close
     (Major    : in RTEMS.Device_Major_Number;
      Minor    : in RTEMS.Device_Minor_Number;
      Argument : in RTEMS.Address;
      Result   : out RTEMS.Status_Codes)
   is
      function Close_Base
        (Major    : RTEMS.Device_Major_Number;
         Minor    : RTEMS.Device_Minor_Number;
         Argument : RTEMS.Address)
         return     RTEMS.Status_Codes;
      pragma Import (C, Close_Base, "rtems_io_close");
   begin

      Result := Close_Base (Major, Minor, Argument);

   end Close;
   pragma Inline (Close);

   procedure Read
     (Major    : in RTEMS.Device_Major_Number;
      Minor    : in RTEMS.Device_Minor_Number;
      Argument : in RTEMS.Address;
      Result   : out RTEMS.Status_Codes)
   is
      function Read_Base
        (Major    : RTEMS.Device_Major_Number;
         Minor    : RTEMS.Device_Minor_Number;
         Argument : RTEMS.Address)
         return     RTEMS.Status_Codes;
      pragma Import (C, Read_Base, "rtems_io_read");
   begin

      Result := Read_Base (Major, Minor, Argument);

   end Read;
   pragma Inline (Read);

   procedure Write
     (Major    : in RTEMS.Device_Major_Number;
      Minor    : in RTEMS.Device_Minor_Number;
      Argument : in RTEMS.Address;
      Result   : out RTEMS.Status_Codes)
   is
      function Write_Base
        (Major    : RTEMS.Device_Major_Number;
         Minor    : RTEMS.Device_Minor_Number;
         Argument : RTEMS.Address)
         return     RTEMS.Status_Codes;
      pragma Import (C, Write_Base, "rtems_io_write");
   begin

      Result := Write_Base (Major, Minor, Argument);

   end Write;
   pragma Inline (Write);

   procedure Control
     (Major    : in RTEMS.Device_Major_Number;
      Minor    : in RTEMS.Device_Minor_Number;
      Argument : in RTEMS.Address;
      Result   : out RTEMS.Status_Codes)
   is
      function Control_Base
        (Major    : RTEMS.Device_Major_Number;
         Minor    : RTEMS.Device_Minor_Number;
         Argument : RTEMS.Address)
         return     RTEMS.Status_Codes;
      pragma Import (C, Control_Base, "rtems_io_control");
   begin

      Result := Control_Base (Major, Minor, Argument);

   end Control;
   pragma Inline (Control);

end RTEMS.IO;
