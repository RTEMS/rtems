/*
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 *
 * $Id$
 */

#ifndef _IRQ_H_
#define _IRQ_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Type definition for RTEMS managed interrupts
 */

  typedef unsigned char rtems_irq_name;
  typedef unsigned char rtems_irq_level;
  typedef unsigned char rtems_irq_trigger;

  struct __rtems_irq_connect_data__;    /* forward declaratiuon */

  typedef void (*rtems_irq_hdl) (void);
  typedef void (*rtems_irq_enable) (const struct __rtems_irq_connect_data__
                                    *);
  typedef void (*rtems_irq_disable) (const struct __rtems_irq_connect_data__
                                     *);
  typedef int (*rtems_irq_is_enabled) (const struct __rtems_irq_connect_data__
                                       *);

/** irq connection data structure */
  typedef struct __rtems_irq_connect_data__
  {
    rtems_irq_name name;
    rtems_irq_hdl hdl;
    rtems_irq_enable on;
    rtems_irq_disable off;
    rtems_irq_is_enabled isOn;
    rtems_irq_level irqLevel;
    rtems_irq_trigger irqTrigger;
  } rtems_irq_connect_data;

  void BSP_rtems_irq_mngt_init (void);

  int BSP_install_rtems_irq_handler (const rtems_irq_connect_data *);

  int BSP_get_current_rtems_irq_handler (rtems_irq_connect_data *);

  int BSP_remove_rtems_irq_handler (const rtems_irq_connect_data *);

#ifdef __cplusplus
}
#endif

#endif
