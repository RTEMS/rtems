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
 *    http://pandora.ist.utl.pt
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
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdlib.h>
#include <bsp.h>
#include <rtems/btimer.h>
#include <bsp/irq.h>
#include <libcpu/cpuModel.h>

/*
 * Constants
 */
#define AVG_OVERHEAD  0         /* 0.1 microseconds to start/stop timer. */
#define LEAST_VALID   1         /* Don't trust a value lower than this.  */
#define SLOW_DOWN_IO  0x80      /* io which does nothing */

#define TWO_MS (uint32_t)(2000) /* TWO_MS = 2000us (sic!) */

#define MSK_NULL_COUNT 0x40     /* bit counter available for reading */

#define CMD_READ_BACK_STATUS 0xE2   /* command read back status */

/*
 * Global Variables
 */
volatile uint32_t         Ttimer_val;
bool                      benchmark_timer_find_average_overhead = true;
volatile unsigned int     fastLoop1ms, slowLoop1ms;

void     (*benchmark_timer_initialize_function)(void) = 0;
uint32_t (*benchmark_timer_read_function)(void) = 0;
void     (*Timer_exit_function)(void) = 0;

/* timer (int 08h) Interrupt Service Routine (defined in 'timerisr.s') */
extern void timerisr(void);

void Timer_exit(void);

/*
 * Pentium optimized timer handling.
 */

/*
 *  Timer cleanup routine at RTEMS exit. NOTE: This routine is
 *  not really necessary, since there will be a reset at exit.
 */

void tsc_timer_exit(void)
{
}

void tsc_timer_initialize(void)
{
  static bool First = true;

  if (First) {
    First = false;

    atexit(Timer_exit); /* Try not to hose the system at exit. */
  }
  Ttimer_val = rdtsc(); /* read starting time */
}

/*
 *
 */
uint32_t tsc_read_timer(void)
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
  /*
   * disable interrrupt at i8259 level
   */
   BSP_irq_disable_at_i8259s(used->idtIndex - BSP_IRQ_VECTOR_BASE);
   /* reset timer mode to standard (DOS) value */
   outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN);
   outport_byte(TIMER_CNTR0, 0);
   outport_byte(TIMER_CNTR0, 0);
}

static void timerOn(const rtems_raw_irq_connect_data* used)
{
  /* load timer for US_PER_ISR microsecond period */
  outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN);
  outport_byte(TIMER_CNTR0, US_TO_TICK(US_PER_ISR) >> 0 & 0xff);
  outport_byte(TIMER_CNTR0, US_TO_TICK(US_PER_ISR) >> 8 & 0xff);

  /*
   * enable interrrupt at i8259 level
   */
  BSP_irq_enable_at_i8259s(used->idtIndex - BSP_IRQ_VECTOR_BASE);
}

static int timerIsOn(const rtems_raw_irq_connect_data *used)
{
  return BSP_irq_enabled_at_i8259s(used->idtIndex - BSP_IRQ_VECTOR_BASE);
}

static rtems_raw_irq_connect_data timer_raw_irq_data = {
  BSP_PERIODIC_TIMER + BSP_IRQ_VECTOR_BASE,
  timerisr,
  timerOn,
  timerOff,
  timerIsOn
};

/*
 * Timer cleanup routine at RTEMS exit. NOTE: This routine is
 *  not really necessary, since there will be a reset at exit.
 */ void
i386_timer_exit(void)
{
  i386_delete_idt_entry (&timer_raw_irq_data);
}

extern void rtems_irq_prologue_0(void);
void i386_timer_initialize(void)
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
uint32_t i386_read_timer(void)
{
  register uint32_t         total, clicks;
  register uint8_t          lsb, msb;

  outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_LATCH);
  inport_byte(TIMER_CNTR0, lsb);
  inport_byte(TIMER_CNTR0, msb);
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

static unsigned short lastLoadedValue;

