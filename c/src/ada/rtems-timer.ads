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
--  COPYRIGHT (c) 1997-2008.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with RTEMS.Tasks;

package RTEMS.Timer is

   --
   --  The following type define a pointer to a watchdog/timer service routine.
   --

   type Service_Routine is access procedure (
      ID          : in     RTEMS.ID;
      User_Data   : in     RTEMS.Address
   );
   pragma Convention (C, Service_Routine);

   --
   --  Timer Manager
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

   procedure Fire_After (
      ID        : in     RTEMS.ID;
      Ticks     : in     RTEMS.Interval;
      Routine   : in     RTEMS.Timer.Service_Routine;
      User_Data : in     RTEMS.Address;
      Result    :    out RTEMS.Status_Codes
   );

   procedure Server_Fire_After (
      ID        : in     RTEMS.ID;
      Ticks     : in     RTEMS.Interval;
      Routine   : in     RTEMS.Timer.Service_Routine;
      User_Data : in     RTEMS.Address;
      Result    :    out RTEMS.Status_Codes
   );

   procedure Fire_When (
      ID        : in     RTEMS.ID;
      Wall_Time : in     RTEMS.Time_Of_Day;
      Routine   : in     RTEMS.Timer.Service_Routine;
      User_Data : in     RTEMS.Address;
      Result    :    out RTEMS.Status_Codes
   );

   procedure Server_Fire_When (
      ID        : in     RTEMS.ID;
      Wall_Time : in     RTEMS.Time_Of_Day;
      Routine   : in     RTEMS.Timer.Service_Routine;
      User_Data : in     RTEMS.Address;
      Result    :    out RTEMS.Status_Codes
   );

   procedure Reset (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Cancel (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Initiate_Server (
      Server_Priority : in     RTEMS.Tasks.Priority;
      Stack_Size      : in     Unsigned32;
      Attribute_Set   : in     RTEMS.Attribute;
      Result          :    out RTEMS.Status_Codes
   );

end RTEMS.Timer;
