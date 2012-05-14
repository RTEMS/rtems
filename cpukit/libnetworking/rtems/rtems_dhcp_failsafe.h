/*
  Description: Wrapper around DHCP client to restart it when the interface
               moves to another network.

  Authors: Arnout Vandecappelle <arnout@mind.be>, Essensium/Mind
           Maarten Van Es <maarten@mind.be>, Essensium/Mind
  (C) Septentrio 2008

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution or at
  http://www.rtems.com/license/LICENSE.
*/

#ifndef _RTEMS_DHCP_FAILSAFE_H_
#define _RTEMS_DHCP_FAILSAFE_H_

#ifdef __cplusplus
extern "C" {
#endif

void rtems_bsdnet_do_dhcp_failsafe (void);

#ifdef __cplusplus
}
#endif

#endif
