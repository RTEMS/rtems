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

package body RTEMS.Message_Queue is

   --
   -- Message Queue Manager
   --

   procedure Create
     (Name             : in RTEMS.Name;
      Count            : in RTEMS.Unsigned32;
      Max_Message_Size : in RTEMS.Size;
      Attribute_Set    : in RTEMS.Attribute;
      ID               : out RTEMS.ID;
      Result           : out RTEMS.Status_Codes)
   is
      --  XXX broken
      function Create_Base
        (Name             : RTEMS.Name;
         Count            : RTEMS.Unsigned32;
         Max_Message_Size : RTEMS.Size;
         Attribute_Set    : RTEMS.Attribute;
         ID               : access RTEMS.ID)
         return             RTEMS.Status_Codes;
      pragma Import
        (C,
         Create_Base,
         "rtems_message_queue_create");
      ID_Base : aliased RTEMS.ID;
   begin

      Result :=
         Create_Base
           (Name,
            Count,
            Max_Message_Size,
            Attribute_Set,
            ID_Base'Access);
      ID     := ID_Base;

   end Create;

   procedure Ident
     (Name   : in RTEMS.Name;
      Node   : in RTEMS.Unsigned32;
      ID     : out RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Ident_Base
        (Name : RTEMS.Name;
         Node : RTEMS.Unsigned32;
         ID   : access RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import
        (C,
         Ident_Base,
         "rtems_message_queue_ident");
      ID_Base : aliased RTEMS.ID;
   begin

      Result := Ident_Base (Name, Node, ID_Base'Access);
      ID     := ID_Base;

   end Ident;

   procedure Delete
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Delete_Base
        (ID   : RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import
        (C,
         Delete_Base,
         "rtems_message_queue_delete");
   begin

      Result := Delete_Base (ID);

   end Delete;

   procedure Send
     (ID     : in RTEMS.ID;
      Buffer : in RTEMS.Address;
      Size   : in RTEMS.Size;
      Result : out RTEMS.Status_Codes)
   is
      function Send_Base
        (ID     : RTEMS.ID;
         Buffer : RTEMS.Address;
         Size   : RTEMS.Size)
         return   RTEMS.Status_Codes;
      pragma Import (C, Send_Base, "rtems_message_queue_send");
   begin

      Result := Send_Base (ID, Buffer, Size);

   end Send;

   procedure Urgent
     (ID     : in RTEMS.ID;
      Buffer : in RTEMS.Address;
      Size   : in RTEMS.Size;
      Result : out RTEMS.Status_Codes)
   is
      function Urgent_Base
        (ID     : RTEMS.ID;
         Buffer : RTEMS.Address;
         Size   : RTEMS.Size)
         return   RTEMS.Status_Codes;
      pragma Import
        (C,
         Urgent_Base,
         "rtems_message_queue_urgent");
   begin

      Result := Urgent_Base (ID, Buffer, Size);

   end Urgent;

   procedure Broadcast
     (ID     : in RTEMS.ID;
      Buffer : in RTEMS.Address;
      Size   : in RTEMS.Size;
      Count  : out RTEMS.Unsigned32;
      Result : out RTEMS.Status_Codes)
   is
      function Broadcast_Base
        (ID     : RTEMS.ID;
         Buffer : RTEMS.Address;
         Size   : RTEMS.Size;
         Count  : access RTEMS.Unsigned32)
         return   RTEMS.Status_Codes;
      pragma Import
        (C,
         Broadcast_Base,
         "rtems_message_queue_broadcast");
      Count_Base : aliased RTEMS.Unsigned32;
   begin

      Result :=
         Broadcast_Base (ID, Buffer, Size, Count_Base'Access);
      Count  := Count_Base;

   end Broadcast;

   procedure Receive
     (ID         : in RTEMS.ID;
      Buffer     : in RTEMS.Address;
      Option_Set : in RTEMS.Option;
      Timeout    : in RTEMS.Interval;
      Size       : in out RTEMS.Size;
      Result     : out RTEMS.Status_Codes)
   is
      function Receive_Base
        (ID         : RTEMS.ID;
         Buffer     : RTEMS.Address;
         Size       : access RTEMS.Size;
         Option_Set : RTEMS.Option;
         Timeout    : RTEMS.Interval)
         return       RTEMS.Status_Codes;
      pragma Import
        (C,
         Receive_Base,
         "rtems_message_queue_receive");
      Size_Base : aliased RTEMS.Size;
   begin

      Size_Base := Size;

      Result :=
         Receive_Base
           (ID,
            Buffer,
            Size_Base'Access,
            Option_Set,
            Timeout);
      Size   := Size_Base;

   end Receive;

   procedure Get_Number_Pending
     (ID     : in RTEMS.ID;
      Count  : out RTEMS.Unsigned32;
      Result : out RTEMS.Status_Codes)
   is
      function Get_Number_Pending_Base
        (ID    : RTEMS.ID;
         Count : access RTEMS.Unsigned32)
         return  RTEMS.Status_Codes;
      pragma Import
        (C,
         Get_Number_Pending_Base,
         "rtems_message_queue_get_number_pending");
      Count_Base : aliased RTEMS.Unsigned32;
   begin

      Result := Get_Number_Pending_Base (ID, Count_Base'Access);
      Count  := Count_Base;

   end Get_Number_Pending;

   procedure Flush
     (ID     : in RTEMS.ID;
      Count  : out RTEMS.Unsigned32;
      Result : out RTEMS.Status_Codes)
   is
      function Flush_Base
        (ID    : RTEMS.ID;
         Count : access RTEMS.Unsigned32)
         return  RTEMS.Status_Codes;
      pragma Import
        (C,
         Flush_Base,
         "rtems_message_queue_flush");
      Count_Base : aliased RTEMS.Unsigned32;
   begin

      Result := Flush_Base (ID, Count_Base'Access);
      Count  := Count_Base;

   end Flush;

end RTEMS.Message_Queue;
