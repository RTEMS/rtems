/**
 * @file
 *
 * @ingroup Score
 *
 * @brief Basic Definitions
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2010 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_BASEDEFS_H
#define _RTEMS_BASEDEFS_H

/**
 *  @defgroup ScoreBaseDefs Basic Definitions
 *
 *  @ingroup Score
 */
/**@{*/

#include <rtems/score/cpuopts.h>

#ifndef ASM
  #include <stddef.h>
  #include <stdbool.h>
  #include <stdint.h>

  /*
   * FIXME: This include should not be present.  In older RTEMS versions
   * <rtems.h> provided <limits.h> indirectly.  This include is here to not
   * break application source files that relied on this accidentally.
   */
  #include <limits.h>

  /*
   * FIXME: This include should not be present.  In older RTEMS versions
   * <rtems.h> provided <string.h> indirectly.  This include is here to not
   * break application source files that relied on this accidentally.
   */
  #include <string.h>
#endif

#ifndef TRUE
  /**
   *  This ensures that RTEMS has TRUE defined in all situations.
   */
  #define TRUE 1
#endif

#ifndef FALSE
  /**
   *  This ensures that RTEMS has FALSE defined in all situations.
   */
  #define FALSE 0
#endif

#if TRUE == FALSE
  #error "TRUE equals FALSE"
#endif

/**
 *  The following ensures that all data is declared in the space
 *  of the initialization routine for either the Initialization Manager
 *  or the initialization file for the appropriate API.  It is
 *  referenced as "external" in every other file.
 */
#ifdef SCORE_INIT
  #undef  SCORE_EXTERN
  #define SCORE_EXTERN
#else
  #undef  SCORE_EXTERN
  #define SCORE_EXTERN  extern
#endif

/**
 *  The following ensures that all data is declared in the space
 *  of the initialization routine for either the Initialization Manager
 *  or the initialization file for the appropriate API.  It is
 *  referenced as "external" in every other file.
 */
#ifdef SAPI_INIT
  #undef  SAPI_EXTERN
  #define SAPI_EXTERN
#else
  #undef  SAPI_EXTERN
  #define SAPI_EXTERN  extern
#endif

/**
 *  The following ensures that all data is declared in the space
 *  of the initialization routine for either the Initialization Manager
 *  or the initialization file for the appropriate API.  It is
 *  referenced as "external" in every other file.
 */
#ifdef RTEMS_API_INIT
  #undef  RTEMS_EXTERN
  #define RTEMS_EXTERN
#else
  #undef  RTEMS_EXTERN
  #define RTEMS_EXTERN  extern
#endif

/**
 *  The following ensures that all data is declared in the space
 *  of the initialization routine for either the Initialization Manager
 *  or the initialization file for the appropriate API.  It is
 *  referenced as "external" in every other file.
 */
#ifdef POSIX_API_INIT
  #undef  POSIX_EXTERN
  #define POSIX_EXTERN
#else
  #undef  POSIX_EXTERN
  #define POSIX_EXTERN  extern
#endif

/**
 *  The following (in conjunction with compiler arguments) are used
 *  to choose between the use of static inline functions and macro
 *  functions.   The static inline implementation allows better
 *  type checking with no cost in code size or execution speed.
 */
#ifdef __GNUC__
  #define RTEMS_INLINE_ROUTINE static __inline__
#else
  #define RTEMS_INLINE_ROUTINE static inline
#endif

/**
 *  The following macro is a compiler specific way to ensure that memory
 *  writes are not reordered around certian points.  This specifically can
 *  impact interrupt disable and thread dispatching critical sections.
 */
#ifdef __GNUC__
  #define RTEMS_COMPILER_MEMORY_BARRIER() __asm__ volatile("" ::: "memory")
#else
  #define RTEMS_COMPILER_MEMORY_BARRIER()
#endif

/**
 *  The following macro is a compiler specific way to indicate that
 *  the method will NOT return to the caller.  This can assist the
 *  compiler in code generation and avoid unreachable paths.  This
 *  can impact the code generated following calls to
 *  rtems_fatal_error_occurred and _Terminate.
 */
#if defined(RTEMS_SCHEDSIM)
  #define RTEMS_COMPILER_NO_RETURN_ATTRIBUTE
#elif defined(__GNUC__)
  #define RTEMS_COMPILER_NO_RETURN_ATTRIBUTE \
      __attribute__ ((noreturn))
#else
  #define RTEMS_COMPILER_NO_RETURN_ATTRIBUTE
#endif

/**
 *  The following defines a compiler specific attribute which informs
 *  the compiler that the method has no effect except the return value
 *  and that the return value depends only on parameters and/or global
 *  variables.
 */
#ifdef __GNUC__
  #define RTEMS_COMPILER_PURE_ATTRIBUTE \
     __attribute__ ((pure))
#else
  #define RTEMS_COMPILER_PURE_ATTRIBUTE
#endif

/**
 *  Instructs the compiler to issue a warning whenever a variable or function
 *  with this attribute will be used.
 */
#ifdef __GNUC__
  #define RTEMS_COMPILER_DEPRECATED_ATTRIBUTE \
     __attribute__ ((deprecated))
#else
  #define RTEMS_COMPILER_DEPRECATED_ATTRIBUTE
#endif

/**
 *  Instructs the compiler that a specific variable is deliberately unused.
 *  This can occur when reading volatile device memory or skipping arguments
 *  in a variable argument method.
 */
#if defined(__GNUC__)
  #define RTEMS_COMPILER_UNUSED_ATTRIBUTE __attribute__((unused))
#else
  #define RTEMS_COMPILER_UNUSED_ATTRIBUTE