/*
 *  Loads timer 0 with value passed as arguemnt.
 *
 *  Returns: Nothing. Loaded value must be a number of clock bits...
 */
void loadTimerValue( unsigned short loadedValue )
{
  lastLoadedValue = loadedValue;
  outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_SQWAVE);
  outport_byte(TIMER_CNTR0, loadedValue & 0xff);
  outport_byte(TIMER_CNTR0, (loadedValue >> 8) & 0xff);
}

/*
 * Reads the current value of the timer, and converts the
 *  number of ticks to micro-seconds.
 *
 * Returns: number of clock bits elapsed since last load.
 */
unsigned int readTimer0(void)
{
  unsigned short lsb, msb;
  unsigned char  status;
  unsigned int  count;

  outport_byte(
    TIMER_MODE,
    (TIMER_RD_BACK | (RB_COUNT_0 & ~(RB_NOT_STATUS | RB_NOT_COUNT)))
  );
  inport_byte(TIMER_CNTR0, status);
  inport_byte(TIMER_CNTR0, lsb);
  inport_byte(TIMER_CNTR0, msb);
  count = ( msb << 8 ) | lsb ;
  if (status & RB_OUTPUT )
    count += lastLoadedValue;

  return (2*lastLoadedValue - count);
}

void Timer0Reset(void)
{
  loadTimerValue(0xffff);
  readTimer0();
}

void fastLoop (unsigned int loopCount)
{
  unsigned int i;
  for( i=0; i < loopCount; i++ )outport_byte( SLOW_DOWN_IO, 0 );
}

void slowLoop (unsigned int loopCount)
{
  unsigned int j;
  for (j=0; j <100 ;  j++) {
    fastLoop (loopCount);
  }
}

/*
 * #define DEBUG_CALIBRATE
 */
