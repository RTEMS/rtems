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
#include <mpc8xx.h>
#include <mpc8xx/cpm.h>
#include <mpc8xx/mmu.h>
#include <mpc8xx/console.h>
#include <bsp/vectors.h>

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

#ifdef __cplusplus
}
#endif

#endif
