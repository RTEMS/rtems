/*
 *  Instantiate the clock driver shell.
 */

#include <bsp.h>

#define CLOCK_VECTOR 84

#define Clock_driver_support_install_isr( _new, _old )  \
  do { _old = (rtems_isr_entry) set_vector( _new, CLOCK_VECTOR, 1 ); } while(0)

typedef struct {
  volatile uint8_t    cr;       /*  0 -  0 : Timer Control Register */
  volatile uint8_t    pad0;     /*  1 -  1 : pad */
  volatile uint8_t    ivr;      /*  2 -  2 : Timer Interrupt Vector Register */
  volatile uint8_t    pad1;     /*  3 -  3 : pad */
  volatile uint32_t   cpr;      /*  4 -  7 : Timer Counter Preload Register */
  volatile uint8_t    pad2[12]; /*  8 - 19 : pad */
  volatile uint32_t   sr;       /* 20 - 23 : Timer Status Register */
} timer_hw_t;

#define TIMER_BASE (timer_hw_t *)0x72001

/* 8 microseconds per click, 125,000 per second */

/* XXX should check that microseconds_per_tick is >= 8 */
void Clock_driver_support_initialize_hardware(void)
{
  timer_hw_t *t = TIMER_BASE;

  t->ivr = CLOCK_VECTOR;
  t->cpr = rtems_configuration_get_microseconds_per_tick() / 8;
  t->cr  = 0xA0;  /* initialize with timer disabled */
  t->cr  = 0xA1;  /* enable timer */
}

void Clock_driver_support_at_tick(void)
{
  timer_hw_t *t = TIMER_BASE;

  t->sr  = 0xA0;  /* Negate timer interrupt request */
}

void Clock_driver_support_shutdown_hardware(void)
{
  timer_hw_t *t = TIMER_BASE;

  t->cr  = 0xA0;  /* initialize with timer disabled */
}

#include "../../../shared/clockdrv_shell.h"
