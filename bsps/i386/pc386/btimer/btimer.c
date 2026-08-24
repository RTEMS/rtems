/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 * This file contains the PC386 timer package.
 *
 * Rosimildo daSilva -ConnectTel, Inc - Fixed infinite loop in the Calibration
 * routine. I've seen this problems with faster machines ( pentiums ). Sometimes
 * RTEMS just hangs at startup.
 *
 * Joel 9 May 2010: This is now seen sometimes on qemu.
 *
 *  Modifications by:
 *  (C) Copyright 1997 -
 *    NavIST Group - Real-Time Distributed Systems and Industrial Automation
 *    Instituto Superior Tecnico * Lisboa * PORTUGAL
 *
 *  This file is provided "AS IS" without warranty of any kind, either
 *  expressed or implied.
 *
 *  Based upon code by
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>
#include <bsp.h>
#include <rtems/btimer.h>
#include <bsp/irq-generic.h>
#include <libcpu/cpuModel.h>

/*
 * Constants
 */
#define AVG_OVERHEAD  0         /* 0.1 microseconds to start/stop timer. */
#define LEAST_VALID   1         /* Don't trust a value lower than this.  */

RTEMS_INTERRUPT_LOCK_DEFINE( /* visible global variable */ ,
   rtems_i386_i8254_access_lock, "rtems_i386_i8254_access_lock" );

/*
 * Global Variables
 */
volatile uint32_t         Ttimer_val;
bool                      benchmark_timer_find_average_overhead = true;

void              (*benchmark_timer_initialize_function)(void) = 0;
benchmark_timer_t (*benchmark_timer_read_function)(void) = 0;
void              (*Timer_exit_function)(void) = 0;

/* timer (int 08h) Interrupt Service Routine (defined in 'timerisr.s') */
extern void timerisr(void);

void Timer_exit(void);

/*
 * Pentium optimized timer handling.
 */

/*
 *  Timer cleanup routine at RTEMS exit.
 *
 *  NOTE: This routine is not really necessary, since there will be
 *        a reset at exit.
 */
static void tsc_timer_exit(void)
{
}

static void tsc_timer_initialize(void)
{
  static bool First = true;

  if (First) {
    First = false;

    atexit(Timer_exit); /* Try not to hose the system at exit. */
  }
  Ttimer_val = rdtsc(); /* read starting time */
}

/*
 * Read TSC timer value.
 */
static uint32_t tsc_read_timer(void)
{
  register uint32_t  total;

  total =  (uint32_t)(rdtsc() - Ttimer_val);

  if (benchmark_timer_find_average_overhead)
    return total;

  if (total < LEAST_VALID)
    return 0;                 /* below timer resolution */

  return (total - AVG_OVERHEAD);
}

/*
 * Non-Pentium timer handling.
 */
#define US_PER_ISR   250  /* Number of micro-seconds per timer interruption */

/*
 * Timer cleanup routine at RTEMS exit. NOTE: This routine is
 * not really necessary, since there will be a reset at exit.
 */
static void timerOff(const rtems_raw_irq_connect_data* used)
{
  rtems_interrupt_lock_context lock_context;
  /*
   * disable interrrupt at i8259 level
   */
  bsp_interrupt_vector_disable(used->idtIndex - BSP_IRQ_VECTOR_BASE);

  rtems_interrupt_lock_acquire(&rtems_i386_i8254_access_lock, &lock_context);

   /* reset timer mode to standard (DOS) value */
  outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN);
  outport_byte(TIMER_CNTR0, 0);
  outport_byte(TIMER_CNTR0, 0);

  rtems_interrupt_lock_release(&rtems_i386_i8254_access_lock, &lock_context);
}

static void timerOn(const rtems_raw_irq_connect_data* used)
{
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire(&rtems_i386_i8254_access_lock, &lock_context);

  /* load timer for US_PER_ISR microsecond period */
  outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN);
  outport_byte(TIMER_CNTR0, US_TO_TICK(US_PER_ISR) >> 0 & 0xff);
  outport_byte(TIMER_CNTR0, US_TO_TICK(US_PER_ISR) >> 8 & 0xff);

  rtems_interrupt_lock_release(&rtems_i386_i8254_access_lock, &lock_context);

  /*
   * enable interrrupt at i8259 level
   */
  bsp_interrupt_vector_enable(used->idtIndex - BSP_IRQ_VECTOR_BASE);
}

static rtems_raw_irq_connect_data timer_raw_irq_data = {
  BSP_PERIODIC_TIMER + BSP_IRQ_VECTOR_BASE,
  timerisr,
  timerOn,
  timerOff,
  NULL
};

/*
 * Timer cleanup routine at RTEMS exit.
 *
 * NOTE: This routine is not really necessary, since there will be
 *       a reset at exit.
 */
static void i386_timer_exit(void)
{
  i386_delete_idt_entry (&timer_raw_irq_data);
}

extern void rtems_irq_prologue_0(void);
static void i386_timer_initialize(void)
{
  static bool First = true;

  if (First) {
    rtems_raw_irq_connect_data raw_irq_data = {
      BSP_PERIODIC_TIMER + BSP_IRQ_VECTOR_BASE,
      rtems_irq_prologue_0,
      NULL,
      NULL,
      NULL
    };

    First = false;
    i386_delete_idt_entry (&raw_irq_data);

    atexit(Timer_exit);            /* Try not to hose the system at exit. */
    if (!i386_set_idt_entry (&timer_raw_irq_data)) {
      printk("raw handler connection failed\n");
      rtems_fatal_error_occurred(1);
    }
  }
  /* wait for ISR to be called at least once */
  Ttimer_val = 0;
  while (Ttimer_val == 0)
    continue;
  Ttimer_val = 0;
}

