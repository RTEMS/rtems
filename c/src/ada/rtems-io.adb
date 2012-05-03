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

   procedure Lookup_Name
     (Name        : in String;
      Device_Info : out RTEMS.Driver_Name_t;
      Result      : out RTEMS.Status_Codes)
   is
      function Lookup_Name_Base
        (Name        : Interfaces.C.char_array;
         Device_Info : access RTEMS.Driver_Name_t)
         return        RTEMS.Status_Codes;
      pragma Import (C, Lookup_Name_Base, "rtems_io_lookup_name");
      Device_Info_Base : aliased RTEMS.Driver_Name_t;
   begin

      Result      :=
         Lookup_Name_Base
           (Interfaces.C.To_C (Name),
            Device_Info_Base'Unchecked_Access);
      Device_Info := Device_Info_Base;

   end Lookup_Name;

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
