/*
  Description: Wrapper around DHCP client to restart it when the interface
               moves to another network.

  Authors: Arnout Vandecappelle <arnout@mind.be>, Essensium/Mind
           Maarten Van Es <maarten@mind.be>, Essensium/Mind
  (C) Septentrio 2008

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution or at
  http://www.rtems.org/license/LICENSE.
*/

#ifndef _RTEMS_DHCP_FAILSAFE_H_
#define _RTEMS_DHCP_FAILSAFE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Default settings for the DHCP failsafe.  They can be overridden
 * using rtems_bsdnet_dhcp_failsafe_config(); see that for descriptions.
 */
#ifndef RTEMS_DHCP_FAILSAFE_NETWORK_FAIL_TIMEOUT
#define RTEMS_DHCP_FAILSAFE_NETWORK_FAIL_TIMEOUT 5
#endif

#ifndef RTEMS_DHCP_FAILSAFE_NETWORK_DOWN_TIME
#define RTEMS_DHCP_FAILSAFE_NETWORK_DOWN_TIME 30
#endif

#ifndef RTEMS_DHCP_FAILSAFE_BROADCAST_DELAY
#define RTEMS_DHCP_FAILSAFE_BROADCAST_DELAY 0
#endif

#ifndef RTEMS_DHCP_FAILSAFE_DHCP_MONITOR_PRIORITY
#define RTEMS_DHCP_FAILSAFE_DHCP_MONITOR_PRIORITY 250
#endif


void rtems_bsdnet_do_dhcp_failsafe (void);

/** Set the DHCP fallback options.  See the commentary at the top of the
 * implementation.
 @note Some of these options can be compile-time disabled - see the code.
 */
void rtems_bsdnet_dhcp_failsafe_config(
  int network_fail_timeout, /**< The number of seconds before the interface is
                              *  considered disconnected
                              */
  int network_down_time,    /**< The number of seconds the interface
                              *  remains down.
                              */
  int broadcast_delay,      /**< The delay in seconds before broadcasts
                              * are sent.
                              */
  int dhcp_monitor_priority /**< The monitor priority.*/
);

#ifdef __cplusplus
}
#endif

#endif
