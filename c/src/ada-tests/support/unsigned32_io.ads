--
--  Unsigned32_IO / Specification
--
--  DESCRIPTION:
--
--  This package instantiates the IO routines necessary to 
--  perform IO on data of the type RTEMS.Unsigned32.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
--  On-Line Applications Research Corporation (OAR).
--  All rights assigned to U.S. Government, 1994.
--
--  This material may be reproduced by or for the U.S. Government pursuant
--  to the copyright license under the clause at DFARS 252.227-7013.  This
--  notice must appear in all copies of this file and its derivatives.
--
--  unsigned32_io.ads,v 1.1 1995/07/12 19:37:33 joel Exp
--

with RTEMS;
with Text_IO;

package Unsigned32_IO is new Text_IO.Modular_IO( RTEMS.Unsigned32 );

