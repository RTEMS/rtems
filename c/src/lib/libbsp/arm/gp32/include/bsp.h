/*-------------------------------------------------------------------------+
| bsp.h - ARM BSP 
+--------------------------------------------------------------------------+
| This include file contains definitions related to the ARM BSP.
+--------------------------------------------------------------------------+
|
| Copyright (c) Canon Research France SA.]
| Emmanuel Raguet, mailto:raguet@crf.canon.fr
|
|  The license and distribution terms for this file may be
|  found in found in the file LICENSE in this distribution or at
|  http://www.rtems.com/license/LICENSE.
| 
|  $Id$
+--------------------------------------------------------------------------*/


#ifndef __BSP_H_
#define __BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <s3c2400.h>

extern rtems_configuration_table BSP_Configuration;

void LCD_BREAK();

/* What is the input clock freq in hertz? */
#define BSP_OSC_FREQ  12000000    /* 12 MHz oscillator */
#define M_MDIV 81
#define M_PDIV 2
#define M_SDIV 1

/* How many serial ports? */
#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 1

/* How big should the interrupt stack be? */
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (16 * 1024)

#ifdef __cplusplus
}
#endif

#endif /* __BSP_H_ */
/* end of include file */

