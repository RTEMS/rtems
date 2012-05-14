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

package RTEMS.Stack_Checker is

   --
   --  Stack Bounds Checker
   --

   function Is_Blown return RTEMS.Boolean;
   pragma Interface (C, Is_Blown);
   pragma Interface_Name (Is_Blown, "rtems_stack_checker_is_blown");

   procedure Report_Usage;
   pragma Import (C, Report_Usage, "rtems_stack_checker_report_usage");

end RTEMS.Stack_Checker;

