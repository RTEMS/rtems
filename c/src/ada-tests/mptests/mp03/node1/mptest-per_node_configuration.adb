--
--  MPTEST.PER_NODE_CONFIGURATION / BODY
--
--  DESCRIPTION:
--
--  This package is the specification for the subpackage
--  which will define the per node configuration parameters.
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
--  mptest-per_node_configuration.adb,v 1.2 1995/05/31 16:33:05 joel Exp
--

with RTEMS;

separate ( MPTEST )

package body PER_NODE_CONFIGURATION is

--PAGE
--
--  LOCAL_NODE_NUMBER
--

   function LOCAL_NODE_NUMBER 
   return RTEMS.UNSIGNED32 is
   begin

      return 1;

   end LOCAL_NODE_NUMBER;

end PER_NODE_CONFIGURATION;
