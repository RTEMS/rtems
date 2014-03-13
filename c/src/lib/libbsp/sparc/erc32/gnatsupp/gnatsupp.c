/*
 *
 * Support for gnat/rtems interrupts and exception handling.
 * Jiri Gaisler, ESA/ESTEC, 17-02-1999.
 */

#include <bsp/gnatcommon.h>

void
__gnat_install_handler (void)
{
  __gnat_install_handler_common (0x1d, 0x15);
}
