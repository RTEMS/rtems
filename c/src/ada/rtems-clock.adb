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

package body RTEMS.Clock is

   --
   -- Clock Manager
   --

   procedure Set
     (Time_Buffer : in RTEMS.Time_Of_Day;
      Result      : out RTEMS.Status_Codes)
   is
      function Set_Base
        (Time_Buffer : access RTEMS.Time_Of_Day)
         return        RTEMS.Status_Codes;
      pragma Import (C, Set_Base, "rtems_clock_set");

      Tmp_Time : aliased RTEMS.Time_Of_Day;
   begin

      Tmp_Time := Time_Buffer;
      Result   := Set_Base (Tmp_Time'Access);

   end Set;

   procedure Get
     (Option      : in RTEMS.Clock.Get_Options;
      Time_Buffer : in RTEMS.Address;
      Result      : out RTEMS.Status_Codes)
   is
      function Get_Base
        (Option      : RTEMS.Clock.Get_Options;
         Time_Buffer : RTEMS.Address)
         return        RTEMS.Status_Codes;
      pragma Import (C, Get_Base, "rtems_clock_get");
   begin

      Result := Get_Base (Option, Time_Buffer);

   end Get;

   procedure Get_TOD
     (Time   : out RTEMS.Time_Of_Day;
      Result : out RTEMS.Status_Codes)
   is
      function Get_TOD_Base
        (Time : access RTEMS.Time_Of_Day)
         return RTEMS.Status_Codes;
      pragma Import (C, Get_TOD_Base, "rtems_clock_get_tod");

      Tmp_Time : aliased RTEMS.Time_Of_Day;
   begin
      Result := Get_TOD_Base (Tmp_Time'Access);
      Time   := Tmp_Time;
   end Get_TOD;

   procedure Get_TOD_Time_Value
     (Time   : out RTEMS.Time_Value;
      Result : out RTEMS.Status_Codes)
   is
      function Get_TOD_Time_Value_Base
        (Time : access RTEMS.Time_Value)
         return RTEMS.Status_Codes;
      pragma Import
        (C,
         Get_TOD_Time_Value_Base,
         "rtems_clock_get_tod_timeval");

      Tmp_Time : aliased RTEMS.Time_Value;
   begin
      Result := Get_TOD_Time_Value_Base (Tmp_Time'Access);
      Time   := Tmp_Time;
   end Get_TOD_Time_Value;

   procedure Get_Seconds_Since_Epoch
     (The_Interval : out RTEMS.Interval;
      Result       : out RTEMS.Status_Codes)
   is
      function Get_Seconds_Since_Epoch_Base
        (The_Interval : access RTEMS.Interval)
         return         RTEMS.Status_Codes;
      pragma Import
        (C,
         Get_Seconds_Since_Epoch_Base,
         "rtems_clock_get_seconds_since_epoch");

      Tmp_Interval : aliased RTEMS.Interval;
   begin
      Result       :=
         Get_Seconds_Since_Epoch_Base (Tmp_Interval'Access);
      The_Interval := Tmp_Interval;
   end Get_Seconds_Since_Epoch;

   -- Get_Ticks_Per_Second is in rtems.ads

   -- Get_Ticks_Since_Boot is in rtems.ads

   procedure Get_Uptime
     (Uptime : out RTEMS.Timespec;
      Result : out RTEMS.Status_Codes)
   is
      function Get_Uptime_Base
        (Uptime : access RTEMS.Timespec)
         return   RTEMS.Status_Codes;
      pragma Import (C, Get_Uptime_Base, "rtems_clock_get_uptime");
      Uptime_Base : aliased RTEMS.Timespec;
   begin

      Result := Get_Uptime_Base (Uptime_Base'Access);
      Uptime := Uptime_Base;

   end Get_Uptime;

   procedure Tick (Result : out RTEMS.Status_Codes) is
      function Tick_Base return  RTEMS.Status_Codes;
      pragma Import (C, Tick_Base, "rtems_clock_tick");
   begin

      Result := Tick_Base;

   end Tick;

end RTEMS.Clock;
