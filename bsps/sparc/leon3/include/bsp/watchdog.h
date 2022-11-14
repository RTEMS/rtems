/* SPDX-License-Identifier: BSD-2-Clause */

/*  watchdog.h
 *
 *  The LEON3 BSP timer watch-dog interface
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

#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize BSP watchdog routines. Returns number of watchdog timers found.
 * Currently only one is supported.
 */
int bsp_watchdog_init(void);

/* Reload watchdog (last timer on the first GPTIMER core), all systems does not
 * feature a watchdog, it is expected that if this function is called the
 * user knows that there is a watchdog available.
 *
 * The prescaler is normally set to number of MHz of system, this is to
 * make the system clock tick be stable.
 *
 * Arguments
 *  watchdog       - Always 0 for now
 *  reload_value   - Number of timer clocks (after prescaler) to count before 
 *                   watchdog is woken.
 */
void bsp_watchdog_reload(int watchdog, unsigned int reload_value);

/* Stop watchdog timer */
void bsp_watchdog_stop(int watchdog);

/* Use watchdog0 timer to reset the system */
void bsp_watchdog_system_reset(void);

#ifdef __cplusplus
}
#endif

#endif
