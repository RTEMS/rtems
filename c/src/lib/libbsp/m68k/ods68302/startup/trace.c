/*****************************************************************************/
/*
  Trace Exception dumps a back trace to the debug serial port

 */
/*****************************************************************************/

#include <bsp.h>
#include <debugport.h>

#if 0
/* FIXME : could get the string to print when in the BSP */
static const char *exception_names[] =
{
  "RESET STACK TOP",
  "RESET",
  "BUS ERROR",
  "ADDRESS ERROR",
  "ILLEGAL OPCODE",
  "ZERO DIVIDE",
  "CHK",
  "OVERFLOW",
  "PRIVILEGE",
  "TRACE",
  "LINE 1010 EMULATOR",
  "LINE 1111 EMULATOR",
  "UNASSIGNED 12",
  "UNASSIGNED 13",
  "FORMAT ERROR",
  "UNINITIALISED INTERRUPT",
  "UNASSIGNED 16",
  "NODE ANCHOR",
  "SYSTEM ANCHOR",
  "UNASSIGNED 19",
  "UNASSIGNED 20",
  "UNASSIGNED 21",
  "UNASSIGNED 22",
  "UNASSIGNED 23",
  "SPURIOUS HANDLER",
  "LEVEL 1",
  "LEVEL 2",
  "LEVEL 3",
  "LEVEL 4",
  "LEVEL 5",
  "LEVEL 6",
  "LEVEL 7",
  "TRAP 0",
  "TRAP 1",
  "TRAP 2",
  "TRAP 3",
  "TRAP 4",
  "TRAP 5",
  "TRAP 6",
  "TRAP 7",
  "TRAP 8",
  "TRAP 9",
  "TRAP 10",
  "TRAP 11",
  "TRAP 12",
  "TRAP 13",
  "TRAP 14",
  "TRAP 15"
};
#endif

void trace_exception(unsigned long d0,
                     unsigned long d1,
                     unsigned long d2,
                     unsigned long d3,
                     unsigned long d4,
                     unsigned long d5,
                     unsigned long d6,
                     unsigned long d7,
                     unsigned long a0,
                     unsigned long a1,
                     unsigned long a2,
                     unsigned long a3,
                     unsigned long a4,
                     unsigned long a5,
                     unsigned long a6,
                     unsigned long a7,
                     unsigned long sr_pch,
                     unsigned long pcl_format)
{
  unsigned int sr = sr_pch >> 16;
  unsigned long pc = (sr_pch << 16) | (pcl_format >> 16);
  unsigned int format = pcl_format & 0xFFFF;
  unsigned int index;
  unsigned char ch;

  __asm__ volatile(" orw #0x0700,%sr");

  debug_port_banner();

  debug_port_write("unhandled exception=");
  debug_port_write_hex_uint(format >> 2);
  debug_port_write("\n");

  debug_port_write("sr=");
  debug_port_write_hex_uint(sr);
  debug_port_write(", pc=");
  debug_port_write_hex_ulong(pc);
  debug_port_write("\n");

  for (index = 0; index < 16; index++)
  {
    if (index == 8)
    {
      debug_port_write("\n\r");
    }
    if (index < 8)
    {
      debug_port_out('d');
      debug_port_out('0' + index);
    }
    else
    {
      debug_port_out('a');
      debug_port_out('0' + index - 8);
    }
    debug_port_out('=');
    debug_port_write_hex_ulong(*(((unsigned long*) &d0) + index));
    debug_port_out(' ');
  }

  for (index = 0; index < (16 * 10); index++)
  {
    if ((index % 16) == 0)
    {
      debug_port_write("\n");
      debug_port_write_hex_ulong((unsigned long) (((char*) &index) + index));
      debug_port_write(" : ");
    }
    else
    {
      debug_port_out(' ');
    }

    ch = (*(((char*) &index) + index) >> 4) & 0x0F;

    if (ch < 10)
    {
      ch += '0';
    }
    else
    {
      ch += 'a' - 10;
    }

    debug_port_out((char) ch);

    ch = *(((char*) &index) + index) & 0x0F;

    if (ch < 10)
    {
      ch += '0';
    }
    else
    {
      ch += 'a' - 10;
    }
    debug_port_out((char) ch);
  }

  debug_port_write("\nhalting cpu...");

#if defined(UPDATE_DISPLAY)
  UPDATE_DISPLAY("HALT");
#endif

  WATCHDOG_TRIGGER();
  while (1 == 1);
}
