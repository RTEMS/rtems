/**
 *  @file
 *
 *  @ingroup shared_tod
 *
 *  @brief Real Time Clock Time of Day API Definition
 */

/*
 *
 *  Based on MVME162 TOD by:
 *    COPYRIGHT (C) 1997
 *    by Katsutoshi Shibuya - BU Denken Co.,Ltd. - Sapporo - JAPAN
 *    ALL RIGHTS RESERVED
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef TOD_H
#define TOD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup shared_tod RTC
 *
 *  @ingroup bsp_shared
 *
 *  @brief Set the RTC
 */
int setRealTime(
  const rtems_time_of_day *tod
);

/*
 *  Get the time from the RTC.
 */

void getRealTime(
  rtems_time_of_day *tod
);

/*
 *  Read real time from RTC and set it to RTEMS' clock manager
 */

void setRealTimeToRTEMS(void);

/*
 *  Read time from RTEMS' clock manager and set it to RTC
 */

void setRealTimeFromRTEMS(void);

/*
 *  Return the difference between RTC and RTEMS' clock manager time in minutes.
 *  If the difference is greater than 1 day, this returns 9999.
 */

int checkRealTime(void);

#ifdef __cplusplus
}
#endif

#endif
