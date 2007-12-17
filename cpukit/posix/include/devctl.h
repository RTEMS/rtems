/**
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * @file devctl.h
 *  $Id$
 */

#ifndef _DEVCTL_H
#define _DEVCTL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>

#if defined(_POSIX_DEVICE_CONTROL)

#include <sys/types.h>
#include <unistd.h>

/*
 *  21.2.1 Control a Device, P1003.4b/D8, p. 65
 */

int devctl(
  int     filedes,
  void   *dev_data_ptr,
  size_t  nbyte,
  int    *dev_info_ptr
);

#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
