with Ada.Text_IO;  use Ada.Text_IO;
with System.Machine_Code;  use System.Machine_Code;

package body Serial_Debug is

   ----------------
   -- Breakpoint --
   ----------------

   procedure Breakpoint is
   begin
      Asm ("int $3",
           No_Output_Operands,
           No_Input_Operands,
           Volatile => True);
   end Breakpoint;


   -- This type is from uart.h
   -- Equivalent to an int, with (COM1, COM2) => (0, 1)

   type BSP_Uart is (COM1, COM2);
   pragma Convention (C, BSP_Uart);


   -- The serial port currently used for console I/O

   BSP_Console_Port: BSP_Uart;
   pragma Import (C, BSP_Console_Port, "BSPConsolePort");


   -- C utility routines to set up serial-port debugging

   procedure I386_Stub_Glue_Init (Debug_Port: in BSP_Uart);
   pragma Import (C, I386_Stub_Glue_Init, "i386_stub_glue_init");

   procedure Set_Debug_Traps;
   pragma Import (C, Set_Debug_Traps, "set_debug_traps");

   procedure I386_Stub_Glue_Init_Breakin;
   pragma Import (C, I386_Stub_Glue_Init_Breakin,
                  "i386_stub_glue_init_breakin");

begin

   if BSP_Console_Port /= COM2
   then
      I386_Stub_Glue_Init (COM2);

      Put_Line ("Remote GDB using COM2...");
   else
      I386_Stub_Glue_Init (COM1);

      Put_Line ("Remote GDB using COM1...");
   end if;

   Put_Line ("Remote GDB setting traps...");

   Set_Debug_Traps;

   Put_Line ("Remote GDB waiting for connection...");

   I386_Stub_Glue_Init_Breakin;

end Serial_Debug;
