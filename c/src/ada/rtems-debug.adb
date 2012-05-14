--
--  RTEMS / Body
--
--  DESCRIPTION:
--
--  This package provides the interface to the RTEMS API.
--
--
--  DEPENDENCIES:
--
--
--
--  COPYRIGHT (c) 1997-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

package body RTEMS.Debug is

   --
   -- Debug Manager
   --

   procedure Enable (To_Be_Enabled : in Set) is
      procedure Enable_Base (To_Be_Enabled : Set);
      pragma Import (C, Enable_Base, "rtems_debug_enable");
   begin

      Enable_Base (To_Be_Enabled);

   end Enable;

   procedure Disable (To_Be_Disabled : in Set) is
      procedure Disable_Base (To_Be_Disabled : Set);
      pragma Import (C, Disable_Base, "rtems_debug_disable");
   begin

      Disable_Base (To_Be_Disabled);

   end Disable;

   function Is_Enabled
     (Level : in Set)
      return  RTEMS.Boolean
   is
      function Is_Enabled_Base
        (Level : Set)
         return  RTEMS.Boolean;
      pragma Import (C, Is_Enabled_Base, "rtems_debug_is_enabled");
   begin

      return Is_Enabled_Base (Level);

   end Is_Enabled;

end RTEMS.Debug;
