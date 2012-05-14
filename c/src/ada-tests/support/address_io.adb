--
--  Address_IO / Specification
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
--  COPYRIGHT (c) 1989-2009.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with System.Storage_Elements;
with Text_IO;

package body Address_IO is

   procedure Put (
      Item  : in     System.Address;
      Width : in     Natural := 8;
      Base  : in     Natural := 16
   ) is
     package Integer_IO is new TEXT_IO.Integer_IO( Integer );
   begin

      Integer_IO.Put(
         Integer( System.Storage_Elements.To_Integer( Item ) ),
         Width,
         Base
      );

   end Put;

end Address_IO;

