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

#ifndef __RTC_DS1307__
#define __RTC_DS1307__

#define DS1307_I2C_ADDRESS  (0xD0) /* I2C bus address assigned to DS1307 */

#define DS1307_SECOND       (0x00)
#define DS1307_SECOND_HALT  (0x80) /* High bit is a Clock Halt bit */
#define DS1307_MINUTE       (0x01)
#define DS1307_HOUR         (0x02)
#define DS1307_HOUR_12      (0x40) /* 12-hour mode */
#define DS1307_HOUR_PM      (0x20) /* PM in 12-hour mode */
#define DS1307_DAY_OF_WEEK  (0x03)
#define DS1307_DAY          (0x04)
#define DS1307_MONTH        (0x05)
#define DS1307_YEAR         (0x06)
#define DS1307_CONTROL      (0x07)
#define DS1307_CONTROL_OUT      (0x80) /* Output control */
#define DS1307_CONTROL_SQWE     (0x10) /* Sqware Wave Enable */
#define DS1307_CONTROL_RS_1     (0x00) /* Rate select: 1 Hz */
#define DS1307_CONTROL_RS_4096  (0x01) /* Rate select: 4096 Hz */
#define DS1307_CONTROL_RS_8192  (0x02) /* Rate select: 8192 Hz */
#define DS1307_CONTROL_RS_32768 (0x03) /* Rate select; 32768 Hz */

#define DS1307_NVRAM_START  (0x08) /* Start location of non-volatile memory */
#define DS1307_NVRAM_END    (0x3F) /* End location of non-volatile memory */
#define DS1307_NVRAM_SIZE   (56)   /* Size of non-volatile memory */

#endif __RTC_DS1307__
