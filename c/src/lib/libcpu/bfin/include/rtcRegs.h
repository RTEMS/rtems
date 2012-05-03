/*  Blackfin RTC Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _rtcRegs_h_
#define _rtcRegs_h_

/* register addresses */

#define RTC_STAT                 (RTC_BASE_ADDRESS + 0x0000)
#define RTC_ICTL                 (RTC_BASE_ADDRESS + 0x0004)
#define RTC_ISTAT                (RTC_BASE_ADDRESS + 0x0008)
#define RTC_SWCNT                (RTC_BASE_ADDRESS + 0x000c)
#define RTC_ALARM                (RTC_BASE_ADDRESS + 0x0010)
#define RTC_PREN                 (RTC_BASE_ADDRESS + 0x0014)


/* register fields */

#define RTC_STAT_DAYS_MASK                        0xfffe0000
#define RTC_STAT_DAYS_SHIFT                               17
#define RTC_STAT_HOURS_MASK                       0x0001f000
#define RTC_STAT_HOURS_SHIFT                              12
#define RTC_STAT_MINUTES_MASK                     0x00000fc0
#define RTC_STAT_MINUTES_SHIFT                             6
#define RTC_STAT_SECONDS_MASK                     0x0000003f
#define RTC_STAT_SECONDS_SHIFT                             0

#define RTC_ICTL_WCIE                                 0x8000
#define RTC_ICTL_DAIE                                 0x0040
#define RTC_ICTL_24HIE                                0x0020
#define RTC_ICTL_HIE                                  0x0010
#define RTC_ICTL_MIE                                  0x0008
#define RTC_ICTL_SIE                                  0x0004
#define RTC_ICTL_AIE                                  0x0002
#define RTC_ICTL_SWIE                                 0x0001

#define RTC_ISTAT_WC                                  0x8000
#define RTC_ISTAT_WP                                  0x4000
#define RTC_ISTAT_DAEF                                0x0040
#define RTC_ISTAT_24HE                                0x0020
#define RTC_ISTAT_HEF                                 0x0010
#define RTC_ISTAT_MEF                                 0x0008
#define RTC_ISTAT_SEF                                 0x0004
#define RTC_ISTAT_AEF                                 0x0002
#define RTC_ISTAT_SWEF                                0x0001

#define RTC_ALARM_DAYS_MASK                       0xfff70000
#define RTC_ALARM_DAYS_SHIFT                              17
#define RTC_ALARM_HOURS_MASK                      0x0001f000
#define RTC_ALARM_HOURS_SHIFT                             12
#define RTC_ALARM_MINUTES_MASK                    0x00000fc0
#define RTC_ALARM_MINUTES_SHIFT                           10
#define RTC_ALARM_SECONDS_MASK                    0x0000003f
#define RTC_ALARM_SECONDS_SHIFT                            0

#define RTC_PREN_PREN                                 0x0001

#endif /* _rtcRegs_h_ */

