--  This package allows one to easily add serial debugging support to any
--  Ada program by simply adding "with Serial_Debug; use Serial_Debug".
--  Debugging will be automatically initialized when the program is run,
--  and it should be possible to interrupt the running program from ddd/gdb.
--
--  Additionally, one can place explicit breakpoints for the debugger using
--  the procedure Breakpoint

package Serial_Debug is

   procedure Breakpoint;

private

   pragma Inline (Breakpoint);

end Serial_Debug;
