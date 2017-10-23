/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_IO_H
#define _RTEMS_SCORE_IO_H

#include <rtems/score/basedefs.h>

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void ( *IO_Put_char )(int c, void *arg);

int _IO_Printf(
  IO_Put_char  put_char,
  void        *arg,
  char const  *fmt,
  ...
) RTEMS_PRINTFLIKE( 3, 4 );

int _IO_Vprintf(
  IO_Put_char  put_char,
  void        *arg,
  char const  *fmt,
  va_list      ap
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_IO_H */