void
Calibrate_loop_1ms(void)
{
  unsigned int offset, offsetTmp, emptyCall, emptyCallTmp, res, i, j;
  unsigned int targetClockBits, currentClockBits;
  unsigned int slowLoopGranularity, fastLoopGranularity;
  rtems_interrupt_level  level;
  int retries = 0;

  rtems_interrupt_disable(level);

retry:
  if ( ++retries >= 5 ) {
    printk( "Calibrate_loop_1ms: too many attempts. giving up!!\n" );
    while (1);
  }
#ifdef DEBUG_CALIBRATE
  printk("Calibrate_loop_1ms is starting,  please wait (but not too long.)\n");
#endif
  targetClockBits = US_TO_TICK(1000);
  /*
   * Fill up the cache to get a correct offset
   */
  Timer0Reset();
  readTimer0();
  /*
   * Compute the minimal offset to apply due to read counter register.
   */
  offset = 0xffffffff;
  for (i=0; i <1000; i++) {
    Timer0Reset();
    offsetTmp = readTimer0();
    offset += offsetTmp;
  }
  offset = offset / 1000; /* compute average */
  /*
   * calibrate empty call
   */
  fastLoop (0);
  emptyCall = 0;
  j = 0;
  for (i=0; i <10; i++) {
    Timer0Reset();
    fastLoop (0);
    res =  readTimer0();
    /* res may be inferior to offset on fast
     * machine because we took an average for offset
     */
    if (res >  offset) {
      ++j;
      emptyCallTmp = res - offset;
      emptyCall += emptyCallTmp;
    }
  }
  if (j == 0) emptyCall = 0;
  else emptyCall = emptyCall / j; /* compute average */
  /*
   * calibrate fast loop
   */
  Timer0Reset();
  fastLoop (10000);
  res = readTimer0() - offset;
  if (res < emptyCall) {
    printk(
      "Problem #1 in offset computation in Calibrate_loop_1ms "
        " in file libbsp/i386/pc386/timer/timer.c\n"
    );
    goto retry;
  }
  fastLoopGranularity = (res - emptyCall) / 10000;
  /*
   * calibrate slow loop
   */
  Timer0Reset();
  slowLoop(10);
  res = readTimer0();
  if (res < offset + emptyCall) {
    printk(
      "Problem #2 in offset computation in Calibrate_loop_1ms "
        " in file libbsp/i386/pc386/timer/timer.c\n"
    );
    goto retry;
  }
  slowLoopGranularity = (res - offset - emptyCall)/ 10;

  if (slowLoopGranularity == 0) {
    printk(
      "Problem #3 in offset computation in Calibrate_loop_1ms "
        " in file libbsp/i386/pc386/timer/timer.c\n"
    );
    goto retry;
  }

  targetClockBits += offset;
#ifdef DEBUG_CALIBRATE
  printk("offset = %u, emptyCall = %u, targetClockBits = %u\n",
  offset, emptyCall, targetClockBits);
  printk("slowLoopGranularity = %u fastLoopGranularity =  %u\n",
  slowLoopGranularity, fastLoopGranularity);
#endif
  slowLoop1ms = (targetClockBits - emptyCall) / slowLoopGranularity;
  if (slowLoop1ms != 0) {
    fastLoop1ms = targetClockBits % slowLoopGranularity;
    if (fastLoop1ms > emptyCall) fastLoop1ms -= emptyCall;
  }
  else
    fastLoop1ms = targetClockBits - emptyCall / fastLoopGranularity;

  if (slowLoop1ms != 0) {
    /*
     * calibrate slow loop
     */

    while(1)
      {
 int previousSign = 0; /* 0 = unset, 1 = incrementing,  2 = decrementing */
 Timer0Reset();
 slowLoop(slowLoop1ms);
 currentClockBits = readTimer0();
 if (currentClockBits > targetClockBits) {
   if ((currentClockBits - targetClockBits) < slowLoopGranularity) {
     /* decrement loop counter anyway to be sure slowLoop(slowLoop1ms) < targetClockBits */
     --slowLoop1ms;
     break;
   }
   else {
     --slowLoop1ms;
     if (slowLoop1ms == 0) break;
     if (previousSign == 0) previousSign = 2;
     if (previousSign == 1) break;
   }
 }
 else {
   if ((targetClockBits - currentClockBits) < slowLoopGranularity) {
      break;
    }
    else {
      ++slowLoop1ms;
      if (previousSign == 0) previousSign = 1;
      if (previousSign == 2) break;
    }
  }
      }
  }
  /*
   * calibrate fast loop
   */

  if (fastLoopGranularity != 0 ) {
    while(1) {
      int previousSign = 0; /* 0 = unset, 1 = incrementing,  2 = decrementing */
      Timer0Reset();
      if (slowLoop1ms != 0) slowLoop(slowLoop1ms);
      fastLoop(fastLoop1ms);
      currentClockBits = readTimer0();
      if (currentClockBits > targetClockBits) {
  if ((currentClockBits - targetClockBits) < fastLoopGranularity)
    break;
  else {
    --fastLoop1ms;
    if (previousSign == 0) previousSign = 2;
    if (previousSign == 1) break;
  }
      }
      else {
  if ((targetClockBits - currentClockBits) < fastLoopGranularity)
    break;
  else {
    ++fastLoop1ms;
    if (previousSign == 0) previousSign = 1;
    if (previousSign == 2) break;
  }
      }
    }
  }
#ifdef DEBUG_CALIBRATE
  printk("slowLoop1ms = %u, fastLoop1ms = %u\n", slowLoop1ms, fastLoop1ms);
#endif
  rtems_interrupt_enable(level);

}

/*
 *  loop which waits at least timeToWait ms
 */
void Wait_X_ms( unsigned int timeToWait)
{
  unsigned int j;

  for (j=0; j<timeToWait ; j++) {
    if (slowLoop1ms != 0) slowLoop(slowLoop1ms);
    fastLoop(fastLoop1ms);
  }
}
