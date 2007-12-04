/*
 * Cirrus EP7312 BSP header file
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  $Id$
*/
#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

/*
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: Following are not defined and are board independent
 *
 */
struct rtems_bsdnet_ifconfig;
int cs8900_driver_attach (struct rtems_bsdnet_ifconfig *config,
                          int                          attaching);

#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2

extern rtems_configuration_table BSP_Configuration;

/*
 * Network driver configuration
 */
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"eth0"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	cs8900_driver_attach

#ifdef __cplusplus
}
#endif

#endif /* _BSP_H */
