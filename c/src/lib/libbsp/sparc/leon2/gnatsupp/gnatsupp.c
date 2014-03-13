/**
 * @file
 * @ingroup sparc_leon2
 * @brief Support for gnat/rtems iterrupts and exception handling
 */

/*
 *
 * Support for gnat/rtems interrupts and exception handling.
 * Jiri Gaisler, ESA/ESTEC, 17-02-1999.
 */

#include <bsp/gnatcommon.h>

/*
 *  Avoid trap 0x18 which is used by the clock tick, and
 *  0x12 (UART B interrupt) which is used by the stub.
 */

void
__gnat_install_handler (void)
{
  __gnat_install_handler_common (0x18, 0x12);
}
