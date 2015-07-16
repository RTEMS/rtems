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

#ifndef LIBBSP_M68K_GEN68340_BSP_H
#define LIBBSP_M68K_GEN68340_BSP_H

#ifndef ASM

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/iosupp.h>
#include <rtems/clockdrv.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/*
 *  Methods used across files inside the BSP
 */
int dbug_in_char( int minor );
void dbug_out_char( int minor, int ch );
int dbug_char_present( int minor );
void _dbug_dumpanic(void);

/*
 *  Only called from .S but prototyped here to capture the dependecy.
 */
void _Init68340 (void);

#ifdef __cplusplus
}
#endif

#endif /* !ASM */

#endif
