/* Driver Manager Configuration file.
 *
 * COPYRIGHT (c) 2009.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

/*
 *  The configuration consist of an array with function pointers that
 *  register one or more drivers that will be used by the Driver Manger.
 *
 *  The Functions are called in the order they are declared.
 *
 */

#ifndef _DRIVER_MANAGER_CONFDEFS_H_
#define _DRIVER_MANAGER_CONFDEFS_H_

#include "drvmgr.h"

#ifdef __cplusplus
extern "C" {
#endif

extern struct drvmgr_drv_reg_func drvmgr_drivers[];

#ifdef CONFIGURE_INIT

#if 0 /* EXAMPLE: GPTIMER driver definition */
#define DRIVER_AMBAPP_GAISLER_GPTIMER_REG {gptimer_register_drv}
extern void gptimer_register_drv(void);
#endif

/* CONFIGURE DRIVER MANAGER */
struct drvmgr_drv_reg_func drvmgr_drivers[] = {
#if 0 /* EXAMPLE: GPTIMER Driver registration */
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GPTIMER
	DRIVER_AMBAPP_GAISLER_GPTIMER_REG,
#endif
#endif

/* Macros for adding custom drivers without needing to recompile
 * kernel.
 */
#ifdef CONFIGURE_DRIVER_CUSTOM1
	DRIVER_CUSTOM1_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM2
	DRIVER_CUSTOM2_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM3
	DRIVER_CUSTOM3_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM4
	DRIVER_CUSTOM4_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM5
	DRIVER_CUSTOM5_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM6
	DRIVER_CUSTOM6_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM7
	DRIVER_CUSTOM7_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM8
	DRIVER_CUSTOM8_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM9
	DRIVER_CUSTOM9_REG,
#endif

	/* End array with NULL */
	{NULL}
};

#endif /* CONFIGURE_INIT */

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_MANAGER_CONFDEFS_H_ */
