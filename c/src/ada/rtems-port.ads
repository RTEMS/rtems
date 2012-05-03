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

package RTEMS.Port is

   --
   --  Dual Ported Memory Manager
   --

   procedure Create (
      Name           : in     RTEMS.Name;
      Internal_Start : in     RTEMS.Address;
      External_Start : in     RTEMS.Address;
      Length         : in     RTEMS.Unsigned32;
      ID             :    out RTEMS.ID;
      Result         :    out RTEMS.Status_Codes
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

   procedure External_To_Internal (
      ID       : in     RTEMS.ID;
      External : in     RTEMS.Address;
      Internal :    out RTEMS.Address;
      Result   :    out RTEMS.Status_Codes
   );

   procedure Internal_To_External (
      ID       : in     RTEMS.ID;
      Internal : in     RTEMS.Address;
      External :    out RTEMS.Address;
      Result   :    out RTEMS.Status_Codes
   );

end RTEMS.Port;

