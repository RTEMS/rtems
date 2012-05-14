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

package RTEMS.Event is

   --
   --  Event Manager
   --

   procedure Send (
      ID       : in     RTEMS.ID;
      Event_In : in     RTEMS.Event_Set;
      Result   :    out RTEMS.Status_Codes
   );

   procedure Receive (
      Event_In   : in     RTEMS.Event_Set;
      Option_Set : in     RTEMS.Option;
      Ticks      : in     RTEMS.Interval;
      Event_Out  :    out RTEMS.Event_Set;
      Result     :    out RTEMS.Status_Codes
   );

end RTEMS.Event;

