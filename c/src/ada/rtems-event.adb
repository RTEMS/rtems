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

package body RTEMS.Event is

   --
   -- Event Manager
   --

   procedure Send
     (ID       : in RTEMS.ID;
      Event_In : in RTEMS.Event_Set;
      Result   : out RTEMS.Status_Codes)
   is
      function Send_Base
        (ID       : RTEMS.ID;
         Event_In : RTEMS.Event_Set)
         return     RTEMS.Status_Codes;
      pragma Import (C, Send_Base, "rtems_event_send");
   begin

      Result := Send_Base (ID, Event_In);

   end Send;

   procedure Receive
     (Event_In   : in RTEMS.Event_Set;
      Option_Set : in RTEMS.Option;
      Ticks      : in RTEMS.Interval;
      Event_Out  : out RTEMS.Event_Set;
      Result     : out RTEMS.Status_Codes)
   is
      function Receive_Base
        (Event_In   : RTEMS.Event_Set;
         Option_Set : RTEMS.Option;
         Ticks      : RTEMS.Interval;
         Event_Out  : access RTEMS.Event_Set)
         return       RTEMS.Status_Codes;
      pragma Import (C, Receive_Base, "rtems_event_receive");
      Event_Out_Base : aliased RTEMS.Event_Set;
   begin

      Result    :=
         Receive_Base
           (Event_In,
            Option_Set,
            Ticks,
            Event_Out_Base'Access);
      Event_Out := Event_Out_Base;

   end Receive;

end RTEMS.Event;
