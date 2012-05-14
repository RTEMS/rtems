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

package RTEMS.Interrupt is

   --
   --  Interrupt Manager
   --

   function Disable return RTEMS.ISR_Level;
   pragma Interface (C, Disable);
   pragma Interface_Name (Disable, "rtems_interrupt_disable");

   procedure Enable (
      Level : in     RTEMS.ISR_Level
   );
   pragma Interface (C, Enable);
   pragma Interface_Name (Enable, "rtems_interrupt_enable");

   procedure Flash (
      Level : in     RTEMS.ISR_Level
   );
   pragma Interface (C, Flash);
   pragma Interface_Name (Flash, "rtems_interrupt_flash");

   function Is_In_Progress return RTEMS.Boolean;
   pragma Interface (C, Is_In_Progress);
   pragma Interface_Name (Is_In_Progress, "rtems_interrupt_is_in_progress");

end RTEMS.Interrupt;

