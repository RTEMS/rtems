/**
 *  @file clockdrv.c
 *
 *  Game Boy Advance Clock driver.
 */

/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <gba.h>
#include <assert.h>


void Clock_isr(void * arg);

#define Clock_driver_support_at_tick()

#define Clock_driver_support_install_isr( _new, _old )  \
  do {                                                  \
    rtems_status_code status = RTEMS_SUCCESSFUL;        \
    status = rtems_interrupt_handler_install(           \
        BSP_IRQ_TIMER3,                                 \
        "Clock",                                        \
        RTEMS_INTERRUPT_UNIQUE,                         \
        Clock_isr,                                      \
        NULL                                            \
    );                                                  \
    assert(status == RTEMS_SUCCESSFUL);                 \
    _old = NULL;                                        \
  } while(0)

#define Clock_driver_support_shutdown_hardware()        \
  do {                                                  \
    rtems_status_code status = RTEMS_SUCCESSFUL;        \
    status = rtems_interrupt_handler_remove(            \
        BSP_IRQ_TIMER3,                                 \
        Clock_isr,                                      \
        NULL                                            \
    );                                                  \
    assert(status == RTEMS_SUCCESSFUL);                 \
  } while (0)


/*
 * Calculate Tick Times
 *    1 / 16.78Mhz  => 59.595 ns
 *   64 / 16.78Mhz  =>  3.814 us
 *  256 / 16.78Mhz  => 15.256 us
 * 1024 / 16.78Mhz  => 61.025 us
 */
#define  __TimTickTime_us   ((1000000L/__ClockFrequency)*__TimPreScaler)
#define  __TimTickTime_ns   ((1000000000L/__ClockFrequency)*__TimPreScaler)

#if (__TimPreScaler==1)
 #define GBA_TMCNT_PS    0x0000
#elif (__TimPreScaler==64)
 #define GBA_TMCNT_PS    0x0001
#elif (__TimPreScaler==256)
 #define GBA_TMCNT_PS    0x0002
#elif (__TimPreScaler==1024)
 #define GBA_TMCNT_PS    0x0003
#else
 #define GBA_TMCNT_PS    0x0003
#endif

/**
 *  @brief This function set up the clock hardware
 *
 *  @param  None
 *  @return None
 */
void Clock_driver_support_initialize_hardware(void)
{
  int tmreload = ((rtems_configuration_get_microseconds_per_tick()*1000)/__TimTickTime_ns);

  if (tmreload>0xFFFF) tmreload = 0xFFFF;
  GBA_REG_TM3CNT = (GBA_TMCNT_PS);
  GBA_REG_TM3D   = (0x0000-tmreload);
  GBA_REG_TM3CNT = (0x00c0|GBA_TMCNT_PS);
}

#include "../../../shared/clockdrv_shell.h"
