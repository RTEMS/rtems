with I386_Ports;
use I386_Ports;

package TS1325.Button is

   function Is_Button_Pressed return Boolean;

   procedure Wait_For_Button_Press;

private

   pragma Inline (Is_Button_Pressed, Wait_For_Button_Press);

   Poll_Interval: constant Duration := 0.3;

   Minimum_Press_Time: constant Duration := 0.3;

   Button_Mask: constant Byte := 2#0000_0001#;

   Button_Port: Port_Address renames P1PIN;

end TS1325.Button;
