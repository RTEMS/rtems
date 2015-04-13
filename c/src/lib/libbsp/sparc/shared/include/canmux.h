/*
 *  Header file for RTEMS CAN_MUX driver 
 *
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __CANMUX_H__
#define __CANMUX_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Driver interface */
int canmux_register(void);

/* ioctl calls */
#define CANMUX_IOC_BUSA_SATCAN 1
#define CANMUX_IOC_BUSA_OCCAN1 2
#define CANMUX_IOC_BUSB_SATCAN 3
#define CANMUX_IOC_BUSB_OCCAN2 4

#ifdef __cplusplus
}
#endif

#endif /* __CANMUX_H__ */
