package body TS1325.LED is

   protected body LED_State is

      function Get return LED_Colour is
         State: Byte;
         Red_On, Green_On: Boolean;
      begin
         Inport (LED_Port, State);

         Green_On := (State and Green_Bit) = Green_Bit;
         Red_On := (State and Red_Bit) /= Red_Bit;

         if not (Green_On or Red_On) then
            return Off;
         elsif Green_On and not Red_On then
            return Green;
         elsif Green_On and Red_On then
            return Yellow;
         else
            return Red;
         end if;
      end Get;

      procedure Set (Col: in LED_Colour) is
         State: Byte;
      begin
         Inport (LED_Port, State);

         case Col is
            when Off =>
               State := (State and not Green_Bit) or Red_Bit;
            when Green =>
               State := State or Green_Bit or Red_Bit;
            when Yellow =>
               State := (State or Green_Bit) and not Red_Bit;
            when Red =>
               State := State and not (Green_Bit or Red_Bit);
         end case;

         Outport (LED_Port, State);
      end Set;

   end LED_State;

end TS1325.LED;
