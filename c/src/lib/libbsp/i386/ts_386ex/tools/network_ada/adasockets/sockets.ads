-----------------------------------------------------------------------------
--                                                                         --
--                         ADASOCKETS COMPONENTS                           --
--                                                                         --
--                             S O C K E T S                               --
--                                                                         --
--                                S p e c                                  --
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

with Ada.Streams;
with Interfaces.C;

package Sockets is

   type Socket_FD is tagged private;
   --  A socket

   type Socket_Domain is (AF_INET);
   --  AF_INET: Internet sockets (yes, should be PF_INET, but they hold the
   --  same value)

   type Socket_Type is (SOCK_STREAM, SOCK_DGRAM);
   --  SOCK_STREAM: Stream mode   (TCP)
   --  SOCK_DGRAM:  Datagram mode (UDP, Multicast)

   procedure Socket
     (Sock   : out Socket_FD;
      Domain : in Socket_Domain := AF_INET;
      Typ    : in Socket_Type   := SOCK_STREAM);
   --  Create a socket of the given mode

   Connection_Refused : exception;

   procedure Connect
     (Socket : in Socket_FD;
      Host   : in String;
      Port   : in Positive);
   --  Connect a socket on a given host/port. Raise Connection_Refused if
   --  the connection has not been accepted by the other end.

   procedure Bind
     (Socket : in Socket_FD;
      Port   : in Positive);
   --  Bind a socket on a given port

   procedure Listen
     (Socket     : in Socket_FD;
      Queue_Size : in Positive := 5);
   --  Create a socket's listen queue

   type Socket_Level is (SOL_SOCKET, IPPROTO_IP);

   type Socket_Option is (SO_REUSEADDR, IP_MULTICAST_TTL,
                          IP_ADD_MEMBERSHIP, IP_DROP_MEMBERSHIP,
                          IP_MULTICAST_LOOP);

   procedure Setsockopt
     (Socket  : in Socket_FD'Class;
      Level   : in Socket_Level := SOL_SOCKET;
      Optname : in Socket_Option;
      Optval  : in Integer);
   --  Set a socket option

   generic
      Level   : Socket_Level;
      Optname : Socket_Option;
      type Opt_Type is private;
   procedure Customized_Setsockopt (Socket : in Socket_FD'Class;
                                    Optval : in Opt_Type);
   --  Low level control on setsockopt

   procedure Accept_Socket (Socket     : in Socket_FD;
                            New_Socket : out Socket_FD);
   --  Accept a connection on a socket

   Connection_Closed : exception;

   procedure Send (Socket : in Socket_FD;
                   Data   : in Ada.Streams.Stream_Element_Array);
   --  Send data on a socket. Raise Connection_Closed if the socket
   --  has been closed.

   function Receive (Socket : Socket_FD;
                     Max    : Ada.Streams.Stream_Element_Count := 4096)
     return Ada.Streams.Stream_Element_Array;
   --  Receive data from a socket. May raise Connection_Closed

   procedure Receive (Socket : in Socket_FD'Class;
                      Data   : out Ada.Streams.Stream_Element_Array);
   --  Fill data from a socket. Raise Connection_Closed if the socket has
   --  been closed before the end of the array.

   type Shutdown_Type is (Receive, Send, Both);

   procedure Shutdown (Socket : in Socket_FD;
                       How    : in Shutdown_Type := Both);
   --  Close a previously opened socket

   ---------------------------------
   -- String-oriented subprograms --
   ---------------------------------

   procedure Put (Socket : in Socket_FD'Class;
                  Str    : in String);
   --  Send a string on the socket

   procedure New_Line (Socket : in Socket_FD'Class;
                       Count  : in Natural := 1);
   --  Send CR/LF sequences on the socket

   procedure Put_Line (Socket : in Socket_FD'Class;
                       Str    : in String);
   --  Send a string + CR/LF on the socket

   function Get (Socket : Socket_FD'Class) return String;
   --  Get a string from the socket

   function Get_Line (Socket : Socket_FD'Class) return String;
   --  Get a full line from the socket. CR is ignored and LF is considered
   --  as an end-of-line marker.

private

   type Socket_FD is tagged record
      FD : Interfaces.C.int;
   end record;

end Sockets;
