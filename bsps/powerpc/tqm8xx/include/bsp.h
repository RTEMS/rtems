/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCTQM8XX
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (C) 1989, 2008 On-Line Applications Research Corporation (OAR)
 * Copyright (c) 2008 Thomas Doerfler, embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  RTEMS TQM8xx BSP
 */

#ifndef LIBBSP_POWERPC_TQM8XX_BSP_H
#define LIBBSP_POWERPC_TQM8XX_BSP_H

/**
 * @defgroup RTEMSBSPsPowerPCTQM8XX TQM8XX
 *
 * @ingroup RTEMSBSPsPowerPC
 *
 * @brief TQM8XX Board Support Package.
 *
 * @{
 */

/*
 * indicate, that BSP is booted via TQMMon
 */
#define BSP_HAS_TQMMON

#include <libcpu/powerpc-utility.h>

LINKER_SYMBOL(TopRamReserved)

LINKER_SYMBOL( bsp_ram_start)
LINKER_SYMBOL( bsp_ram_end)
LINKER_SYMBOL( bsp_ram_size)

LINKER_SYMBOL( bsp_rom_start)
LINKER_SYMBOL( bsp_rom_end)
LINKER_SYMBOL( bsp_rom_size)

LINKER_SYMBOL( bsp_section_text_start)
LINKER_SYMBOL( bsp_section_text_end)
LINKER_SYMBOL( bsp_section_text_size)

LINKER_SYMBOL( bsp_section_data_start)
LINKER_SYMBOL( bsp_section_data_end)
LINKER_SYMBOL( bsp_section_data_size)

LINKER_SYMBOL( bsp_section_bss_start)
LINKER_SYMBOL( bsp_section_bss_end)
LINKER_SYMBOL( bsp_section_bss_size)

LINKER_SYMBOL( bsp_work_area_start)

#ifndef ASM

#include <bspopts.h>

#include <rtems.h>
#include <rtems/irq.h>
#include <mpc8xx.h>
#include <mpc8xx/cpm.h>
#include <mpc8xx/mmu.h>
#include <mpc8xx/console.h>
#include <bsp/vectors.h>
#include <bsp/tqm.h>
#include <libcpu/powerpc-utility.h>
#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/*
 *  Interfaces to required Clock Driver support methods
 */
int BSP_disconnect_clock_handler(void);
int BSP_connect_clock_handler (rtems_irq_hdl);

extern uint32_t bsp_clock_speed;

char serial_getc(void);

int serial_tstc(void);

void serial_init(void);

int mbx8xx_console_get_configuration(void);

void _InitTQM8xx (void);

rtems_status_code bsp_register_spi(void);

void *bsp_idle_thread( uintptr_t ignored );

void cpu_init(void);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
