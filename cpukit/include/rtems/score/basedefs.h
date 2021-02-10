/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file provides basic definitions used by the API and the
 *   implementation.
 */

/*
 * Copyright (C) 2014 Paval Pisa
 * Copyright (C) 2011, 2013 On-Line Applications Research Corporation (OAR)
 * Copyright (C) 2009, 2021 embedded brains GmbH (http://www.embedded-brains.de)
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/basedefs/if/header */

#ifndef _RTEMS_SCORE_BASEDEFS_H
#define _RTEMS_SCORE_BASEDEFS_H

#include <rtems/score/cpuopts.h>

#if !defined(ASM)
  #include <stdbool.h>
  #include <stddef.h>
  #include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/if/api */

/**
 * @defgroup RTEMSAPI API
 *
 * @brief API
 *
 * This group contains the RTEMS Application Programming Interface (API).
 */

/* Generated from spec:/rtems/basedefs/if/group */

/**
 * @defgroup RTEMSAPIBaseDefs Base Definitions
 *
 * @ingroup RTEMSAPI
 *
 * @brief This group contains basic macros and defines to give access to
 *   compiler-specific features.
 */

/* Generated from spec:/rtems/basedefs/if/alias */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Instructs the compiler to generate an alias to the target function.
 *
 * @param _target is the target function name.
 */
#if defined(__GNUC__)
  #define RTEMS_ALIAS( _target ) __attribute__(( __alias__( #_target ) ))
#else
  #define RTEMS_ALIAS( _target )
#endif

/* Generated from spec:/rtems/basedefs/if/align-down */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Aligns down the value to the alignment.
 *
 * @param _value is the value to align down.
 *
 * @param _alignment is the desired alignment in bytes.  The alignment shall be
 *   a power of two, otherwise the returned value is undefined.  The alignment
 *   parameter is evaluated twice.
 *
 * @return Returns the value aligned down to the alignment.
 */
#define RTEMS_ALIGN_DOWN( _value, _alignment ) \
  ( ( _value ) & ~( ( _alignment ) - 1 ) )

/* Generated from spec:/rtems/basedefs/if/align-up */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Aligns up the value to the alignment.
 *
 * @param _value is the value to align up.
 *
 * @param _alignment is the desired alignment in bytes.  The alignment shall be
 *   a power of two, otherwise the returned value is undefined.  The alignment
 *   parameter is evaluated twice.
 *
 * @return Returns the value aligned up to the alignment.
 */
#define RTEMS_ALIGN_UP( _value, _alignment ) \
  ( ( ( _value ) + ( _alignment ) - 1 ) & ~( ( _alignment ) - 1 ) )

/* Generated from spec:/rtems/basedefs/if/aligned */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Instructs the compiler in a declaration or definition to enforce the
 *   alignment.
 *
 * @param _alignment is the desired alignment in bytes.
 */
#if defined(__GNUC__)
  #define RTEMS_ALIGNED( _alignment ) __attribute__(( __aligned__( _alignment ) ))
#else
  #define RTEMS_ALIGNED( _alignment )
#endif

/* Generated from spec:/rtems/basedefs/if/alignof */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Gets the alignment requirement of the type.
 *
 * @param _type_name is the type name to get the alignment requirement for.
 *
 * @return Returns the alignment requirement of the type.
 */
#if __cplusplus >= 201103L
  #define RTEMS_ALIGNOF( _type_name ) alignof( _type_name )
#elif __STDC_VERSION__ >= 201112L
  #define RTEMS_ALIGNOF( _type_name ) _Alignof( _type_name )
#else
  #define RTEMS_ALIGNOF( _type_name ) sizeof( _type_name )
#endif

/* Generated from spec:/rtems/basedefs/if/alloc-align */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Tells the compiler in a declaration that the memory allocation
 *   alignment parameter of this function is similar to aligned_alloc().
 *
 * @param _index is the allocation alignment parameter index (starting with
 *   one).
 */
#if defined(__GNUC__)
  #define RTEMS_ALLOC_ALIGN( _index ) __attribute__(( __alloc_align__( _index ) ))
#else
  #define RTEMS_ALLOC_ALIGN( _index )
#endif

/* Generated from spec:/rtems/basedefs/if/alloc-size */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Tells the compiler in a declaration that the memory allocation size
 *   parameter of this function is similar to malloc().
 *
 * @param _index is the allocation size parameter index (starting with one).
 */
