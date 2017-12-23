/*  GPTIMER and GRTIMER timer driver
 *
 *  COPYRIGHT (c) 2015.
 *  Cobham Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#ifndef __GPTIMER_H__
#define __GPTIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* The GPTIMER_INFO_AVAIL define set from the BSP bsp.h configures if the 
 * Driver manager info interface is built.
 */

/* Register GPTIMER and GRTIMER driver to Driver Manager */
void gptimer_register_drv (void);

#ifdef __cplusplus
}
#endif

#endif
