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
 * Driver for the DS1339 RTC (Maxim Semiconductors) -> RTC1
 *        and the M41ST87 RTC (ST Microelectronics) -> RTC2
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

#define ALTERA_CYCLONE_V_RTC_NUMBER  2


/* ******************************* DS1339 ********************************** */


#define DS1339_I2C_ADDRESS     (0xD0 >> 1)  /* 7-bit addressing! */
#define DS1339_I2C_BUS_DEVICE  "/dev/i2c0"

#define DS1339_ADDR_TIME    0x00

#define DS1339_ADDR_CTRL   0x0E
#define   DS1339_CTRL_EOSC   0x80
#define   DS1339_CTRL_BBSQI  0x20
#define   DS1339_CTRL_RS2    0x10
#define   DS1339_CTRL_RS1    0x08
#define   DS1339_CTRL_INTCN  0x04
#define   DS1339_CTRL_A2IE   0x02
#define   DS1339_CTRL_A1IE   0x01

#define DS1339_CTRL_DEFAULT  (0x00)

#define DS1339_ADDR_STATUS  0x0F
#define   DS1339_STATUS_OSF   0x80
#define   DS1339_STATUS_A2F   0x02
#define   DS1339_STATUS_A1F   0x01

#define DS1339_STATUS_CLEAR  (0x00)

#define DS1339_ADDR_TRICKLE_CHARGE  0x10


typedef struct
{
  uint8_t  seconds;
  uint8_t  minutes;
  uint8_t  hours;
#define DS1339_HOURS_12_24_FLAG 0x40
#define DS1339_HOURS_AM_PM_FLAG_OR_20_HOURS 0x20
#define DS1339_HOURS_10_HOURS 0x10
  uint8_t  weekday;
  uint8_t  date;
  uint8_t  month;
#define DS1339_MONTH_CENTURY 0x80
  uint8_t  year;
}
ds1339_time_t;


/* The longest write transmission is writing the time + one address bit */
#define DS1339_MAX_WRITE_SIZE  (sizeof(ds1339_time_t) + 1)


/* Functions for converting the fields */
static unsigned int  ds1339_get_seconds(ds1339_time_t* time)
{
  uint8_t  tens = time->seconds >> 4;
  uint8_t  ones = time->seconds & 0x0F;

  return tens * 10 + ones;
}


static unsigned int  ds1339_get_minutes(ds1339_time_t* time)
{
  uint8_t  tens = time->minutes >> 4;
  uint8_t  ones = time->minutes & 0x0F;

  return tens * 10 + ones;
}


static unsigned int  ds1339_get_hours(ds1339_time_t* time)
{

  uint8_t  value = time->hours & 0x0F;

  if (time->hours & DS1339_HOURS_10_HOURS)
  {
    value += 10;
  }
  if (time->hours & DS1339_HOURS_AM_PM_FLAG_OR_20_HOURS)
  {
    if (time->hours & DS1339_HOURS_12_24_FLAG)
      value += 12;
    else
      value += 20;
  }

  return value;
}


static unsigned int  ds1339_get_day_of_month(ds1339_time_t* time)
{

  uint8_t  tens = time->date >> 4;
  uint8_t  ones = time->date & 0x0F;

  return tens * 10 + ones;
}


static unsigned int  ds1339_get_month(ds1339_time_t* time)
{

  uint8_t  tens = (time->month >> 4) & 0x07;
  uint8_t  ones = time->month & 0x0F;

  return tens * 10 + ones;
}


static unsigned int  ds1339_get_year(ds1339_time_t* time)
{

  unsigned int  year = 1900;

  year += (time->year >> 4) * 10;
  year += time->year & 0x0F;
  if (time->month & DS1339_MONTH_CENTURY)
    year += 100;
  if (year < TOD_BASE_YEAR)
    year += 200;

  return year;
}


