/**
 * @file
 *
 * @ingroup RTEMSScoreAssert
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreAssert.
 */

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
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_ASSERT_H
#define _RTEMS_SCORE_ASSERT_H

#include <rtems/score/basedefs.h>

/**
 * @defgroup RTEMSScoreAssert Assert Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Assert Handler implementation.
 *
 * @{
 */

#if defined( RTEMS_DEBUG )
  #include <assert.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Assertion similar to assert() controlled via RTEMS_DEBUG instead of
 *   NDEBUG and static analysis runs.
 */
#if defined( RTEMS_DEBUG ) || defined( RTEMS_STATIC_ANALYSIS )

  /**
   * @brief Macro with method name used in assert output
   *
   * Given the variations in compilers and standards, we have to poke a bit.
   *
   * @note This is based on the code in newlib's assert.h.
   */
  #ifndef __RTEMS_ASSERT_FUNCTION
    /* Use g++'s demangled names in C++.  */
    #if defined __cplusplus && defined __GNUC__
      #define __RTEMS_ASSERT_FUNCTION __PRETTY_FUNCTION__

    /* C99 requires the use of __func__.  */
    #elif __STDC_VERSION__ >= 199901L
      #define __RTEMS_ASSERT_FUNCTION __func__

    /* Older versions of gcc don't have __func__ but can use __FUNCTION__.  */
    #elif __GNUC__ >= 2
      #define __RTEMS_ASSERT_FUNCTION __FUNCTION__

    /* failed to detect __func__ support.  */
    #else
      #define __RTEMS_ASSERT_FUNCTION ((char *) 0)
    #endif
  #endif /* !__RTEMS_ASSERT_FUNCTION */

  #if !defined( RTEMS_SCHEDSIM )
    /* normal build is newlib. */

    #define _Assert( _e ) \
       ( ( _e ) ? \
         ( void ) 0 : \
           __assert_func( __FILE__, __LINE__, __RTEMS_ASSERT_FUNCTION, #_e ) )

  #elif defined(__linux__)
    /* Scheduler simulator has only beed tested on glibc. */
    #define _Assert( _e ) \
     ( ( _e ) ? \
       ( void ) 0 : \
         __assert_fail( #_e, __FILE__, __LINE__, __RTEMS_ASSERT_FUNCTION ) )
  #else
    #error "Implement RTEMS assert support for this C Library"
  #endif

#else
  #define _Assert( _e ) ( ( void ) 0 )
#endif

/**
 * @brief Assert if unused return value is equal.
 *
 * Assert whether @a _var and @a _val are equal and ensure @a _var is
 * marked as used when not building for debug.
 *
 * @param _var The return value to be checked.
 * @param _val Indicates what @a _var is supposed to be.
 */
#define _Assert_Unused_variable_equals(_var,_val) \
        do { \
          _Assert((_var) == (_val)); \
          (void) (_var); \
        } while (0)

/**
 * @brief Assert if unused return value is not equal.
 *
 * Assert whether @a _var and @a _val are not equal and ensure @a _var
 * is marked as used when not building for debug.
 *
 * @param _var The return value to be checked.
 * @param _val Indicates what @a _var is not supposed to be.
 */
#define _Assert_Unused_variable_unequal(_var,_val) \
         do { \
          _Assert((_var) != (_val)); \
           (void) (_var); \
        } while (0)

/**
 * @brief Returns true if thread dispatching is allowed.
 *
 * Thread dispatching can be repressed via _Thread_Disable_dispatch() or
 * _ISR_Local_disable().
 */
#if defined( RTEMS_DEBUG )
  bool _Debug_Is_thread_dispatching_allowed( void );
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* _RTEMS_SCORE_ASSERT_H */
