package body TS1325.Parallel is

   procedure Read_Parallel_Port (Data: out Byte) is
   begin
      Inport (Parallel_Port_In, Data);
   end Read_Parallel_Port;

   procedure Write_Parallel_Port (Data: in Byte) is
   begin
      Outport (Parallel_Port_Out, Data);
   end Write_Parallel_Port;

end TS1325.Parallel;
