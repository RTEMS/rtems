-----------------------------------------------------------------------------
--                                                                         --
--                         ADASOCKETS COMPONENTS                           --
--                                                                         --
--                   S O C K E T S . M U L T I C A S T                     --
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

with Sockets.Thin;

package Sockets.Multicast is

   pragma Elaborate_Body;

   --  This package aims at helping the creation of multicast sockets

   type Multicast_Socket_FD is new Socket_FD with private;

   function Create_Multicast_Socket
     (Group     : String;
      Port      : Positive;
      TTL       : Positive := 16;
      Self_Loop : Boolean  := True)
     return Multicast_Socket_FD;
   --  Create a multicast socket

   procedure Send (Socket : in Multicast_Socket_FD;
                   Data   : in Ada.Streams.Stream_Element_Array);
   --  Send data over a multicast socket

private

   procedure Socket
     (Sock   : out Multicast_Socket_FD;
      Domain : in Socket_Domain := AF_INET;
      Typ    : in Socket_Type   := SOCK_STREAM);
   --  Do not call this one, it will raise Program_Error

   type Multicast_Socket_FD is new Socket_FD with record
      Target : Sockets.Thin.Sockaddr_In;
   end record;

end Sockets.Multicast;
