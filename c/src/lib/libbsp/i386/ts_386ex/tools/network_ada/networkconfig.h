/*
 * Network configuration
 * 
 ************************************************************
 * EDIT THIS FILE TO REFLECT YOUR NETWORK CONFIGURATION     *
 * BEFORE RUNNING ANY RTEMS PROGRAMS WHICH USE THE NETWORK! * 
 ************************************************************
 *
 *  $Id$
 */

#ifndef _RTEMS_NETWORKCONFIG_H_
#define _RTEMS_NETWORKCONFIG_H_

#define RTEMS_USE_BOOTP

#include <bsp.h>

/*
 * Define RTEMS_SET_ETHERNET_ADDRESS if you want to specify the
 * Ethernet address here.  If RTEMS_SET_ETHERNET_ADDRESS is not
 * defined the driver will choose an address.
 */

/*#define RTEMS_SET_ETHERNET_ADDRESS*/

#if (defined (RTEMS_SET_ETHERNET_ADDRESS))
static char ethernet_address[6] = { 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX };
#endif

/*
 * Default network interface
 */
static struct rtems_bsdnet_ifconfig netdriver_config = {
	RTEMS_BSP_NETWORK_DRIVER_NAME,		/* name */
	RTEMS_BSP_NETWORK_DRIVER_ATTACH,	/* attach function */

	NULL,				/* link to next interface */

#if (defined (RTEMS_USE_BOOTP))
	NULL,				/* BOOTP supplies IP address */
	NULL,				/* BOOTP supplies IP net mask */
#else
	"127.37.12.19",			/* IP address of device */
	"255.255.255.0",		/* IP net mask */
#endif /* !RTEMS_USE_BOOTP */

#if (defined (RTEMS_SET_ETHERNET_ADDRESS))
	ethernet_address,               /* Ethernet hardware address */
#else
	NULL,                           /* Driver supplies hardware address */
#endif
	0,				/* TRUE == Ignore broadcast packets */

	0,				/* Default MTU */
	0,				/* Default rbufs */
	0,				/* Default xbufs */

	0x300,				/* I/O port on ethernet card */
	5,				/* IRQ */
	0x0000				/* Shared memory start */

};

/*
 * Network configuration
 */
struct rtems_bsdnet_config rtems_bsdnet_config = {
	&netdriver_config,

#if (defined (RTEMS_USE_BOOTP))
	rtems_bsdnet_do_bootp,
#else
	NULL,
#endif

	0,			/* Default network task priority */
	0,			/* Default mbuf capacity */
	0,			/* Default mbuf cluster capacity */

#if defined (RTEMS_USE_BOOTP)
	NULL,
	NULL,
	NULL,
	NULL,
	{NULL,NULL,NULL},
#else
	"rtems0",		/* Host name */
	"ece.ubc.ca",		/* Domain name */
	"127.37.12.254",	/* Gateway: */
	"127.37.12.19",		/* Log host: */
	{"127.37.15.9"},	/* Name server(s) */
#endif /* RTEMS_USE_BOOTP */
};

/*
 * For TFTP test application
 */
#if (!defined (RTEMS_USE_BOOTP))
#define RTEMS_TFTP_TEST_HOST_NAME "127.37.12.19"
#define RTEMS_TFTP_TEST_FILE_NAME "bootfiles/xxx"
#endif

#endif /* _RTEMS_NETWORKCONFIG_H_ */
