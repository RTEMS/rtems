with System.Machine_Code;
use System.Machine_Code;

package body I386_Ports is

   procedure Outport (Addr: in Port_Address; Data: in Byte) is
   begin
      Asm ("movb %0, %%al;" &
           "movw %1, %%dx;" &
           "outb %%al, %%dx",
           No_Output_Operands,
           (Byte'Asm_Input ("g", Data),
            Port_Address'Asm_Input ("g", Addr)),
           Clobber => "al dx",
           Volatile => True);
   end Outport;

   procedure Inport (Addr: in Port_Address; Data: out Byte) is
   begin
      Asm ("movw %1, %%dx;"   &
           "inb %%dx, %%al;"  &
           "movb %%al, %0",
           Byte'Asm_Output ("=g", Data),
           Port_Address'Asm_Input ("g", Addr),
           Clobber => "dx al",
           Volatile => True);
   end Inport;

end I386_Ports;
