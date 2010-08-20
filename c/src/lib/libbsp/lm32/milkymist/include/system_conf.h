/*  system_conf.h
 *  Global System conf
 * 
 *  Milkymist port of RTEMS
 *
 *  The license and distribution terms for this file may be 
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 */

#ifndef __SYSTEM_CONFIG_H_
#define __SYSTEM_CONFIG_H_

#define CPU_FREQUENCY (83333333)
#define UART_BAUD_RATE  (115200)

#define MM_TIMER1_COMPARE (0xe0001024)
#define MM_TIMER1_COUNTER (0xe0001028)
#define MM_TIMER1_CONTROL (0xe0001020)

#define MM_TIMER0_COMPARE (0xe0001014)
#define MM_TIMER0_COUNTER (0xe0001018)
#define MM_TIMER0_CONTROL (0xe0001010)

#define TIMER_ENABLE  (0x01)
#define TIMER_AUTORESTART (0x02)

#define MM_VGA_RESET_MODE (0x01)
#define MM_VGA_RESET  (0xe0003000)
#define MM_VGA_BASEADDRESS (0xe0003024)
#define MM_VGA_BASEADDRESS_ACT  (0xe0003028)

#define MM_MINIMAC_SETUP  (0xe0009000)
#define MM_MINIMAC_STATE0  (0xe0009008)
#define MM_MINIMAC_ADDR0 (0xe000900C)
#define MM_MINIMAC_COUNT0  (0xe0009010)

#define MM_MINIMAC_STATE1  (0xe0009014)
#define MM_MINIMAC_ADDR1 (0xe0009018)
#define MM_MINIMAC_COUNT1  (0xe000901C)

#define MM_MINIMAC_STATE2  (0xe0009020)
#define MM_MINIMAC_ADDR2 (0xe0009024)
#define MM_MINIMAC_COUNT2  (0xe0009028)

#define MM_MINIMAC_STATE3  (0xe000902C)
#define MM_MINIMAC_ADDR3 (0xe0009030)
#define MM_MINIMAC_COUNT3  (0xe0009034)

#define MM_MINIMAC_TXREMAINING  (0xe000903C)
#define MM_MINIMAC_TXADR (0xe0009038)

#endif /* __SYSTEM_CONFIG_H_ */
