--
--  A test program that uses the TS1325 Button and LED packages.
--

with Text_IO;

with I386_Ports;
use I386_Ports;

with TS1325.LED;
use TS1325.LED;

with TS1325.Button;
use TS1325.Button;

with TS1325.Parallel;
use TS1325.Parallel;

procedure Ts1325_Test is

   Test_Parallel_Interval: Duration := 1.0;
   Read_Button_Interval: Duration := 1.0;
   Read_LED_Interval: Duration := 1.0;
   Set_LED_Interval: Duration := 0.2;

   task Test_Parallel is
      entry Start;
   end Test_Parallel;

   task body Test_Parallel is
      W_Data, R_Data: I386_Ports.Byte := 0;
   begin
      accept Start;

      loop
         W_Data := W_Data + 1;
         Write_Parallel_Port (W_Data);
         Read_Parallel_Port (R_Data);
         Text_IO.Put_Line ("Parallel Port Loopback: Data Write = " &
                           I386_Ports.Byte'Image (W_Data) &
                           ", Data Read = " &
                           I386_Ports.Byte'Image (R_Data));
         delay Test_Parallel_Interval;
      end loop;
   end Test_Parallel;

   task Read_Button is
      entry Start;
   end Read_Button;

   task body Read_Button is
   begin
      accept Start;

      loop
         if Is_Button_Pressed then
            Text_IO.Put_Line ("Button is pressed.");
         else
            Text_IO.Put_Line ("Button is not pressed.");
         end if;
         delay Read_Button_Interval;
      end loop;
   end Read_Button;

   task Read_Colour is
      entry Start;
   end Read_Colour;

   task body Read_Colour is
   begin
      accept Start;

      loop
         Text_IO.Put_Line ("Reading LED colour as " &
                           LED_Colour'Image (LED_State.Get));
         delay Read_LED_Interval;
      end loop;
   end Read_Colour;

   task Set_Colour is
      entry Start;
   end Set_Colour;

   task body Set_Colour is
      Colour: LED_Colour := Off;
   begin
      accept Start;

      loop
         LED_State.Set (Colour);

         if Colour = LED_Colour'Last then
            Colour := LED_Colour'First;
         else
            Colour := LED_Colour'Succ (Colour);
         end if;

         delay Set_LED_Interval;
      end loop;
   end Set_Colour;

begin
   Text_IO.Put_Line ("TS-1325 Utilities Test");
   Text_IO.Put_Line ("======================");
   Text_IO.New_Line;
   Text_IO.Put_Line ("-=> Press the button to begin.");
   Text_IO.New_Line;

   Wait_For_Button_Press;

   Set_Colour.Start;
   Read_Colour.Start;
   Read_Button.Start;
   Test_Parallel.Start;
end Ts1325_Test;
