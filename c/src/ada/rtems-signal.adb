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

package body RTEMS.Signal is

   --
   -- Signal Manager
   --

   procedure Catch
     (ASR_Handler : in RTEMS.ASR_Handler;
      Mode_Set    : in RTEMS.Mode;
      Result      : out RTEMS.Status_Codes)
   is
      function Catch_Base
        (ASR_Handler : RTEMS.ASR_Handler;
         Mode_Set    : RTEMS.Mode)
         return        RTEMS.Status_Codes;
      pragma Import (C, Catch_Base, "rtems_signal_catch");
   begin

      Result := Catch_Base (ASR_Handler, Mode_Set);

   end Catch;

   procedure Send
     (ID         : in RTEMS.ID;
      Signal_Set : in RTEMS.Signal_Set;
      Result     : out RTEMS.Status_Codes)
   is
      function Send_Base
        (ID         : RTEMS.ID;
         Signal_Set : RTEMS.Signal_Set)
         return       RTEMS.Status_Codes;
      pragma Import (C, Send_Base, "rtems_signal_send");
   begin

      Result := Send_Base (ID, Signal_Set);

   end Send;

end RTEMS.Signal;
