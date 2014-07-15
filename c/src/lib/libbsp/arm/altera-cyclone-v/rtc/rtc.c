/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/*
 * Driver for the DS1339 RTC.
 *
 * Please note the following points:
 * - The day of week is ignored.
 * - The century bit is interpreted the following way:
 *   - century not set: TOD_BASE_YEAR .. 1999
 *   - century set: 2000 .. 2099
 *   - century not set: 2100 .. (TOD_BASE_YEAR + 200)
 */

#include <libchip/rtc.h>
#include <assert.h>
#include <rtems/score/todimpl.h>
#include <sys/filio.h>
#include <fcntl.h>
#include <unistd.h>
#include <bsp/i2cdrv.h>

#define ALTERA_CYCLONE_V_RTC_NUMBER 1

#define DS1339_I2C_ADDRESS 0x68
#define DS1339_I2C_BUS_DEVICE "/dev/i2c0"

#define DS1339_ADDR_CTRL 0x0E
#define DS1339_CTRL_EOSC 0x80
#define DS1339_CTRL_BBSQI 0x20
#define DS1339_CTRL_RS2 0x10
#define DS1339_CTRL_RS1 0x08
#define DS1339_CTRL_INTCN 0x04
#define DS1339_CTRL_A2IE 0x02
#define DS1339_CTRL_A1IE 0x01

#define DS1339_CTRL_DEFAULT (0x00)

#define DS1339_ADDR_TIME 0x00
#define DS1339_ADDR_STATUS 0x0F
#define DS1339_STATUS_OSF 0x80
#define DS1339_STATUS_A2F 0x02
#define DS1339_STATUS_A1F 0x01

#define DS1339_STATUS_CLEAR (0x00)

typedef struct {
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
#define DS1339_HOURS_12_24_FLAG 0x40
#define DS1339_HOURS_AM_PM_FLAG_OR_20_HOURS 0x20
#define DS1339_HOURS_10_HOURS 0x10
  uint8_t weekday;
  uint8_t date;
  uint8_t month;
#define DS1339_MONTH_CENTURY 0x80
  uint8_t year;
} ds1339_time_t;

/* The longest write transmission is writing the time + one address bit */
#define DS1339_MAX_WRITE_SIZE (sizeof(ds1339_time_t) + 1)

/* Functions for converting the fields */
static unsigned int get_seconds (ds1339_time_t *time) {
  uint8_t tens = time->seconds >> 4;
  uint8_t ones = time->seconds & 0x0F;
  return tens * 10 + ones;
}

static unsigned int get_minutes (ds1339_time_t *time) {
  uint8_t tens = time->minutes >> 4;
  uint8_t ones = time->minutes & 0x0F;
  return tens * 10 + ones;
}

static unsigned int get_hours (ds1339_time_t *time) {
  uint8_t value = time->hours & 0x0F;

  if(time->hours & DS1339_HOURS_10_HOURS) {
    value += 10;
  }
  if(time->hours & DS1339_HOURS_AM_PM_FLAG_OR_20_HOURS) {
    if(time->hours & DS1339_HOURS_12_24_FLAG) {
      value += 12;
    } else {
      value += 20;
    }
  }

  return value;
}

static unsigned int get_day_of_month (ds1339_time_t *time) {
  uint8_t tens = time->date >> 4;
  uint8_t ones = time->date & 0x0F;
  return tens * 10 + ones;
}

static unsigned int get_month (ds1339_time_t *time) {
  uint8_t tens = (time->month >> 4) & 0x07;
  uint8_t ones = time->month & 0x0F;
  return tens * 10 + ones;
}

static unsigned int get_year (ds1339_time_t *time) {
  unsigned int year = 1900;
  year += (time->year >> 4) * 10;
  year += time->year & 0x0F;
  if(time->month & DS1339_MONTH_CENTURY) {
    year += 100;
  }
  if(year < TOD_BASE_YEAR) {
    year += 200;
  }
  return year;
}

static void set_time (
  ds1339_time_t *time,
  unsigned int second,
  unsigned int minute,
  unsigned int hour,
  unsigned int day,
  unsigned int month,
  unsigned int year
) {
  unsigned int tens;
  unsigned int ones;
  uint8_t century = 0;

  tens = second / 10;
  ones = second % 10;
  time->seconds = tens << 4 | ones;

  tens = minute / 10;
  ones = minute % 10;
  time->minutes = tens << 4 | ones;

  tens = hour / 10;
  ones = hour % 10;
  time->hours = tens << 4 | ones;

  /* Weekday is not used. Therefore it can be set to an arbitrary valid value */
  time->weekday = 1;

  tens = day / 10;
  ones = day % 10;
  time->date = tens << 4 | ones;

  tens = month / 10;
  ones = month % 10;
  if(year >= 2000 && year < 2100) {
    century = DS1339_MONTH_CENTURY;
  }
  time->month = century | tens << 4 | ones;

  tens = (year % 100) / 10;
  ones = year % 10;
  time->year = tens << 4 | ones;
}

static rtems_status_code ds1339_open_file(int *fd)
{
  int rv = 0;
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  *fd = open(DS1339_I2C_BUS_DEVICE, O_RDWR);
  if ( *fd == -1 ) {
    sc = RTEMS_IO_ERROR;
  }

  if ( sc == RTEMS_SUCCESSFUL ) {
    rv = ioctl(*fd, I2C_IOC_SET_SLAVE_ADDRESS, DS1339_I2C_ADDRESS);
    if ( rv == -1 ) {
      sc = RTEMS_IO_ERROR;
    }
  }

  return sc;
}