#if defined(__GNUC__)
  #define RTEMS_ALLOC_SIZE( _index ) __attribute__(( __alloc_size__( _index ) ))
#else
  #define RTEMS_ALLOC_SIZE( _index )
#endif

/* Generated from spec:/rtems/basedefs/if/alloc-size-2 */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Tells the compiler in a declaration that the memory allocation item
 *   count and item size parameter of this function is similar to calloc().
 *
 * @param _count_index is the allocation item count parameter index (starting
 *   with one).
 *
 * @param _size_index is the allocation item size parameter index (starting
 *   with one).
 */
#if defined(__GNUC__)
  #define RTEMS_ALLOC_SIZE_2( _count_index, _size_index ) \
    __attribute__(( __alloc_size__( _count_index, _size_index ) ))
#else
  #define RTEMS_ALLOC_SIZE_2( _count_index, _size_index )
#endif

/* Generated from spec:/rtems/basedefs/if/array-size */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Gets the element count of the array.
 *
 * @param _array is the name of the array.  This parameter is evaluated twice.
 *
 * @return Returns the element count of the array.
 */
#define RTEMS_ARRAY_SIZE( _array ) \
  ( sizeof( _array ) / sizeof( ( _array )[ 0 ] ) )

/* Generated from spec:/rtems/basedefs/if/compiler-memory-barrier */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief This macro forbids the compiler to reorder read and write commands
 *   around it.
 */
#if defined(__GNUC__)
  #define RTEMS_COMPILER_MEMORY_BARRIER() __asm__ volatile( "" ::: "memory" )
#else
  #define RTEMS_COMPILER_MEMORY_BARRIER() do { } while ( 0 )
#endif

/* Generated from spec:/rtems/basedefs/if/concat */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Concatenates _x and _y without expanding.
 *
 * @param _x is the left hand side token of the concatenation.
 *
 * @param _y is the right hand side token of the concatenation.
 *
 * @return Returns the concatenation of the tokens _x and _y.
 */
#define RTEMS_CONCAT( _x, _y ) _x##_y

/* Generated from spec:/rtems/basedefs/if/const */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Tells the compiler in a function declaration that this function has
 *   no effect except the return value and that the return value depends only
 *   on the value of parameters.
 */
#if defined(__GNUC__)
  #define RTEMS_CONST __attribute__(( __const__ ))
#else
  #define RTEMS_CONST
#endif

/* Generated from spec:/rtems/basedefs/if/container-of */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Gets the container of a member.
 *
 * @param _m is the pointer to a member of the container.
 *
 * @param _type is the type of the container.
 *
 * @param _member_name is the designator name of the container member.
 *
 * @return Returns the pointer to the container of a member pointer.
 */
#define RTEMS_CONTAINER_OF( _m, _type, _member_name ) \
  ( (_type *) ( (uintptr_t) ( _m ) - offsetof( _type, _member_name ) ) )

/* Generated from spec:/rtems/basedefs/if/declare-global-symbol */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Declares a global symbol with the name.
 *
 * @param _name is the name of the global symbol.  It shall be a valid
 *   designator.
 *
 * This macro must be placed at file scope.
 */
#define RTEMS_DECLARE_GLOBAL_SYMBOL( _name ) extern char _name[]

/* Generated from spec:/rtems/basedefs/if/deprecated */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Instructs the compiler in a declaration to issue a warning whenever a
 *   variable, function, or type using this declaration will be used.
 */
#if defined(__GNUC__)
  #define RTEMS_DEPRECATED __attribute__(( __deprecated__ ))
#else
  #define RTEMS_DEPRECATED
#endif

/* Generated from spec:/rtems/basedefs/if/compiler-deprecated-attribute */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Provided for backward compatibility.
 */
#define RTEMS_COMPILER_DEPRECATED_ATTRIBUTE RTEMS_DEPRECATED

/* Generated from spec:/rtems/basedefs/if/expand */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Helper macro to perform a macro expansion on the token.
 *
 * @param _token is the token to expand.
 */
#define RTEMS_EXPAND( _token ) _token

/* Generated from spec:/rtems/basedefs/if/string */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Stringifies the arguments without expanding them.
 *
 * @param ... are the arguments to stringify.
 *
 * @return Returns the stringification of the arguments.
 */
#define RTEMS_STRING( ... ) #__VA_ARGS__

