/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 *
 *  This file includes definitions for the MBX860 and MBX821.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_MBX8XX_BSP_H
#define LIBBSP_POWERPC_MBX8XX_BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <rtems/irq.h>
#include <mpc8xx.h>
#include <mpc8xx/cpm.h>
#include <mpc8xx/mmu.h>
#include <mpc8xx/console.h>
#include <bsp/vectors.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Representation of initialization data in NVRAM
 */
typedef volatile struct nvram_config_ {
  unsigned char  cache_mode;            /* 0xFA001000 */
  unsigned char  console_mode;          /* 0xFA001001 */
  unsigned char  console_printk_port;   /* 0xFA001002 */
  unsigned char  eppcbug_smc1;          /* 0xFA001003 */
  unsigned long	 ipaddr;                /* 0xFA001004 */
  unsigned long	 netmask;               /* 0xFA001008 */
  unsigned char	 enaddr[6];             /* 0xFA00100C */
  unsigned short processor_id;          /* 0xFA001012 */
  unsigned long	 rma_start;             /* 0xFA001014 */
  unsigned long	 vma_start;             /* 0xFA001018 */
  unsigned long	 ramsize;               /* 0xFA00101C */
} nvram_config;

/*
 *  Pointer to the base of User Area NVRAM
 */
#define nvram			((nvram_config * const) 0xFA001000)

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
extern int rtems_enet_driver_attach (struct rtems_bsdnet_ifconfig *config, int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"scc1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_enet_driver_attach

/*
 * We need to decide how much memory will be non-cacheable. This
 * will mainly be memory that will be used in DMA (network and serial
 * buffers).
 */
#define NOCACHE_MEM_SIZE 512*1024

/*
 * indicate, that BSP has IDE driver
 */
#define RTEMS_BSP_HAS_IDE_DRIVER

extern uint32_t bsp_clock_speed;

char serial_getc(void);

int serial_tstc(void);

void serial_init(void);

int mbx8xx_console_get_configuration(void);

void _InitMBX8xx(void);

int BSP_disconnect_clock_handler(void);

int BSP_connect_clock_handler (rtems_irq_hdl);

#ifdef __cplusplus
}
#endif

#endif
