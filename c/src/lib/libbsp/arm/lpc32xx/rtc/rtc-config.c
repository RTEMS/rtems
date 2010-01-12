/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief RTC configuration.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <libchip/rtc.h>

#include <bsp.h>
#include <bsp/lpc32xx.h>

#define LPC32XX_RTC_COUNT 1U

#define LPC32XX_RTC_COUNTER_DELTA 0xfffffffeU

#define LPC32XX_RTC_KEY 0xb5c13f27U

#define LPC32XX_RTC_CTRL_FORCE_ONSW (1U << 7)
#define LPC32XX_RTC_CTRL_STOP (1U << 6)
#define LPC32XX_RTC_CTRL_RESET (1U << 4)
#define LPC32XX_RTC_CTRL_MATCH_1_ONSW (1U << 3)
#define LPC32XX_RTC_CTRL_MATCH_0_ONSW (1U << 2)
#define LPC32XX_RTC_CTRL_MATCH_1_INTR (1U << 1)
#define LPC32XX_RTC_CTRL_MATCH_0_INTR (1U << 0)

typedef struct {
  uint32_t ucount;
  uint32_t dcount;
  uint32_t match0;
  uint32_t match1;
  uint32_t ctrl;
  uint32_t intstat;
  uint32_t key;
  uint32_t sram [32];
} lpc32xx_rtc_registers;

static volatile lpc32xx_rtc_registers *const lpc32xx_rtc =
  (volatile lpc32xx_rtc_registers *) LPC32XX_BASE_RTC;

static void lpc32xx_rtc_set(uint32_t val)
{
  unsigned i = LPC32XX_ARM_CLK / LPC32XX_OSCILLATOR_RTC;

  lpc32xx_rtc->ctrl |= LPC32XX_RTC_CTRL_STOP;
  lpc32xx_rtc->ucount = val;
  lpc32xx_rtc->dcount = LPC32XX_RTC_COUNTER_DELTA - val;
  lpc32xx_rtc->ctrl &= ~LPC32XX_RTC_CTRL_STOP;

  /* It needs some time before we can read the values back */
  while (i != 0) {
    asm volatile ("nop");
    --i;
  }
}

static void lpc32xx_rtc_reset(void)
{
  lpc32xx_rtc->ctrl = LPC32XX_RTC_CTRL_RESET;
  lpc32xx_rtc->ctrl = 0;
  lpc32xx_rtc->key = LPC32XX_RTC_KEY;
  lpc32xx_rtc_set(0);
}

static void lpc32xx_rtc_initialize(int minor)
{
  uint32_t up_first = 0;
  uint32_t up_second = 0;
  uint32_t down_first = 0;
  uint32_t down_second = 0;

  if (lpc32xx_rtc->key != LPC32XX_RTC_KEY) {
    lpc32xx_rtc_reset();
  }

  do {
    up_first = lpc32xx_rtc->ucount;
    down_first = lpc32xx_rtc->dcount;
    up_second = lpc32xx_rtc->ucount;
    down_second = lpc32xx_rtc->dcount;
  } while (up_first != up_second || down_first != down_second);

  if (up_first + down_first != LPC32XX_RTC_COUNTER_DELTA) {
    lpc32xx_rtc_reset();
  }
}

static int lpc32xx_rtc_get_time(int minor, rtems_time_of_day *tod)
{
  struct timeval now = {
    .tv_sec = lpc32xx_rtc->ucount,
    .tv_usec = 0
  };
  struct tm time;

  gmtime_r(&now.tv_sec, &time);

  tod->year   = time.tm_year + 1900;
  tod->month  = time.tm_mon + 1;
  tod->day    = time.tm_mday;
  tod->hour   = time.tm_hour;
  tod->minute = time.tm_min;
  tod->second = time.tm_sec;
  tod->ticks  = 0;

  return RTEMS_SUCCESSFUL;
}

static int lpc32xx_rtc_set_time(int minor, const rtems_time_of_day *tod)
{
  lpc32xx_rtc_set(_TOD_To_seconds(tod));

  return 0;
}

static bool lpc32xx_rtc_probe(int minor)
{
  return true;
}

const rtc_fns lpc32xx_rtc_ops = {
  .deviceInitialize = lpc32xx_rtc_initialize,
  .deviceGetTime = lpc32xx_rtc_get_time,
  .deviceSetTime = lpc32xx_rtc_set_time
};

unsigned long RTC_Count = LPC32XX_RTC_COUNT;

rtems_device_minor_number RTC_Minor = 0;

rtc_tbl	RTC_Table [LPC32XX_RTC_COUNT] = {
  {
    .sDeviceName = "/dev/rtc",
    .deviceType = RTC_CUSTOM,
    .pDeviceFns = &lpc32xx_rtc_ops,
    .deviceProbe = lpc32xx_rtc_probe,
    .pDeviceParams = NULL,
    .ulCtrlPort1 = 0,
    .ulDataPort = 0,
    .getRegister = NULL,
    .setRegister = NULL
  }
};
