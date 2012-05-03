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

with RTEMS.Tasks;

package RTEMS.Semaphore is

   --
   --  Semaphore Manager
   --

   procedure Create (
      Name             : in     RTEMS.Name;
      Count            : in     RTEMS.Unsigned32;
      Attribute_Set    : in     RTEMS.Attribute;
      Priority_Ceiling : in     RTEMS.Tasks.Priority;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   );

   procedure Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Ident (
      Name   : in     RTEMS.Name;
      Node   : in     RTEMS.Unsigned32;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Obtain (
      ID         : in     RTEMS.ID;
      Option_Set : in     RTEMS.Option;
      Timeout    : in     RTEMS.Interval;
      Result     :    out RTEMS.Status_Codes
   );

   procedure Release (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Flush (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

end RTEMS.Semaphore;

