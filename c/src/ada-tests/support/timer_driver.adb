--
--  Timer_Driver / Body
--
--  Description:
--
--  This package is the body for the Timer Driver.
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
--  $Id$
--

with RTEMS;

package body Timer_Driver is

--PAGE
--
--  Set_Find_Average_Overhead
--
--
 
   procedure Set_Find_Average_Overhead (
      Find_Flag : in     Standard.Boolean
   ) is
      procedure Set_Find_Average_Overhead_base (
         Find_Flag : in     RTEMS.Boolean
      );
      pragma Import (C, Set_Find_Average_Overhead_base,
         "Set_find_average_overhead");
   begin
      Set_Find_Average_Overhead_base (RTEMS.From_Ada_Boolean (Find_Flag));
   end Set_Find_Average_Overhead;

end Timer_Driver;
