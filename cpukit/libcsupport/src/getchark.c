/**
 * @file
 *
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief This source file contains the implementation of getchark().
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/bspIo.h>

int getchark( void )
{
  BSP_polling_getchar_function_type poll_char;

  poll_char = BSP_poll_char;

  if ( poll_char == NULL ) {
    return -1;
  }

  return ( *poll_char )();
}
