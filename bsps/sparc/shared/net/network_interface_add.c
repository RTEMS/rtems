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

#include <rtems/rtems_bsdnet.h>
#include <stdio.h>

#include <grlib/network_interface_add.h>

extern struct rtems_bsdnet_config rtems_bsdnet_config;

/* Number of interfaces taken */
int network_interface_cnt = 0;

int network_interface_add(struct rtems_bsdnet_ifconfig *interface)
{
	struct ethernet_config *cfg = NULL;
	int i, last_entry = 1;

	/* Init interface description */
	interface->next = NULL;

	cfg = &interface_configs[network_interface_cnt];
	for(i=0; i<6; i++) {
		if ( cfg->eth_adr[i] != 0 ) {
			last_entry = 0;
			break;
		}
	}
	/* Do we have a valid configuration? */
	if ( last_entry == 0 ) {
		cfg = &interface_configs[network_interface_cnt];

		interface->ip_address = cfg->ip_addr;
		interface->ip_netmask = cfg->ip_netmask;
		interface->hardware_address = cfg->eth_adr;

		network_interface_cnt++;
	} else {
		interface->ip_address = NULL;
		interface->ip_netmask = NULL;
		interface->hardware_address = NULL;
	}

	/* Insert interface first into list */
	interface->next = rtems_bsdnet_config.ifconfig;
	rtems_bsdnet_config.ifconfig = interface;

	return 0;
}
