/* SPDX-License-Identifier: BSD-2-Clause */

/*  GPTIMER Watchdog timer routines. On some systems the first GPTIMER
 *  core's last Timer instance underflow signal is connected to system
 *  reset.
 *
 *  COPYRIGHT (c) 2012.
 *  Cobham Gaisler AB.
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
#include <bsp/leon3.h>
#include <bsp/watchdog.h>

struct gptimer_watchdog_priv {
  gptimer *regs;
  gptimer_timer *timer;
  int timerno;
};

struct gptimer_watchdog_priv bsp_watchdogs[1];
int bsp_watchdog_count = 0;

int bsp_watchdog_init(void)
{
  int timercnt;

  if (!LEON3_Timer_Regs)
    return 0;

  /* Get Watchdogs in system, this is implemented for one GPTIMER core
   * only.
   *
   * First watchdog is a special case, we can get the first timer core by
   * looking at LEON3_Timer_Regs, the watchdog within a timer core is
   * always the last timer. Unfortunately we can not know it the watchdog
   * functionality is available or not, we assume that it is if we
   * reached this function.
   */
  bsp_watchdogs[0].regs = LEON3_Timer_Regs;

  /* Find Timer that has watchdog functionality */
  timercnt = grlib_load_32(&bsp_watchdogs[0].regs->config) & 0x7;
  if (timercnt < 2) /* First timer system clock timer */
    return 0;

  bsp_watchdogs[0].timerno = timercnt - 1;
  bsp_watchdogs[0].timer = &bsp_watchdogs[0].regs->timer[bsp_watchdogs[0].timerno];

  bsp_watchdog_count = 1;
  return bsp_watchdog_count;
}

void bsp_watchdog_reload(int watchdog, unsigned int reload_value)
{
  gptimer_timer *timer;
  uint32_t tctrl;

  if (bsp_watchdog_count == 0)
    bsp_watchdog_init();

  if (bsp_watchdog_count <= watchdog)
    return;

  /* Kick watchdog, and clear interrupt pending bit */
  timer = bsp_watchdogs[watchdog].timer;
  grlib_store_32(&timer->trldval, reload_value);
  tctrl = grlib_load_32(&timer->tctrl);
  tctrl |= GPTIMER_TCTRL_LD | GPTIMER_TCTRL_EN;
  tctrl &= ~GPTIMER_TCTRL_IP;
  grlib_store_32(&timer->tctrl, tctrl);
}

void bsp_watchdog_stop(int watchdog)
{
  if (bsp_watchdog_count == 0)
    bsp_watchdog_init();

  if (bsp_watchdog_count <= watchdog)
    return;

  /* Stop watchdog timer */
  grlib_store_32(&bsp_watchdogs[watchdog].timer->tctrl, 0);
}

/* Use watchdog timer to reset system */
void bsp_watchdog_system_reset(void)
{
  sparc_disable_interrupts();
  bsp_watchdog_reload(0, 1);
}
