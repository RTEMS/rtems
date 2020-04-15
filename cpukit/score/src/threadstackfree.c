/**
 * @file
 *
 * @ingroup RTEMSScoreStack
 *
 * @brief Deallocate Thread Stack
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

#include <rtems/score/stackimpl.h>
#include <rtems/config.h>

void _Stack_Free( void *stack_area )
{
  ( *rtems_configuration_get_stack_free_hook() )( stack_area );
}
