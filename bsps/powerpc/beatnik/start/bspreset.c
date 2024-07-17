#include <rtems.h>
#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/bspIo.h>
#include <libcpu/io.h>
#include <stdint.h>

void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
  (void) source;
  (void) code;

  /* Mvme5500 board reset : 2004 S. Kate Feng <feng1@bnl.gov>  */
  out_8((volatile uint8_t*) (BSP_MV64x60_DEV1_BASE +2), 0x80);
  RTEMS_UNREACHABLE();
}
