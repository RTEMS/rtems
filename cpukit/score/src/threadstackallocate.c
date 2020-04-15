/**
 * @file
 *
 * @ingroup RTEMSScoreStack
 *
 * @brief Stack Allocate Helper
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/stackimpl.h>
#include <rtems/config.h>

void *_Stack_Allocate( size_t stack_size )
{
  return ( *rtems_configuration_get_stack_allocate_hook() )( stack_size );
}
