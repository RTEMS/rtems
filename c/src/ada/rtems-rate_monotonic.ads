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

package RTEMS.Rate_Monotonic is

   --
   --  The following type defines the status information returned
   --  about a period.
   --

   type Period_States is (
     Inactive,               -- off chain, never initialized
     Owner_Is_Blocking,      -- on chain, owner is blocking on it
     Active,                 -- on chain, running continuously
     Expired_While_Blocking, -- on chain, expired while owner was was blocking
     Expired                 -- off chain, will be reset by next
                             --   rtems_rate_monotonic_period
   );

   for Period_States'Size use 32;

   for Period_States use (
     Inactive                => 0,
     Owner_Is_Blocking       => 1,
     Active                  => 2,
     Expired_While_Blocking  => 3,
     Expired                 => 4
   );

   type Period_Status is
      record
         Owner                            : RTEMS.ID;
         State                            : RTEMS.Rate_Monotonic.Period_States;
         Ticks_Since_Last_Period          : RTEMS.Unsigned32;
         Ticks_Executed_Since_Last_Period : RTEMS.Unsigned32;
      end record;

   --
   --  Rate Monotonic Manager
   --

   procedure Create (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Cancel (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Period (
      ID      : in     RTEMS.ID;
      Length  : in     RTEMS.Interval;
      Result  :    out RTEMS.Status_Codes
   );

   procedure Get_Status (
      ID      : in     RTEMS.ID;
      Status  :    out RTEMS.Rate_Monotonic.Period_Status;
      Result  :    out RTEMS.Status_Codes
   );

   procedure Reset_Statistics (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Reset_All_Statistics;
   pragma Import (
      C,
      Reset_All_Statistics,
      "rtems_rate_monotonic_reset_all_statistics"
   );

   procedure Report_Statistics;
   pragma Import (
      C,
      Report_Statistics,
      "rtems_rate_monotonic_report_statistics"
   );

end RTEMS.Rate_Monotonic;

