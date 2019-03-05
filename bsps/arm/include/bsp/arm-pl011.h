/**
 *  @file
 *
 *  @ingroup RTEMSBSPsARMShared
 *
 *  @brief ARM PL011 Support Package
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_ARM_PL011_H
#define LIBBSP_ARM_SHARED_ARM_PL011_H

#include <rtems/termiostypes.h>

#include <bsp/arm-pl011-regs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  rtems_termios_device_context base;
  volatile pl011 *regs;
  rtems_vector_number irq;
  uint32_t initial_baud;
} arm_pl011_context;

bool arm_pl011_probe(rtems_termios_device_context *base);

void arm_pl011_write_polled(rtems_termios_device_context *base, char c);

extern const rtems_termios_device_handler arm_pl011_fns;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_ARM_PL011_H */
