/*
 * This file contains the RTC driver table for Motorola shared BSPs.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#include <bsp.h>
#include <libchip/rtc.h>
#include <libchip/m48t08.h>

/* Forward function declaration */
#if !defined(mvme2100)
unsigned32 mvmertc_get_register( unsigned32, unsigned8 );
void mvmertc_set_register( unsigned32, unsigned8, unsigned32 );
#endif

/* The following table configures the RTC drivers used in this BSP */
rtc_tbl RTC_Table[] = {
    {
        "/dev/rtc",                /* sDeviceName */
        RTC_M48T08,                /* deviceType  -- actually M48T59 */
        &m48t08_fns,               /* pDeviceFns */
        rtc_probe,                 /* deviceProbe */
        NULL,                      /* pDeviceParams */
#if defined(mvme2100)
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

unsigned long RTC_Count = NUM_RTCS;

rtems_device_minor_number RTC_Minor;

#if !defined(mvme2100)
#include <rtems/bspIo.h>
void mvmertc_set_register(
  unsigned32 base,
  unsigned8  reg,
  unsigned32 value
)
{
  printk( "RTC SUPPORT NOT IMPLEMENTED ON THIS BOARD\n");
}

unsigned32 mvmertc_get_register(
  unsigned32 base,
  unsigned8  reg
)
{
  printk( "RTC SUPPORT NOT IMPLEMENTED ON THIS BOARD\n");
  return 0;
}
#endif
