/* $Id$ */

#include "console.inl"
#include <rtems/bspIo.h>
#include <libcpu/stackTrace.h>

/*-------------------------------------------------------------------------+
|         Function: rtemsReboot
|      Description: Reboot the PC.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
void rtemsReboot(void)
{
  printk("Printing a stack trace for your convenience :-)\n");
  CPU_print_stack();
  /* shutdown and reboot */
  kbd_outb(0x4, 0xFE);      /* use keyboard controler to do the job... */
} /* rtemsReboot */
