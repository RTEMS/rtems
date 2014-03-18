/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_ASSERT_H
#define _RTEMS_SCORE_ASSERT_H

#include <rtems/score/basedefs.h>

#if defined( RTEMS_DEBUG )
  #include <assert.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Assertion similar to assert() controlled via RTEMS_DEBUG instead of
 * NDEBUG.
 */
#if defined( RTEMS_DEBUG )
  #define _Assert( _e ) \
    ( ( _e ) ? \
      ( void ) 0 : \
        __assert_func( __FILE__, __LINE__, __ASSERT_FUNC, #_e ) )
#else
  #define _Assert( _e ) ( ( void ) 0 )
#endif

/**
 * @brief Returns true if thread dispatching is allowed.
 *
 * Thread dispatching can be repressed via _Thread_Disable_dispatch() or
 * _ISR_Disable().
 */
#if defined( RTEMS_DEBUG )
  bool _Debug_Is_thread_dispatching_allowed( void );
#endif

/**
 * @brief Returns true if the current thread of execution owns the giant lock.
 */
#if defined( RTEMS_DEBUG )
  #if defined( RTEMS_SMP )
    bool _Debug_Is_owner_of_giant( void );
  #else
    #define _Debug_Is_owner_of_giant() (true)
  #endif
#endif

/**
 * @brief Returns true if the current thread of execution owns the allocator
 * mutex.
 */
#if defined( RTEMS_DEBUG )
  bool _Debug_Is_owner_of_allocator( void );
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_ASSERT_H */
