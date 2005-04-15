#include <rtems.h>
#include <bsp.h>
#include <rtems/bspIo.h>
#include <libcpu/io.h>
#include <libcpu/stackTrace.h>

void rtemsReboot()
{

  printk("\nPrinting a stack trace for your convenience :-)\n");
  CPU_print_stack();

  printk("RTEMS terminated; Rebooting ...\n");
  /* Mvme5500 board reset  <skf> */
  out_8((volatile unsigned char*) (GT64260_DEV1_BASE +2), 0x80);
}
