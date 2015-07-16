/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_M68K_ODS68302_BSP_H
#define LIBBSP_M68K_ODS68302_BSP_H

#ifndef ASM

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/iosupp.h>
#include <rtems/clockdrv.h>
#include <rtems/m68k/m68302.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VARIANT
#define VARIANT bare
#endif
#if defined(VARIANT)
#define HQUOTE(a) <a.h>
#include HQUOTE(VARIANT)
#undef HQUOTE
#endif

/* Constants */

#define RAM_START RAM_BASE
#define RAM_END   (RAM_BASE + RAM_SIZE)

/* Structures */

#ifdef GEN68302_INIT
#undef EXTERN
#define EXTERN
#else
#undef EXTERN
#define EXTERN extern
#endif

extern rtems_isr_entry M68Kvec[];   /* vector table address */

/* functions */

rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

/*
 * Prototypes for methods called only from .S files
 */
void boot_phase_1(void);
void boot_phase_2(void);
void boot_phase_3(void);
void trace_exception(
  unsigned long d0,
  unsigned long d1,
  unsigned long d2,
  unsigned long d3,
  unsigned long d4,
  unsigned long d5,
  unsigned long d6,
  unsigned long d7,
  unsigned long a0,
  unsigned long a1,
  unsigned long a2,
  unsigned long a3,
  unsigned long a4,
  unsigned long a5,
  unsigned long a6,
  unsigned long a7,
  unsigned long sr_pch,
  unsigned long pcl_format
);

#ifdef __cplusplus
}
#endif

#endif

#endif
