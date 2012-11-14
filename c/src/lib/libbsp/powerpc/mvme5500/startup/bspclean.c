/* Copyright 2003, Shuchen Kate Feng <feng1@bnl.gov>,
 *                    NSLS,Brookhaven National Laboratory
 */
#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/bspIo.h>
#include <libcpu/stackTrace.h>

#define AUTO_BOOT 0

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code error
)
{
#if AUTO_BOOT
  /* Till Straumann <strauman@slac.stanford.edu> for SVGM */
  bsp_reset();
#else
  /* Kate Feng <feng1@bnl.gov> for the MVME5500 */
  printk("\nPrinting a stack trace for your convenience :-)\n");
  CPU_print_stack();
  printk("RTEMS terminated; Boot manually or turn on AUTO_BOOT.\n");
#endif
}

void bsp_cleanup(
  uint32_t status
)
{
  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, status );
}
