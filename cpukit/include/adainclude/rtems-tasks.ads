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
--  http://www.rtems.org/license/LICENSE.
--

package RTEMS.Tasks is

   subtype Priority       is RTEMS.Unsigned32;

   Current_Priority : constant Priority := 0;
   No_Priority      : constant Priority := 0;

   subtype Argument       is RTEMS.Unsigned32;
   type Argument_PTR      is access all Argument;

   type Entry_Point is access procedure (
      Argument : RTEMS.Unsigned32
   );
   pragma Convention (C, Entry_Point);


   --
   --  Task Manager
   --

   procedure Create (
      Name             : in     RTEMS.Name;
      Initial_Priority : in     Priority;
      Stack_Size       : in     RTEMS.Size;
      Initial_Modes    : in     RTEMS.Mode;
      Attribute_Set    : in     RTEMS.Attribute;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   );

   procedure Ident (
      Name   : in     RTEMS.Name;
      Node   : in     RTEMS.Node;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Start (
      ID          : in     RTEMS.ID;
      Entry_Point : in     RTEMS.Tasks.Entry_Point;
      Argument    : in     RTEMS.Tasks.Argument;
      Result      :    out RTEMS.Status_Codes
   );

   procedure Restart (
      ID       : in     RTEMS.ID;
      Argument : in     RTEMS.Tasks.Argument;
      Result   :    out RTEMS.Status_Codes
   );

   procedure Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Suspend (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Resume (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Is_Suspended (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Set_Priority (
      ID           : in     RTEMS.ID;
      New_Priority : in     Priority;
      Old_Priority :    out Priority;
      Result       :    out RTEMS.Status_Codes
   );

   procedure Mode (
      Mode_Set          : in     RTEMS.Mode;
      Mask              : in     RTEMS.Mode;
      Previous_Mode_Set :    out RTEMS.Mode;
      Result            :    out RTEMS.Status_Codes
   );

   procedure Wake_When (
      Time_Buffer : in     RTEMS.Time_Of_Day;
      Result      :    out RTEMS.Status_Codes
   );

   procedure Wake_After (
      Ticks  : in     RTEMS.Interval;
      Result :    out RTEMS.Status_Codes
   );

end RTEMS.Tasks;

