-----------------------------------------------------------------------------
--                                                                         --
--                         ADASOCKETS COMPONENTS                           --
--                                                                         --
--                            L I S T E N E R                              --
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

procedure Listener is

   --  Usage: listener
   --  Example: listener
   --  then telnet localhost `listen_port'

   Listen_Port : Positive := 5000;

   task type Echo is
      entry Start (FD : in Socket_FD);
   end Echo;

   function Rev (S : String) return String;
   --  Reverse a string

   ----------
   -- Echo --
   ----------

   task body Echo is
      Sock : Socket_FD;
   begin
      select
         accept Start (FD : in Socket_FD) do
            Sock := FD;
         end Start;
      or
         terminate;
      end select;

      loop
         Put_Line (Sock, Rev (Get_Line (Sock)));
      end loop;

   exception
      when Connection_Closed =>
         Put_Line ("Connection closed");
         Shutdown (Sock, Both);
   end Echo;

   Accepting_Socket : Socket_FD;
   Incoming_Socket  : Socket_FD;

   type Echo_Access is access Echo;
   Dummy : Echo_Access;

   ---------
   -- Rev --
   ---------

   function Rev (S : String) return String is
      Result : String (1 .. S'Length);
      Index  : Natural := 0;
   begin
      for I in reverse S'Range loop
         Index := Index + 1;
         Result (Index) := S (I);
      end loop;
      return Result;
   end Rev;

begin
   Socket (Accepting_Socket, AF_INET, SOCK_STREAM);
   Setsockopt (Accepting_Socket, SOL_SOCKET, SO_REUSEADDR, 1);
   Bind (Accepting_Socket, Listen_Port);
   Listen (Accepting_Socket);
   loop
      Put_Line ("Waiting for new connection");
      Accept_Socket (Accepting_Socket, Incoming_Socket);
      Put_Line ("New connection acknowledged");
      Dummy := new Echo;
      Dummy.Start (Incoming_Socket);
   end loop;
end Listener;
