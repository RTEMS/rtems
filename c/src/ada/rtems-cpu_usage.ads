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

package RTEMS.CPU_Usage is

   --
   --  CPU Usage Statistics
   --

   procedure Report;
   pragma Import (C, Report, "rtems_cpu_usage_report");

   procedure Reset;
   pragma Import (C, Reset, "rtems_cpu_usage_reset");

end RTEMS.CPU_Usage;