/* Generated from spec:/rtems/basedefs/if/typeof-refx */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Gets the pointer reference type.
 *
 * @param _level is the pointer indirection level expressed in *.
 *
 * @param _target is the reference target type.
 *
 * The reference type idea is based on libHX by Jan Engelhardt.
 *
 * @return Returns the type of a pointer reference of the specified level to
 *   the specified type.
 */
#if defined(__GNUC__)
  #define RTEMS_TYPEOF_REFX( _level, _target ) \
    __typeof__( _level( union { int _z; __typeof__( _target ) _x; } ){ 0 }._x )
#else
  #define RTEMS_TYPEOF_REFX( _level, _target )
#endif

/* Generated from spec:/rtems/basedefs/if/xconcat */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Concatenates expansion of _x and expansion of _y.
 *
 * @param _x is expanded first and then used as the left hand side token of the
 *   concatenation.
 *
 * @param _y is expanded first and then used as the right hand side token of
 *   the concatenation.
 *
 * @return Returns the concatenation of the expansions of tokens _x and _y.
 */
#define RTEMS_XCONCAT( _x, _y ) RTEMS_CONCAT( _x, _y )

/* Generated from spec:/score/if/assert-unreachable */

/**
 * @brief Asserts that this program point is unreachable.
 */
#if defined(RTEMS_DEBUG)
  #define _Assert_Unreachable() _Assert( 0 )
#else
  #define _Assert_Unreachable() do { } while ( 0 )
#endif

#if !defined(ASM)
  /* Generated from spec:/score/if/dequalify-types-not-compatible */

  /**
   * @brief A not implemented function to trigger compile time errors with an
   *   error message.
   */
  #if defined(__GNUC__)
    __attribute__((__error__("RTEMS_DEQUALIFY() types differ not only by volatile and const"))) void *
    RTEMS_DEQUALIFY_types_not_compatible( void );
  #else
    void *RTEMS_DEQUALIFY_types_not_compatible( void );
  #endif
#endif

/* Generated from spec:/rtems/basedefs/if/dequalify-depthx */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Performs a type cast which removes qualifiers without warnings to the
 *   type for the variable.
 *
 * @param _ptr_level is the pointer indirection level expressed in *.
 *
 * @param _type is the target type of the cast.
 *
 * @param _var is the variable.
 */
#if defined(__cplusplus)
  #define RTEMS_DEQUALIFY_DEPTHX( _ptr_level, _type, _var ) \
    ( const_cast<_type>( _var ) )
#elif defined(__GNUC__)
  #define RTEMS_DEQUALIFY_DEPTHX( _ptr_level, _type, _var ) \
    __builtin_choose_expr( __builtin_types_compatible_p( \
        RTEMS_TYPEOF_REFX( _ptr_level, _var ), \
        RTEMS_TYPEOF_REFX( _ptr_level, _type ) \
      ) || __builtin_types_compatible_p( _type, void * ), \
    (_type) ( _var ), \
    RTEMS_DEQUALIFY_types_not_compatible() )
#else
  #define RTEMS_DEQUALIFY_DEPTHX( _ptr_level, _type, _var ) \
    ( (_type) (uintptr_t) (const volatile void *)( _var ) )
#endif

/* Generated from spec:/rtems/basedefs/if/deconst */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Performs a type cast which removes const qualifiers without warnings
 *   to the type for the pointer variable.
 *
 * @param _type is the target type of the cast.
 *
 * @param _var is the pointer variable.
 */
#define RTEMS_DECONST( _type, _var ) RTEMS_DEQUALIFY_DEPTHX( *, _type, _var )

/* Generated from spec:/rtems/basedefs/if/dequalify */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Performs a type cast which removes all qualifiers without warnings to
 *   the type for the pointer variable.
 *
 * @param _type is the target type of the cast.
 *
 * @param _var is the pointer variable.
 */
#define RTEMS_DEQUALIFY( _type, _var ) RTEMS_DEQUALIFY_DEPTHX( *, _type, _var )

/* Generated from spec:/rtems/basedefs/if/devolatile */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Performs a type cast which removes volatile qualifiers without
 *   warnings to the type for the pointer variable.
 *
 * @param _type is the target type of the cast.
 *
 * @param _var is the pointer variable.
 */
#define RTEMS_DEVOLATILE( _type, _var ) RTEMS_DEQUALIFY_DEPTHX( *, _type, _var )

#if !defined(FALSE)
  /* Generated from spec:/rtems/basedefs/if/false */

  /**
   * @ingroup RTEMSAPIBaseDefs
   *
   * @brief If FALSE is undefined, then FALSE is defined to 0.
   */
  #define FALSE 0
