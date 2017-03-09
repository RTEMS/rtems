/**
 * @file tms570-sci-driver.h
 *
 * @ingroup tms570
 *
 * @brief Declaration of serial's driver inner structure.
 */

/*
 * Copyright (c) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Based on LPC24xx and LPC1768 BSP
 * by embedded brains GmbH and others
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef TMS570_SCI_DRIVER
#define TMS570_SCI_DRIVER

#include <rtems/termiostypes.h>
#include <rtems/irq.h>
#include <bsp/tms570-sci.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Low-level driver specific data structure */
typedef struct {
  rtems_termios_device_context base;
  const char *device_name;
  volatile tms570_sci_t *regs;
  int tx_chars_in_hw;
  rtems_vector_number irq;
} tms570_sci_context;

extern const rtems_termios_device_handler tms570_sci_handler_polled;

extern const rtems_termios_device_handler tms570_sci_handler_interrupt;

extern tms570_sci_context driver_context_table[];

void tms570_sci_initialize(tms570_sci_context *ctx);

bool tms570_sci_set_attributes(
  rtems_termios_device_context *base,
  const struct termios *term
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TMS570_SCI_DRIVER */
