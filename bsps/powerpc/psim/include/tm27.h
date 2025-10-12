/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 * @file
 * @ingroup powerpc_psim
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

#include <bsp/irq.h>

/*
 *  Stuff for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 1

static void stub_rtems_irq_enable( const struct __rtems_irq_connect_data__ *i )
{
  (void) i;
}

static void stub_rtems_irq_disable( const struct __rtems_irq_connect_data__ *i )
{
  (void) i;
}

static int stub_rtems_irq_is_enabled(
  const struct __rtems_irq_connect_data__ *i
)
{
  (void) i;
  return 0;
}

static rtems_irq_connect_data clockIrqData = {
  .name = BSP_DECREMENTER,
  .hdl = 0,
  .handle = 0,
  .on = stub_rtems_irq_enable,
  .off = stub_rtems_irq_disable,
  .isOn = stub_rtems_irq_is_enabled,
  .next_handler = NULL
};

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  clockIrqData.hdl = handler;
  if ( !BSP_install_rtems_irq_handler( &clockIrqData ) ) {
    printk( "Error installing clock interrupt handler!\n" );
    rtems_fatal_error_occurred( 1 );
  }
}

#define Cause_tm27_intr()                   \
  do {                                      \
    uint32_t _clicks = 1;                   \
    __asm__ volatile( "mtdec %0"            \
                      : "=r"(( _clicks ))   \
                      : "r"(( _clicks )) ); \
  } while ( 0 )

#define Clear_tm27_intr()                   \
  do {                                      \
    uint32_t _clicks = 0xffffffff;          \
    __asm__ volatile( "mtdec %0"            \
                      : "=r"(( _clicks ))   \
                      : "r"(( _clicks )) ); \
  } while ( 0 )

#define Lower_tm27_intr()              \
  do {                                 \
    uint32_t _msr = 0;                 \
    _ISR_Set_level( 0 );               \
    __asm__ volatile( "mfmsr %0 ;"     \
                      : "=r"( _msr )   \
                      : "r"( _msr ) ); \
    _msr |= 0x8002;                    \
    __asm__ volatile( "mtmsr %0 ;"     \
                      : "=r"( _msr )   \
                      : "r"( _msr ) ); \
  } while ( 0 )

#endif