static void  ds1339_set_time(ds1339_time_t* time,
                             unsigned int second,
                             unsigned int minute,
                             unsigned int hour,
                             unsigned int day,
                             unsigned int month,
                             unsigned int year)
{

  unsigned int  tens;
  unsigned int  ones;
  uint8_t       century = 0;

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
  if ((year >= 2000) && (year < 2100))
    century = DS1339_MONTH_CENTURY;
  time->month = century | tens << 4 | ones;

  tens = (year % 100) / 10;
  ones = year % 10;
  time->year = tens << 4 | ones;

}



static rtems_status_code  ds1339_open_file(int* fd)
{

  int                rv = 0;
  rtems_status_code  sc = RTEMS_SUCCESSFUL;

  *fd = open(DS1339_I2C_BUS_DEVICE, O_RDWR);
  if (*fd == -1)
    sc = RTEMS_IO_ERROR;

  if (sc == RTEMS_SUCCESSFUL)
  {
    rv = ioctl(*fd, I2C_IOC_SET_SLAVE_ADDRESS, DS1339_I2C_ADDRESS);
    if (rv == -1)
      sc = RTEMS_IO_ERROR;
  }

  return sc;
}


/* Read size bytes from ds1339 register address addr to buf. */
static rtems_status_code  ds1339_read(uint8_t addr, void* buf, size_t size)
{

  int                fd = -1;
  int                rv = 0;
  rtems_status_code  sc = RTEMS_SUCCESSFUL;

  sc = ds1339_open_file(&fd);

  if (sc == RTEMS_SUCCESSFUL)
  {
    rv = write(fd, &addr, sizeof(addr));
    if (rv != sizeof(addr))
      sc = RTEMS_IO_ERROR;
  }

  if (sc == RTEMS_SUCCESSFUL)
  {
    rv = read(fd, buf, size);
    if (rv != size)
      sc = RTEMS_IO_ERROR;
  }

  rv = close(fd);
  if (rv != 0)
    sc = RTEMS_IO_ERROR;

  return sc;
}


/* Write size bytes from buf to ds1339 register address addr. */
static rtems_status_code  ds1339_write(uint8_t addr, void* buf, size_t size)
{

  int                fd = -1;
  int                rv = 0;
  rtems_status_code  sc = RTEMS_SUCCESSFUL;
  /* The driver never writes many bytes. Therefore it should be less expensive
   * to reserve the maximum number of bytes that will be written in one go than
   * use a malloc. */
  uint8_t            local_buf[DS1339_MAX_WRITE_SIZE];
  int                write_size = size + 1;

  assert(write_size <= DS1339_MAX_WRITE_SIZE);

  local_buf[0] = addr;
  memcpy(&local_buf[1], buf, size);

  sc = ds1339_open_file(&fd);

  if (sc == RTEMS_SUCCESSFUL)
  {
    rv = write(fd, local_buf, write_size);
    if (rv != write_size)
      sc = RTEMS_IO_ERROR;
  }

  rv = close(fd);
  if (rv != 0)
    sc = RTEMS_IO_ERROR;

  return RTEMS_SUCCESSFUL;
}


static void altera_cyclone_v_ds1339_initialize(int minor)
{

  rtems_status_code  sc = RTEMS_SUCCESSFUL;
  uint8_t            status = 0;

  /* Check RTC valid */
  sc = ds1339_read(DS1339_ADDR_STATUS, &status, sizeof(status));
  assert(sc == RTEMS_SUCCESSFUL);

  if (status & DS1339_STATUS_OSF)
  {
    /* RTC has been stopped. Initialise it. */
    ds1339_time_t time;

    uint8_t  write = DS1339_CTRL_DEFAULT;
    sc = ds1339_write(DS1339_ADDR_CTRL, &write, sizeof(write));
    assert(sc == RTEMS_SUCCESSFUL);

    write = DS1339_STATUS_CLEAR;
    sc = ds1339_write(DS1339_ADDR_STATUS, &write, sizeof(write));
    assert(sc == RTEMS_SUCCESSFUL);

    ds1339_set_time(&time, 0, 0, 0, 1, 1, TOD_BASE_YEAR);
    sc = ds1339_write(DS1339_ADDR_TIME, &time, sizeof(time));
    assert(sc == RTEMS_SUCCESSFUL);
  }

}


