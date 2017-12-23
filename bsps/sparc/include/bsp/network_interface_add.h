/*  Network interface register help function
 * 
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  This function adds a network interface to the 
 *  rtems_bsdnet_config.ifconfig linked list of interfaces.
 *  The interface configuration is taken from the user defined
 *  array interface_configs. This function is useful for PnP
 *  systems when an unknown number of interfaces are available.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __NETWORK_INTERFACE_ADD_H__
#define __NETWORK_INTERFACE_ADD_H__

#include <rtems/rtems_bsdnet.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Interface configuration description */
struct ethernet_config {
	char	*ip_addr;	/* IP address */
	char	*ip_netmask;	/* IP Netmask */
	char	eth_adr[6];	/* Ethernet hardware MAC address */
};

/* Array with configurations for all interfaces in the system
 * Must be defined by the user.
 */
extern struct ethernet_config interface_configs[];

/* Routine adding interface to rtems_bsdnet_config.ifconfig linked 
 * list of interfaces.
 */
int network_interface_add(struct rtems_bsdnet_ifconfig *interface);

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_NETWORKCONFIG_H_ */