#endif

/**
 *  Instructs the compiler that a specific structure or union members will be
 *  placed so that the least memory is used.
 */
#if defined(__GNUC__)
  #define RTEMS_COMPILER_PACKED_ATTRIBUTE __attribute__((packed))
#else
  #define RTEMS_COMPILER_PACKED_ATTRIBUTE
#endif

#if __cplusplus >= 201103L
  #define RTEMS_STATIC_ASSERT(cond, msg) \
    static_assert(cond, # msg)
#elif __STDC_VERSION__ >= 201112L
  #define RTEMS_STATIC_ASSERT(cond, msg) \
    _Static_assert(cond, # msg)
#else
  #define RTEMS_STATIC_ASSERT(cond, msg) \
    typedef int rtems_static_assert_ ## msg [(cond) ? 1 : -1]
#endif

#define RTEMS_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/*
 * Zero-length arrays are valid in C99 as flexible array members.  C++11
 * doesn't allow flexible array members.  Use the GNU extension which is also
 * supported by other compilers.
 */
#define RTEMS_ZERO_LENGTH_ARRAY 0

/**
 * @brief Returns a pointer to the container of a specified member pointer.
 *
 * @param[in] _m The pointer to a member of the container.
 * @param[in] _type The type of the container.
 * @param[in] _member_name The designator name of the container member.
 */
#define RTEMS_CONTAINER_OF( _m, _type, _member_name ) \
  ( (_type *) ( (uintptr_t) ( _m ) - offsetof( _type, _member_name ) ) )

#ifdef __cplusplus
#define RTEMS_DEQUALIFY_DEPTHX( _ptr_level, _type, _var ) \
            (const_cast<_type>( _var ))
#else /* Standard C code */

/* The reference type idea based on libHX by Jan Engelhardt */
#define RTEMS_TYPEOF_REFX(_ptr_level, _ptr_type) \
  typeof(_ptr_level(union { int z; typeof(_ptr_type) x; }){0}.x)

#if defined(__GNUC__) && !defined(ASM)
#if  ((__GNUC__ * 1000 + __GNUC_MINOR__) >= 4004)
extern void* RTEMS_DEQUALIFY_types_not_compatible(void)
  __attribute__((error ("RTEMS_DEQUALIFY types differ not only by volatile and const")));
#else
extern void RTEMS_DEQUALIFY_types_not_compatible(void);
#endif
#define RTEMS_DEQUALIFY_DEPTHX( _ptr_level, _type, _var ) ( \
    __builtin_choose_expr( __builtin_types_compatible_p ( \
        RTEMS_TYPEOF_REFX( _ptr_level, _var ), \
        RTEMS_TYPEOF_REFX( _ptr_level, _type ) \
      ) || __builtin_types_compatible_p ( _type, void * ), \
    (_type)(_var), \
    RTEMS_DEQUALIFY_types_not_compatible() \
  ) \
)
#endif /*__GNUC__*/
#endif /*__cplusplus*/

#ifndef RTEMS_DECONST
#ifdef RTEMS_DEQUALIFY_DEPTHX
#define RTEMS_DECONST( _type, _var ) \
  RTEMS_DEQUALIFY_DEPTHX( *, _type, _var )
#else /*RTEMS_DEQUALIFY_DEPTHX*/
/**
 * @brief Removes the const qualifier from a type of a variable.
 *
 * @param[in] _type The target type for the variable.
 * @param[in] _var The variable.
 */
#define RTEMS_DECONST( _type, _var ) \
  ((_type)(uintptr_t)(const void *) ( _var ))

#endif /*RTEMS_DEQUALIFY_DEPTHX*/
#endif /*RTEMS_DECONST*/

#ifndef RTEMS_DEVOLATILE
#ifdef RTEMS_DEQUALIFY_DEPTHX
#define RTEMS_DEVOLATILE( _type, _var ) \
  RTEMS_DEQUALIFY_DEPTHX( *, _type, _var )
#else /*RTEMS_DEQUALIFY_DEPTHX*/
/**
 * @brief Removes the volatile qualifier from a type of a variable.
 *
 * @param[in] _type The target type for the variable.
 * @param[in] _var The variable.
 */
#define RTEMS_DEVOLATILE( _type, _var ) \
  ((_type)(uintptr_t)(volatile void *) ( _var ))

#endif /*RTEMS_DEQUALIFY_DEPTHX*/
#endif /*RTEMS_DEVOLATILE*/

#ifndef RTEMS_DEQUALIFY
#ifdef RTEMS_DEQUALIFY_DEPTHX
#define RTEMS_DEQUALIFY( _type, _var ) \
  RTEMS_DEQUALIFY_DEPTHX( *, _type, _var )
#else /*RTEMS_DEQUALIFY_DEPTHX*/
/**
 * @brief Removes the all qualifiers from a type of a variable.
 *
 * @param[in] _type The target type for the variable.
 * @param[in] _var The variable.
 */
#define RTEMS_DEQUALIFY( _type, _var ) \
  ((_type)(uintptr_t)(const volatile void *) ( _var ))

#endif /*RTEMS_DEQUALIFY_DEPTHX*/
#endif /*RTEMS_DEQUALIFY*/

#ifndef ASM
  #ifdef RTEMS_DEPRECATED_TYPES
    typedef bool boolean;
    typedef float single_precision;
    typedef double double_precision;
  #endif

  /**
   * XXX: Eventually proc_ptr needs to disappear!!!
   */
  typedef void * proc_ptr;
#endif

/**@}*/

#endif /* _RTEMS_BASEDEFS_H */
