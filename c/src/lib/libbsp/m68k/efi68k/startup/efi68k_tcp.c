/*
 *-------------------------------------------------------------------
 *
 * This file contains the subroutines necessary to initalize
 * the DP8750A TCP on the efi68k board.
 *
 * This file has been created by John S. Gwynne for the efi68k
 * project.
 * 
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *------------------------------------------------------------------
 *
 *  $Id$
 */

#include <bsp.h>
#include <DP8570A.h>

/* define tcp struct pointers */
struct clock_ram * const tcp_power_up =
   (struct clock_ram * const)(0x16*2+TCP_BASE_ADDRESS);

struct clock_ram * const tcp_power_down =
   (struct clock_ram * const)(0x1b*2+TCP_BASE_ADDRESS);

struct clock_counters * const tcp_clock =
   (struct clock_counters * const)(0x05*2+TCP_BASE_ADDRESS);

struct clock_ram * const tcp_save_ram =
   (struct clock_ram * const)(0x19*2+TCP_BASE_ADDRESS);

#define X_DELAY 300		/* time-out delay for crystal start */
#define X1_DELAY 100000

void tcp_delay(int count)
{
  int i;
  /* change latter to use a counter !!! */
  for (i=0;i<count/4;i++);
}

void bsp_tcp_init()
{
  unsigned char low_bat, osc_fail, power_up;
  unsigned char mon, dom, hrs, min, sec;
  int i, count;

  /* delay about 60us to ensure TCP is not locked-out */
  tcp_delay(80);

  /* set normal supply mode and reset test mode bit */
  *MSR = 0;
  *PFR = 0;

  /* save oscillator failure condition */
  *MSR = 0;			/* set RS and PS to zero */
  osc_fail = (*PFR & OSF ? 1 : 0);
  *MSR = PS;
  *RAM_OSC_FAIL = *RAM_OSC_FAIL || osc_fail;

  *MSR = PS;
  if (*RAM_OSC_FAIL) {
    power_up = 1;
    *MSR = PS;
    *RAM_POWERUP = power_up;
    /* clear time counters and power up & down ram */
    *MSR = 0;
    tcp_clock->hofs = 0;
    tcp_clock->sec = 0;
    tcp_clock->min = 0;
    tcp_clock->hrs = 0;
    tcp_clock->dom = 1;
    tcp_clock->mon = 1;
    tcp_clock->yr = 0x95;
    tcp_clock->jd0 = 0x01;
    tcp_clock->jd1 = 0;
    tcp_clock->dow = 1;
    *MSR = PS;
    tcp_power_up->sec = 0;
    tcp_power_up->min = 0;
    tcp_power_up->hrs = 0;
    tcp_power_up->dom = 0;
    tcp_power_up->mon = 0;
    tcp_power_down->sec = 0;
    tcp_power_down->min = 0;
    tcp_power_down->hrs = 0;
    tcp_power_down->dom = 0;
    tcp_power_down->mon = 0;
  } else {
    /* save for power-up test */
    *MSR = 0;
    power_up = (*IRR & TMSE ? 0 : 1);
    *MSR = PS;
    *RAM_POWERUP = power_up;

    /* update tcp_power_up and tcp_power_down on power up */
    if (power_up) {
      *MSR = 0;
      do {
	*PFR;
	sec = tcp_clock->sec;
	min = tcp_clock->min;
	hrs = tcp_clock->hrs;
	dom = tcp_clock->dom;
	mon = tcp_clock->mon;
      } while (*PFR & R_1S);
      *MSR = PS;
      tcp_power_up->sec = sec;
      tcp_power_up->min = min;
      tcp_power_up->hrs = hrs;
      tcp_power_up->dom = dom;
      tcp_power_up->mon = ( (((mon>>4)*10)+(mon&0xf))>12 ? 0 : mon );
      *MSR = 0;			/* save ram is not running */
      sec = tcp_save_ram->sec;
      min = tcp_save_ram->min;
      hrs = tcp_save_ram->hrs;
      dom = tcp_save_ram->dom;
      mon = tcp_save_ram->mon;
      *MSR = PS;
      tcp_power_down->sec = sec;
      tcp_power_down->min = min;
      tcp_power_down->hrs = hrs;
      tcp_power_down->dom = dom;
      tcp_power_down->mon = ( (((mon>>4)*10)+(mon&0xf))>12 ? 0 : mon );
    }
  }

  /* load interrupt routing reg. PF must be enabled to test
     for low battery, but I route it to MFO to avoid any
     potential problems */
  *MSR = 0;
  *IRR = PF_R | TMSE;

  /* initialize the output mode register */
  *MSR = RS;
  *OMR = IP | MP | MO;		/* INTR active low and push/pull */

  /* initialize interrupt control reg 0 */
  *MSR = RS;
  *ICR0 = 0;			/* disable all interrupts */

  /* initialize interrupt control reg 1 */
  *MSR = RS;
  *ICR1 = PFE;			/* this also enables the low battery
				   detection circuit. */

  /* I had trouble getting the tcp to be completely
     flexible to supply modes (i.e., automatically
     selecting single or normal battery backup modes based
     on inputs at power-up. If single supply mode is
     selected, the low battery detect is disabled and the
     low battery detect in normal mode does not seem to
     detect when no battery is present at all. If normal
     mode is selected and no battery is present, the
     crystal will stop, but only if reset after
     power-up. It would seem that after a power-up reset,
     with no battery, the chip may automaticlly switch to
     single supply mode which disables the low battery
     detection circuit.)  The following software tests
     works for all permiatations of low batt, reset,
     power-on reset, battery, no battery, battery on after
     Vcc,....  *except* for battery switched on for the
     first time before power up in which case the chip
     will still be in single suppy mode till restarted (a
     second call to bsp_tcp_init such as when the time is set
     or a reboot.)  The timer/clock itself should always
     be completely functional regardless of the supply
     mode. */


  /* initialize the real time mode register */
  /* note: write mode bits *before* CSS, then set CSS */
  *MSR = 0;			/* clear roll-over */
  *PFR;
  count=1;
  for (i=0;i<X_DELAY;i++) {	/* loop till xtal starts */
    *MSR = RS;
    *RTMR = (*RTMR & (LY0 | LY1 )) | CSS;
    *MSR = 0;
    if (*PFR & R_1MS) 
      if (!(count--)) break;
  }
  if (i>=X_DELAY) {
    {
      /* xtal didn't start; try single supply mode */
      *MSR = 0;			/* single supply */
      *PFR = OSF;
      *MSR = 0;			/* clear roll-over */
      *PFR;
      count=1;
      for (i=0;i<X1_DELAY;i++) {	/* loop till xtal starts */
	*MSR = RS;
	*RTMR = (*RTMR & (LY0 | LY1 )) | CSS;
	*MSR = 0;
	if (*PFR & R_1MS)
	  if (!(count--)) break;
      }
      if (i>=X1_DELAY) {
	/* xtal didn't start; fail tcp */
	*MSR = PS;
	*RAM_TCP_FAILURE = 1;
	*MSR = PS;
	*RAM_SINGLE_SUP=1;
      } else {
	*MSR = PS;
	*RAM_TCP_FAILURE = 0;
	*MSR = PS;
	*RAM_SINGLE_SUP=1;
      }
    }
  } else {
    *MSR = PS;
    *RAM_TCP_FAILURE = 0;
    *MSR = PS;
    *RAM_SINGLE_SUP=0;
  }

  /* wait for low battery detection circuit to stabalize */
  tcp_delay(1000);

  /* battery test */
  *MSR = 0;
  low_bat = (*IRR & LBF ? 1 : 0 );
  *MSR = PS;
  *RAM_LOWBAT = low_bat & !(*RAM_SINGLE_SUP);

  /* reset pending interrupts */
  *MSR = ( PER | AL | T0 | T1 );

  /* resync the time save ram with the clock */
  tcp_save_ram->sec = 0;
  tcp_save_ram->min = 0;
  tcp_save_ram->hrs = 0;
  tcp_save_ram->dom = 0;
  tcp_save_ram->mon = 0;
}  
