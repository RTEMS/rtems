/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief RTC driver for AM335x SoC.
 *
 */

/*
 * Copyright (c) 2015 Ragunath <ragunath3252@gmail.com>.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bspopts.h>

#if IS_AM335X
#include <rtems.h>
#include <bsp.h>
#include <time.h>
#include <libchip/rtc.h>
#include <libcpu/omap3.h>

#define setbit(a,x) (a | (1<<x))
#define bcd(a) ((a & 0x0f)+ (((a & 0xf0) >> 4 )*10))
#define dec(a) (((a / 10) << 4) | (a % 10))
#define WRITE_WAIT_MAX_COUNT 10000

size_t RTC_Count = 1;

static void rtc_write_enable(void);
static void rtc_write_disable(void);
static int rtc_write_wait(void);
static void rtc_clk_init(void);
void rtc_init(int minor);
void print_time(void);
int am335x_rtc_gettime(int minor,rtems_time_of_day *t);
int am335x_rtc_settime(int minor, const rtems_time_of_day *t);
void am335x_rtc_debug(void);

/*
 * probe for a rtc. we always claim to have one.
 */
static bool am335x_rtc_probe (int minor)
{
  return true;
}

/*
 * Write key values to kick0 and kick1 registers to enable write access
 */
static void rtc_write_enable(void)
{
  mmio_write(AM335X_RTC_BASE+AM335X_RTC_KICK0,AM335X_RTC_KICK0_KEY);
  mmio_write(AM335X_RTC_BASE+AM335X_RTC_KICK1,AM335X_RTC_KICK1_KEY);
}

/*
 * Write random (0x11111111) value to kick0 and kick1 registers to disable write access
 */
static void rtc_write_disable(void)
{
  /* Write some random value other than key to disable*/
  mmio_write(AM335X_RTC_BASE+AM335X_RTC_KICK0,0x11111111);
  mmio_write(AM335X_RTC_BASE+AM335X_RTC_KICK1,0x11111111);
}

/*
 * Wait till busy bit is reset
 */
static int rtc_write_wait(void)
{
  int i = WRITE_WAIT_MAX_COUNT;
  while((mmio_read(AM335X_RTC_BASE+AM335X_RTC_STATUS_REG) & 0x1) && (i--));

  if(i == 0)
      return RTEMS_RESOURCE_IN_USE;
  else
      return RTEMS_SUCCESSFUL;
}


/*
 * Initialize RTC clock
 */
static void rtc_clk_init(void)
{
  uint32_t a = 0x0;

  a = setbit(a,1);
  /* IDLEST = 0x0 & MODULEMODE = 0x1*/
  mmio_write(CM_RTC_BASE+CM_RTC_RTC_CLKCTRL,a);
  a = 0x0;

  /*32K rtc clock active*/
  a = setbit(a,9);
  a = setbit(a,8);
  mmio_write(CM_RTC_BASE+CM_RTC_CLKSTCTRL,a);
}

void rtc_init(int minor)
{
  uint32_t a = 0x0;

  rtc_clk_init();
   /*
   * Steps to enable RTC
   * 1. Enable the module clock domains (rtc_clk_init).
   * 2. Enable the RTC module using CTRL_REG.RTC_disable. (Default enabled. Nothing done)
   * 3. Enable the 32K clock from PER PLL, if using the internal RTC oscillator.
   * 4. Write to the kick registers (KICK0R, KICK1R) in the RTC.
   * 5. Configure the timer in RTCSS for desired application (set time and date, alarm wakeup, and so on).
   * 6. Start the RTC (in CTRL_REG.STOP_RTC).
   */
  rtc_write_enable();
  a = setbit(a,0);
  mmio_write(AM335X_RTC_BASE+AM335X_RTC_SYSCONFIG,a);
  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_OSC_CLOCK);
  a = setbit(a,6);
  mmio_write(AM335X_RTC_BASE+AM335X_RTC_OSC_CLOCK,a);
  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_CTRL_REG);
  a = setbit(a,0);
  mmio_write(AM335X_RTC_BASE+AM335X_RTC_CTRL_REG,a);

  rtc_write_disable();
}