#endif

/* Generated from spec:/rtems/basedefs/if/have-member-same-type */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Checks if members of two types have compatible types.
 *
 * @param _t_lhs is the left hand side type.
 *
 * @param _m_lhs is the left hand side member.
 *
 * @param _t_rhs is the right hand side type.
 *
 * @param _m_rhs is the right hand side member.
 *
 * @return Returns to true, if the members of two types have compatible types,
 *   otherwise false.
 */
#if defined(__GNUC__)
  #define RTEMS_HAVE_MEMBER_SAME_TYPE( _t_lhs, _m_lhs, _t_rhs, _m_rhs ) \
    __builtin_types_compatible_p( \
      __typeof__( ( (_t_lhs *) 0 )->_m_lhs ), \
      __typeof__( ( (_t_rhs *) 0 )->_m_rhs ) \
    )
#else
  #define RTEMS_HAVE_MEMBER_SAME_TYPE( _t_lhs, _m_lhs, _t_rhs, _m_rhs ) true
#endif

/* Generated from spec:/rtems/basedefs/if/inline-routine */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Gives a hint to the compiler in a function declaration to inline this
 *   function.
 */
#if defined(__GNUC__)
  #define RTEMS_INLINE_ROUTINE static __inline__
#else
  #define RTEMS_INLINE_ROUTINE static inline
#endif

/* Generated from spec:/rtems/basedefs/if/malloclike */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Tells the compiler in a declaration that this function is a memory
 *   allocation function similar to malloc().
 */
#if defined(__GNUC__)
  #define RTEMS_MALLOCLIKE __attribute__(( __malloc__ ))
#else
  #define RTEMS_MALLOCLIKE
#endif

/* Generated from spec:/rtems/basedefs/if/no-inline */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Instructs the compiler in a function declaration to not inline this
 *   function.
 */
#if defined(__GNUC__)
  #define RTEMS_NO_INLINE __attribute__(( __noinline__ ))
#else
  #define RTEMS_NO_INLINE
#endif

/* Generated from spec:/rtems/basedefs/if/no-return */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Tells the compiler in a function declaration that this function does
 *   not return.
 */
#if __cplusplus >= 201103L
  #define RTEMS_NO_RETURN [[noreturn]]
#elif __STDC_VERSION__ >= 201112L
  #define RTEMS_NO_RETURN _Noreturn
#elif defined(__GNUC__)
  #define RTEMS_NO_RETURN __attribute__(( __noreturn__ ))
#else
  #define RTEMS_NO_RETURN
#endif

/* Generated from spec:/rtems/basedefs/if/compiler-no-return-attribute */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Provided for backward compatibility.
 */
#define RTEMS_COMPILER_NO_RETURN_ATTRIBUTE RTEMS_NO_RETURN

/* Generated from spec:/rtems/basedefs/if/obfuscate-variable */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Obfuscates the variable so that the compiler cannot perform
 *   optimizations based on the variable value.
 *
 * @param _var is the variable to obfuscate.
 *
 * The variable must be simple enough to fit into a register.
 */
#if defined(__GNUC__)
  #define RTEMS_OBFUSCATE_VARIABLE( _var ) __asm__( "" : "+r" ( _var ) )
#else
  #define RTEMS_OBFUSCATE_VARIABLE( _var ) (void) ( _var )
#endif

/* Generated from spec:/rtems/basedefs/if/packed */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Instructs the compiler in a type definition to place members of a
 *   structure or union so that the memory required is minimized.
 */
#if defined(__GNUC__)
  #define RTEMS_PACKED __attribute__(( __packed__ ))
#else
  #define RTEMS_PACKED
#endif

/* Generated from spec:/rtems/basedefs/if/compiler-packed-attribute */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Provided for backward compatibility.
 */
#define RTEMS_COMPILER_PACKED_ATTRIBUTE RTEMS_PACKED

/* Generated from spec:/rtems/basedefs/if/predict-false */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Evaluates the integral expression and tells the compiler that the
 *   predicted value is false (0).
 *
 * @param _exp is the integral expression.
 *
 * @return Returns the value of the integral expression and tells the compiler
 *   that the predicted value is false (0).
 */
#if defined(__GNUC__)
  #define RTEMS_PREDICT_FALSE( _exp ) __builtin_expect( ( _exp ), 0 )
