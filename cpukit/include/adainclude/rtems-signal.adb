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
