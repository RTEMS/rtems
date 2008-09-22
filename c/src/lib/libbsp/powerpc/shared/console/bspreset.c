/* $Id$ */

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
#if defined(BSP_KBD_IOBASE)
  kbd_outb(0x4, 0xFE);      /* use keyboard controler to do the job... */
#endif
#if defined(mvme2100)
  *(unsigned char*)0xffe00000 |= 0x80;
#endif
} /* bsp_reset */
