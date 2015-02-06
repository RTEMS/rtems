/*
 *  Memory Controller driver interface
 *
 *  COPYRIGHT (c) 2015.
 *  Cobham Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __MCTRL_H__
#define __MCTRL_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Register MEMCTRL driver to Driver manager */
void mctrl_register_drv (void);

#ifdef __cplusplus
}
#endif

#endif
