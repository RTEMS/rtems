/**
 * @file
 *
 * @ingroup m68k_gen68340
 *
 * @brief Global BSP definitions.
 */

/*
 * Board Support Package for `Generic' Motorola MC68340
 *
 * Based on the `gen68360' board support package, and covered by the
 * original distribution terms.
 */

/*  bsp.h
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/iosupp.h>
#include <rtems/clockdrv.h>

/**
 * @defgroup m68k_gen68340 Motorola MC68340 Support
 *
 * @ingroup bsp_m68k
 *
 * @brief Motorola MC68340 support.
 */

/* Constants */

/* Structures */

extern rtems_isr_entry M68Kvec[];   /* vector table address */

/* functions */

rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

#ifdef __cplusplus
}
#endif

#endif
