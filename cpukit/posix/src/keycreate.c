/*
 *  $Id$
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

/*PAGE
 *
 *  17.1.1 Thread-Specific Data Key Create, P1003.1c/Draft 10, p. 163
 */

int pthread_key_create(
  pthread_key_t  *key,
  void          (*destructor)( void * )
)
{
  POSIX_Keys_Control  *the_key;
  void                *table;
  unsigned32           the_api;
  unsigned32           bytes_to_allocate;


  _Thread_Disable_dispatch();
 
  the_key = _POSIX_Keys_Allocate();
 
  if ( !the_key ) {
    _Thread_Enable_dispatch();
    return EAGAIN;
  }

  the_key->destructor = destructor;

  /*
   *  This is a bit more complex than one might initially expect because
   *  APIs are optional.  Thus there may be no ITRON tasks to have keys
   *  for.  [NOTE: Currently RTEMS Classic API tasks are not always enabled.]
   */

  for ( the_api = 1;
        the_api <= OBJECTS_APIS_LAST;
        the_api++ ) {

    if ( _Objects_Information_table[ the_api ] &&
         _Objects_Information_table[ the_api ][ 1 ] ) {
      bytes_to_allocate = sizeof( void * ) * 
        (_Objects_Information_table[ the_api ][ 1 ]->maximum + 1);
      table = _Workspace_Allocate( bytes_to_allocate );
      if ( !table ) {
        for ( --the_api;
              the_api >= 1;
              the_api-- )
          _Workspace_Free( the_key->Values[ the_api ] );
  
        _POSIX_Keys_Free( the_key );
        _Thread_Enable_dispatch();
        return ENOMEM;
      }

      the_key->Values[ the_api ] = table;
      memset( table, '\0', bytes_to_allocate ); 
    } else {
      the_key->Values[ the_api ] = NULL;
    }


  }

  the_key->is_active = TRUE;

  _Objects_Open( &_POSIX_Keys_Information, &the_key->Object, 0 );
 
  *key = the_key->Object.id;

  _Thread_Enable_dispatch();

  return 0;
}
