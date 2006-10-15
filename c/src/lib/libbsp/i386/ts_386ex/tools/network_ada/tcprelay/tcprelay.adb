-----------------------------------------------------------------------------
--                                                                         --
--                         ADASOCKETS COMPONENTS                           --
--                                                                         --
--                            T C P R E L A Y                              --
--                                                                         --
--                                B o d y                                  --
--                                                                         --
--                        $ReleaseVersion: 0.1.3 $                         --
--                                                                         --
--  Copyright (C) 1998  École Nationale Supérieure des Télécommunications  --
--                                                                         --
--   AdaSockets is free software; you can  redistribute it and/or modify   --
--   it  under terms of the GNU  General  Public License as published by   --
--   the Free Software Foundation; either version 2, or (at your option)   --
--   any later version.   AdaSockets is distributed  in the hope that it   --
--   will be useful, but WITHOUT ANY  WARRANTY; without even the implied   --
--   warranty of MERCHANTABILITY   or FITNESS FOR  A PARTICULAR PURPOSE.   --
--   See the GNU General Public  License  for more details.  You  should   --
--   have received a copy of the  GNU General Public License distributed   --
--   with AdaSockets; see   file COPYING.  If  not,  write  to  the Free   --
--   Software  Foundation, 59   Temple Place -   Suite  330,  Boston, MA   --
--   02111-1307, USA.                                                      --
--                                                                         --
--   As a special exception, if  other  files instantiate generics  from   --
--   this unit, or  you link this  unit with other  files to produce  an   --
--   executable,  this  unit does  not  by  itself cause  the  resulting   --
--   executable to be  covered by the  GNU General Public License.  This   --
--   exception does  not  however invalidate any  other reasons  why the   --
--   executable file might be covered by the GNU Public License.           --
--                                                                         --
--   The main repository for this software is located at:                  --
--       http://www-inf.enst.fr/ANC/                                       --
--                                                                         --
-----------------------------------------------------------------------------

with Ada.Exceptions;   use Ada.Exceptions;
with Ada.Text_IO;      use Ada.Text_IO;
with Sockets;          use Sockets;

procedure TCPRelay is

   Relay_Host : String := "host.domain";

   procedure Print_Error;
   pragma Import (C, Print_Error, "print_error");

   task type Relay is
      pragma Storage_Size (8192);
      entry Start (From, To : Socket_FD);
   end Relay;

   -----------
   -- Relay --
   -----------

   task body Relay
   is
      From_FD, To_FD : Socket_FD;
   begin
      select
         accept Start (From, To : Socket_FD) do
            From_FD := From;
            To_FD   := To;
         end Start;
      or
         terminate;
      end select;

      loop
         Send (To_FD, Receive (From_FD));
      end loop;
   exception
      when Connection_Closed =>
         Put_Line ("Connection closed");
         Shutdown (From_FD, Receive);
         Shutdown (To_FD, Send);
   end Relay;

   Accepting_Socket,
   Incoming_Socket,
   Outgoing_Socket   : Socket_FD;

   type Relay_Access is access Relay;
   Dummy : Relay_Access;

begin
   Socket (Accepting_Socket, AF_INET, SOCK_STREAM);
   Setsockopt (Accepting_Socket, SOL_SOCKET, SO_REUSEADDR, 1);
   Bind (Accepting_Socket, 4567);
   Listen (Accepting_Socket);
   loop
      Put_Line ("Waiting for new connection");
      Accept_Socket (Accepting_Socket, Incoming_Socket);
      Put_Line ("New connection acknowledged");
      Socket (Outgoing_Socket, AF_INET, SOCK_STREAM);
      Put_Line ("Connecting to remote host");
      Connect (Outgoing_Socket, Relay_Host, 5000);
      Put_Line ("Connection established");
      Dummy := new Relay;
      Dummy.Start (Incoming_Socket, Outgoing_Socket);
      Dummy := new Relay;
      Dummy.Start (Outgoing_Socket, Incoming_Socket);
   end loop;
exception
   when others =>
      Print_Error;
end TCPRelay;
