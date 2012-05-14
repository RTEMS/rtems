--
--  RTEMS Multiprocessing Manager/ Body
--  
--  DESCRIPTION:
--  
--  This package provides the interface to the Multiprocessing Manager
--  of the RTEMS API.
--
--  DEPENDENCIES:
--
--
--  COPYRIGHT (c) 1997.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

package body RTEMS.Multiprocessing is

   --
   --  Announce
   --

   procedure Announce is
      procedure Multiprocessing_Announce_Base;
      pragma Import (C, Multiprocessing_Announce_Base,
         "rtems_multiprocessing_announce");
   begin

      Multiprocessing_Announce_Base;

   end Announce;

end RTEMS.Multiprocessing;