static int altera_cyclone_v_ds1339_get_time(int minor, rtems_time_of_day* tod)
{

  ds1339_time_t      time;
  rtems_status_code  sc = RTEMS_SUCCESSFUL;
  rtems_time_of_day  temp_tod;

  sc = ds1339_read(DS1339_ADDR_TIME, &time, sizeof(time));

  if (sc == RTEMS_SUCCESSFUL)
  {
    temp_tod.ticks  = 0;
    temp_tod.second = ds1339_get_seconds(&time);
    temp_tod.minute = ds1339_get_minutes(&time);
    temp_tod.hour   = ds1339_get_hours(&time);
    temp_tod.day    = ds1339_get_day_of_month(&time);
    temp_tod.month  = ds1339_get_month(&time);
    temp_tod.year   = ds1339_get_year(&time);

    if (_TOD_Validate(&temp_tod))
      memcpy(tod, &temp_tod, sizeof(temp_tod));
    else
      sc = RTEMS_INVALID_CLOCK;
  }

  return -sc;
}


static int  altera_cyclone_v_ds1339_set_time(int minor, const rtems_time_of_day* tod)
{

  ds1339_time_t      time;
  rtems_status_code  sc = RTEMS_SUCCESSFUL;

  ds1339_set_time(&time,
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


static bool  altera_cyclone_v_ds1339_probe(int minor)
{

  rtems_status_code  sc = RTEMS_SUCCESSFUL;
  uint8_t            buf;

  /* try to read from register address 0x00 */
  sc = ds1339_read(0x00, &buf, 1);
  if (sc != RTEMS_SUCCESSFUL)
    /* no RTC implemented */
    return false;
  /* try to read from register address 0x20 (not implemented in DS1339) */
  sc = ds1339_read(0x20, &buf, 1);
  if (sc == RTEMS_SUCCESSFUL)
    /* RTC is not DS1339 */
    return false;

  return true;

}


/* ******************************* M41ST87 ********************************** */


#define M41ST87_I2C_ADDRESS       (0xD0 >> 1)  /* 7-bit addressing! */
#define M41ST87_I2C_BUS_DEVICE    "/dev/i2c0"

#define M41ST87_ADDR_TIME         0x00

#define M41ST87_ADDR_CTRL         0x08
#define   M41ST87_CTRL_OUT          0x80
#define   M41ST87_CTRL_FT           0x40
#define   M41ST87_CTRL_S            0x20
#define   M41ST87_CTRL_CAL          0x1F

#define M41ST87_ADDR_ALARM_HOUR   0x0C
#define   M41ST87_BIT_HT            0x40

#define M41ST87_ADDR_FLAGS        0x0F
#define   M41ST87_FLAG_WDF          0x80
#define   M41ST87_FLAG_AF           0x40
#define   M41ST87_FLAG_BL           0x10
#define   M41ST87_FLAG_OF           0x04
#define   M41ST87_FLAG_TB1          0x02
#define   M41ST87_FLAG_TB2          0x01

#define M41ST87_ADDR_USER_RAM     0x20


typedef struct
{
  uint8_t  sec100;
  uint8_t  seconds;
#define M41ST87_BIT_ST          0x80
  uint8_t  minutes;
#define M41ST87_BIT_OFIE        0x80
  uint8_t  hours;
#define M41ST87_BIT_CB1         0x80
#define M41ST87_BIT_CB0         0x40
  uint8_t  weekday;
#define M41ST87_BIT_TR          0x80
#define M41ST87_BIT_THS         0x40
#define M41ST87_BIT_CLRPW1      0x20
#define M41ST87_BIT_CLRPW0      0x10
#define M41ST87_BIT_32KE        0x08
  uint8_t  day;
#define M41ST87_BIT_PFOD        0x80
  uint8_t  month;
  uint8_t  year;
}
m41st87_time_t;


/* The longest write transmission is writing the time + one address bit */
#define M41ST87_MAX_WRITE_SIZE  (sizeof(m41st87_time_t) + 1)


/* Functions for converting the fields */

/*
static unsigned int  m41st87_get_sec100(m41st87_time_t* time)
{

  uint8_t  tens = time->sec100 >> 4;
  uint8_t  ones = time->sec100 & 0x0F;

  return tens * 10 + ones;
}
*/


static unsigned int  m41st87_get_seconds(m41st87_time_t* time)
{

  uint8_t  tens = (time->seconds >> 4) & 0x07;
  uint8_t  ones = time->seconds & 0x0F;

  return tens * 10 + ones;
}


static unsigned int  m41st87_get_minutes(m41st87_time_t* time)
{

  uint8_t  tens = (time->minutes >> 4) & 0x07;
  uint8_t  ones = time->minutes & 0x0F;

  return tens * 10 + ones;
}


static unsigned int  m41st87_get_hours(m41st87_time_t* time)
{

  uint8_t  tens = (time->hours >> 4) & 0x03;
  uint8_t  ones = time->hours & 0x0F;

  return tens * 10 + ones;
}


/*
static unsigned int  m41st87_get_day_of_week(m41st87_time_t* time)
{

  return time->weekday & 0x07;
}
*/


static unsigned int  m41st87_get_day_of_month(m41st87_time_t* time)
{

  uint8_t  tens = (time->day >> 4) & 0x03;
  uint8_t  ones = time->day & 0x0F;

  return tens * 10 + ones;
}


static unsigned int  m41st87_get_month(m41st87_time_t* time)
{

  uint8_t  tens = (time->month >> 4) & 0x01;
  uint8_t  ones = time->month & 0x0F;

  return tens * 10 + ones;
}


static unsigned int  m41st87_get_year(m41st87_time_t* time)
{

  uint8_t  century = time->hours >> 6;
  uint8_t  tens    = time->year >> 4;
  uint8_t  ones    = time->year & 0x0F;

  return 1900 + century * 100 + tens * 10 + ones;
}


static void  m41st87_set_time(m41st87_time_t* time,
                              unsigned int second,
                              unsigned int minute,
                              unsigned int hour,
                              unsigned int day,
                              unsigned int month,
                              unsigned int year)
{

  unsigned int  century;
  unsigned int  tens;
  unsigned int  ones;

  if (year < 1900)
    year = 1900;
  if (year > 2399)
    year = 2399;
  century = (year - 1900) / 100;

  /* Hundreds of seconds is not used, set to 0 */
  time->sec100 = 0;

  tens = second / 10;
  ones = second % 10;
  time->seconds = (time->seconds & 0x80) | (tens << 4) | ones;

  tens = minute / 10;
  ones = minute % 10;
  time->minutes = (time->minutes & 0x80) | (tens << 4) | ones;

  tens = hour / 10;
  ones = hour % 10;
  time->hours = (century << 6) | (tens << 4) | ones;

  /* Weekday is not used. Therefore it can be set to an arbitrary valid value */
  time->weekday = (time->weekday & 0xF8) | 1;

  tens = day / 10;
  ones = day % 10;
  time->day = (time->day & 0x80) | (tens << 4) | ones;

  tens = month / 10;
  ones = month % 10;
  time->month = (tens << 4) | ones;

  tens = (year % 100) / 10;
  ones = year % 10;
  time->year = (tens << 4) | ones;

}



static rtems_status_code  m41st87_open_file(int* fd)
{

  int                rv = 0;
  rtems_status_code  sc = RTEMS_SUCCESSFUL;

  *fd = open(M41ST87_I2C_BUS_DEVICE, O_RDWR);
  if (*fd == -1)
    sc = RTEMS_IO_ERROR;

  if (sc == RTEMS_SUCCESSFUL)
  {
    rv = ioctl(*fd, I2C_IOC_SET_SLAVE_ADDRESS, M41ST87_I2C_ADDRESS);
    if (rv == -1)
      sc = RTEMS_IO_ERROR;
  }

  return sc;
}


/* Read size bytes from m41st87 register address addr to buf. */
static rtems_status_code  m41st87_read(uint8_t addr, void* buf, size_t size)
{

  int                fd = -1;
  int                rv = 0;
  rtems_status_code  sc = RTEMS_SUCCESSFUL;

  sc = m41st87_open_file(&fd);

  if (sc == RTEMS_SUCCESSFUL)
  {
    rv = write(fd, &addr, sizeof(addr));
    if (rv != sizeof(addr))
      sc = RTEMS_IO_ERROR;
  }

  if (sc == RTEMS_SUCCESSFUL)
  {
    rv = read(fd, buf, size);
    if (rv != size)
      sc = RTEMS_IO_ERROR;
  }

  rv = close(fd);
  if (rv != 0)
    sc = RTEMS_IO_ERROR;

  return sc;
}


/* Write size bytes from buf to m41st87 register address addr. */
static rtems_status_code  m41st87_write(uint8_t addr, void* buf, size_t size)
{

  int                fd = -1;
  int                rv = 0;
  rtems_status_code  sc = RTEMS_SUCCESSFUL;
  /* The driver never writes many bytes. Therefore it should be less expensive
   * to reserve the maximum number of bytes that will be written in one go than
   * use a malloc. */
  uint8_t            local_buf[M41ST87_MAX_WRITE_SIZE];
  int                write_size = size + 1;

  assert(write_size <= M41ST87_MAX_WRITE_SIZE);

  local_buf[0] = addr;
  memcpy(&local_buf[1], buf, size);

  sc = m41st87_open_file(&fd);

  if (sc == RTEMS_SUCCESSFUL)
  {
    rv = write(fd, local_buf, write_size);
    if (rv != write_size)
      sc = RTEMS_IO_ERROR;
  }

  rv = close(fd);
  if (rv != 0)
    sc = RTEMS_IO_ERROR;

  return RTEMS_SUCCESSFUL;
}


static void  altera_cyclone_v_m41st87_initialize(int minor)
{

  m41st87_time_t     time;
  rtems_status_code  sc = RTEMS_SUCCESSFUL;
  uint8_t            value;

  /* Check RTC valid */
  sc = m41st87_read(M41ST87_ADDR_TIME, &time, sizeof(time));
  assert(sc == RTEMS_SUCCESSFUL);

  if (time.seconds & M41ST87_BIT_ST)
  {
    /* RTC has been stopped. Reset stop flag. */
    time.seconds = 0;
    /* Initialise RTC. */
    m41st87_set_time(&time, 0, 0, 0, 1, 1, TOD_BASE_YEAR);
    sc = m41st87_write(M41ST87_ADDR_TIME, &time, sizeof(time));
    assert(sc == RTEMS_SUCCESSFUL);
  }

  /* Reset HT bit */
  sc = m41st87_read(M41ST87_ADDR_ALARM_HOUR, &value, 1);
  assert(sc == RTEMS_SUCCESSFUL);
  value &= ~M41ST87_BIT_HT;
  sc = m41st87_write(M41ST87_ADDR_ALARM_HOUR, &value, 1);
  assert(sc == RTEMS_SUCCESSFUL);

}


static int  altera_cyclone_v_m41st87_get_time(int minor, rtems_time_of_day* tod)
{

  m41st87_time_t     time;
  rtems_status_code  sc = RTEMS_SUCCESSFUL;
  rtems_time_of_day  temp_tod;

  sc = m41st87_read(M41ST87_ADDR_TIME, &time, sizeof(time));
  if (sc != RTEMS_SUCCESSFUL)
    return -sc;

  temp_tod.ticks  = 0;
  temp_tod.second = m41st87_get_seconds(&time);
  temp_tod.minute = m41st87_get_minutes(&time);
  temp_tod.hour   = m41st87_get_hours(&time);
  temp_tod.day    = m41st87_get_day_of_month(&time);
  temp_tod.month  = m41st87_get_month(&time);
  temp_tod.year   = m41st87_get_year(&time);

  if (_TOD_Validate(&temp_tod))
    memcpy(tod, &temp_tod, sizeof(temp_tod));
  else
    sc = RTEMS_INVALID_CLOCK;

  return -sc;
}


static int  altera_cyclone_v_m41st87_set_time(int minor, const rtems_time_of_day* tod)
{

  m41st87_time_t     time;
  rtems_status_code  sc = RTEMS_SUCCESSFUL;

  /* first read to preserve the additional flags */
  sc = m41st87_read(M41ST87_ADDR_TIME, &time, sizeof(time));
  if (sc != RTEMS_SUCCESSFUL)
    return -sc;

  m41st87_set_time(&time,
                   tod->second,
                   tod->minute,
                   tod->hour,
                   tod->day,
                   tod->month,
                   tod->year
                  );

  sc = m41st87_write(M41ST87_ADDR_TIME, &time, sizeof(time));

  return -sc;
}


static bool  altera_cyclone_v_m41st87_probe(int minor)
{

  rtems_status_code  sc = RTEMS_SUCCESSFUL;
  uint8_t            buf;

  /* try to read from register address 0x00 */
  sc = m41st87_read(0x00, &buf, 1);
  if (sc != RTEMS_SUCCESSFUL)
    /* no RTC implemented */
    return false;
  /* try to read from register address 0x20 (implemented in M41ST87) */
  sc = m41st87_read(0x20, &buf, 1);
  if (sc != RTEMS_SUCCESSFUL)
    /* RTC is not M41ST87 */
    return false;

  return true;

}


/* **************************************** General ********************************** */


const rtc_fns  altera_cyclone_v_ds1339_ops =
{
  .deviceInitialize = altera_cyclone_v_ds1339_initialize,
  .deviceGetTime = altera_cyclone_v_ds1339_get_time,
  .deviceSetTime = altera_cyclone_v_ds1339_set_time
};


const rtc_fns  altera_cyclone_v_m41st87_ops =
{
  .deviceInitialize = altera_cyclone_v_m41st87_initialize,
  .deviceGetTime = altera_cyclone_v_m41st87_get_time,
  .deviceSetTime = altera_cyclone_v_m41st87_set_time
};


size_t  RTC_Count = ALTERA_CYCLONE_V_RTC_NUMBER;

rtc_tbl  RTC_Table[ALTERA_CYCLONE_V_RTC_NUMBER] =
{
  {
    .sDeviceName = "/dev/rtc",
    .deviceType = RTC_CUSTOM,
    .pDeviceFns = &altera_cyclone_v_ds1339_ops,
    .deviceProbe = altera_cyclone_v_ds1339_probe,
    .pDeviceParams = NULL,
    .ulCtrlPort1 = 0,
    .ulDataPort = 0,
    .getRegister = NULL,
    .setRegister = NULL
  },
  {
    .sDeviceName = "/dev/rtc",
    .deviceType = RTC_CUSTOM,
    .pDeviceFns = &altera_cyclone_v_m41st87_ops,
    .deviceProbe = altera_cyclone_v_m41st87_probe,
    .pDeviceParams = NULL,
    .ulCtrlPort1 = 0,
    .ulDataPort = 0,
    .getRegister = NULL,
    .setRegister = NULL
  }
};
