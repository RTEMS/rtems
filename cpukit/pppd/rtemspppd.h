/*
 * COPYRIGHT (c) 2001, Michael Siers <mikes@poliac.com>.
 *                     Poliac Research, Burnsville, Minnesota USA.
 * COPYRIGHT (c) 2001, On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef RTEMSPPPD_H
#define RTEMSPPPD_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* define hook function identifiers */
#define RTEMS_PPPD_LINKUP_HOOK      1
#define RTEMS_PPPD_LINKDOWN_HOOK    2
#define RTEMS_PPPD_IPUP_HOOK        3
#define RTEMS_PPPD_IPDOWN_HOOK      4
#define RTEMS_PPPD_ERROR_HOOK       5
#define RTEMS_PPPD_EXIT_HOOK        6

/* define hook function pointer prototype */
typedef void (*rtems_pppd_hookfunction)(void);
typedef int  (*rtems_pppd_dialerfunction)(int tty, int mode, char *pScript);


/* define pppd function prototyes */
int rtems_pppd_initialize(void);
int rtems_pppd_terminate(void);
int rtems_pppd_reset_options(void);
int rtems_pppd_set_hook(int id, rtems_pppd_hookfunction hookfp);
int rtems_pppd_set_dialer(rtems_pppd_dialerfunction dialerfp);
int rtems_pppd_set_option(const char *pOption, const char *pValue);
int rtems_pppd_connect(void);
int rtems_pppd_disconnect(void);

struct rtems_bsdnet_ifconfig;

int rtems_ppp_driver_attach(struct rtems_bsdnet_ifconfig *config, int attaching);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
