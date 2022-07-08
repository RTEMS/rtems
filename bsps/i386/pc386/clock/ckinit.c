/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  Clock Tick Device Driver
 *
 *  History:
 *    + Original driver was go32 clock by Joel Sherrill
 *    + go32 clock driver hardware code was inserted into new
 *      boilerplate when the pc386 BSP by:
 *        Pedro Miguel Da Cruz Neto Romano <pmcnr@camoes.rnl.ist.utl.pt>
 *        Jose Rufino <ruf@asterix.ist.utl.pt>
 *    + Reworked by Joel Sherrill to use clock driver template.
 *      This removes all boilerplate and leave original hardware
 *      code I developed for the go32 BSP.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <bspopts.h>
#include <libcpu/cpuModel.h>
#include <assert.h>
#include <rtems/timecounter.h>
#ifdef RTEMS_SMP
#include <rtems/score/smpimpl.h>
#endif

#define CLOCK_VECTOR 0

volatile uint32_t pc386_microseconds_per_isr;
volatile uint32_t pc386_isrs_per_tick;
uint32_t pc386_clock_click_count;

/* forward declaration */
void Clock_isr(void *param);
static void Clock_isr_handler(void *param);

/*
 * Roughly the number of cycles per second. Note that these
 * will be wildly inaccurate if the chip speed changes due to power saving
 * or thermal modes.
 *
 * NOTE: These are only used when the TSC method is used.
 */
static uint64_t pc586_tsc_frequency;

static struct timecounter pc386_tc;

/* this driver may need to count ISRs per tick */
#define CLOCK_DRIVER_ISRS_PER_TICK       1
#define CLOCK_DRIVER_ISRS_PER_TICK_VALUE pc386_isrs_per_tick

extern volatile uint32_t Clock_driver_ticks;

#define READ_8254( _lsb, _msb )                               \
  do { outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_LATCH);      \
     inport_byte(TIMER_CNTR0, _lsb);                          \
     inport_byte(TIMER_CNTR0, _msb);                          \
  } while (0)


#ifdef RTEMS_SMP
#define Clock_driver_support_at_tick() \
  do {                                                              \
    Processor_mask targets;                                         \
    _Processor_mask_Assign(&targets, _SMP_Get_online_processors()); \
    _Processor_mask_Clear(&targets, _SMP_Get_current_processor());  \
    _SMP_Multicast_action(&targets, rtems_timecounter_tick, NULL);               \
  } while (0)
#endif

static uint32_t pc386_get_timecount_tsc(struct timecounter *tc)
{
  return (uint32_t)rdtsc();
}

static uint32_t pc386_get_timecount_i8254(struct timecounter *tc)
{
  uint32_t                 irqs;
  uint8_t                  lsb, msb;
  rtems_interrupt_lock_context lock_context;

  /*
   * Fetch all the data in an interrupt critical section.
   */

  rtems_interrupt_lock_acquire(&rtems_i386_i8254_access_lock, &lock_context);

    READ_8254(lsb, msb);
    irqs = Clock_driver_ticks;

  rtems_interrupt_lock_release(&rtems_i386_i8254_access_lock, &lock_context);

  return (irqs + 1) * pc386_microseconds_per_isr - ((msb << 8) | lsb);
}

/*
 * Calibrate CPU cycles per tick. Interrupts should be disabled.
 * Will also set the PIT, so call this before registering the 
 * periodic timer for rtems tick generation
 */
static void calibrate_tsc(void)
{
  uint64_t              begin_time;
  uint8_t               lsb, msb;
  uint32_t              max_timer_value;
  uint32_t              last_tick, cur_tick;
  int32_t               diff, remaining;

  /* Set the timer to free running mode */
  outport_byte(TIMER_MODE, TIMER_SEL0 | TIMER_16BIT | TIMER_INTTC);
  /* Reset the 16 timer reload value, first LSB, then MSB */
  outport_byte(TIMER_CNTR0, 0);
  outport_byte(TIMER_CNTR0, 0);
  /* We use the full 16 bit */
  max_timer_value = 0xffff;
  /* Calibrate for 1s, i.e. TIMER_TICK PIT ticks */
  remaining = TIMER_TICK;

  begin_time = rdtsc();
  READ_8254(lsb, msb);
  last_tick = (msb << 8) | lsb;
  while(remaining > 0) {
    READ_8254(lsb, msb);
    cur_tick = (msb << 8) | lsb;
    /* PIT counts down, so subtract cur from last */
    diff = last_tick - cur_tick;
    last_tick = cur_tick;
    if (diff < 0) {
        diff += max_timer_value;
    }
    remaining -= diff;
  }

  pc586_tsc_frequency = rdtsc() - begin_time;

#if 0
  printk( "CPU clock at %u Hz\n", (uint32_t)(pc586_tsc_frequency ));
#endif
}

