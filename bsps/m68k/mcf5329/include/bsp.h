/**
 * @file
 *
 * @ingroup RTEMSBSPsM68kMCF5329
 *
 * @brief Global BSP definitions.
 */


/*
 *  mcf52235 BSP header file
 */

#ifndef LIBBSP_M68K_MCF5329_BSP_H
#define LIBBSP_M68K_MCF5329_BSP_H

/**
 * @defgroup RTEMSBSPsM68kMCF5329 MCF5329
 *
 * @ingroup RTEMSBSPsM68k
 *
 * @brief MCF5329 Board Support Package.
 *
 * @{
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>
#include <rtems.h>
#include <rtems/bspIo.h>

/***************************************************************************/
/**  Hardware data structure headers                                      **/
#include <mcf532x/mcf532x.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile unsigned char vuint8;
typedef volatile unsigned short vuint16;
typedef volatile unsigned long vuint32;

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

uint32_t bsp_get_CPU_clock_speed(void);
uint32_t bsp_get_BUS_clock_speed(void);

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

#define PIT3_IRQ_LEVEL      4

#define UART0_IRQ_LEVEL     3
#define UART1_IRQ_LEVEL     3
#define UART2_IRQ_LEVEL     3

/*
 * Prototypes for BSP methods which cross file boundaries
 */
void Init5329(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
