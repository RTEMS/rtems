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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __NO_BSP_h
#define __NO_BSP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <console.h>
#include <clockdrv.h>
#include <mpc8260.h>
#include <mpc8260/cpm.h>
#include <mpc8260/mmu.h>
#include <mpc8260/console.h>
#include <bsp/irq.h>

/*
 * Board configuration registers
 */

typedef struct bcsr

{
    rtems_unsigned32  bcsr0; /* Board Control and Status Register */
    rtems_unsigned32  bcsr1;
    rtems_unsigned32  bcsr2;
    rtems_unsigned32  bcsr3;

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

extern rtems_configuration_table BSP_Configuration;
/*
 * We need to decide how much memory will be non-cacheable. This
 * will mainly be memory that will be used in DMA (network and serial
 * buffers).
 */
/*
#define NOCACHE_MEM_SIZE 512*1024
*/

/*
 *  Define the time limits for RTEMS Test Suite test durations.
 *  Long test and short test duration limits are provided.  These
 *  values are in seconds and need to be converted to ticks for the
 *  application.
 *
 */

#define MAX_LONG_TEST_DURATION       300 /* 5 minutes = 300 seconds */
#define MAX_SHORT_TEST_DURATION      3   /* 3 seconds */

/*
 *  Stuff for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) \
   do { \
	static rtems_irq_connect_data scIrqData = { \
	  PPC_IRQ_SCALL, \
	  (rtems_irq_hdl) handler, \
	  NULL, \
	  NULL, \
	  NULL  \
	}; \
	BSP_install_rtems_irq_handler (&scIrqData); \
   } while(0)

#define Cause_tm27_intr() asm volatile ("sc")

#define Clear_tm27_intr()

#define Lower_tm27_intr()

/* Constants */
/*
#define RAM_START 0
#define RAM_END   0x1000000
*/


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
 * How many libio files we want
 */

#define BSP_LIBIO_MAX_FDS       20

/* functions */

void bsp_cleanup( void );

#if 0
void M8260ExecuteRISC( rtems_unsigned32 command );
void *M8260AllocateBufferDescriptors( int count );
void *M8260AllocateRiscTimers( int count );
extern char M8260DefaultWatchdogFeeder;
#endif

rtems_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
