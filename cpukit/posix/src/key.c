/*  key.c
 *
 */

#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>

#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/key.h>

/*
 *  _POSIX_Key_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _POSIX_Key_Manager_initialization(
  unsigned32 maximum_keys
)
{
  _Objects_Initialize_information(
    &_POSIX_Keys_Information,
    OBJECTS_POSIX_KEYS,
    FALSE,
    maximum_keys,
    sizeof( POSIX_Keys_Control ),
    FALSE,
    0,
    FALSE
  );
}
 
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
  unsigned32           the_class;
  unsigned32           bytes_to_allocate;


  _Thread_Disable_dispatch();
 
  the_key = _POSIX_Keys_Allocate();
 
  if ( !the_key ) {
    _Thread_Enable_dispatch();
    return EAGAIN;
  }

  the_key->destructor = destructor;

  for ( the_class = OBJECTS_CLASSES_FIRST_THREAD_CLASS;
        the_class <= OBJECTS_CLASSES_LAST_THREAD_CLASS;
        the_class++ ) {

    bytes_to_allocate =
      (_Objects_Information_table[ the_class ]->maximum + 1) * sizeof( void * );

    table = _Workspace_Allocate( bytes_to_allocate );

    if ( !table ) {
      for ( --the_class;
            the_class >= OBJECTS_CLASSES_FIRST_THREAD_CLASS;
            the_class-- )
        _Workspace_Free( the_key->Values[ the_class ] );

      _POSIX_Keys_Free( the_key );
      _Thread_Enable_dispatch();
      return ENOMEM;
    }

    the_key->Values[ the_class ] = table;
    memset( table, '\0', bytes_to_allocate ); 
  }

  the_key->is_active = TRUE;

  _Objects_Open( &_POSIX_Keys_Information, &the_key->Object, 0 );
 
  *key = the_key->Object.id;

  _Thread_Enable_dispatch();

  return 0;
}

/*PAGE
 *
 *  17.1.2 Thread-Specific Data Management, P1003.1c/Draft 10, p. 165
 */

int pthread_setspecific(
  pthread_key_t  key,
  const void    *value
)
{
  register POSIX_Keys_Control *the_key;
  unsigned32                   index;
  unsigned32                   class;
  Objects_Locations            location;
 
  the_key = _POSIX_Keys_Get( key, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_REMOTE:
      return EINVAL;       /* should never happen */
    case OBJECTS_LOCAL:
      index = _Objects_Get_index( _Thread_Executing->Object.id );
      class = _Objects_Get_class( _Thread_Executing->Object.id );
      the_key->Values[ class ][ index ] = (void *) value;
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  17.1.2 Thread-Specific Data Management, P1003.1c/Draft 10, p. 165
 */

void *pthread_getspecific(
  pthread_key_t  key
)
{
  register POSIX_Keys_Control *the_key;
  unsigned32                   index;
  unsigned32                   class;
  Objects_Locations            location;
 
  the_key = _POSIX_Keys_Get( key, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return NULL;
    case OBJECTS_REMOTE:
      return 0;       /* should never happen */
    case OBJECTS_LOCAL:
      index = _Objects_Get_index( _Thread_Executing->Object.id );
      class = _Objects_Get_class( _Thread_Executing->Object.id );
      return (void *) the_key->Values[ class ][ index ];
  }
  return (void *) POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  17.1.3 Thread-Specific Data Key Deletion, P1003.1c/Draft 10, p. 167
 */

int pthread_key_delete(
  pthread_key_t  key
)
{
  register POSIX_Keys_Control *the_key;
  Objects_Locations            location;
  unsigned32                   the_class;
 
  the_key = _POSIX_Keys_Get( key, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_REMOTE:
      return 0;       /* should never happen */
    case OBJECTS_LOCAL:
      _Objects_Close( &_POSIX_Keys_Information, &the_key->Object );

      the_key->is_active = FALSE;

      for ( the_class = OBJECTS_CLASSES_FIRST_THREAD_CLASS;
            the_class <= OBJECTS_CLASSES_LAST_THREAD_CLASS;
            the_class++ )
        _Workspace_Free( the_key->Values[ the_class ] );

      /*
       *  NOTE:  The destructor is not called and it is the responsibility
       *         of the application to free the memory.
       */

      _POSIX_Keys_Free( the_key );
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  _POSIX_Keys_Run_destructors
 *
 *  17.1.1 Thread-Specific Data Key Create, P1003.1c/Draft 10, p. 163
 *
 *  NOTE:  This is the routine executed when a thread exits to
 *         run through all the keys and do the destructor action.
 *
 *  XXX: This needs to be hooked to the thread exitting -- SOMEHOW.
 */

void _POSIX_Keys_Run_destructors(
  Thread_Control *thread
)
{
  unsigned32           index;
  unsigned32           pthread_index;
  unsigned32           pthread_class;
  unsigned32           iterations;
  boolean              are_all_null;
  POSIX_Keys_Control  *the_key;
  void                *value;

  pthread_index = _Objects_Get_index( thread->Object.id );
  pthread_class = _Objects_Get_class( thread->Object.id );

  iterations = 0;

  for ( ; ; ) {

    are_all_null = TRUE;

    for ( index=1 ; index <= _POSIX_Keys_Information.maximum ; index++ ) {

      the_key = (POSIX_Keys_Control *) 
        _POSIX_Keys_Information.local_table[ index ];
      
      if ( the_key && the_key->is_active && the_key->destructor ) {
        value = the_key->Values[ pthread_class ][ pthread_index ];
        if ( value ) {
          (*the_key->destructor)( value );
          if ( the_key->Values[ pthread_class ][ pthread_index ] )
            are_all_null = FALSE;
        }
      }
    }

    if ( are_all_null == TRUE ) 
      return;

    iterations++;

    /*
     *  The standard allows one to not do this and thus go into an infinite
     *  loop.  It seems rude to unnecessarily lock up a system.
     */

    if ( iterations >= PTHREAD_DESTRUCTOR_ITERATIONS )
      return; 
  }
}
