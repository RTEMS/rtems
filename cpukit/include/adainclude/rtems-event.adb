-- SPDX-License-Identifier: BSD-2-Clause

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
--  Redistribution and use in source and binary forms, with or without
--  modification, are permitted provided that the following conditions
--  are met:
--  1. Redistributions of source code must retain the above copyright
--     notice, this list of conditions and the following disclaimer.
--  2. Redistributions in binary form must reproduce the above copyright
--     notice, this list of conditions and the following disclaimer in the
--     documentation and/or other materials provided with the distribution.
--
--  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
--  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
--  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
--  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
--  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
--  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
--  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
--  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
--  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
--  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
--  POSSIBILITY OF SUCH DAMAGE.
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
