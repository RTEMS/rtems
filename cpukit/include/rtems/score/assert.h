/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreAssert
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreAssert.
 */

/*
 * Copyright (C) 2013, 2014 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
    #define __RTEMS_ASSERT_FUNCTION RTEMS_FUNCTION_NAME
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
