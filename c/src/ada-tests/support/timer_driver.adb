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
--  COPYRIGHT (c) 1989-2009.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with Interfaces;

package body Timer_Driver is

--PAGE
--
--  Empty_function
--
--
   procedure Empty_Function 
   is
   begin
      Null;
   end Empty_Function;

--PAGE
--
--  Set_Find_Average_Overhead
--
--
 
   procedure Set_Find_Average_Overhead (
      Find_Flag : in     Standard.Boolean
   ) is
      procedure Set_Find_Average_Overhead_base (
         Find_Flag : in     Interfaces.Unsigned_8
      );
      pragma Import (C, Set_Find_Average_Overhead_base,
         "benchmark_timer_disable_subtracting_average_overhead");
      c: Interfaces.Unsigned_8;
   begin
      if Find_Flag then
        c := 1;
      else
        c := 0;
      end if;
      Set_Find_Average_Overhead_base (c);
   end Set_Find_Average_Overhead;

end Timer_Driver;
