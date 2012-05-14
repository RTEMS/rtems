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

package body RTEMS.Fatal is

   --
   -- Fatal Error Manager
   --

   procedure Error_Occurred (The_Error : in RTEMS.Unsigned32) is
      procedure Error_Occurred_Base (The_Error : RTEMS.Unsigned32);
      pragma Import (C, Error_Occurred_Base, "rtems_fatal_error_occurred");
   begin

      Error_Occurred_Base (The_Error);

   end Error_Occurred;

end RTEMS.Fatal;
