/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Real Time Clock Driver Wrapper for Libchip
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
 * Copyright (C) 1998, 2014 On-Line Applications Research Corporation (OAR)
 * Copyright (C) 2004 Eric Norum
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems.h>
#include <rtems/rtc.h>
#include <rtems/tod.h>
#include <rtems/libio.h>
#include <rtems/rtems/clockimpl.h>

#include <libchip/rtc.h>

/*
 *  Configuration Information
 */
static rtems_device_minor_number RTC_Minor = UINT32_MAX;

static bool RTC_Is_present(void)
{
  return RTC_Minor != UINT32_MAX;
}

/*
 *  rtc_initialize
 *
 *  Initialize the RTC driver
 */
rtems_device_driver rtc_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor_arg,
  void                      *arg
)
{
  (void) major;
  (void) minor_arg;
  (void) arg;

  rtems_device_minor_number minor;
  rtems_status_code status;

  for (minor=0; minor < RTC_Count ; minor++) {
    /*
     * First perform the configuration dependent probe, then the
     * device dependent probe
     */

    if (RTC_Table[minor].deviceProbe && RTC_Table[minor].deviceProbe(minor)) {
      /*
       * Use this device as the primary RTC
       */
      RTC_Minor = minor;
      break;
    }
  }

  if ( !RTC_Is_present() ) {
    /*
     * Failed to find an RTC -- this is not a fatal error.
     */

    return RTEMS_INVALID_NUMBER;
  }

  /*
   *  Register and initialize the primary RTC's
   */
  status = rtems_io_register_name( RTC_DEVICE_NAME, major, RTC_Minor );
  if (status != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(status);
  }

  RTC_Table[minor].pDeviceFns->deviceInitialize( RTC_Minor );

  /*
   *  Now initialize any secondary RTC's
   */
  for ( minor++ ; minor<RTC_Count ; minor++) {
    /*
     * First perform the configuration dependent probe, then the
     * device dependent probe
     */

    if (RTC_Table[minor].deviceProbe && RTC_Table[minor].deviceProbe(minor)) {
      status = rtems_io_register_name(
        RTC_Table[minor].sDeviceName,
        major,
        minor );
      if (status != RTEMS_SUCCESSFUL) {
        rtems_fatal_error_occurred(status);
      }

      /*
       * Initialize the hardware device.
       */
      RTC_Table[minor].pDeviceFns->deviceInitialize(minor);

    }
  }

  setRealTimeToRTEMS();
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver rtc_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void *arg
)
{
  (void) major;
  (void) minor;

  int rv = 0;
  rtems_libio_rw_args_t *rw = arg;
  rtems_time_of_day *tod = (rtems_time_of_day *) rw->buffer;

  rw->offset = 0;
  rw->bytes_moved = 0;

  if (!RTC_Is_present()) {
    return RTEMS_NOT_CONFIGURED;
  }

  if (rw->count != sizeof( rtems_time_of_day)) {
    return RTEMS_INVALID_SIZE;
  }

  rv = RTC_Table [RTC_Minor].pDeviceFns->deviceGetTime(
    RTC_Minor,
    tod
  );
  if (rv != 0) {
    return RTEMS_IO_ERROR;
  }

  rw->bytes_moved = rw->count;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver rtc_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void *arg
)
{
  (void) major;
  (void) minor;

  int rv = 0;
  rtems_libio_rw_args_t *rw = arg;
  const rtems_time_of_day *tod = (const rtems_time_of_day *) rw->buffer;

  rw->offset = 0;
  rw->bytes_moved = 0;

  if (!RTC_Is_present()) {
    return RTEMS_NOT_CONFIGURED;
  }

  if (rw->count != sizeof( rtems_time_of_day)) {
    return RTEMS_INVALID_SIZE;
  }

  rv = RTC_Table [RTC_Minor].pDeviceFns->deviceSetTime(
    RTC_Minor,
    tod
  );
  if (rv != 0) {
    return RTEMS_IO_ERROR;
  }

  rw->bytes_moved = rw->count;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver rtc_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver rtc_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver rtc_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  return RTEMS_NOT_IMPLEMENTED;
}

/*
 *  This routine copies the time from the real time clock to RTEMS
 */
void setRealTimeToRTEMS()
{
  rtems_time_of_day rtc_tod;

  if (!RTC_Is_present())
    return;

  RTC_Table[RTC_Minor].pDeviceFns->deviceGetTime(RTC_Minor, &rtc_tod);
  rtems_clock_set( &rtc_tod );
}

/*
 *  setRealTimeFromRTEMS
 *
 *  This routine copies the time from RTEMS to the real time clock
 */
void setRealTimeFromRTEMS(void)
{
  rtems_time_of_day rtems_tod;

  if (!RTC_Is_present())
    return;

  rtems_clock_get_tod( &rtems_tod );
  RTC_Table[RTC_Minor].pDeviceFns->deviceSetTime(RTC_Minor, &rtems_tod);
}

/*
 *  getRealTime
 *
 *  This routine reads the current time from the RTC.
 */
void getRealTime(
  rtems_time_of_day *tod
)
{
  if (!RTC_Is_present())
    return;

  RTC_Table[RTC_Minor].pDeviceFns->deviceGetTime(RTC_Minor, tod);
}

/*
 *  setRealTime
 *
 *  This routine sets the RTC.
 */
int setRealTime(
  const rtems_time_of_day *tod
)
{
  if (!RTC_Is_present())
    return -1;

  if (_TOD_Validate(tod, TOD_ENABLE_TICKS_VALIDATION) != RTEMS_SUCCESSFUL)
    return -1;

  RTC_Table[RTC_Minor].pDeviceFns->deviceSetTime(RTC_Minor, tod);
  return 0;
}

/*
 *  checkRealTime
 *
 *  This routine reads the returns the variance betweent the real time and
 *  RTEMS time.
 */
int checkRealTime(void)
{
  rtems_time_of_day rtems_tod;
  rtems_time_of_day rtc_tod;
  uint32_t   rtems_time;
  uint32_t   rtc_time;

  if (!RTC_Is_present())
    return -1;

  rtems_clock_get_tod( &rtems_tod );
  RTC_Table[RTC_Minor].pDeviceFns->deviceGetTime(RTC_Minor, &rtc_tod);

  rtems_time = _TOD_To_seconds( &rtems_tod );
  rtc_time = _TOD_To_seconds( &rtc_tod );

  return rtems_time - rtc_time;
}
