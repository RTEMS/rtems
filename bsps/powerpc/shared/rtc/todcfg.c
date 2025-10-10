/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 * This file contains the RTC driver table for Motorola shared BSPs.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <libchip/rtc.h>
#include <libchip/m48t08.h>

/* Forward function declaration */
#if !defined(mot_ppc_mvme2100)
uint32_t mvmertc_get_register( uintptr_t, uint8_t );
void mvmertc_set_register( uintptr_t, uint8_t, uint32_t );
#endif

/* The following table configures the RTC drivers used in this BSP */
rtc_tbl RTC_Table[] = {
    {
        "/dev/rtc",                /* sDeviceName */
        RTC_M48T08,                /* deviceType  -- actually M48T59 */
        &m48t08_fns,               /* pDeviceFns */
        rtc_probe,                 /* deviceProbe */
        NULL,                      /* pDeviceParams */
#if defined(mot_ppc_mvme2100)
        0xFFE81ff8,                /* ulCtrlPort1 */
        0x00,                      /* ulDataPort */
        m48t08_get_register,       /* getRegister */
        m48t08_set_register        /* setRegister */
#else
        0xFFE81ff8,                /* ulCtrlPort1 */
        0x00,                      /* ulDataPort */
        mvmertc_get_register,      /* getRegister */
        mvmertc_set_register       /* setRegister */
#endif
    }
};

/* Some information used by the RTC driver */

#define NUM_RTCS (sizeof(RTC_Table)/sizeof(rtc_tbl))

size_t RTC_Count = NUM_RTCS;

#if !defined(mot_ppc_mvme2100)
#include <rtems/bspIo.h>
void mvmertc_set_register(
  uintptr_t base,
  uint8_t   reg,
  uint32_t  value
)
{
  (void) base;
  (void) reg;
  (void) value;

  printk( "RTC SUPPORT NOT IMPLEMENTED ON THIS BOARD\n");
}

uint32_t mvmertc_get_register(
  uintptr_t base,
  uint8_t   reg
)
{
  (void) base;
  (void) reg;

  printk( "RTC SUPPORT NOT IMPLEMENTED ON THIS BOARD\n");
  return 0;
}
#endif
