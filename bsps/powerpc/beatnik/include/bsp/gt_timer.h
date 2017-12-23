#ifndef BSP_GT_TIMER_H
#define BSP_GT_TIMER_H

/* Support for hardware timers in the discovery bridge */

/* 
 * Authorship
 * ----------
 * This software ('beatnik' RTEMS BSP for MVME6100 and MVME5500) was
 *     created by Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 * 
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'beatnik' BSP was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 * 
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 * 
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 * 
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.  
 * 
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 * 
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */ 

#include <stdint.h>

#ifdef __cplusplus
  extern "C" {
#endif

/* Obtain the number of hardware timers present
 * The 'timer' argument in the routines below addresses
 * one of 0..(BSP_timer_instances()-1)
 */
int BSP_timer_instances(void);

/* Setup timer but don't start yet; interrupts are enabled if an isr argument is passed
 * no interrupts are generated otherwise.
 *
 * If 'reload' is nonzero then the period is automatically restarted.
 *
 * RETURNS: 0 on success, nonzero on error (argument error)
 *
 * NOTE: If an ISR is already connected, it must be removed by passing a NULL isr first.
 */
int      BSP_timer_setup(uint32_t timer, void (*isr)(void *arg), void *arg, int reload);

/* Stop timer;
 *
 * RETURNS: 0 on success, nonzero on argument error
 */
int      BSP_timer_stop(uint32_t timer);

/* Start timer with 'period' (in ticks)
 *
 * RETURNS: 0 on success, nonzero on argument error
 */
int      BSP_timer_start(uint32_t timer, uint32_t period);

/* read decrementing timer on the fly
 *
 * RETURNS: current count in ticks
 */
uint32_t BSP_timer_read(uint32_t timer);

/* get clock rate in Hz */
uint32_t BSP_timer_clock_get(uint32_t timer);

/* Initialize timer facility -- to be used by BSP implementors only
 *
 * RETURNS: 0 on success, nonzero if ISR wrapper couldn't be installed
 */
int	BSP_timers_initialize(void);

/* WATCHDOG TIMER (resets board if enabled and not 'petted' for
 * some time).
 */

/* Enable watchdog and set a timeout (in us)
 * RETURNS 0 on success
 */
int BSP_watchdog_enable(uint32_t timeout_us);

/* Disable watchdog
 * RETURNS 0 on success
 */
int BSP_watchdog_disable(void);

/* Check status -- unfortunately there seems to be no way
 * to read the running value...
 *
 * RETURNS nonzero if enabled/running, zero if disabled/stopped
 */
int BSP_watchdog_status(void);

/* Pet the watchdog (rearm to configured timeout)
 * RETURNS: 0 on success, nonzero on failure (watchdog
 * currently not running).
 */
int BSP_watchdog_pet(void);


#ifdef __cplusplus
  }
#endif

#endif
