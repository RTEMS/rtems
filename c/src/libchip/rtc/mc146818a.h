/*
 *  This file contains the definitions for the following real-time clocks:
 *
 *    + Motorola MC146818A
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __LIBCHIP_MC146818A_h
#define __LIBCHIP_MC146818A_h

/*
 *  Register addresses within chip
 */
#define MC146818A_SEC         0x00    /* seconds */
#define MC146818A_SECALRM     0x01    /* seconds alarm */
#define MC146818A_MIN         0x02    /* minutes */
#define MC146818A_MINALRM     0x03    /* minutes alarm */
#define MC146818A_HRS         0x04    /* hours */
#define MC146818A_HRSALRM     0x05    /* hours alarm */
#define MC146818A_WDAY        0x06    /* week day */
#define MC146818A_DAY         0x07    /* day of month */
#define MC146818A_MONTH       0x08    /* month of year */
#define MC146818A_YEAR        0x09    /* month of year */

#define MC146818A_STATUSA     0x0a    /* status register A */
#define  MC146818ASA_TUP       0x80   /* time update in progress */
#define  MC146818ASA_DIVIDER   0x20   /* divider for 32768 crystal */
#define  MC146818ASA_1024      0x06   /* divide to 1024 Hz */

#define MC146818A_STATUSB     0x0b    /* status register B */
#define  MC146818ASB_DST       0x01   /* Daylight Savings Time */
#define  MC146818ASB_24HR      0x02   /* 0 = 12 hours, 1 = 24 hours */
#define  MC146818ASB_HALT      0x80   /* stop clock updates */

#define MC146818A_STATUSD     0x0d    /* status register D */
#define  MC146818ASD_PWR       0x80   /* clock lost power */


/*
 *  Driver function table
 */
extern rtc_fns mc146818a_fns;
bool mc146818a_probe(
  int minor
);

/*
 * Default register access routines
 */
uint32_t mc146818a_get_register(
  uint32_t  ulCtrlPort,
  uint8_t   ucRegNum
);

void  mc146818a_set_register(
  uint32_t  ulCtrlPort,
  uint8_t   ucRegNum,
  uint32_t  ucData
);

#endif
/* end of include file */
