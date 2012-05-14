--
--  Float_IO / Specification
--
--  Description:
--
--  This package instantiates the IO routines necessary to 
--  perform IO on data of the type Float.
--
--  Dependencies: 
--
--  
--
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR). 
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with Text_IO;

package Float_IO is new Text_IO.Float_IO( Float );

