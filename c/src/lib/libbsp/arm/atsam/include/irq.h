/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_ATSAM_ESV_70_IRQ_H
#define LIBBSP_ARM_ATSAM_ESV_70_IRQ_H

#include <bspopts.h>

#ifndef ASM
  #include <rtems.h>
  #include <rtems/irq.h>
  #include <rtems/irq-extension.h>
#endif

#define BSP_INTERRUPT_VECTOR_MIN 0

#define BSP_INTERRUPT_VECTOR_MAX 64

#endif /* LIBBSP_ARM_ATSAM_ESV_70_IRQ_H */
