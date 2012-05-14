/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <libcpu/io.h>
#include <libcpu/spr.h>

#include <bsp.h>
#include <bsp/pci.h>
#include <bsp/consoleIo.h>
#include <bsp/residual.h>
#include <bsp/openpic.h>

#include <rtems/bspIo.h>

/*
 * For the 8240 and the 8245 there is no host bridge the
 * Open PIC device is built into the processor chip.
 */

void detect_host_bridge(void)
{
  OpenPIC=(volatile struct OpenPIC *) (EUMBBAR + BSP_OPEN_PIC_BASE_OFFSET );
}
