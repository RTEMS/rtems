-----------------------------------------------------------------------------
--                                                                         --
--                         ADASOCKETS COMPONENTS                           --
--                                                                         --
--                       S O C K E T S . U T I L S                         --
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

with Ada.Exceptions; use Ada.Exceptions;
with GNAT.OS_Lib;    use GNAT.OS_Lib;
with System;         use System;

package body Sockets.Utils is

   use Interfaces.C;

   ---------------------
   -- Port_To_Network --
   ---------------------

   function Port_To_Network (Port : unsigned_short)
     return unsigned_short
   is
   begin
      if Default_Bit_Order = High_Order_First then
         return Port;
      else
         return (Port / 256) + (Port mod 256) * 256;
      end if;
   end Port_To_Network;

   ------------------------
   -- Raise_With_Message --
   ------------------------

   procedure Raise_With_Message (Message    : in String;
                                 With_Errno : in Boolean := True)
   is
   begin
      if With_Errno then
         Raise_Exception (Constraint_Error'Identity,
                          Message & " (errno is" & Integer'Image (Errno) &
                          ")");
      else
         Raise_Exception (Constraint_Error'Identity, Message);
      end if;

      --  The following line works around a bug in GNAT that does not
      --  recognize Ada.Exceptions.Raise_Exception as raising an exception,
      --  even if it can compute statically that the occurrence cannot
      --  be Null_Occurrence ???

      raise Program_Error;
   end Raise_With_Message;

end Sockets.Utils;
