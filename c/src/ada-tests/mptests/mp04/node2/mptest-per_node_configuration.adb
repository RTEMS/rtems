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
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR).
--  Copyright assigned to U.S. Government, 1994.
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.OARcorp.com/rtems/license.html.
--
--  $Id$
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

      return 2;

   end LOCAL_NODE_NUMBER;

end PER_NODE_CONFIGURATION;
