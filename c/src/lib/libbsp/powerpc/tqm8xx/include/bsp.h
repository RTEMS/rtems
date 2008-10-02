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
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _BSP_H
#define _BSP_H

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

/*
 * NOTE: start.S also needs to know, whether we use U-Boot
 */
#include <bspopts.h>
/*
 * indicate, that BSP is booted via TQMMon
 * (unless we use U-Boot)
 */
#if !defined(BSP_HAS_UBOOT)
#define BSP_HAS_TQMMON

#include <bsp/tqm.h>
#endif

#ifndef ASM
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <mpc8xx.h>
#include <mpc8xx/cpm.h>
#include <mpc8xx/mmu.h>
#include <mpc8xx/console.h>
#include <bsp/vectors.h>
#include <libcpu/powerpc-utility.h> 


#ifdef BSP_HAS_TQMMON
#endif

#ifdef BSP_HAS_UBOOT
#define CONFIG_8xx  /* select the proper U-Boot configuration */
#include <bsp/u-boot.h>
extern bd_t mpc8xx_uboot_board_info;
extern const size_t mpc8xx_uboot_board_info_size;
#endif /* BSP_HAS_UBOOT */

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

/* miscellaneous stuff assumed to exist */

/*
 *  Device Driver Table Entries
 */

/*
 * NOTE: Use the standard Console driver entry
 */

/*
 * NOTE: Use the standard Clock driver entry
 */

/*
 * indicate, that BSP has IDE driver
 */
#undef RTEMS_BSP_HAS_IDE_DRIVER

/*
 * How many libio files we want
 */

#define BSP_LIBIO_MAX_FDS       20

/*
 * our bus frequency
 */
extern unsigned int BSP_bus_frequency;

/* functions */

void bsp_cleanup( void );

rtems_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);


#ifdef __cplusplus
}
#endif
#endif
#endif
