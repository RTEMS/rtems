/**
 * @file
 *
 * @brief Initializes the Time of Day Handler
 *
 * @ingroup RTEMSScoreTOD
 */

/*  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/todimpl.h>
#include <rtems/score/apimutex.h>

TOD_Control _TOD;

static API_Mutex_Control _TOD_Mutex = API_MUTEX_INITIALIZER( "_TOD" );

void _TOD_Lock( void )
{
  _API_Mutex_Lock( &_TOD_Mutex );
}

void _TOD_Unlock( void )
{
  _API_Mutex_Unlock( &_TOD_Mutex );
}

#if defined(RTEMS_DEBUG)
bool _TOD_Is_owner( void )
{
  return _API_Mutex_Is_owner( &_TOD_Mutex );
}
#endif
