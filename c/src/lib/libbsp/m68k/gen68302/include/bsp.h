/**
 * @file
 *
 * @ingroup m68k_gen68302
 *
 * @brief Global BSP definitions.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_M68K_GEN68302_BSP_H
#define LIBBSP_M68K_GEN68302_BSP_H

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
 * @defgroup m68k_gen68302 GEN68302 Support
 *
 * @ingroup bsp_m68k
 *
 * @brief GEN68302 support.
 */

/* Constants */

#define RAM_START 0
#define RAM_END   0x040000

/* Structures */

#ifdef GEN68302_INIT
#undef EXTERN
#define EXTERN
#else
#undef EXTERN
#define EXTERN extern
#endif

/* miscellaneous stuff assumed to exist */

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
