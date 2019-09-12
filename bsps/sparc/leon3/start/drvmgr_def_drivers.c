/*
 *  Default BSP drivers when Driver Manager enabled
 *
 *  COPYRIGHT (c) 2019.
 *  Cobham Gaisler
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
#include <bsp.h>

#ifdef RTEMS_DRVMGR_STARTUP
#include <drvmgr/drvmgr.h>

extern void gptimer_register_drv (void);
extern void apbuart_cons_register_drv(void);
/* All drivers included by BSP, this is overridden by the user by including
 * the drvmgr_confdefs.h. By default the Timer and UART driver are included.
 */
drvmgr_drv_reg_func drvmgr_drivers[] __attribute__((weak)) =
{
  gptimer_register_drv,
  apbuart_cons_register_drv,
  NULL /* End array with NULL */
};

#endif
