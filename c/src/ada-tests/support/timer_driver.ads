--
--  Timer_Driver / Specification
--
--  Description:
--
--  This package is the specification for the Timer Driver.
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

with RTEMS;

package Timer_Driver is

--
--  Initialize
--
--  Description:
--
--  This subprogram initializes the hardware timer to start it.
--

   procedure Initialize;
   pragma Import (C, Initialize, "benchmark_timer_initialize");

--
--  Read_Timer
--
--  Description:
--
--  This subprogram stops the timer, calculates the length of time
--  in microseconds since the timer was started, and returns that
--  value.
--

   function Read_Timer 
   return RTEMS.Unsigned32;
   pragma Import (C, Read_Timer, "benchmark_timer_read");

--
--  Empty_Function
--
--  Description:
--
--  This subprogram is an empty subprogram.  It is used to
--  insure that a loop will be included in the final executable
--  so that loop overhead can be subtracted from the directive
--  times reported.
--

   procedure Empty_Function;

--
--  Set_Find_Average_Overhead
--
--  Description:
--
--  This subprogram sets the Find_Average_Overhead flag to the
--  the value passed.
--
 
   procedure Set_Find_Average_Overhead (
      Find_Flag : in     Standard.Boolean
   );

private

end Timer_Driver;
