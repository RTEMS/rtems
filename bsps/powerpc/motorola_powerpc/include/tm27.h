/*
 * @file
 * @ingroup powerpc_motorola_powerpc
 * @brief Implementations for interrupt mechanisms for Time Test 27
 */

/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

/*
 *  Stuff for Time Test 27
 */

#include <bsp/irq.h>

#define MUST_WAIT_FOR_INTERRUPT 1

static void null_irq_enable(const rtems_irq_connect_data* a) { (void) a; }
static void null_irq_disable(const rtems_irq_connect_data* a) { (void) a; }
static int null_irq_is_enabled(const rtems_irq_connect_data* a) { (void) a; return 0; }

static rtems_irq_connect_data clockIrqData =
{
 .name = BSP_DECREMENTER,
 .hdl = 0,
 .handle = 0,
 .on = null_irq_enable,
 .off = null_irq_disable,
 .isOn = null_irq_is_enabled
};

static void Install_tm27_vector(rtems_isr (*_handler)(rtems_vector_number))
{
  clockIrqData.hdl = (rtems_irq_hdl) _handler;
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
