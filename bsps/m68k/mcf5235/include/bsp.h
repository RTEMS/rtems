/**
 * @file
 *
 * @ingroup RTEMSBSPsM68kMCF5235
 *
 * @brief Global BSP definitions.
 */

/*
 *  mcf5235 BSP header file
 */

#ifndef LIBBSP_M68K_MCF5235_BSP_H
#define LIBBSP_M68K_MCF5235_BSP_H

/**
 * @defgroup RTEMSBSPsM68kMCF5235 MCF5235
 *
 * @ingroup RTEMSBSPsM68k
 *
 * @brief MCF5235 Board Support Package.
 *
 * @{
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>
#include <rtems.h>
#include <rtems/bspIo.h>

/***************************************************************************/
/**  Hardware data structure headers                                      **/
#include <mcf5235/mcf5235.h>   /* internal MCF5235 modules */

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/
/**  Network driver configuration                                         **/
struct rtems_bsdnet_ifconfig;
extern int rtems_fec_driver_attach (struct rtems_bsdnet_ifconfig *config, int attaching );
#define RTEMS_BSP_NETWORK_DRIVER_NAME     "fec0"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH   rtems_fec_driver_attach

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
 * Prototypes for BSP methods which cross file boundaries
 */
void Init5235(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
