/* $Id$ */

#include "console.inl"

/*-------------------------------------------------------------------------+
|         Function: rtemsReboot
|      Description: Reboot the PC.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
void rtemsReboot(void)
{
  /* shutdown and reboot */
  kbd_outb(0x4, 0xFE);      /* use keyboard controler to do the job... */
} /* rtemsReboot */
