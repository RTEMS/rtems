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
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with System;
with System.Storage_Elements; use System.Storage_Elements;
with Interfaces;
with Interfaces.C;

package RTEMS.IO is

   --
   --  Input/Output Manager
   --

   procedure Register_Name (
      Name   : in     String;
      Major  : in     RTEMS.Device_Major_Number;
      Minor  : in     RTEMS.Device_Minor_Number;
      Result :    out RTEMS.Status_Codes
   );

   procedure Lookup_Name (
      Name         : in     String;
      Device_Info  :    out RTEMS.Driver_Name_t;
      Result       :    out RTEMS.Status_Codes
   );

   procedure Open (
      Major        : in     RTEMS.Device_Major_Number;
      Minor        : in     RTEMS.Device_Minor_Number;
      Argument     : in     RTEMS.Address;
      Result       :    out RTEMS.Status_Codes
   );
   pragma Inline (Open);

   procedure Close (
      Major        : in     RTEMS.Device_Major_Number;
      Minor        : in     RTEMS.Device_Minor_Number;
      Argument     : in     RTEMS.Address;
      Result       :    out RTEMS.Status_Codes
   );
   pragma Inline (Close);

   procedure Read (
      Major        : in     RTEMS.Device_Major_Number;
      Minor        : in     RTEMS.Device_Minor_Number;
      Argument     : in     RTEMS.Address;
      Result       :    out RTEMS.Status_Codes
   );
   pragma Inline (Read);

   procedure Write (
      Major        : in     RTEMS.Device_Major_Number;
      Minor        : in     RTEMS.Device_Minor_Number;
      Argument     : in     RTEMS.Address;
      Result       :    out RTEMS.Status_Codes
   );
   pragma Inline (Write);

   procedure Control (
      Major        : in     RTEMS.Device_Major_Number;
      Minor        : in     RTEMS.Device_Minor_Number;
      Argument     : in     RTEMS.Address;
      Result       :    out RTEMS.Status_Codes
   );
   pragma Inline (Control);

end RTEMS.IO;