static void clockOn(void)
{

  /*
   * First calibrate the TSC. Do this every time we
   * turn the clock on in case the CPU clock speed has changed.
   */
  if ( x86_has_tsc() ) {
    calibrate_tsc();
  }

  rtems_interrupt_lock_context lock_context;
  pc386_isrs_per_tick        = 1;
  pc386_microseconds_per_isr = rtems_configuration_get_microseconds_per_tick();

  while (US_TO_TICK(pc386_microseconds_per_isr) > 65535) {
    pc386_isrs_per_tick  *= 10;
    pc386_microseconds_per_isr /= 10;
  }
  pc386_clock_click_count = US_TO_TICK(pc386_microseconds_per_isr);

  #if 0
    printk( "configured usecs per tick=%d \n",
      rtems_configuration_get_microseconds_per_tick() );
    printk( "Microseconds per ISR =%d\n", pc386_microseconds_per_isr );
    printk( "final ISRs per=%d\n", pc386_isrs_per_tick );
    printk( "final timer counts=%d\n", pc386_clock_click_count );
  #endif

  rtems_interrupt_lock_acquire(&rtems_i386_i8254_access_lock, &lock_context);
  outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN);
  outport_byte(TIMER_CNTR0, pc386_clock_click_count >> 0 & 0xff);
  outport_byte(TIMER_CNTR0, pc386_clock_click_count >> 8 & 0xff);
  rtems_interrupt_lock_release(&rtems_i386_i8254_access_lock, &lock_context);

  bsp_interrupt_vector_enable( BSP_PERIODIC_TIMER );
}

bool Clock_isr_enabled = false;
static void Clock_isr_handler(void *param)
{
  if ( Clock_isr_enabled )
    Clock_isr( param );
}

void Clock_driver_install_handler(void)
{
  rtems_status_code status;

  status = rtems_interrupt_handler_install(
    BSP_PERIODIC_TIMER,
    "ckinit",
    RTEMS_INTERRUPT_UNIQUE,
    Clock_isr_handler,
    NULL
  );
  assert(status == RTEMS_SUCCESSFUL);
  clockOn();
}

#define Clock_driver_support_set_interrupt_affinity(online_processors) \
  do { \
    /* FIXME: Is there a way to do this on x86? */ \
    (void) online_processors; \
  } while (0)

void Clock_driver_support_initialize_hardware(void)
{
  bool use_tsc = false;
  bool use_8254 = false;

  #if (CLOCK_DRIVER_USE_TSC == 1)
    use_tsc = true;
  #endif

  #if (CLOCK_DRIVER_USE_8254 == 1)
    use_8254 = true;
  #endif

  if ( !use_tsc && !use_8254 ) {
    if ( x86_has_tsc() ) use_tsc  = true;
    else                 use_8254 = true;
  }

  if ( use_8254 ) {
    /* printk( "Use 8254\n" ); */
    pc386_tc.tc_get_timecount = pc386_get_timecount_i8254;
    pc386_tc.tc_counter_mask = 0xffffffff;
    pc386_tc.tc_frequency = TIMER_TICK;
  } else {
    /* printk( "Use TSC\n" ); */
    pc386_tc.tc_get_timecount = pc386_get_timecount_tsc;
    pc386_tc.tc_counter_mask = 0xffffffff;
    pc386_tc.tc_frequency = pc586_tsc_frequency;
  }

  pc386_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&pc386_tc);
  Clock_isr_enabled = true;
}

#include "../../../shared/dev/clock/clockimpl.h"
