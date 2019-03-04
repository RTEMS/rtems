/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCLEON2
 *
 * @brief Support for gnat/rtems iterrupts and exception handling
 */

/*
 * COPYRIGHT (c) 1999.
 * European Space Agency.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 *
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
