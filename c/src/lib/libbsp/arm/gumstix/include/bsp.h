/*
 *  By Yang Xi <hiyangxi@gmail.com>.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
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
#include <libchip/serial.h>

/* What is the input clock freq in hertz */
#define BSP_MAIN_FREQ 3686400      /* 3.6864 MHz */
#define BSP_SLCK_FREQ   32768      /* 32.768 KHz */

/* What is the last interrupt */
#define BSP_MAX_INT AT91RM9200_MAX_INT

console_tbl *BSP_get_uart_from_minor(int minor);
static inline int32_t BSP_get_baud(void) {return 115200;}

/* How many serial ports? */
#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 1

/* How big should the interrupt stack be? */
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (16 * 1024)

extern rtems_configuration_table Configuration;

#define ST_PIMR_PIV	33	/* 33 ticks of the 32.768Khz clock ~= 1msec */

#ifdef __cplusplus
}
#endif

#endif /* _BSP_H */

