package body TS1325.Button is

   function Is_Button_Pressed return Boolean is
      State: Byte;
   begin
      Inport (Button_Port, State);
      return (State and Button_Mask) /= Button_Mask;
   end Is_Button_Pressed;

   procedure Wait_For_Button_Press is
   begin
  Poll_Loop:
      loop
         if Is_Button_Pressed then
            delay Minimum_Press_Time;
            exit Poll_Loop when Is_Button_Pressed;
         end if;
         delay Poll_Interval;
      end loop Poll_Loop;
   end Wait_For_Button_Press;

end TS1325.Button;
