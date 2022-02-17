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
--  COPYRIGHT (c) 1997-2008.
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

package RTEMS.Tasks is

   subtype Priority       is RTEMS.Unsigned32;

   Current_Priority : constant Priority := 0;
   No_Priority      : constant Priority := 0;

   subtype Argument       is RTEMS.Unsigned32;
   type Argument_PTR      is access all Argument;

   type Entry_Point is access procedure (
      Argument : RTEMS.Unsigned32
   );
   pragma Convention (C, Entry_Point);


   --
   --  Task Manager
   --

   procedure Create (
      Name             : in     RTEMS.Name;
      Initial_Priority : in     Priority;
      Stack_Size       : in     RTEMS.Size;
      Initial_Modes    : in     RTEMS.Mode;
      Attribute_Set    : in     RTEMS.Attribute;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   );

   procedure Ident (
      Name   : in     RTEMS.Name;
      Node   : in     RTEMS.Node;
      ID     :    out RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Start (
      ID          : in     RTEMS.ID;
      Entry_Point : in     RTEMS.Tasks.Entry_Point;
      Argument    : in     RTEMS.Tasks.Argument;
      Result      :    out RTEMS.Status_Codes
   );

   procedure Restart (
      ID       : in     RTEMS.ID;
      Argument : in     RTEMS.Tasks.Argument;
      Result   :    out RTEMS.Status_Codes
   );

   procedure Delete (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Suspend (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Resume (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Is_Suspended (
      ID     : in     RTEMS.ID;
      Result :    out RTEMS.Status_Codes
   );

   procedure Set_Priority (
      ID           : in     RTEMS.ID;
      New_Priority : in     Priority;
      Old_Priority :    out Priority;
      Result       :    out RTEMS.Status_Codes
   );

   procedure Mode (
      Mode_Set          : in     RTEMS.Mode;
      Mask              : in     RTEMS.Mode;
      Previous_Mode_Set :    out RTEMS.Mode;
      Result            :    out RTEMS.Status_Codes
   );

   procedure Wake_When (
      Time_Buffer : in     RTEMS.Time_Of_Day;
      Result      :    out RTEMS.Status_Codes
   );

   procedure Wake_After (
      Ticks  : in     RTEMS.Interval;
      Result :    out RTEMS.Status_Codes
   );

end RTEMS.Tasks;

