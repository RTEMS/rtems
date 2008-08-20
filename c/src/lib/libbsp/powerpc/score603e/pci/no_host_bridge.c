/*
 *  detect_host_bridge.c
 *
 * $Id$
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
}
