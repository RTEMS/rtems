with I386_Ports;
use I386_Ports;

package TS1325.Parallel is

   procedure Read_Parallel_Port (Data: out Byte);

   procedure Write_Parallel_Port (Data: in Byte);

private

   pragma Inline (Read_Parallel_Port, Write_Parallel_Port);

   -- These are non-standard IO locations, which is why they are here instead
   -- of in the I386_Ports package.

   Parallel_Port_In: constant Port_Address := 16#75#;

   Parallel_Port_Out: constant Port_Address := 16#74#;

end TS1325.Parallel;
