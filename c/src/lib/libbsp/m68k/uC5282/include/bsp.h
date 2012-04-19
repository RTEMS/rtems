/*
 *  uC5282 BSP header file
 *
 *  Author: W. Eric Norum <norume@aps.anl.gov>
 *
 *  COPYRIGHT (c) 2005.
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

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <rtems/iosupp.h>
#include <rtems/bspIo.h>

/***************************************************************************/
/**  BSP Configuration                                                    **/
/*
 * Uncomment to use instruction/data cache
 * Leave commented to use instruction-only cache
 */
#define RTEMS_MCF5282_BSP_ENABLE_DATA_CACHE

/***************************************************************************/
/**  Hardware data structure headers                                      **/
#include <mcf5282/mcf5282.h>   /* internal MCF5282 modules */

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

/* functions */

typedef struct {
    unsigned int l;
    void        *v;
} bsp_mnode_t;

#define RTEMS_BSP_PGM_ERASE_FIRST   0x1
#define RTEMS_BSP_PGM_RESET_AFTER   0x2
#define RTEMS_BSP_PGM_EXEC_AFTER    0x4
#define RTEMS_BSP_PGM_HALT_AFTER    0x8

uint32_t bsp_get_CPU_clock_speed(void);
rtems_status_code bsp_allocate_interrupt(int level, int priority);
int bsp_sysReset(int flags);
int bsp_program(bsp_mnode_t *chain, int flags);
unsigned const char *bsp_gethwaddr(int a);
const char *bsp_getbenv(const char *a);
int bsp_flash_erase_range(volatile unsigned short *flashptr, int start, int end);
int bsp_flash_write_range(volatile unsigned short *flashptr, bsp_mnode_t *chain, int offset);

rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

/*
 * Interrupt assignments
 *  Highest-priority listed first
 */
#define FEC_IRQ_LEVEL       4
#define FEC_IRQ_RX_PRIORITY 7
#define FEC_IRQ_TX_PRIORITY 6

#define PIT3_IRQ_LEVEL      4
#define PIT3_IRQ_PRIORITY   0

#define UART0_IRQ_LEVEL     3
#define UART0_IRQ_PRIORITY  7
#define UART1_IRQ_LEVEL     3
#define UART1_IRQ_PRIORITY  6
#define UART2_IRQ_LEVEL     3
#define UART2_IRQ_PRIORITY  5

/*
 * Fake VME support
 * This makes it easier to use EPICS driver support on this BSP.
 */
#define VME_AM_STD_SUP_ASCENDING   0x3f
#define VME_AM_STD_SUP_PGM         0x3e
#define VME_AM_STD_USR_ASCENDING   0x3b
#define VME_AM_STD_USR_PGM         0x3a
#define VME_AM_STD_SUP_DATA        0x3d
#define VME_AM_STD_USR_DATA        0x39
#define VME_AM_EXT_SUP_ASCENDING   0x0f
#define VME_AM_EXT_SUP_PGM         0x0e
#define VME_AM_EXT_USR_ASCENDING   0x0b
#define VME_AM_EXT_USR_PGM         0x0a
#define VME_AM_EXT_SUP_DATA        0x0d
#define VME_AM_EXT_USR_DATA        0x09
#define VME_AM_SUP_SHORT_IO        0x2d
#define VME_AM_USR_SHORT_IO        0x29

/*
 * 'Extended' BSP support
 */
rtems_status_code bspExtInit(void);
typedef void (*BSP_VME_ISR_t)(void *usrArg, unsigned long vector);
BSP_VME_ISR_t BSP_getVME_isr(unsigned long vector, void **parg);
int BSP_installVME_isr(unsigned long vector, BSP_VME_ISR_t handler, void *usrArg);
int BSP_removeVME_isr(unsigned long vector, BSP_VME_ISR_t handler, void *usrArg);
int BSP_enableVME_int_lvl(unsigned int level);
int BSP_disableVME_int_lvl(unsigned int level);
int BSP_vme2local_adrs(unsigned am, unsigned long vmeaddr, unsigned long *plocaladdr);

/*
 *  This BSP provides its own IDLE task to override the RTEMS one.
 *  So we prototype it and define the constant confdefs.h expects
 *  to configure a BSP specific one.
 */
void *bsp_idle_thread( uintptr_t ignored );
#define BSP_IDLE_TASK_BODY bsp_idle_thread

/*
 * SRAM. The BSP uses SRAM for maintaining some clock-driver data
 *       and for ethernet descriptors (and the initial stack during
 *       early boot).
 */

typedef struct mcf5282BufferDescriptor_ {
    volatile uint16_t   status;
    uint16_t			length;
    volatile void      *buffer;
} mcf5282BufferDescriptor_t;

extern struct {
	uint32_t                  idle_counter;
	uint32_t                  filtered_idle;
	uint32_t                  max_idle_count;
	uint32_t                  pitc_per_tick;
	uint32_t                  nsec_per_pitc;
	uint32_t                  pad[3]; /* align to 16-bytes for descriptors */
	mcf5282BufferDescriptor_t fec_descriptors[];
	/* buffer descriptors are allocated from here */

    /* initial stack is at top of SRAM (start.S)  */
} __SRAMBASE;

#ifdef __cplusplus
}
#endif

#endif
