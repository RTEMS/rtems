/*  ckinit.c
 *
 *  This file provides a template for the clock device driver initialization.
 *
 *  Modified for sun4v - niagara
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications Copyright (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdlib.h>

#include <rtems.h>
#include <bsp.h>
#include <bspopts.h>
#include <boot/ofw.h>

/* this is default frequency for simics simulator of niagara. Use the 
 * get_Frequency function to determine the CPU clock frequency at runtime.
 */
#define CPU_FREQ (5000000)

uint64_t sparc64_cycles_per_tick;

/* TICK_CMPR and STICK_CMPR trigger soft interrupt 14 */
#define CLOCK_VECTOR SPARC_SYNCHRONOUS_TRAP(0x4E)

static unsigned int get_Frequency(void)
{
	phandle root = ofw_find_device("/");
	unsigned int freq;
	if (ofw_get_property(root, "clock-frequency", &freq, sizeof(freq)) <= 0) {
		printk("Unable to determine frequency, default: 0x%x\n",CPU_FREQ);
		return CPU_FREQ;
	}

	return freq;
} 


void Clock_driver_support_at_tick(void)
{
  uint64_t tick_reg;
  int bit_mask;
  uint64_t pil_reg;

  bit_mask = SPARC_SOFTINT_TM_MASK | SPARC_SOFTINT_SM_MASK | (1<<14);
  sparc64_clear_interrupt_bits(bit_mask);

  sparc64_get_pil(pil_reg);
  if(pil_reg == 0xe) { /* 0xe is the tick compare interrupt (softint(14)) */
    pil_reg--;
    sparc64_set_pil(pil_reg); /* enable the next timer interrupt */
  }
  /* Note: sun4v uses stick_cmpr for clock driver for M5 simulator, which 
   * does not currently have tick_cmpr implemented */
  /* TODO: this could be more efficiently implemented as a single assembly 
   * inline */
#if defined (SUN4U)
  sparc64_read_tick(tick_reg);
#elif defined (SUN4V)
  sparc64_read_stick(tick_reg);
#endif
  tick_reg &= ~(1UL<<63); /* mask out NPT bit, prevents int_dis from being set */

  tick_reg += sparc64_cycles_per_tick;

#if defined (SUN4U)
  sparc64_write_tick_cmpr(tick_reg);
#elif defined (SUN4V)
  sparc64_write_stick_cmpr(tick_reg);
#endif
}

#define Clock_driver_support_install_isr(_new, _old) \
  do { \
    _old = set_vector( _new, CLOCK_VECTOR, 1 ); \
  } while ( 0 )

void Clock_driver_support_initialize_hardware(void)
{
  uint64_t tick_reg; 	
  int bit_mask;

  bit_mask = SPARC_SOFTINT_TM_MASK | SPARC_SOFTINT_SM_MASK | (1<<14);
  sparc64_clear_interrupt_bits(bit_mask);

  sparc64_cycles_per_tick = rtems_configuration_get_microseconds_per_tick()*(get_Frequency()/1000000);

#if defined (SUN4U)
  sparc64_read_tick(tick_reg);
#elif defined (SUN4V)
  sparc64_read_stick(tick_reg);
#endif

  tick_reg &= ~(1UL<<63); /* mask out NPT bit, prevents int_dis from being set */
  tick_reg += sparc64_cycles_per_tick;

#if defined (SUN4U)
  sparc64_write_tick_cmpr(tick_reg);
#elif defined (SUN4V)
  sparc64_write_stick_cmpr(tick_reg);
#endif
}


#define Clock_driver_support_shutdown_hardware( ) \
  do { \
    \
  } while ( 0 )


#include "../../../shared/clockdrv_shell.h"

