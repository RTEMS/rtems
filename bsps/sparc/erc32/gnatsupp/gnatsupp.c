/**
 * @file
 *
 * @ingroup sparc_erc32
 *
 * @brief Support for gnat/rtems interrupts and exception handling
 */

/*
 * COPYRIGHT (c) 1999.
 * European Space Agency.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/gnatcommon.h>

void
__gnat_install_handler (void)
{
  __gnat_install_handler_common (0x1d, 0x15);
}
