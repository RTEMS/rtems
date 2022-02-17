-- SPDX-License-Identifier: BSD-2-Clause

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

with System;
with System.Storage_Elements; use System.Storage_Elements;
with Interfaces;
with Interfaces.C;

package RTEMS.Barrier is

   --
   --  Barrier Manager
   --

   procedure Barrier_Create (
      Name            : in     RTEMS.Name;
      Attribute_Set   : in     RTEMS.Attribute;
      Maximum_Waiters : in     RTEMS.Unsigned32;
      ID              :    out RTEMS.ID;
      Result          :    out RTEMS.Status_Codes
   );

   procedure Barrier_Ident (
      Name   : in     RTEMS.Name;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Barrier_Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Barrier_Wait (
      ID         : in     RTEMS.ID;
      Timeout    : in     RTEMS.Interval;
      Result     :    out RTEMS.Status_Codes
   );

   procedure Barrier_Release (
      ID       : in     RTEMS.ID;
      Released :    out RTEMS.Unsigned32;
      Result   :    out RTEMS.Status_Codes
   );

end RTEMS.Barrier;

