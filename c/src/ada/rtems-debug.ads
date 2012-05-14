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

package RTEMS.Debug is

   subtype Set is RTEMS.Unsigned32;

   --
   --  Debug Manager
   --

   All_Mask : constant Set := 16#ffffffff#;
   Region   : constant Set := 16#00000001#;

   procedure Enable (
      To_Be_Enabled : in     Set
   );

   procedure Disable (
      To_Be_Disabled : in     Set
   );

   function Is_Enabled (
      Level : in     Set
   ) return RTEMS.Boolean;

end RTEMS.Debug;
