/*
 * RTEMS threads compatibily routines for libgcc2.
 *
 * by: Rosimildo da Silva ( rdasilva@connecttel.com
 *
 * Used ideas from:
 *    W. Eric Norum
 *    Canadian Light Source
 *    University of Saskatchewan
 *    Saskatoon, Saskatchewan, CANADA
 *    eric@cls.usask.ca
 *
 * Eric sent some e-mail in the rtems-list as a start point for this
 * module implementation.
 *
 * 
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

/* We might not need, defined just in case */
#define  __RTEMS_INSIDE__  1


#include <stdlib.h>
#include <stdio.h>

#include <rtems.h>
#include <rtems/system.h>
#include <rtems/rtems/tasks.h>

/* 
 * These typedefs should match with the ones defined in the file
 * gcc/gthr-rtems.h in the gcc distribution.
 */
typedef void *__gthread_key_t;
typedef int   __gthread_once_t;
typedef void *__gthread_mutex_t;


/* uncomment this if you need to debug this interface */

/*
#define DEBUG_GXX_WRAPPERS 1
*/


/* prototype for the terminate() */
extern void __terminate( void );


#ifdef DEBUG_GXX_WRAPPERS
/* local function to return the ID of the calling thread */
static rtems_id get_tid( void )
{
   rtems_id id = 0; 
   rtems_task_ident( RTEMS_SELF, 0, &id );
   return id;
}
#endif


int rtems_gxx_once(__gthread_once_t *once, void (*func) ())
{
#ifdef DEBUG_GXX_WRAPPERS
   printk( "gxx_wrappers: once=%x, func=%x\n", *once, func );
#endif
   if( *once == 0 ) 
   {
      /* 
       * NOTE: could not use the call to disable "preemption", it causes
       * one exception. Somebody might want to investiage it further
       * sometime later.
       */
      _Thread_Disable_dispatch();
      *once = 1;
      (*func)();
      _Thread_Enable_dispatch();
   }
   return 0;
}


int rtems_gxx_key_create (__gthread_key_t *key, void (*dtor) (void *))
{
  /* Ok, this can be a bit tricky. We are going to return a "key" as a
   * pointer to the buffer that will hold the value of the key itself.
   * We have to to this, becuase the others functions on this interface 
   * deal with the value of the key, as used with the POSIX API.
   */
   /* Do not pull your hair, trust me this works. :-) */
  __gthread_key_t *new_key = ( __gthread_key_t * )malloc( sizeof( __gthread_key_t ) );
  *key = ( __gthread_key_t )new_key;
  *new_key = NULL;

#ifdef DEBUG_GXX_WRAPPERS
  printk( "gxx_wrappers: create key=%x, dtor=%x, new_key=%x\n", key, dtor, new_key );
#endif
  /* register with RTEMS the buffer that will hold the key values */
  if( rtems_task_variable_add( RTEMS_SELF, (void **)new_key, NULL ) == RTEMS_SUCCESSFUL )
       return 0;
  return -1;
}

int rtems_gxx_key_dtor (__gthread_key_t key, void *ptr)
{
#ifdef DEBUG_GXX_WRAPPERS
  printk( "gxx_wrappers: dtor key=%x, ptr=%x\n", key, ptr );
#endif
   *(void **)key = 0;
   return 0;
}

int rtems_gxx_key_delete (__gthread_key_t key)
{
#ifdef DEBUG_GXX_WRAPPERS
  printk( "gxx_wrappers: delete key=%x\n", key );
#endif
  /* register with RTEMS the buffer that will hold the key values */
  if( rtems_task_variable_delete( RTEMS_SELF, (void **)key ) == RTEMS_SUCCESSFUL )
  {
     if( key ) free( (void *)key );
     return 0;
  }
  return 0;
}


void *rtems_gxx_getspecific(__gthread_key_t key)
{
  void *p= 0;

  /* register with RTEMS the buffer that will hold the key values */
  if( rtems_task_variable_get( RTEMS_SELF, (void **)key, &p ) == RTEMS_SUCCESSFUL )
  {
    /* We do not have to do this, but what the heck ! */
     p= *( void **)key;
  }
  else
  {
    /* fisrt time, always set to zero, it is unknown the value that the others
     * threads are using at the moment of this call
     */
    if( rtems_task_variable_add( RTEMS_SELF, (void **)key, NULL ) != RTEMS_SUCCESSFUL )
    {
       __terminate ();
    }
    *( void ** )key = (void *)0;
  }

#ifdef DEBUG_GXX_WRAPPERS
   printk( "gxx_wrappers: getspecific key=%x, ptr=%x, id=%x\n", key, p, get_tid() );
#endif
   return p;
}


int rtems_gxx_setspecific(__gthread_key_t key, const void *ptr)
{
#ifdef DEBUG_GXX_WRAPPERS
  printk( "gxx_wrappers: setspecific key=%x, ptr=%x, id=%x\n", key, ptr, get_tid() );
#endif
  /* register with RTEMS the buffer that will hold the key values */
  if( rtems_task_variable_add( RTEMS_SELF, (void **)key, NULL ) == RTEMS_SUCCESSFUL )
  {
    /* now let's set the proper value */
    *( void ** )key = (void *)ptr;
     return 0;
  }
  return -1;
}


/*
 * MUTEX support
 */
void rtems_gxx_mutex_init (__gthread_mutex_t *mutex)
{
#ifdef DEBUG_GXX_WRAPPERS
  printk( "gxx_wrappers: mutex init =%X\n", *mutex );
#endif
  if( rtems_semaphore_create( rtems_build_name ('G', 'C', 'C', '2'),
                              1,
                             RTEMS_PRIORITY|RTEMS_BINARY_SEMAPHORE
                             |RTEMS_INHERIT_PRIORITY | RTEMS_NO_PRIORITY_CEILING|RTEMS_LOCAL,
                             0,
                             (rtems_id *)mutex ) != RTEMS_SUCCESSFUL )
  {
      __terminate ();
  }
#ifdef DEBUG_GXX_WRAPPERS
  printk( "gxx_wrappers: mutex init complete =%X\n", *mutex );
#endif
}

int rtems_gxx_mutex_lock (__gthread_mutex_t *mutex)
{
#ifdef DEBUG_GXX_WRAPPERS
  printk( "gxx_wrappers: lock mutex=%X\n", *mutex );
#endif
  return ( rtems_semaphore_obtain( (rtems_id)*mutex, 
            RTEMS_WAIT, RTEMS_NO_TIMEOUT ) ==  RTEMS_SUCCESSFUL) ? 0 : -1;
}

int rtems_gxx_mutex_trylock (__gthread_mutex_t *mutex)
{
#ifdef DEBUG_GXX_WRAPPERS
  printk( "gxx_wrappers: trylock mutex=%X\n", *mutex );
#endif
  return (rtems_semaphore_obtain ((rtems_id)*mutex, 
               RTEMS_NO_WAIT, 0) == RTEMS_SUCCESSFUL) ? 0 : -1;
}

int rtems_gxx_mutex_unlock (__gthread_mutex_t *mutex)
{
#ifdef DEBUG_GXX_WRAPPERS
   printk( "gxx_wrappers: unlock mutex=%X\n", *mutex );
#endif
  return (rtems_semaphore_release( (rtems_id)*mutex ) ==  RTEMS_SUCCESSFUL) ? 0 :-1;
}

