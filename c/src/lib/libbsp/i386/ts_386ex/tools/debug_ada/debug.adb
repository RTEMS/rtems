with Ada.Text_IO;  use Ada.Text_IO;
with Ada.Numerics.Discrete_Random;

with TS1325.LED;  use TS1325.LED;

with Serial_Debug;  use Serial_Debug;

procedure Debug is

   package Random_LED_Colour is
     new Ada.Numerics.Discrete_Random (LED_Colour);
   use Random_LED_Colour;

   Colour_Gen: Random_LED_Colour.Generator;

   New_Colour: LED_Colour;

   Count: Integer := 0;

begin
   Breakpoint;

   Put_Line ("******* Starting Random LED Debug Test *******");

   for I in 1 .. 10_000 loop
      Count := Count + 1;
      New_Colour := Random_LED_Colour.Random (Colour_Gen);
      LED_State.Set (New_Colour);
   end loop;

   Put_Line ("******* Finished Random LED Debug Test *******");
end Debug;
