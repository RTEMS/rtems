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


void Clock_isr(rtems_irq_hdl_param arg);
static void clock_isr_on(const rtems_irq_connect_data *unused);
static void clock_isr_off(const rtems_irq_connect_data *unused);
static int clock_isr_is_on(const rtems_irq_connect_data *irq);

rtems_irq_connect_data clock_isr_data = {
  .name   = BSP_IRQ_TIMER3,
  .hdl    = Clock_isr,
  .handle = NULL,
  .on     = clock_isr_on,
  .off    = clock_isr_off,
  .isOn   = clock_isr_is_on,
};

#define Clock_driver_support_at_tick()

#define Clock_driver_support_install_isr( _new, _old )  \
  do {                                                  \
    BSP_install_rtems_irq_handler(&clock_isr_data);     \
    _old = NULL;                                        \
  } while(0)

#define Clock_driver_support_shutdown_hardware()    \
  do {                                              \
    BSP_remove_rtems_irq_handler(&clock_isr_data);  \
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

/**
 *  @brief This function is empty
 *
 *  @param  unused an unused parameter
 *  @return None
 */
static void clock_isr_on(const rtems_irq_connect_data *unused)
{
}

/**
 *  @brief This function is empty
 *
 *  @param  unused an unused parameter
 *  @return None
 */
static void clock_isr_off(const rtems_irq_connect_data *unused)
{
}

/**
 *  @brief This function is empty
 *
 *  @param  irq unused
 *  @return constant 1
 */
static int clock_isr_is_on(const rtems_irq_connect_data *irq)
{
  return 1;
}

#include "../../../shared/clockdrv_shell.h"
