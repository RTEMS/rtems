/*
 *  tm27.h
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

#include <bsp/irq.h>

/*
 *  Stuff for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 1

void nullFunc() {}

static rtems_irq_connect_data clockIrqData = {BSP_DECREMENTER,
                                              0,
                                              (rtems_irq_enable)nullFunc,
                                              (rtems_irq_disable)nullFunc,
                                              (rtems_irq_is_enabled) nullFunc};
void Install_tm27_vector(void (*_handler)())
{
  clockIrqData.hdl = _handler;
  if (!BSP_install_rtems_irq_handler (&clockIrqData)) {
        printk("Error installing clock interrupt handler!\n");
        rtems_fatal_error_occurred(1);
  }
}

#define Cause_tm27_intr()  \
  do { \
    uint32_t   _clicks = 8; \
    __asm__ volatile( "mtdec %0" : "=r" ((_clicks)) : "r" ((_clicks)) ); \
  } while (0)

#define Clear_tm27_intr() \
  do { \
    uint32_t   _clicks = 0xffffffff; \
    __asm__ volatile( "mtdec %0" : "=r" ((_clicks)) : "r" ((_clicks)) ); \
  } while (0)

#define Lower_tm27_intr() \
  do { \
    uint32_t   _msr = 0; \
    _ISR_Set_level( 0 ); \
    __asm__ volatile( "mfmsr %0 ;" : "=r" (_msr) : "r" (_msr) ); \
    _msr |=  0x8002; \
    __asm__ volatile( "mtmsr %0 ;" : "=r" (_msr) : "r" (_msr) ); \
  } while (0)

#endif
