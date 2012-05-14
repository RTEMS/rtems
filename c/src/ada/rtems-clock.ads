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


package RTEMS.Clock is

   --
   --  Clock Types
   --

   type Time_Value is
      record
         Seconds      : RTEMS.Unsigned32;
         Microseconds : RTEMS.Unsigned32;
      end record;

   type Clock_Get_Options is (
      Clock_Get_TOD,
      Clock_Get_Seconds_Since_Epoch,
      Clock_Get_Ticks_Since_Boot,
      Clock_Get_Ticks_Per_Second,
      Clock_Get_Time_Value
   );

   type Get_Options is (
      Get_TOD,
      Get_Seconds_Since_Epoch,
      Get_Ticks_Since_Boot,
      Get_Ticks_Per_Second,
      Get_Time_Value
   );

   --
   --  Clock Manager
   --

   procedure Set (
      Time_Buffer : in     RTEMS.Time_Of_Day;
      Result      :    out RTEMS.Status_Codes
   );

   procedure Get (
      Option      : in     RTEMS.Clock.Get_Options;
      Time_Buffer : in     RTEMS.Address;
      Result      :    out RTEMS.Status_Codes
   );

   procedure Get_TOD (
      Time   :    out RTEMS.Time_Of_Day;
      Result :    out RTEMS.Status_Codes
   );

   procedure Get_TOD_Time_Value (
      Time   :    out RTEMS.Time_Value;
      Result :    out RTEMS.Status_Codes
   );

   procedure Get_Seconds_Since_Epoch(
      The_Interval :    out RTEMS.Interval;
      Result       :    out RTEMS.Status_Codes
   );

   function Get_Ticks_Per_Second
   return RTEMS.Interval;
   pragma Import (
      C,
      Get_Ticks_Per_Second,
      "rtems_clock_get_ticks_per_second"
   );

   function Get_Ticks_Since_Boot
   return RTEMS.Interval;
   pragma Import (
      C,
      Get_Ticks_Since_Boot,
      "rtems_clock_get_ticks_since_boot"
   );

   procedure Get_Uptime (
      Uptime :    out RTEMS.Timespec;
      Result :    out RTEMS.Status_Codes
   );

   procedure Tick (
      Result :    out RTEMS.Status_Codes
   );

end RTEMS.Clock;

