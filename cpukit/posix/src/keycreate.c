/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/key.h>

/*
 *  17.1.1 Thread-Specific Data Key Create, P1003.1c/Draft 10, p. 163
 */

int pthread_key_create(
  pthread_key_t  *key,
  void          (*destructor)( void * )
)
{
  POSIX_Keys_Control  *the_key;
  void                *table;
  uint32_t             the_api;
  uint32_t             bytes_to_allocate;


  _Thread_Disable_dispatch();

  the_key = _POSIX_Keys_Allocate();

  if ( !the_key ) {
    _Thread_Enable_dispatch();
    return EAGAIN;
  }

  the_key->destructor = destructor;

  /*
   *  This is a bit more complex than one might initially expect because
   *  APIs are optional.
   *
   *  NOTE: Currently RTEMS Classic API tasks are always enabled.
   */
  for ( the_api = 1; the_api <= OBJECTS_APIS_LAST; the_api++ ) {
    the_key->Values[ the_api ] = NULL;

    #if defined(RTEMS_DEBUG)
      /*
       *  Since the removal of ITRON, this cannot occur.
       */
      if ( !_Objects_Information_table[ the_api ] )
	continue;

      /*
       * Currently all managers are installed if the API is installed.
       * This would be a horrible implementation error.
       */
      if (_Objects_Information_table[ the_api ][ 1 ] == NULL )
	_Internal_error_Occurred(
	  INTERNAL_ERROR_CORE,
	  true,
	  INTERNAL_ERROR_IMPLEMENTATION_KEY_CREATE_INCONSISTENCY
	);
    #endif

    bytes_to_allocate = sizeof( void * ) *
      (_Objects_Information_table[ the_api ][ 1 ]->maximum + 1);
    table = _Workspace_Allocate( bytes_to_allocate );
    if ( !table ) {
      _POSIX_Keys_Free_memory( the_key );

      _POSIX_Keys_Free( the_key );
      _Thread_Enable_dispatch();
      return ENOMEM;
    }

    the_key->Values[ the_api ] = table;
    memset( table, '\0', bytes_to_allocate );
  }

  _Objects_Open_u32( &_POSIX_Keys_Information, &the_key->Object, 0 );
  *key = the_key->Object.id;
  _Thread_Enable_dispatch();
  return 0;
}
