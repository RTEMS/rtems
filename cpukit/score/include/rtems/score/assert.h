/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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
 * @brief Asserts that thread dispatching is repressed.
 *
 * Thread dispatching can be repressed via _Thread_Disable_dispatch() or
 * _ISR_Disable().
 */
#if defined( RTEMS_DEBUG )
  void _Assert_Thread_dispatching_repressed( void );
#else
  #define _Assert_Thread_dispatching_repressed() ( ( void ) 0 )
#endif

/**
 * @brief Asserts that current thread of execution owns the giant lock.
 */
#if defined( RTEMS_DEBUG ) && defined( RTEMS_SMP )
  void _Assert_Owner_of_giant( void );
#else
  #define _Assert_Owner_of_giant() ( ( void ) 0 )
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_ASSERT_H */
