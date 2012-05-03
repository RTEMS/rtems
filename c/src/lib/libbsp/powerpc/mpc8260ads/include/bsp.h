/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 *
 *  XXX : put yours in here
 *
 *  COPYRIGHT (c) 1989-1999.
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
#include <mpc8260.h>
#include <mpc8260/cpm.h>
#include <mpc8260/mmu.h>
#include <mpc8260/console.h>
#include <bsp/irq.h>
#include <bsp/vectors.h>

/*
 * Board configuration registers
 */

typedef struct bcsr

{
    uint32_t          bcsr0; /* Board Control and Status Register */
    uint32_t          bcsr1;
    uint32_t          bcsr2;
    uint32_t          bcsr3;

} BCSR;

#define UART1_E	0x02000002		/* bit 6 of BCSR1 */
#define UART2_E	0x01000001		/* bit 7 of BCSR1 */

#define GP0_LED 0x02000002    /*  bit 6 of BCSR0 */
#define GP1_LED 0x01000001    /*  bit 7 of BCSR0 */

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
extern int rtems_enet_driver_attach (struct rtems_bsdnet_ifconfig *config, int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"eth1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_enet_driver_attach

/* miscellaneous stuff assumed to exist */

/*
 * We need to decide how much memory will be non-cacheable. This
 * will mainly be memory that will be used in DMA (network and serial
 * buffers).
 */
/*
#define NOCACHE_MEM_SIZE 512*1024
*/

/* functions */

#if 0
void M8260ExecuteRISC( uint32_t         command );
void *M8260AllocateBufferDescriptors( int count );
void *M8260AllocateRiscTimers( int count );
extern char M8260DefaultWatchdogFeeder;
#endif

#ifdef __cplusplus
}
#endif

#endif
