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


package RTEMS.Clock is

   --
   --  Clock Types
   --

   type Time_Value is
      record
         Seconds      : RTEMS.Unsigned32;
         Microseconds : RTEMS.Unsigned32;
      end record;

   --
   --  Clock Manager
   --

   procedure Set (
      Time_Buffer : in     RTEMS.Time_Of_Day;
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

   procedure Get_Uptime (
      Uptime :    out RTEMS.Timespec;
      Result :    out RTEMS.Status_Codes
   );

   procedure Tick (
      Result :    out RTEMS.Status_Codes
   );

end RTEMS.Clock;

