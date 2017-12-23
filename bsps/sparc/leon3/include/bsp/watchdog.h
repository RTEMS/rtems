/*  watchdog.h
 *
 *  The LEON3 BSP timer watch-dog interface
 *
 *  COPYRIGHT (c) 2012.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
