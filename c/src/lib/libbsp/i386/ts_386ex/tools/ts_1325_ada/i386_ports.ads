with Interfaces;

package I386_Ports is

   type Port_Address is new Interfaces.Unsigned_16;

   type Byte is new Interfaces.Unsigned_8;

   type Word is new Interfaces.Unsigned_16;

   type Long is new Interfaces.Unsigned_32;

   procedure Outport (Addr: in Port_Address; Data: in Byte);

   procedure Inport (Addr: in Port_Address; Data: out Byte);

   P1PIN: constant Port_Address;
   P1LTC: constant Port_Address;
   P1DIR: constant Port_Address;

   P2PIN: constant Port_Address;
   P2LTC: constant Port_Address;
   P2DIR: constant Port_Address;

   P3PIN: constant Port_Address;
   P3LTC: constant Port_Address;
   P3DIR: constant Port_Address;

private

   pragma Inline (Outport, Inport);

   P1PIN: constant Port_Address := 16#F860#;
   P1LTC: constant Port_Address := 16#F862#;
   P1DIR: constant Port_Address := 16#F864#;

   P2PIN: constant Port_Address := 16#F868#;
   P2LTC: constant Port_Address := 16#F86A#;
   P2DIR: constant Port_Address := 16#F86C#;

   P3PIN: constant Port_Address := 16#F870#;
   P3LTC: constant Port_Address := 16#F872#;
   P3DIR: constant Port_Address := 16#F874#;

end I386_Ports;
