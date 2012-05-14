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

package RTEMS.Barrier is

   --
   --  Barrier Manager
   --

   procedure Barrier_Create (
      Name            : in     RTEMS.Name;
      Attribute_Set   : in     RTEMS.Attribute;
      Maximum_Waiters : in     RTEMS.Unsigned32;
      ID              :    out RTEMS.ID;
      Result          :    out RTEMS.Status_Codes
   );

   procedure Barrier_Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Barrier_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Barrier_Wait (
      ID         : in     RTEMS.ID;
      Timeout    : in     RTEMS.Interval;
      Result     :    out RTEMS.Status_Codes
   );

   procedure Barrier_Release (
      ID       : in     RTEMS.ID;
      Released :    out RTEMS.Unsigned32;
      Result   :    out RTEMS.Status_Codes
   );

end RTEMS.Barrier;

