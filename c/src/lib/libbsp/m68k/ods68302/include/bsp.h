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
#include <rtems/m68k/m68302.h>

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

#ifdef __cplusplus
}
#endif

#endif
