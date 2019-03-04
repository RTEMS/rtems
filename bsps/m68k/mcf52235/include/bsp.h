/**
 * @file
 *
 * @ingroup RTEMSBSPsM68kMCF52235
 *
 * @brief Global BSP definitions.
 */

/*
 *  mcf52235 BSP header file
 */

#ifndef LIBBSP_M68K_MCF52235_BSP_H
#define LIBBSP_M68K_MCF52235_BSP_H

/**
 * @defgroup RTEMSBSPsM68kMCF52235 MCF52235
 *
 * @ingroup RTEMSBSPsM68k
 *
 * @brief MCF52235 Board Support Package.
 *
 * @{
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>
#include <rtems.h>
#include <rtems/bspIo.h>

/***************************************************************************/
/**  Hardware data structure headers                                      **/
#include <mcf5223x/mcf5223x.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Declare base address of peripherals area */
#define __IPSBAR ((vuint8 *) 0x40000000)

/***************************************************************************/
/**  Network driver configuration                                         **/

/***************************************************************************/
/**  User Definable configuration                                         **/

/* define which port the console should use - all other ports are then defined as general purpose */
#define CONSOLE_PORT        0

/* externals */

/* constants */

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
void Init52235(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif

