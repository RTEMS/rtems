#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/bspIo.h>

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code error
)
{
  printk("fatal source: %s\n", rtems_fatal_source_text(source));

  if (source == RTEMS_FATAL_SOURCE_EXCEPTION) {
    rtems_exception_frame_print((const rtems_exception_frame *) error);
  }

  /* We can't go back to MotLoad since we blew it's memory area
   * and vectors. Just pull the reset line...
   */
  printk(
    "bsp_fatal_extension(): RTEMS terminated -- no way back to MotLoad "
      "so I reset the card\n"
  );
  bsp_reset();
}