/* Read size bytes from ds1339 register address addr to buf. */
static rtems_status_code ds1339_read(uint8_t addr, void *buf, size_t size)
{
  int fd = -1;
  int rv = 0;
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = ds1339_open_file(&fd);

  if ( sc == RTEMS_SUCCESSFUL ) {
    rv = write(fd, &addr, sizeof(addr));
    if ( rv != sizeof(addr) ) {
      sc = RTEMS_IO_ERROR;
    }
  }

  if ( sc == RTEMS_SUCCESSFUL ) {
    rv = read(fd, buf, size);
    if ( rv != size ) {
      sc = RTEMS_IO_ERROR;
    }
  }

  rv = close(fd);
  if ( rv != 0 ) {
    sc = RTEMS_IO_ERROR;
  }

  return sc;
}

/* Write size bytes from buf to ds1339 register address addr. */
static rtems_status_code ds1339_write(uint8_t addr, void *buf, size_t size)
{
  int fd = -1;
  int rv = 0;
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  /* The driver never writes many bytes. Therefore it should be less expensive
   * to reserve the maximum number of bytes that will be written in one go than
   * use a malloc. */
  uint8_t local_buf[DS1339_MAX_WRITE_SIZE];
  int write_size = size + 1;

  assert(write_size <= DS1339_MAX_WRITE_SIZE);

  local_buf[0] = addr;
  memcpy(&local_buf[1], buf, size);

  sc = ds1339_open_file(&fd);

  if ( sc == RTEMS_SUCCESSFUL ) {
    rv = write(fd, local_buf, write_size);
    if ( rv != write_size ) {
      sc = RTEMS_IO_ERROR;
    }
  }

  rv = close(fd);
  if ( rv != 0 ) {
    sc = RTEMS_IO_ERROR;
  }

  return RTEMS_SUCCESSFUL;
}

static void altera_cyclone_v_rtc_initialize(int minor)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  uint8_t status = 0;

  /* Check RTC valid */
  sc = ds1339_read(DS1339_ADDR_STATUS, &status, sizeof(status));
  assert(sc == RTEMS_SUCCESSFUL);
  if(status & DS1339_STATUS_OSF) {
    /* RTC has been stopped. Initialise it. */
    ds1339_time_t time;

    uint8_t write = DS1339_CTRL_DEFAULT;
    sc = ds1339_write(DS1339_ADDR_CTRL, &write, sizeof(write));
    assert(sc == RTEMS_SUCCESSFUL);

    write = DS1339_STATUS_CLEAR;
    sc = ds1339_write(DS1339_ADDR_STATUS, &write, sizeof(write));
    assert(sc == RTEMS_SUCCESSFUL);

    set_time(&time, 0, 0, 0, 1, 1, TOD_BASE_YEAR);
    sc = ds1339_write(DS1339_ADDR_TIME, &time, sizeof(time));
    assert(sc == RTEMS_SUCCESSFUL);
  }
}

static int altera_cyclone_v_rtc_get_time(int minor, rtems_time_of_day *tod)
{
  ds1339_time_t time;
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_time_of_day temp_tod;

  sc = ds1339_read(DS1339_ADDR_TIME, &time, sizeof(time));

  if ( sc == RTEMS_SUCCESSFUL ) {
    temp_tod.ticks = 0;
    temp_tod.second = get_seconds(&time);
    temp_tod.minute = get_minutes(&time);
    temp_tod.hour = get_hours(&time);
    temp_tod.day = get_day_of_month(&time);
    temp_tod.month = get_month(&time);
    temp_tod.year = get_year(&time);

    if ( _TOD_Validate(&temp_tod) ) {
      memcpy(tod, &temp_tod, sizeof(temp_tod));
    } else {
      sc = RTEMS_INVALID_CLOCK;
    }
  }

  return -sc;
}

static int altera_cyclone_v_rtc_set_time(int minor, const rtems_time_of_day *tod)
{
  ds1339_time_t time;
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  set_time (
    &time,
    tod->second,
    tod->minute,
    tod->hour,
    tod->day,
    tod->month,
    tod->year
  );

  sc = ds1339_write(DS1339_ADDR_TIME, &time, sizeof(time));

  return -sc;
}

static bool altera_cyclone_v_rtc_probe(int minor)
{
  /* FIXME: Probe for i2c device */
  return true;
}

const rtc_fns altera_cyclone_v_rtc_ops = {
  .deviceInitialize = altera_cyclone_v_rtc_initialize,
  .deviceGetTime = altera_cyclone_v_rtc_get_time,
  .deviceSetTime = altera_cyclone_v_rtc_set_time
};

size_t RTC_Count = ALTERA_CYCLONE_V_RTC_NUMBER;

rtems_device_minor_number RTC_Minor = 0;

rtc_tbl RTC_Table [ALTERA_CYCLONE_V_RTC_NUMBER] = {
  {
    .sDeviceName = "/dev/rtc",
    .deviceType = RTC_CUSTOM,
    .pDeviceFns = &altera_cyclone_v_rtc_ops,
    .deviceProbe = altera_cyclone_v_rtc_probe,
    .pDeviceParams = NULL,
    .ulCtrlPort1 = 0,
    .ulDataPort = 0,
    .getRegister = NULL,
    .setRegister = NULL
  }
};
