/**
 *  @file
 *
 *  @brief RTEMS Threads Compatibility Routines for Libgcc2
 *  @ingroup GxxWrappersSupport
 */

/*
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/gxx_wrappers.h>
#include <rtems/score/onceimpl.h>

#include <errno.h>
#include <stdlib.h>
#include <pthread.h>

#include <rtems.h>

/* uncomment this if you need to debug this interface */
/*#define DEBUG_GXX_WRAPPERS 1*/

int rtems_gxx_once(__gthread_once_t *once, void (*func) (void))
{
  #ifdef DEBUG_GXX_WRAPPERS
    printk( "gxx_wrappers: once=%x, func=%x\n", *once, func );
  #endif

  return _Once( (unsigned char *) once, func );
}

int rtems_gxx_key_create (__gthread_key_t *key, void (*dtor) (void *))
{
  int eno;
  pthread_key_t *pkey;

  pkey = malloc( sizeof( *pkey ) );
  *key = pkey;
  if ( pkey == NULL )
  {
    return ENOMEM;
  }

  #ifdef DEBUG_GXX_WRAPPERS
    printk(
      "gxx_wrappers: create key=%x, dtor=%x, pkey=%x\n", key, dtor, pkey
    );
  #endif

  eno = pthread_key_create(pkey, dtor);
  if ( eno != 0 ) {
    free( pkey );
    *key = NULL;
  }

  return eno;
}

int rtems_gxx_key_delete (__gthread_key_t key)
{
  int eno = 0;
  pthread_key_t *pkey = key;

  #ifdef DEBUG_GXX_WRAPPERS
    printk( "gxx_wrappers: delete key=%x\n", pkey );
  #endif

  if ( pkey == NULL ) {
    return EINVAL;
  }

  eno = pthread_key_delete(*pkey);
  if ( eno == 0 ) {
    free( pkey );
  }
  return eno;
}

void *rtems_gxx_getspecific(__gthread_key_t key)
{
  pthread_key_t *pkey = key;
  void *p = NULL;

  if ( pkey != NULL ) {
    p = pthread_getspecific( *pkey );
  }

  #ifdef DEBUG_GXX_WRAPPERS
    printk(
      "gxx_wrappers: getspecific key=%x, ptr=%x, id=%x\n",
       pkey,
       p,
       rtems_task_self()
    );
  #endif
  return p;
}

int rtems_gxx_setspecific(__gthread_key_t key, const void *ptr)
{
  pthread_key_t *pkey = key;
  int eno;

  if ( pkey == NULL ) {
    return EINVAL;
  }

  eno = pthread_setspecific( *pkey, ptr );

  #ifdef DEBUG_GXX_WRAPPERS
    printk(
      "gxx_wrappers: setspecific key=%x, ptr=%x, id=%x\n",
      pkey,
      ptr,
      rtems_task_self()
      );
  #endif

  if ( eno != 0 ) {
    _Internal_error( INTERNAL_ERROR_GXX_KEY_ADD_FAILED );
  }

  return 0;
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
    _Internal_error( INTERNAL_ERROR_GXX_MUTEX_INIT_FAILED );
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
