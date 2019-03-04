/**
 * @file
 *
 * @ingroup RTEMSBSPsM68kAV5282
 *
 * @brief Global BSP definitions.
 */

/*
 *  av5282 BSP header file
 */

#ifndef LIBBSP_M68K_AV5282_BSP_H
#define LIBBSP_M68K_AV5282_BSP_H

/**
 * @defgroup RTEMSBSPsM68kAV5282 AV5282
 *
 * @ingroup RTEMSBSPsM68k
 *
 * @brief AV5282 Board Support Package.
 *
 * @{
 */

#ifndef ASM

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/bspIo.h>

/***************************************************************************/
/**  Hardware data structure headers                                      **/
#include <mcf5282/mcf5282.h>   /* internal MCF5282 modules */

#ifdef __cplusplus
extern "C" {
#endif

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

/* externals */

/* constants */

/* miscellaneous stuff assumed to exist */

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
 * Prototypes for methods called from .S to alow dependency tracking
 */
void Init5282(void);

#ifdef __cplusplus
}
#endif

#endif /* !ASM */

/** @} */

#endif
