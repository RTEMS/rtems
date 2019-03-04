/**
 * @file
 *
 * @ingroup RTEMSBSPsM68kMCF5225X
 *
 * @brief Global BSP definitions.
 */

/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
 
#ifndef LIBBSP_M68K_MCF5225X_BSP_H
#define LIBBSP_M68K_MCF5225X_BSP_H

/**
 * @defgroup RTEMSBSPsM68kMCF5225X MCF5225X
 *
 * @ingroup RTEMSBSPsM68k
 *
 * @brief MCF5225X Board Support Package.
 *
 * @{
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>
#include <rtems.h>
#include <rtems/bspIo.h>

/***************************************************************************/
/**  Hardware data structure headers                                      **/
#include <mcf5225x/mcf5225x.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/*
 * Prototypes for BSP methods which cross file boundaries
 */
void Init5225x(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
