-----------------------------------------------------------------------------
--                                                                         --
--                         ADASOCKETS COMPONENTS                           --
--                                                                         --
--                      S O C K E T S . N A M I N G                        --
--                                                                         --
--                                S p e c                                  --
--                                                                         --
--                        $ReleaseVersion: 0.1.3 $                         --
--                                                                         --
--            Copyright (C) 1996-1998 Free Software Foundation             --
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

with Ada.Finalization;
with Sockets.Thin;

package Sockets.Naming is

   type String_Access is access String;

   type String_Array is array (Positive range <>) of String_Access;

   subtype Address_Component is Natural range 0 .. 255;

   type Address is record
      H1, H2, H3, H4 : Address_Component;
   end record;
   --  An IPv4 address such as 137.194.160.12

   type Address_Array is array (Positive range <>) of Address;

   type Host_Entry (N_Aliases, N_Addresses : Natural) is
      new Ada.Finalization.Controlled with record
         Name      : String_Access;
         Aliases   : String_Array  (1 .. N_Aliases);
         Addresses : Address_Array (1 .. N_Addresses);
      end record;
   --  A complete host structure. A host may have several IP addresses as
   --  well as several aliases.

   procedure Adjust   (Object : in out Host_Entry);
   procedure Finalize (Object : in out Host_Entry);

   Naming_Error : exception;
   --  This exception is raised when a name cannot be resolved

   function Image (Add : Address) return String;
   --  The dotted form corresponding to an IP address

   function Image (Add : Thin.In_Addr) return String;
   --  The dotted form corresponding to the packed form of an IP address

   function Value (Add : String) return Address;
   --  The IP address corresponding to a dotted form

   function Info_Of (Name : String)
     return Host_Entry;
   --  Host entry of an IP name

   function Info_Of (Addr : Address)
     return Host_Entry;
   --  Host entry of an IP address

   function Is_IP_Address (Something : String)
     return Boolean;
   --  Return True if the name looks like an IP address, False otherwise

   function Info_Of_Name_Or_IP (Something : String)
     return Host_Entry;
   --  Host entry of an IP name or a dotted form

   function Address_Of (Something : String) return Address;
   --  Address of an IP name or a dotted form

   function Host_Name return String;
   --  Return the name of the current host

   function Name_Of (Something : String) return String;
   --  Return the official name of an IP name or a dotted form

   function To_In_Addr (Addr : Address) return Thin.In_Addr;
   --  Convert an IP address to a In_Addr structure

   function To_Address (Addr : Thin.In_Addr) return Address;
   --  Convert a In_Addr structure to an IP address

   function Any_Address return Address;
   --  Return the value of inaddr_any

end Sockets.Naming;
