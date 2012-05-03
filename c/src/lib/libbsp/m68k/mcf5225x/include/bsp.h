/*
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
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <rtems/iosupp.h>
#include <rtems/bspIo.h>

#define BSP_SMALL_MEMORY 1

/***************************************************************************/
/**  Hardware data structure headers                                      **/
#include <mcf5225x/mcf5225x.h>
    
/* Declare base address of peripherals area */
#define __IPSBAR ((vuint8 *) 0x40000000)    

/***************************************************************************/
/**  Network driver configuration                                         **/

/***************************************************************************/
/**  User Definable configuration                                         **/

/* define ports for console and DPU specific for BLUETOOTH and STATIONS */
#define STATIONS_PORT			0
#define CONSOLE_PORT      1
#define BLUETOOTH_PORT		2

/* externals */

/* constants */

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


/* functions */

uint32_t bsp_get_CPU_clock_speed(void);

rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

/*
 * Interrupt assignments
 *  Highest-priority listed first
 */

#define PIT3_IRQ_LEVEL      4
#define PIT3_IRQ_PRIORITY   0

#define UART0_IRQ_LEVEL     3
#define UART0_IRQ_PRIORITY  7
#define UART1_IRQ_LEVEL     3
#define UART1_IRQ_PRIORITY  6
#define UART2_IRQ_LEVEL     3
#define UART2_IRQ_PRIORITY  5

#ifdef __cplusplus
}
#endif

#endif
