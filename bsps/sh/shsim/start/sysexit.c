/*
 *  This file contains the simulator specific exit trap.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/syscall.h>

int errno; /* assumed to exist by exit_k() */

int _sys_exit (int n)
{
  return __trap34 (SYS_exit, n, 0, 0);
}
