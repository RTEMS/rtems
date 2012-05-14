/*===============================================================*\
| Project: RTEMS TQM8xx BSP                                       |
+-----------------------------------------------------------------+
| This file has been adapted to MPC8xx by                         |
|    Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>         |
|                    Copyright (c) 2008                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
|                                                                 |
| See the other copyright notice below for the original parts.    |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the console driver                           |
\*===============================================================*/
/* derived from MBX8xx BSP: */
/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 *
 *  This file includes definitions for the MBX860 and MBX821.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _BSP_H
#define _BSP_H

/*
 * indicate, that BSP is booted via TQMMon
 */
#define BSP_HAS_TQMMON

#include <libcpu/powerpc-utility.h>

LINKER_SYMBOL(TopRamReserved);

LINKER_SYMBOL( bsp_ram_start);
LINKER_SYMBOL( bsp_ram_end);
LINKER_SYMBOL( bsp_ram_size);

LINKER_SYMBOL( bsp_rom_start);
LINKER_SYMBOL( bsp_rom_end);
LINKER_SYMBOL( bsp_rom_size);

LINKER_SYMBOL( bsp_section_text_start);
LINKER_SYMBOL( bsp_section_text_end);
LINKER_SYMBOL( bsp_section_text_size);

LINKER_SYMBOL( bsp_section_data_start);
LINKER_SYMBOL( bsp_section_data_end);
LINKER_SYMBOL( bsp_section_data_size);

LINKER_SYMBOL( bsp_section_bss_start);
LINKER_SYMBOL( bsp_section_bss_end);
LINKER_SYMBOL( bsp_section_bss_size);

LINKER_SYMBOL( bsp_interrupt_stack_start);
LINKER_SYMBOL( bsp_interrupt_stack_end);
LINKER_SYMBOL( bsp_interrupt_stack_size);

LINKER_SYMBOL( bsp_work_area_start);

#ifndef ASM
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
#include <bsp/tqm.h>
#include <libcpu/powerpc-utility.h>

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;

#if BSP_USE_NETWORK_FEC
extern int rtems_fec_enet_driver_attach (struct rtems_bsdnet_ifconfig *config,
					 int attaching);
#define RTEMS_BSP_FEC_NETWORK_DRIVER_NAME	"fec1"
#define RTEMS_BSP_FEC_NETWORK_DRIVER_ATTACH	rtems_fec_enet_driver_attach
#endif

#if BSP_USE_NETWORK_SCC
extern int rtems_scc_enet_driver_attach (struct rtems_bsdnet_ifconfig *config,
					 int attaching);
#define RTEMS_BSP_SCC_NETWORK_DRIVER_NAME	"scc1"
#define RTEMS_BSP_SCC_NETWORK_DRIVER_ATTACH	rtems_scc_enet_driver_attach
#endif

#if BSP_USE_NETWORK_FEC
#define RTEMS_BSP_NETWORK_DRIVER_NAME	RTEMS_BSP_FEC_NETWORK_DRIVER_NAME
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	RTEMS_BSP_FEC_NETWORK_DRIVER_ATTACH
#elif BSP_USE_NETWORK_SCC
#define RTEMS_BSP_NETWORK_DRIVER_NAME	RTEMS_BSP_SCC_NETWORK_DRIVER_NAME
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	RTEMS_BSP_SCC_NETWORK_DRIVER_ATTACH
#endif
/*
 * We need to decide how much memory will be non-cacheable. This
 * will mainly be memory that will be used in DMA (network and serial
 * buffers).
 */
#define NOCACHE_MEM_SIZE 512*1024

/*
 * indicate, that BSP has IDE driver
 */
#undef RTEMS_BSP_HAS_IDE_DRIVER

/*
 * SPI driver configuration
 */

  /* select values for SPI addressing */
#define PGHPLUS_SPI_ADDR_EEPROM 0
#define PGHPLUS_SPI_ADDR_DISP4  1
  /* NOTE: DISP4 occupies two consecutive addresses for data and control port */
#define PGHPLUS_SPI_ADDR_DISP4_DATA  (PGHPLUS_SPI_ADDR_DISP4)
#define PGHPLUS_SPI_ADDR_DISP4_CTRL  (PGHPLUS_SPI_ADDR_DISP4_DATA+1)

  /* bit masks for Port B lines */
#define PGHPLUS_PB_SPI_EEP_CE_MSK     (1<< 0)
#define PGHPLUS_PB_SPI_DISP4_RS_MSK   (1<<15)
#define PGHPLUS_PB_SPI_DISP4_CE_MSK   (1<<14)
/*
 * our (internal) bus frequency
 */
extern uint32_t BSP_bus_frequency;

#ifdef __cplusplus
}
#endif
#endif
#endif