#else
  #define RTEMS_PREDICT_FALSE( _exp ) ( _exp )
#endif

/* Generated from spec:/rtems/basedefs/if/predict-true */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Evaluates the integral expression and tells the compiler that the
 *   predicted value is true (1).
 *
 * @param _exp is the integral expression.
 *
 * @return Returns the value of the integral expression and tells the compiler
 *   that the predicted value is true (1).
 */
#if defined(__GNUC__)
  #define RTEMS_PREDICT_TRUE( _exp ) __builtin_expect( ( _exp ), 1 )
#else
  #define RTEMS_PREDICT_TRUE( _exp ) ( _exp )
#endif

/* Generated from spec:/rtems/basedefs/if/printflike */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Tells the compiler in a declaration that this function expects
 *   printf()-like arguments.
 *
 * @param _format_pos is the position of the format parameter index (starting
 *   with one).
 *
 * @param _ap_pos is the position of the argument pointer parameter index
 *   (starting with one).
 */
#if defined(__GNUC__)
  #define RTEMS_PRINTFLIKE( _format_pos, _ap_pos ) \
    __attribute__(( __format__( __printf__, _format_pos, _ap_pos ) ))
#else
  #define RTEMS_PRINTFLIKE( _format_pos, _ap_pos )
#endif

/* Generated from spec:/rtems/basedefs/if/pure */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Tells the compiler in a function declaration that this function has
 *   no effect except the return value and that the return value depends only
 *   on the value of parameters and/or global variables.
 */
#if defined(__GNUC__)
  #define RTEMS_PURE __attribute__(( __pure__ ))
#else
  #define RTEMS_PURE
#endif

/* Generated from spec:/rtems/basedefs/if/compiler-pure-attribute */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Provided for backward compatibility.
 */
#define RTEMS_COMPILER_PURE_ATTRIBUTE RTEMS_PURE

/* Generated from spec:/rtems/basedefs/if/return-address */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Gets the return address of the current function.
 *
 * @return Returns the return address of the current function.
 */
#if defined(__GNUC__)
  #define RTEMS_RETURN_ADDRESS() __builtin_return_address( 0 )
#else
  #define RTEMS_RETURN_ADDRESS() NULL
#endif

/* Generated from spec:/rtems/basedefs/if/section */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Instructs the compiler to place the variable or function in the
 *   section.
 *
 * @param _section is the section name as a string.
 */
#if defined(__GNUC__)
  #define RTEMS_SECTION( _section ) __attribute__(( __section__( _section ) ))
#else
  #define RTEMS_SECTION( _section )
#endif

/* Generated from spec:/rtems/basedefs/if/static-analysis */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief It is defined if a static analysis run is performed.
 */
#if defined(__COVERITY__)
  #define RTEMS_STATIC_ANALYSIS
#endif

/* Generated from spec:/rtems/basedefs/if/static-assert */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Asserts at compile time that the condition is satisfied.
 *
 * @param _cond is the condition this static assertion shall satisfy.
 *
 * @param _msg is the error message in case the static assertion fails.
 */
#if __cplusplus >= 201103L
  #define RTEMS_STATIC_ASSERT( _cond, _msg ) static_assert( _cond, # _msg )
#elif __STDC_VERSION__ >= 201112L
  #define RTEMS_STATIC_ASSERT( _cond, _msg ) _Static_assert( _cond, # _msg )
