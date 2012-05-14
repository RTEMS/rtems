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

package RTEMS.Message_Queue is

   --
   --  Message Queue Manager
   --

   procedure Create (
      Name             : in     RTEMS.Name;
      Count            : in     RTEMS.Unsigned32;
      Max_Message_Size : in     RTEMS.Unsigned32;
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

   procedure Send (
      ID     : in     RTEMS.ID;
      Buffer : in     RTEMS.Address;
      Size   : in     RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   );

   procedure Urgent (
      ID     : in     RTEMS.ID;
      Buffer : in     RTEMS.Address;
      Size   : in     RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   );

   procedure Broadcast (
      ID     : in     RTEMS.ID;
      Buffer : in     RTEMS.Address;
      Size   : in     RTEMS.Unsigned32;
      Count  :    out RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   );

   procedure Receive (
      ID         : in     RTEMS.ID;
      Buffer     : in     RTEMS.Address;
      Option_Set : in     RTEMS.Option;
      Timeout    : in     RTEMS.Interval;
      Size       : in out RTEMS.Unsigned32;
      Result     :    out RTEMS.Status_Codes
   );

   procedure Get_Number_Pending (
      ID     : in     RTEMS.ID;
      Count  :    out RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   );

   procedure Flush (
      ID     : in     RTEMS.ID;
      Count  :    out RTEMS.Unsigned32;
      Result :    out RTEMS.Status_Codes
   );

end RTEMS.Message_Queue;

