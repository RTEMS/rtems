/*
 *  RTEMS threads compatibility routines for libgcc2.
 *
 *  by: Rosimildo da Silva (rdasilva@connecttel.com)
 *
 *  Used ideas from:
 *    W. Eric Norum
 *    Canadian Light Source
 *    University of Saskatchewan
 *    Saskatoon, Saskatchewan, CANADA
 *    eric@cls.usask.ca
 *
 *  Eric sent some e-mail in the rtems-list as a start point for this
 *  module implementation.
 */

/*
 * This file is only used if using gcc
 */
#if defined(__GNUC__)

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/gxx_wrappers.h>

#include <stdlib.h>

#include <rtems.h>

/* uncomment this if you need to debug this interface */
/*#define DEBUG_GXX_WRAPPERS 1*/

int rtems_gxx_once(__gthread_once_t *once, void (*func) (void))
{
  #ifdef DEBUG_GXX_WRAPPERS
    printk( "gxx_wrappers: once=%x, func=%x\n", *once, func );
  #endif

  if ( *(volatile __gthread_once_t *)once == 0 ) {
    rtems_mode saveMode;
    __gthread_once_t o;

    rtems_task_mode(RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &saveMode);
    if ( (o = *(volatile __gthread_once_t *)once) == 0 ) {
      *(volatile __gthread_once_t *)once = 1;
    }
    rtems_task_mode(saveMode, RTEMS_PREEMPT_MASK, &saveMode);
    if ( o == 0 )
      (*func)();
  }
  return 0;
}

int rtems_gxx_key_create (__gthread_key_t *key, void (*dtor) (void *))
{
  rtems_status_code status;

  /* Ok, this can be a bit tricky. We are going to return a "key" as a
   * pointer to the buffer that will hold the value of the key itself.
   * We have to to this, because the others functions on this interface
   * deal with the value of the key, as used with the POSIX API.
   */
   /* Do not pull your hair, trust me this works. :-) */
  __gthread_key_t new_key = (__gthread_key_t) malloc( sizeof( *new_key ) );
  *key = new_key;
  new_key->val  = NULL;
  new_key->dtor = dtor;

  #ifdef DEBUG_GXX_WRAPPERS
    printk(
      "gxx_wrappers: create key=%x, dtor=%x, new_key=%x\n", key, dtor, new_key
    );
  #endif

  /* register with RTEMS the buffer that will hold the key values */
  status = rtems_task_variable_add( RTEMS_SELF, (void **)new_key, dtor );
  if ( status == RTEMS_SUCCESSFUL )
    return 0;

  free( new_key );
  return -1;
}

int rtems_gxx_key_dtor (__gthread_key_t key, void *ptr)
{
  #ifdef DEBUG_GXX_WRAPPERS
    printk( "gxx_wrappers: dtor key=%x, ptr=%x\n", key, ptr );
  #endif

  key->val  = 0;
  return 0;
}

int rtems_gxx_key_delete (__gthread_key_t key)
{
  rtems_status_code status;

  #ifdef DEBUG_GXX_WRAPPERS
    printk( "gxx_wrappers: delete key=%x\n", key );
  #endif

  /* register with RTEMS the buffer that will hold the key values */
  status = rtems_task_variable_delete( RTEMS_SELF, (void **)key );
  if ( status == RTEMS_SUCCESSFUL ) {
    /* Hmm - hopefully all tasks using this key have gone away... */
    if ( key ) free( *(void **)key );
    return 0;
  }
  key = NULL;
  return 0;
}

void *rtems_gxx_getspecific(__gthread_key_t key)
{
  rtems_status_code  status;
  void              *p= 0;

  /* register with RTEMS the buffer that will hold the key values */
  status = rtems_task_variable_get( RTEMS_SELF, (void **)key, &p );
  if ( status == RTEMS_SUCCESSFUL ) {
    /* We do not have to do this, but what the heck ! */
     p= key->val;
  } else {
    /* fisrt time, always set to zero, it is unknown the value that the others
     * threads are using at the moment of this call
     */
    status = rtems_task_variable_add( RTEMS_SELF, (void **)key, key->dtor );
    if ( status != RTEMS_SUCCESSFUL ) {
      _Internal_error_Occurred(
        INTERNAL_ERROR_CORE,
        true,
        INTERNAL_ERROR_GXX_KEY_ADD_FAILED
      );
    }
    key->val = (void *)0;
  }

  #ifdef DEBUG_GXX_WRAPPERS
    printk(
      "gxx_wrappers: getspecific key=%x, ptr=%x, id=%x\n",
       key,
       p,
       rtems_task_self()
    );
  #endif
  return p;
}

