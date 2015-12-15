/**
 * @file
 *
 * @brief Initialize POSIX API
 *
 * @ingroup ClassicRTEMS
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/posixapi.h>

void _POSIX_Fatal_error( POSIX_Fatal_domain domain, int eno )
{
  uint32_t code = ( domain << 8 ) | ( ( uint32_t ) eno & 0xffU );

  _Terminate( INTERNAL_ERROR_POSIX_API, false, code );
}
