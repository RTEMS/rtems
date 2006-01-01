/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains declarations for the pcf8563 RTC driver      |
\*===============================================================*/
/*
 * This file contains the definitions for Dallas Semiconductor
 * DS1307/DS1308 serial real-time clock/NVRAM.
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef __RTC_PCF8563__
#define __RTC_PCF8563__

#define PCF8563_I2C_ADDRESS  (0xA2) /* I2C bus address assigned to PCF8563 */

#define PCF8563_CONTROL1_ADR   (0x00)
#define PCF8563_CONTROL1_TEST1 (0x80) /* EXT_CLK test mode */
#define PCF8563_CONTROL1_STOP  (0x20) /* stop RTC source clock, clear divider*/
#define PCF8563_CONTROL1_TESTC (0x08) /* enable power-on reset override */
                                      /***********/
#define PCF8563_CONTROL2_ADR   (0x01)
#define PCF8563_CONTROL2_TITP  (0x10) /* 0: int output is level */
#define PCF8563_CONTROL2_AF    (0x08) /* alarm flag */
#define PCF8563_CONTROL2_TF    (0x04) /* timer flag */
#define PCF8563_CONTROL2_AIE   (0x02) /* alarm interrupt enable */
#define PCF8563_CONTROL2_TIE   (0x01) /* timer interrupt enable */
                                      /***********/

#define PCF8563_SECOND_ADR   (0x02)
#define PCF8563_SECOND_VL    (0x80) /* clock integrity no longer guaranteed */
#define PCF8563_SECOND_MASK  (0x7f)
                                      /***********/

#define PCF8563_MINUTE_ADR   (0x03)
#define PCF8563_MINUTE_MASK  (0x7f)
                                      /***********/

#define PCF8563_HOUR_ADR     (0x04)
#define PCF8563_HOUR_MASK    (0x3f)
                                      /***********/

#define PCF8563_DAY_ADR      (0x05)
#define PCF8563_DAY_MASK     (0x3f)

#define PCF8563_DAY_OF_WEEK_ADR  (0x06)
#define PCF8563_DAY_OF_WEEK_MASK (0x07)

#define PCF8563_MONTH_ADR    (0x07)
#define PCF8563_MONTH_MASK   (0x1f)
#define PCF8563_MONTH_CENTURY (0x80)
                                      /***********/

#define PCF8563_YEAR_ADR     (0x08)
#define PCF8563_YEAR_MASK    (0xff)

#define PCF8563_MINUTE_ALARM_ADR      (0x09)
#define PCF8563_HOUR_ALARM_ADR        (0x0A)
#define PCF8563_DAY_ALARM_ADR         (0x0B)
#define PCF8563_DAY_OF_WEEK_ALARM_ADR (0x0C)
#define PCF8563_XXX_ALARM_AE          (0x80)
                                      /***********/

#define PCF8563_CLKOUTCTL_ADR  (0x0D)
#define PCF8563_CLKOUTCTL_FE   (0x80) /*  */
#define PCF8563_CLKOUTCTL_FD   (0x03) /*  */
                                      /***********/

#define PCF8563_TIMERCTL_ADR   (0x0E)
#define PCF8563_TIMERCTL_FE    (0x80) /*  */
#define PCF8563_TIMERCTL_FD    (0x03) /*  */
                                      /***********/

#define PCF8563_TIMER_ADR      (0x0F)

#endif /* __RTC_PCF8563__ */
