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

package RTEMS.Region is

   --
   --  Region Manager
   --

   procedure Create (
      Name             : in     RTEMS.Name;
      Starting_Address : in     RTEMS.Address;
      Length           : in     RTEMS.Unsigned32;
      Page_Size        : in     RTEMS.Unsigned32;
      Attribute_Set    : in     RTEMS.Attribute;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
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

   procedure Extend (
      ID               : in     RTEMS.ID;
      Starting_Address : in     RTEMS.Address;
      Length           : in     RTEMS.Unsigned32;
      Result           :    out RTEMS.Status_Codes
   );

   procedure Get_Segment (
      ID         : in     RTEMS.ID;
      Size       : in     RTEMS.Unsigned32;
      Option_Set : in     RTEMS.Option;
      Timeout    : in     RTEMS.Interval;
      Segment    :    out RTEMS.Address;
      Result     :    out RTEMS.Status_Codes
   );

   procedure Get_Segment_Size (
      ID         : in     RTEMS.ID;
      Segment    : in     RTEMS.Address;
      Size       :    out RTEMS.Unsigned32;
      Result     :    out RTEMS.Status_Codes
   );

   procedure Return_Segment (
      ID      : in     RTEMS.ID;
      Segment : in     RTEMS.Address;
      Result  :    out RTEMS.Status_Codes
   );

   procedure Resize_Segment (
      ID         : in     RTEMS.ID;
      Segment    : in     RTEMS.Address;
      Size       : in     RTEMS.Unsigned32;
      Old_Size   :    out RTEMS.Unsigned32;
      Result     :    out RTEMS.Status_Codes
   );

end RTEMS.Region;

