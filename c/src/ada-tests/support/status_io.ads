--
--  Status_IO / Specification
--
--  DESCRIPTION:
--
--  This package instantiates the IO routines necessary to 
--  perform IO on data of the type Status.CODES.
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
--  status_io.ads,v 1.2 1995/05/31 16:28:20 joel Exp
--

with RTEMS;
with Text_IO;

package Status_IO is new Text_IO.Enumeration_IO( RTEMS.Status_Codes );

