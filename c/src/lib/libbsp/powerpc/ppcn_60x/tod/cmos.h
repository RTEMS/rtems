/* Structure map for CMOS on PowerPC Reference Platform */
/*
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 */
/* CMOS is the 64 bytes of RAM in the DS1385 chip */
/* The CRC's are computed with x**16+x**12+x**5 + 1 polynomial */
/* The clock is kept in 24 hour BCD mode and should be set to UT(GMT) */
 
#ifndef _CMOS_
#define _CMOS_

/*
 * Address port is at 0x70, data at 0x71
 */
#define RTC_PORT 0x70

/* Define Realtime Clock register numbers. */

#define RTC_SECOND 0                    /* second of minute [0..59] */
#define RTC_SECOND_ALARM 1              /* seconds to alarm */
#define RTC_MINUTE 2                    /* minute of hour [0..59] */
#define RTC_MINUTE_ALARM 3              /* minutes to alarm */
#define RTC_HOUR 4                      /* hour of day [0..23] */
#define RTC_HOUR_ALARM 5                /* hours to alarm */
#define RTC_DAY_OF_WEEK 6               /* day of week [1..7] */
#define RTC_DAY_OF_MONTH 7              /* day of month [1..31] */
#define RTC_MONTH 8                     /* month of year [1..12] */
#define RTC_YEAR 9                      /* year [00..99] */
#define RTC_CONTROL_REGISTERA 10        /* control register A */
#define RTC_CONTROL_REGISTERB 11        /* control register B */
#define RTC_CONTROL_REGISTERC 12        /* control register C */
#define RTC_CONTROL_REGISTERD 13        /* control register D */
#define RTC_BATTERY_BACKED_UP_RAM 14    /* battery backed up RAM [0..49] */

/* Define Control Register A structure. */
#define DS1385_REGA_UIP		0x80

/* Define Control Register B structure. */
#define DS1385_REGB_SET_TIME	0x80
#define DS1385_REGB_TIM_IRQ_EN	0x40
#define DS1385_REGB_ALM_IRQ_EN	0x20
#define DS1385_REGB_UPD_IRQ_EN	0x10
#define DS1385_REGB_SQR_EN	0x08
#define DS1385_REGB_DATA_M	0x04
#define DS1385_REGB_HOURS_FMT	0x02
#define DS1385_REGB_DLS_EN	0x01

/* Define Control Register C structure. */
#define DS1385_REGC_IRQ_REQ	0x08
#define DS1385_REGC_IRQ_TIME	0x04
#define DS1385_REGC_IRQ_ALM	0x02
#define DS1385_REGC_IRQ_UPD	0x01

/* Define Control Register D structure. */
#define DS1385_REGD_VALID	0x80
 
typedef struct _CMOS_MAP {
    volatile rtems_unsigned8 DateAndTime[14];

    rtems_unsigned8 SystemDependentArea1[2];
    rtems_unsigned8 SystemDependentArea2[8];
    rtems_unsigned8 FeatureByte0[1];
    rtems_unsigned8 FeatureByte1[1]; /* 19 = PW Flag;
    attribute = write protect */
    rtems_unsigned8 Century[1]; /* century byte in BCD, e.g. 0x19 currently */
    rtems_unsigned8 FeatureByte3[1];
    rtems_unsigned8 FeatureByte4[1];
    rtems_unsigned8 FeatureByte5[1];
    rtems_unsigned8 FeatureByte6[1];
    rtems_unsigned8 FeatureByte7[1]; /* 1F = Alternate PW Flag;
    attribute = write protect */
    rtems_unsigned8 BootPW[14]; /* Power-on password needed to boot system;
    reset value = 0x00000000000000005a5a5a5a5a5a);
    attribute = lock */
    rtems_unsigned8 BootCrc[2]; /* CRC on BootPW */
    rtems_unsigned8 ConfigPW[14]; /* Configuration Password needed to
    change configuration of system;
    reset value = 0x00000000000000005a5a5a5a5a5a);
    attribute = lock */
    rtems_unsigned8 ConfigCrc[2]; /* CRC on ConfigPW */
} CMOS_MAP, *PCMOS_MAP;
 
#endif /* _CMOS_ */
