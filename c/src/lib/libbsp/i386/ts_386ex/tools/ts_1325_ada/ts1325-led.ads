with I386_Ports;
use I386_Ports;

package TS1325.LED is

   type LED_Colour is (Off, Green, Yellow, Red);

   protected LED_State is

      function Get return LED_Colour;

      procedure Set (Col: in LED_Colour);

   end LED_State;

private

   Green_Bit: constant Byte := 2#0010_0000#; -- bit set = LED on

   Red_Bit: constant Byte := 2#0100_0000#; -- bit clear = LED on

   LED_Mask: constant Byte := 2#0110_0000#;

   LED_Port: Port_Address renames P1LTC;

end TS1325.LED;
