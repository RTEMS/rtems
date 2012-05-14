/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bspopts.h>

/* XXX convert to macros? Move to score/cpu? */
void mips_set_timer(uint32_t timer_clock_interval);
uint32_t mips_get_timer(void);
uint32_t bsp_clock_nanoseconds_since_last_tick(void);

/* XXX move to BSP.h or irq.h?? */
#define EXT_INT5             0x8000  /* external interrupt 5 */
#define CLOCK_VECTOR_MASK    EXT_INT5
#define CLOCK_VECTOR         (MIPS_INTERRUPT_BASE+0x7)

extern uint32_t bsp_clicks_per_microsecond;

static uint32_t   mips_timer_rate = 0;

/* refresh the internal CPU timer */
#define Clock_driver_support_at_tick() \
  mips_set_timer( mips_timer_rate );

#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
    rtems_interrupt_handler_install(CLOCK_VECTOR, \
      "PIT clock",0, _new, NULL); \
   } while(0)

#define Clock_driver_support_initialize_hardware() \
  do { \
    mips_timer_rate = rtems_configuration_get_microseconds_per_tick() * \
      bsp_clicks_per_microsecond; \
    mips_set_timer( mips_timer_rate ); \
    printk("Clock_mask: %x\n", CLOCK_VECTOR_MASK ); \
    mips_enable_in_interrupt_mask(CLOCK_VECTOR_MASK); \
  } while(0)

uint32_t bsp_clock_nanoseconds_since_last_tick(void)
{
  return 0;
}

#define Clock_driver_nanoseconds_since_last_tick \
  bsp_clock_nanoseconds_since_last_tick

#define Clock_driver_support_shutdown_hardware() \
  do { \
    mips_disable_in_interrupt_mask(CLOCK_VECTOR_MASK); \
  } while (0)

#include "../../../shared/clockdrv_shell.h"
