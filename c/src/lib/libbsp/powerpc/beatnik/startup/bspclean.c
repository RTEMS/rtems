#include <bsp.h>
#include <rtems/bspIo.h>

void bsp_cleanup(
  uint32_t status
)
{
  /* We can't go back to MotLoad since we blew it's memory area
   * and vectors. Just pull the reset line...
   */
  printk(
    "bsp_cleanup(): RTEMS terminated -- no way back to MotLoad "
      "so I reset the card\n"
  );
  bsp_reset();
}