/*
 * Read hardware timer value.
 */
static uint32_t i386_read_timer(void)
{
  register uint32_t         total, clicks;
  register uint8_t          lsb, msb;
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire(&rtems_i386_i8254_access_lock, &lock_context);
  outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_LATCH);
  inport_byte(TIMER_CNTR0, lsb);
  inport_byte(TIMER_CNTR0, msb);
  rtems_interrupt_lock_release(&rtems_i386_i8254_access_lock, &lock_context);

  clicks = (msb << 8) | lsb;
  total  = (Ttimer_val * US_PER_ISR) + (US_PER_ISR - TICK_TO_US(clicks));

  if (benchmark_timer_find_average_overhead)
    return total;

  if (total < LEAST_VALID)
    return 0;                            /* below timer resolution */

  return (total - AVG_OVERHEAD);
}

/*
 * General timer functions using either TSC-based implementation
 * or interrupt-based implementation
 */

void benchmark_timer_initialize(void)
{
  static bool First = true;

  if (First) {
    if (x86_has_tsc()) {
#if defined(DEBUG)
      printk("TSC: timer initialization\n");
#endif /* DEBUG */
      benchmark_timer_initialize_function = &tsc_timer_initialize;
      benchmark_timer_read_function = &tsc_read_timer;
      Timer_exit_function = &tsc_timer_exit;
    } else {
#if defined(DEBUG)
      printk("ISR: timer initialization\n");
#endif /* DEBUG */
      benchmark_timer_initialize_function = &i386_timer_initialize;
      benchmark_timer_read_function = &i386_read_timer;
      Timer_exit_function = &i386_timer_exit;
    }
    First = false;
  }
  (*benchmark_timer_initialize_function)();
}

uint32_t benchmark_timer_read(void)
{
  return (*benchmark_timer_read_function)();
}

void Timer_exit(void)
{
  if ( Timer_exit_function )
    return (*Timer_exit_function)();
}

/*
 * Set internal benchmark_timer_find_average_overhead flag value.
 */
void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}

/*
 * The ports and the command of channel 2 of the 8254.  Software gates that
 * channel and no interrupt takes its output, so a delay may own it.  The
 * clock driver takes the tick interrupt from channel 0.
 */
#define PC386_WAIT_CHAN2        0x42
#define PC386_WAIT_MCR          0x43
#define PC386_WAIT_GATE         0x61
#define PC386_WAIT_GATE_TIMER   0x01
#define PC386_WAIT_GATE_SPEAKER 0x02
#define PC386_WAIT_SELECT_CHAN2 0x80
#define PC386_WAIT_ACCESS_LOHI  0x30
#define PC386_WAIT_ONE_SHOT     0x02

/* The counter of the 8254 is 16 bits wide, which is 54.9ms at TIMER_TICK */
#define PC386_WAIT_COUNTS_PER_MS ( TIMER_TICK / 1000 )

/*
 *  waits at least timeToWait ms
 *
 * Channel 2 runs in the hardware retriggerable one shot mode, so a rising edge
 * of the gate starts the count.  The counter counts down through zero and
 * continues at 0xffff, so a count above the load value shows that the
 * millisecond elapsed.
 */
void Wait_X_ms( unsigned int timeToWait)
{
  unsigned int j;
  uint8_t      gate;

  inport_byte( PC386_WAIT_GATE, gate );
  gate = ( gate | PC386_WAIT_GATE_TIMER ) & ~PC386_WAIT_GATE_SPEAKER;
  outport_byte( PC386_WAIT_GATE, gate );

  for (j=0; j<timeToWait ; j++) {
    uint32_t count;

    outport_byte(
      PC386_WAIT_MCR,
      PC386_WAIT_SELECT_CHAN2 | PC386_WAIT_ACCESS_LOHI | PC386_WAIT_ONE_SHOT
    );
    outport_byte( PC386_WAIT_CHAN2, PC386_WAIT_COUNTS_PER_MS & 0xff );
    outport_byte( PC386_WAIT_CHAN2, ( PC386_WAIT_COUNTS_PER_MS >> 8 ) & 0xff );

    gate &= ~PC386_WAIT_GATE_TIMER;
    outport_byte( PC386_WAIT_GATE, gate );
    gate |= PC386_WAIT_GATE_TIMER;
    outport_byte( PC386_WAIT_GATE, gate );

    do {
      uint8_t lsb;
      uint8_t msb;

      outport_byte( PC386_WAIT_MCR, PC386_WAIT_SELECT_CHAN2 );
      inport_byte( PC386_WAIT_CHAN2, lsb );
      inport_byte( PC386_WAIT_CHAN2, msb );
      count = ( (uint32_t) msb << 8 ) | lsb;
    } while ( count <= PC386_WAIT_COUNTS_PER_MS );
  }

  gate &= ~PC386_WAIT_GATE_TIMER;
  outport_byte( PC386_WAIT_GATE, gate );
}
