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

package RTEMS.Partition is

   --
   --  Partition Manager
   --

   procedure Create (
      Name             : in     RTEMS.Name;
      Starting_Address : in     RTEMS.Address;
      Length           : in     RTEMS.Unsigned32;
      Buffer_Size      : in     RTEMS.Unsigned32;
      Attribute_Set    : in     RTEMS.Attribute;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   );

   procedure Ident (
      Name   : in     RTEMS.Name;
      Node   : in     RTEMS.Unsigned32;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Get_Buffer (
      ID     : in     RTEMS.ID;
      Buffer :    out RTEMS.Address;
      Result :    out RTEMS.Status_Codes
   );

   procedure Return_Buffer (
      ID     : in     RTEMS.ID;
      Buffer : in     RTEMS.Address;
      Result :    out RTEMS.Status_Codes
   );

end RTEMS.Partition;

