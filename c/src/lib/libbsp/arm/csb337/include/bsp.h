/*
 * CSB337 BSP header file
 *
 * Copyright (c) 2004 by Cogent Computer Systems
 * Writtent by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <libchip/serial.h>

#define BSP_FEATURE_IRQ_EXTENSION

/* What is the input clock freq in hertz? */
#define BSP_MAIN_FREQ 3686400      /* 3.6864 MHz */
#define BSP_SLCK_FREQ   32768      /* 32.768 KHz */

/* What is the last interrupt? */
#define BSP_MAX_INT AT91RM9200_MAX_INT

console_tbl *BSP_get_uart_from_minor(int minor);
static inline int32_t BSP_get_baud(void) {return 38400;}

#define ST_PIMR_PIV	33	/* 33 ticks of the 32.768Khz clock ~= 1msec */

/*
 * Network driver configuration
 */
extern struct rtems_bsdnet_ifconfig *config;

/* Change these to match your board */
int rtems_at91rm9200_emac_attach(struct rtems_bsdnet_ifconfig *config, int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"eth0"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_at91rm9200_emac_attach

#ifdef __cplusplus
}
#endif

#endif /* _BSP_H */

