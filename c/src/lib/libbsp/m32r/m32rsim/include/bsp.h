/**
 *  @file
 *
 *  @ingroup m32r_bsp
 *
 *  @brief m32r definitions in gdb
 */

/*  bsp.h
 *
 *  This include file contains some definitions specific to the
 *  h8 simulator in gdb.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_M32R_M32RSIM_BSP_H
#define LIBBSP_M32R_M32RSIM_BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup m32r_bsp Clock Tick Support
 *
 *  @ingroup m32r_m32rsim
 *
 *  @brief Clock Tick Support Package
 */

Thread clock_driver_sim_idle_body(uintptr_t);
#define BSP_IDLE_TASK_BODY clock_driver_sim_idle_body

/* Trap support interface from Newlib 1.16.0 */
#define SYS_exit        1
#define SYS_open        2
#define SYS_close       3
#define SYS_read        4
#define SYS_write       5
#define SYS_lseek       6
#define SYS_unlink      7
#define SYS_getpid      8
#define SYS_kill        9
#define SYS_fstat       10

int __trap0 (int function, int p1, int p2, int p3, struct _reent *r);

#define TRAP0(f, p1, p2, p3) \
  __trap0 (f, (int) (p1), (int) (p2), (int) (p3), _REENT)

#ifdef __cplusplus
}
#endif

#endif
