--
--  ADDRESS_IO / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package instantiates the IO routines necessary to 
--  perform IO on data of the type System.Address.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.OARcorp.com/rtems/license.html.
--
--  $Id$
--

with System;

package Address_IO is

   procedure Put (
      Item  : in     System.Address;
      Width : in     Natural := 8;
      Base  : in     Natural := 16
   );

end Address_IO;

