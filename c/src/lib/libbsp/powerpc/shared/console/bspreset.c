#include "console.inl"
#include <rtems/bspIo.h>
#include <libcpu/stackTrace.h>

/*-------------------------------------------------------------------------+
|         Function: bsp_reset
|      Description: Reboot the PC.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
void bsp_reset(void)
{
  printk("Printing a stack trace for your convenience :-)\n");
  CPU_print_stack();
  /* shutdown and reboot */

#if defined(mvme2100)
  *(unsigned char*)0xffe00000 |= 0x80;
#else
   /* Memory-mapped Port 92 PIB device access
   *(unsigned char*)0x80000092 |= 0x01;
   */
   outb(1, 0x92);

#endif
} /* bsp_reset */
