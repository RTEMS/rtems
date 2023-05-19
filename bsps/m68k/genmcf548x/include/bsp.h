/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsM68kGenMCF548X
 *
 * @brief Global BSP definitions.
 */

/*
 * RTEMS generic mcf548x BSP
 *
 * The file contains the BSP header of generic MCF548x BSP.
 *
 * Parts of the code has been derived from the "dBUG source code"
 * package Freescale is providing for M548X EVBs. The usage of
 * the modified or unmodified code and it's integration into the
 * generic mcf548x BSP has been done according to the Freescale
 * license terms.
 *
 * The Freescale license terms can be reviewed in the file
 *
 *    LICENSE.Freescale
 *
 * The generic mcf548x BSP has been developed on the basic
 * structures and modules of the av5282 BSP.
 */

/*
 * Copyright (c) 2007 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_M68K_GENMCF548X_BSP_H
#define LIBBSP_M68K_GENMCF548X_BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

/**
 * @defgroup RTEMSBSPsM68kGenMCF548X MCF548X
 *
 * @ingroup RTEMSBSPsM68k
 *
 * @brief MCF548X Board Support Package.
 *
 * @{
 */

#include <rtems.h>
#include <rtems/bspIo.h>

/***************************************************************************/
/**  Hardware data structure headers                                      **/
#include <mcf548x/mcf548x.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/
/**  User Definable configuration                                         **/

/* define which port the console should use - all other ports are then defined as general purpose */
#define CONSOLE_PORT        0

/* functions */

uint32_t get_CPU_clock_speed(void);

rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

/* Initial values for the interrupt level and priority registers (INTC_ICRn) */
extern const uint8_t mcf548x_intc_icr_init_values[64];

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
extern int rtems_mcf548x_fec_driver_attach_detach(struct rtems_bsdnet_ifconfig *config,int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_mcf548x_fec_driver_attach_detach

#define RTEMS_BSP_NETWORK_DRIVER_NAME	"fec1"
#define RTEMS_BSP_NETWORK_DRIVER_NAME2	"fec2"

#ifdef HAS_DBUG
  typedef struct {
    uint32_t console_baudrate;
    uint8_t  server_ip [4];
    uint8_t  client_ip [4];
    uint8_t  gateway_ip[4];
    uint8_t  netmask   [4];
    uint8_t  spare[4];
    uint8_t  macaddr   [6];
    uint32_t ethport;   /* default fec port: 1 = fec1, 2 = fec2 */
    uint32_t uartport;  /* default fec port: 1 = psc0, 2 = psc1... */    
  } dbug_settings_t;
  
#define DBUG_SETTINGS (*(const dbug_settings_t *)0xFC020000)
#endif /* HAS_DBUG */

void bsp_cacr_set_flags(uint32_t flags);

void bsp_cacr_set_self_clear_flags(uint32_t flags);

void bsp_cacr_clear_flags(uint32_t flags);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
