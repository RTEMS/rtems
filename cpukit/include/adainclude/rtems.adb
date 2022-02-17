-- SPDX-License-Identifier: BSD-2-Clause

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

with Ada;
with Ada.Unchecked_Conversion;
with Interfaces;               use Interfaces;
with Interfaces.C;             use Interfaces.C;

package body RTEMS is

   --
   --  Utility Functions
   --

   function From_Ada_Boolean
     (Ada_Boolean : Standard.Boolean)
      return        RTEMS.Boolean
   is
   begin

      if Ada_Boolean then
         return RTEMS.True;
      end if;

      return RTEMS.False;

   end From_Ada_Boolean;

   function To_Ada_Boolean
     (RTEMS_Boolean : RTEMS.Boolean)
      return          Standard.Boolean
   is
   begin

      if RTEMS_Boolean = RTEMS.True then
         return Standard.True;
      end if;

      return Standard.False;

   end To_Ada_Boolean;

   function Milliseconds_To_Microseconds
     (Milliseconds : RTEMS.Unsigned32)
      return         RTEMS.Unsigned32
   is
   begin

      return Milliseconds * 1000;

   end Milliseconds_To_Microseconds;

   function Microseconds_To_Ticks
     (Microseconds : RTEMS.Unsigned32)
      return         RTEMS.Interval
   is
      function Microseconds_Per_Tick return  RTEMS.Unsigned32;
      pragma Import (C, Microseconds_Per_Tick, "_ada_microseconds_per_tick");
   begin

      return Microseconds / Microseconds_Per_Tick;

   end Microseconds_To_Ticks;

   function Milliseconds_To_Ticks
     (Milliseconds : RTEMS.Unsigned32)
      return         RTEMS.Interval
   is
   begin

      return Microseconds_To_Ticks
               (Milliseconds_To_Microseconds (Milliseconds));

   end Milliseconds_To_Ticks;

   procedure Name_To_Characters
     (Name : in RTEMS.Name;
      C1   : out Character;
      C2   : out Character;
      C3   : out Character;
      C4   : out Character)
   is
      C1_Value : RTEMS.Unsigned32;
      C2_Value : RTEMS.Unsigned32;
      C3_Value : RTEMS.Unsigned32;
      C4_Value : RTEMS.Unsigned32;
   begin

      C1_Value := Interfaces.Shift_Right (Name, 24);
      C2_Value := Interfaces.Shift_Right (Name, 16);
      C3_Value := Interfaces.Shift_Right (Name, 8);
      C4_Value := Name;

      C1_Value := C1_Value and 16#00FF#;
      C2_Value := C2_Value and 16#00FF#;
      C3_Value := C3_Value and 16#00FF#;
      C4_Value := C4_Value and 16#00FF#;

      C1 := Character'Val (C1_Value);
      C2 := Character'Val (C2_Value);
      C3 := Character'Val (C3_Value);
      C4 := Character'Val (C4_Value);

   end Name_To_Characters;

   function Get_Node (ID : in RTEMS.ID) return RTEMS.Unsigned32 is
   begin

      -- May not be right
      return Interfaces.Shift_Right (ID, 16);

   end Get_Node;

   function Get_Index (ID : in RTEMS.ID) return RTEMS.Unsigned32 is
   begin

      -- May not be right
      return ID and 16#FFFF#;

   end Get_Index;

   function Are_Statuses_Equal
     (Status  : in RTEMS.Status_Codes;
      Desired : in RTEMS.Status_Codes)
      return    Standard.Boolean
   is
   begin

      if Status = Desired then
         return Standard.True;
      end if;

      return Standard.False;

   end Are_Statuses_Equal;

   function Is_Status_Successful
     (Status : in RTEMS.Status_Codes)
      return   Standard.Boolean
   is
   begin

      if Status = RTEMS.Successful then
         return Standard.True;
      end if;

      return Standard.False;

   end Is_Status_Successful;

   function Subtract
     (Left  : in RTEMS.Address;
      Right : in RTEMS.Address)
      return  RTEMS.Unsigned32
   is
      function To_Unsigned32 is new Ada.Unchecked_Conversion (
         System.Address,
         RTEMS.Unsigned32);

   begin
      return To_Unsigned32 (Left) - To_Unsigned32 (Right);
   end Subtract;

   function Are_Equal
     (Left  : in RTEMS.Address;
      Right : in RTEMS.Address)
      return  Standard.Boolean
   is
      function To_Unsigned32 is new Ada.Unchecked_Conversion (
         System.Address,
         RTEMS.Unsigned32);

   begin
      return (To_Unsigned32 (Left) = To_Unsigned32 (Right));
   end Are_Equal;

   --
   --
   --  RTEMS API
   --

   function Build_Name (
      C1 : in     Character;
      C2 : in     Character;
      C3 : in     Character;
      C4 : in     Character
   ) return RTEMS.Name is
      C1_Value : RTEMS.Unsigned32;
      C2_Value : RTEMS.Unsigned32;
      C3_Value : RTEMS.Unsigned32;
      C4_Value : RTEMS.Unsigned32;
   begin

     C1_Value := Character'Pos( C1 );
     C2_Value := Character'Pos( C2 );
     C3_Value := Character'Pos( C3 );
     C4_Value := Character'Pos( C4 );

     return Interfaces.Shift_Left( C1_Value, 24 ) or
            Interfaces.Shift_Left( C2_Value, 16 ) or
            Interfaces.Shift_Left( C3_Value, 8 )  or
            C4_Value;

   end Build_Name;

   --
   --  Initialization Manager -- Shutdown Only
   --
   procedure Shutdown_Executive (Status : in RTEMS.Unsigned32) is
      procedure Shutdown_Executive_Base (Status : RTEMS.Unsigned32);
      pragma Import (C, Shutdown_Executive_Base, "rtems_shutdown_executive");
   begin
      Shutdown_Executive_Base (Status);
   end Shutdown_Executive;

   function Minimum_Stack_Size return RTEMS.Size is
      size : RTEMS.Unsigned32;
      pragma Import (C, size, "rtems_minimum_stack_size");
   begin
      return RTEMS.Size (size);
   end Minimum_Stack_Size;

end RTEMS;
