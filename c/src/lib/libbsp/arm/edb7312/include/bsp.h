/*
 * Cirrus EP7312 BSP header file
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *
 *  $Id$
*/
#ifndef __BSP_H__
#define __BSP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <iosupp.h>
#include <console.h>
#include <clockdrv.h>
  
/*
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: Following are not defined and are board independent
 *
 */
struct rtems_bsdnet_ifconfig *config;
int cs8900_driver_attach (struct rtems_bsdnet_ifconfig *config,
                          int                          attaching);

#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (16 * 1024)

extern rtems_configuration_table BSP_Configuration;

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) 

#define Cause_tm27_intr()  

#define Clear_tm27_intr()  

#define Lower_tm27_intr()


  
/*
 * Network driver configuration
 */
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"eth0"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	cs8900_driver_attach




#ifdef __cplusplus
}
#endif

#endif /* __BSP_H__ */