#else
  #define RTEMS_STATIC_ASSERT( _cond, _msg ) \
    struct rtems_static_assert_ ## _msg \
      { int rtems_static_assert_ ## _msg : ( _cond ) ? 1 : -1; }
#endif

/* Generated from spec:/rtems/basedefs/if/symbol-name */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Maps the name to the associated symbol name.
 *
 * @param _name is the user defined name of the symbol.  The name shall be a
 *   valid designator.  On the name a macro expansion is performed.
 *
 * @return Returns the symbol name associated with the name.
 */
#if defined(__USER_LABEL_PREFIX__)
  #define RTEMS_SYMBOL_NAME( _name ) RTEMS_XCONCAT( __USER_LABEL_PREFIX__, _name )
#else
  #define RTEMS_SYMBOL_NAME( _name ) RTEMS_EXPAND( _name )
#endif

#if !defined(TRUE)
  /* Generated from spec:/rtems/basedefs/if/true */

  /**
   * @ingroup RTEMSAPIBaseDefs
   *
   * @brief If TRUE is undefined, then TRUE is defined to 1.
   */
  #define TRUE 1
#endif

/* Generated from spec:/rtems/basedefs/if/unreachable */

/**
 * @brief Tells the compiler that this program point is unreachable.
 */
#if defined(__GNUC__)
  #define RTEMS_UNREACHABLE() \
    do { \
      __builtin_unreachable(); \
      _Assert_Unreachable(); \
    } while ( 0 )
#else
  #define RTEMS_UNREACHABLE() _Assert_Unreachable()
#endif

/* Generated from spec:/rtems/basedefs/if/unused */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Tells the compiler that the variable or function is deliberately
 *   unused.
 */
#if defined(__GNUC__)
  #define RTEMS_UNUSED __attribute__(( __unused__ ))
#else
  #define RTEMS_UNUSED
#endif

/* Generated from spec:/rtems/basedefs/if/compiler-unused-attribute */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Provided for backward compatibility.
 */
#define RTEMS_COMPILER_UNUSED_ATTRIBUTE RTEMS_UNUSED

/* Generated from spec:/rtems/basedefs/if/used */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Tells the compiler that the variable or function is used.
 */
#if defined(__GNUC__)
  #define RTEMS_USED __attribute__(( __used__ ))
#else
  #define RTEMS_USED
#endif

/* Generated from spec:/rtems/basedefs/if/warn-unused-result */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Tells the compiler in a declaration that the result of this function
 *   should be used.
 */
#if defined(__GNUC__)
  #define RTEMS_WARN_UNUSED_RESULT __attribute__(( __warn_unused_result__ ))
#else
  #define RTEMS_WARN_UNUSED_RESULT
#endif

/* Generated from spec:/rtems/basedefs/if/weak */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Tells the compiler in a function definition that this function should
 *   be weak.
 *
 * Use this attribute for function definitions.  Do not use it for function
 * declarations.
 */
#if defined(__GNUC__)
  #define RTEMS_WEAK __attribute__(( __weak__ ))
#else
  #define RTEMS_WEAK
#endif

/* Generated from spec:/rtems/basedefs/if/weak-alias */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Instructs the compiler to generate a weak alias to the target
 *   function.
 *
 * @param _target is the target function name.
 */
#if defined(__GNUC__)
  #define RTEMS_WEAK_ALIAS( _target ) \
    __attribute__(( __weak__, __alias__( #_target ) ))
#else
  #define RTEMS_WEAK_ALIAS( _target )
#endif

/* Generated from spec:/rtems/basedefs/if/xstring */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Stringifies the expansion of the arguments.
 *
 * @param ... are the arguments to expand and stringify.
 *
 * @return Returns the stringification of the expansion of the arguments.
 */
#define RTEMS_XSTRING( ... ) RTEMS_STRING( __VA_ARGS__ )

/* Generated from spec:/rtems/basedefs/if/define-global-symbol */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief Defines a global symbol with the name and value.
 *
 * @param _name is the user defined name of the symbol.  The name shall be a
 *   valid designator.  On the name a macro expansion is performed and
 *   afterwards it is stringified.
 *
 * @param _value is the value of the symbol.  On the value a macro expansion is
 *   performed and afterwards it is stringified.  It shall expand to an integer
 *   expression understood by the assembler.
 *
 * This macro shall be placed at file scope.
 */
#if defined(__USER_LABEL_PREFIX__)
  #define RTEMS_DEFINE_GLOBAL_SYMBOL( _name, _value ) \
    __asm__( \
      "\t.globl " RTEMS_XSTRING( RTEMS_SYMBOL_NAME( _name ) ) \
      "\n\t.set " RTEMS_XSTRING( RTEMS_SYMBOL_NAME( _name ) ) \
      ", " RTEMS_STRING( _value ) "\n" \
    )
#else
  #define RTEMS_DEFINE_GLOBAL_SYMBOL( _name, _value )
#endif

/* Generated from spec:/rtems/basedefs/if/zero-length-array */

/**
 * @ingroup RTEMSAPIBaseDefs
 *
 * @brief This constant represents the element count of a zero-length array.
 *
 * Zero-length arrays are valid in C99 as flexible array members.  C++11 does
 * not allow flexible array members.  Use the GNU extension which is also
 * supported by other compilers.
 */
#if __STDC_VERSION__ >= 199409L
  #define RTEMS_ZERO_LENGTH_ARRAY
#else
  #define RTEMS_ZERO_LENGTH_ARRAY 0
#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_BASEDEFS_H */