int am335x_rtc_gettime(int minor,rtems_time_of_day *t)
{
  uint32_t a = 0x0;

  if(minor != 0)
    return RTEMS_INVALID_NUMBER;

  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_SECS);
  t->second = bcd(a);
  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_MINS);
  t->minute = bcd(a);
  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_HOURS);
  t->hour = bcd(a);
  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_DAYS);
  t->day = bcd(a);
  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_MONTHS);
  t->month = bcd(a);
  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_YEARS);
  t->year = bcd(a)+2000;
  t->ticks=0;
  return RTEMS_SUCCESSFUL;
}

int am335x_rtc_settime(int minor,const rtems_time_of_day *t)
{
  uint32_t a=0x0;
  int rv;

  if(minor != 0)
    return RTEMS_INVALID_NUMBER;

  rtc_write_enable();

  /* Wait till the busy bit is reset to write again*/
  a = t->second;
  rv=rtc_write_wait();
  if(rv != RTEMS_SUCCESSFUL)
      return rv;
  a = mmio_write(AM335X_RTC_BASE+AM335X_RTC_SECS,dec(a) & 0x7f);

  a = t->minute;
  rv=rtc_write_wait();
  if(rv != RTEMS_SUCCESSFUL)
      return rv;
  a = mmio_write(AM335X_RTC_BASE+AM335X_RTC_MINS,dec(a) & 0x7f);

  a = t->hour;
  rv=rtc_write_wait();
  if(rv != RTEMS_SUCCESSFUL)
      return rv;
  a = mmio_write(AM335X_RTC_BASE+AM335X_RTC_HOURS,dec(a) & 0x3f);

  a = t->day;
  rv=rtc_write_wait();
  if(rv != RTEMS_SUCCESSFUL)
      return rv;
  a = mmio_write(AM335X_RTC_BASE+AM335X_RTC_DAYS,dec(a) & 0x3f);

  a = t->month;
  rv=rtc_write_wait();
  if(rv != RTEMS_SUCCESSFUL)
      return rv;
  a = mmio_write(AM335X_RTC_BASE+AM335X_RTC_MONTHS,dec(a) & 0x1f);

  a = t->year;
  rv=rtc_write_wait();
  if(rv != RTEMS_SUCCESSFUL)
      return rv;
  a = mmio_write(AM335X_RTC_BASE+AM335X_RTC_YEARS,(dec(a)%100) & 0xff);

  rtc_write_disable();
  return rv;
}

#if BBB_DEBUG
void print_time(void)
{
  uint32_t a = 0x0;
  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_SECS);
  printk("\n\rSecs %x",a);
  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_MINS);
  printk("\n\rMins %x",a);
  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_HOURS);
  printk("\n\rHours %x",a);
  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_DAYS);
  printk("\n\rDays %x",a);
  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_MONTHS);
  printk("\n\r Months %x",a);
  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_YEARS);
  printk("\n\rYears %x",a);
  a = mmio_read(AM335X_RTC_BASE+AM335X_RTC_WEEKS);
  printk("\n\rWeeks %x",a);
}

void am335x_rtc_debug(void)
{
  int i;
  rtems_time_of_day t,r;

  t.second = 1;
  t.minute = 1;
  t.hour = 1;
  t.day = 7;
  t.month = 3;
  t. year = 2015;

  am335x_rtc_settime(0,&t);
  am335x_rtc_gettime(0,&r);

  printk("Secs %x",r.second);
  printk("Mins %x",r.minute);
  printk("Hours %x",r.hour);
  printk("Days %x",r.day);
  printk("Months %x",r.month);
  printk("Years %x",r.year);
}
#endif

/*
 * driver function table.
 */
rtc_fns am335x_rtc_fns = {
  rtc_init,
  am335x_rtc_gettime,
  am335x_rtc_settime
};

/*
 * the following table configures the RTC drivers used in this BSP
 */

rtc_tbl RTC_Table[] = {
  {
   "/dev/rtc",                  /* sDeviceName */
   RTC_CUSTOM,                  /* deviceType */
   &am335x_rtc_fns,             /* pDeviceFns */
   am335x_rtc_probe,            /* deviceProbe */
   NULL,                        /* pDeviceParams */
   0,                           /* ulCtrlPort1 */
   0,                           /* ulDataPort */
   NULL,                        /* getRegister */
   NULL                         /* setRegister */
   }
};

#endif
