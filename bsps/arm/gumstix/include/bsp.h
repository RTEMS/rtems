/**
 * @file
 * @ingroup RTEMSBSPsARMGumstix
 * @brief Global BSP definitions.
 */

/*
 *  By Yang Xi <hiyangxi@gmail.com>.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_GUMSTIX_BSP_H
#define LIBBSP_ARM_GUMSTIX_BSP_H

/**
 * @defgroup RTEMSBSPsARMGumstix Gumstix
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief Gumstix Board Support Package.
 *
 * @{
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_HAS_FRAME_BUFFER 1

/** @brief What is the input clock freq in hertz */
#define BSP_MAIN_FREQ 3686400      /* 3.6864 MHz */
#define BSP_SLCK_FREQ   32768      /* 32.768 KHz */

/** @brief What is the last interrupt */
#define BSP_MAX_INT AT91RM9200_MAX_INT

/*
 * forward reference the type to avoid conflicts between libchip serial
 * and libchip rtc get and set register types.
 */
typedef struct _console_tbl console_tbl;
console_tbl *BSP_get_uart_from_minor(int minor);

static inline int32_t BSP_get_baud(void) {return 115200;}

#define ST_PIMR_PIV	33	/* 33 ticks of the 32.768Khz clock ~= 1msec */

#define outport_byte(port,val) *((unsigned char volatile*)(port)) = (val)
#define inport_byte(port,val) (val) = *((unsigned char volatile*)(port))
#define outport_word(port,val) *((unsigned short volatile*)(port)) = (val)
#define inport_word(port,val) (val) = *((unsigned short volatile*)(port))

struct rtems_bsdnet_ifconfig;
extern int rtems_ne_driver_attach(struct rtems_bsdnet_ifconfig *, int);
#define BSP_NE2000_NETWORK_DRIVER_NAME      "ne1"
#define BSP_NE2000_NETWORK_DRIVER_ATTACH    rtems_ne_driver_attach

#ifndef RTEMS_BSP_NETWORK_DRIVER_NAME
#define RTEMS_BSP_NETWORK_DRIVER_NAME   BSP_NE2000_NETWORK_DRIVER_NAME
#endif

#ifndef RTEMS_BSP_NETWORK_DRIVER_ATTACH
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH BSP_NE2000_NETWORK_DRIVER_ATTACH
#endif

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _BSP_H */

