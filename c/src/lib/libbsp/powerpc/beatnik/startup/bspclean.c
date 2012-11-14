#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/bspIo.h>

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code error
)
{
  /* We can't go back to MotLoad since we blew it's memory area
   * and vectors. Just pull the reset line...
   */
  printk(
    "bsp_fatal_extension(): RTEMS terminated -- no way back to MotLoad "
      "so I reset the card\n"
  );
  bsp_reset();
}

void bsp_cleanup(
  uint32_t status
)
{
  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, status );
}