int rtems_gxx_setspecific(__gthread_key_t key, const void *ptr)
{
  rtems_status_code status;

  #ifdef DEBUG_GXX_WRAPPERS
    printk(
      "gxx_wrappers: setspecific key=%x, ptr=%x, id=%x\n",
      key,
      ptr,
      rtems_task_self()
      );
  #endif

  /* register with RTEMS the buffer that will hold the key values */
  status = rtems_task_variable_add( RTEMS_SELF, (void **)key, key->dtor );
  if ( status == RTEMS_SUCCESSFUL ) {
    /* now let's set the proper value */
    key->val =  (void *)ptr;
    return 0;
  }
  return -1;
}


/*
 * MUTEX support
 */
void rtems_gxx_mutex_init (__gthread_mutex_t *mutex)
{
  rtems_status_code status;

  #ifdef DEBUG_GXX_WRAPPERS
    printk( "gxx_wrappers: mutex init =%X\n", *mutex );
  #endif

  status = rtems_semaphore_create(
    rtems_build_name ('G', 'C', 'C', '2'),
    1,
    RTEMS_PRIORITY|RTEMS_BINARY_SEMAPHORE|
      RTEMS_INHERIT_PRIORITY|RTEMS_NO_PRIORITY_CEILING|RTEMS_LOCAL,
    0,
    (rtems_id *)mutex
  );
  if ( status != RTEMS_SUCCESSFUL ) {
    #ifdef DEBUG_GXX_WRAPPERS
      printk(
        "gxx_wrappers: mutex init failed %s (%d)\n",
        rtems_status_text(status),
        status
      );
    #endif
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      true,
      INTERNAL_ERROR_GXX_MUTEX_INIT_FAILED
    );
  }
  #ifdef DEBUG_GXX_WRAPPERS
    printk( "gxx_wrappers: mutex init complete =%X\n", *mutex );
  #endif
}

int rtems_gxx_mutex_lock (__gthread_mutex_t *mutex)
{
  rtems_status_code status;

  #ifdef DEBUG_GXX_WRAPPERS
    printk( "gxx_wrappers: lock mutex=%X\n", *mutex );
  #endif

  status = rtems_semaphore_obtain(
    *(rtems_id *)mutex,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
  if ( status == RTEMS_SUCCESSFUL )
    return 0;
  return -1;
}

int rtems_gxx_mutex_destroy (__gthread_mutex_t *mutex)
{
  rtems_status_code status;

  #ifdef DEBUG_GXX_WRAPPERS
    printk( "gxx_wrappers: destroy mutex=%X\n", *mutex );
  #endif

  status = rtems_semaphore_delete(*(rtems_id *)mutex);
  if ( status == RTEMS_SUCCESSFUL )
    return 0;
  return -1;
}

int rtems_gxx_mutex_trylock (__gthread_mutex_t *mutex)
{
  rtems_status_code status;

  #ifdef DEBUG_GXX_WRAPPERS
    printk( "gxx_wrappers: trylock mutex=%X\n", *mutex );
  #endif

  status = rtems_semaphore_obtain (*(rtems_id *)mutex, RTEMS_NO_WAIT, 0);
  if ( status == RTEMS_SUCCESSFUL )
    return 0;
  return -1;
}

int rtems_gxx_mutex_unlock (__gthread_mutex_t *mutex)
{
  rtems_status_code status;

  #ifdef DEBUG_GXX_WRAPPERS
    printk( "gxx_wrappers: unlock mutex=%X\n", *mutex );
  #endif

  status = rtems_semaphore_release( *(rtems_id *)mutex );
  if ( status == RTEMS_SUCCESSFUL )
    return 0;
  return -1;
}

void rtems_gxx_recursive_mutex_init(__gthread_recursive_mutex_t *mutex)
{
  rtems_gxx_mutex_init(mutex);
}

int rtems_gxx_recursive_mutex_lock(__gthread_recursive_mutex_t *mutex)
{
  return rtems_gxx_mutex_lock(mutex);
}

int rtems_gxx_recursive_mutex_trylock(__gthread_recursive_mutex_t *mutex)
{
  return rtems_gxx_mutex_trylock(mutex);
}

int rtems_gxx_recursive_mutex_unlock(__gthread_recursive_mutex_t *mutex)
{
  return rtems_gxx_mutex_unlock(mutex);
}

#endif /* __GNUC__ */
