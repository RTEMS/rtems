/*===============================================================*\
| Project: RTEMS generic mcf548x BSP                              |
+-----------------------------------------------------------------+
| File: bsp.h                                                     |
+-----------------------------------------------------------------+
| The file contains the BSP header of generic MCF548x BSP.        |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
|                                                                 |
| Parts of the code has been derived from the "dBUG source code"  |
| package Freescale is providing for M548X EVBs. The usage of     |
| the modified or unmodified code and it's integration into the   |
| generic mcf548x BSP has been done according to the Freescale    |
| license terms.                                                  |
|                                                                 |
| The Freescale license terms can be reviewed in the file         |
|                                                                 |
|    Freescale_license.txt                                        |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
| The generic mcf548x BSP has been developed on the basic         |
| structures and modules of the av5282 BSP.                       |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 12.11.07                    1.0                            ras  |
|                                                                 |
\*===============================================================*/

#ifndef __GENMCF548X_BSP_H
#define __GENMCF548X_BSP_H

#ifdef __cplusplus
extern "C" {
#endif
#include <bspopts.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <rtems/iosupp.h>
#include <rtems/bspIo.h>

/***************************************************************************/
/**  Hardware data structure headers                                      **/
#include <mcf548x/mcf548x.h>

/***************************************************************************/
/**  Network driver configuration                                         **/
struct rtems_bsdnet_ifconfig;
extern int rtems_fec_driver_attach (struct rtems_bsdnet_ifconfig *config, int attaching );
#define RTEMS_BSP_NETWORK_DRIVER_NAME     "fs1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH   rtems_fec_driver_attach

/***************************************************************************/
/**  User Definable configuration                                         **/

/* define which port the console should use - all other ports are then defined as general purpose */
#define CONSOLE_PORT        0

#define RAM_END 0x4000000 /* 64 MB */

/* functions */

uint32_t get_CPU_clock_speed(void);

rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

/*
 * Interrupt assignments
 *  Highest-priority listed first
 */
#define SLT0_IRQ_LEVEL      4
#define SLT0_IRQ_PRIORITY   0

#define PSC0_IRQ_LEVEL      3
#define PSC0_IRQ_PRIORITY   7
#define PSC1_IRQ_LEVEL      3
#define PSC1_IRQ_PRIORITY   6
#define PSC2_IRQ_LEVEL      3
#define PSC2_IRQ_PRIORITY   5
#define PSC3_IRQ_LEVEL      3
#define PSC3_IRQ_PRIORITY   4

#define FEC_IRQ_LEVEL       2
#define FEC_IRQ_PRIORITY    3

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
#ifdef __cplusplus
}
#endif

#endif
