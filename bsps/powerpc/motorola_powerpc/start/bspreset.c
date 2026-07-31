/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMotorola
 *
 * @brief Motorola PowerPC Reset Support
 */

#include <bsp.h>
#include <rtems/bspIo.h>
#include <libcpu/stackTrace.h>
#include <bsp/bootcard.h>

/*-------------------------------------------------------------------------+
|         Function: bsp_reset
|      Description: Reboot the PC.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/

void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
  (void) source;
  (void) code;

  printk("Printing a stack trace for your convenience :-)\n");
  CPU_print_stack();
  /* shutdown and reboot */

#if defined(mot_ppc_mvme2100)
  *(unsigned char*)0xffe00000 |= 0x80;
#elif defined(mot_ppc_mvme5100)
  out_8((volatile uint8_t *)0xfef880a0, 0x1);
#else
   /* Memory-mapped Port 92 PIB device access
   *(unsigned char*)0x80000092 |= 0x01;
   */
   outb(1, 0x92);

#endif
   RTEMS_UNREACHABLE();
} /* bsp_reset */
