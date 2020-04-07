/**
 * @file
 *
 * @ingroup RTEMSScore
 *
 * @brief Basic Definitions
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (C) 2010, 2019 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_BASEDEFS_H
#define _RTEMS_BASEDEFS_H

/**
 * @defgroup RTEMSScoreBaseDefs Basic Definitions
 *
 * @ingroup RTEMSScore
 *
 * @{
 */

#include <rtems/score/cpuopts.h>

#ifndef ASM
  #include <stddef.h>
  #include <stdbool.h>
  #include <stdint.h>
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
 *  writes are not reordered around certain points.  This specifically can
 *  impact interrupt disable and thread dispatching critical sections.
 */
#ifdef __GNUC__
  #define RTEMS_COMPILER_MEMORY_BARRIER() __asm__ volatile("" ::: "memory")
#else
  #define RTEMS_COMPILER_MEMORY_BARRIER()
#endif

/**
 *  The following defines a compiler specific attribute which informs
 *  the compiler that the method must not be inlined.
 */
#ifdef __GNUC__
  #define RTEMS_NO_INLINE __attribute__((__noinline__))
#else
  #define RTEMS_NO_INLINE
#endif

/**
 *  The following macro is a compiler specific way to indicate that
 *  the method will NOT return to the caller.  This can assist the
 *  compiler in code generation and avoid unreachable paths.  This
 *  can impact the code generated following calls to
 *  rtems_fatal_error_occurred and _Terminate.
 */
#if defined(RTEMS_SCHEDSIM)
  #define RTEMS_NO_RETURN
#elif defined(__GNUC__) && !defined(RTEMS_DEBUG)
  #define RTEMS_NO_RETURN __attribute__((__noreturn__))
#else
  #define RTEMS_NO_RETURN
#endif

/* Provided for backward compatibility */
#define RTEMS_COMPILER_NO_RETURN_ATTRIBUTE RTEMS_NO_RETURN

/**
 *  The following defines a compiler specific attribute which informs
 *  the compiler that the method has no effect except the return value
 *  and that the return value depends only the value of parameters.
 */
#ifdef __GNUC__
  #define RTEMS_CONST __attribute__((__const__))
#else
  #define RTEMS_CONST
#endif

/**
 *  The following defines a compiler specific attribute which informs
 *  the compiler that the method has no effect except the return value
 *  and that the return value depends only on parameters and/or global
 *  variables.
 */
#ifdef __GNUC__
  #define RTEMS_PURE __attribute__((__pure__))
#else
  #define RTEMS_PURE
#endif

/* Provided for backward compatibility */
#define RTEMS_COMPILER_PURE_ATTRIBUTE RTEMS_PURE

/**
 *  Instructs the compiler to issue a warning whenever a variable or function
 *  with this attribute will be used.
 */
#ifdef __GNUC__
  #define RTEMS_DEPRECATED __attribute__((__deprecated__))
#else
  #define RTEMS_DEPRECATED
#endif

/* Provided for backward compatibility */
#define RTEMS_COMPILER_DEPRECATED_ATTRIBUTE RTEMS_DEPRECATED

/**
 * @brief Instructs the compiler to place a specific variable or function in
 * the specified section.
 */
#if defined(__GNUC__)
  #define RTEMS_SECTION( _section ) __attribute__((__section__(_section)))
#else
  #define RTEMS_SECTION( _section )
#endif

/**
 * @brief Instructs the compiler that a specific variable or function is used.
 */
#if defined(__GNUC__)
  #define RTEMS_USED __attribute__((__used__))
#else
  #define RTEMS_USED
#endif

/**
 *  Instructs the compiler that a specific variable is deliberately unused.
 *  This can occur when reading volatile device memory or skipping arguments
 *  in a variable argument method.
 */
#if defined(__GNUC__)
  #define RTEMS_UNUSED __attribute__((__unused__))
#else
  #define RTEMS_UNUSED
#endif

/* Provided for backward compatibility */
#define RTEMS_COMPILER_UNUSED_ATTRIBUTE RTEMS_UNUSED

/**
 *  Instructs the compiler that a specific structure or union members will be
 *  placed so that the least memory is used.
 */
#if defined(__GNUC__)
  #define RTEMS_PACKED __attribute__((__packed__))
#else
  #define RTEMS_PACKED
#endif

/**
 * @brief Instructs the compiler to generate an alias to the specified target
 * function.
 */
#if defined(__GNUC__)
  #define RTEMS_ALIAS( _target ) __attribute__((__alias__(#_target)))
#else
  #define RTEMS_ALIAS( _target )
#endif

/**
 * @brief Instructs the compiler to generate a weak alias to the specified
 * target function.
 */
#if defined(__GNUC__)
  #define RTEMS_WEAK_ALIAS( _target ) __attribute__((__weak__, __alias__(#_target)))
#else
  #define RTEMS_WEAK_ALIAS( _target )
#endif

/**
 * @brief Instructs the compiler to enforce the specified alignment.
 */
#if defined(__GNUC__)
  #define RTEMS_ALIGNED( _alignment ) __attribute__((__aligned__(_alignment)))
#else
  #define RTEMS_ALIGNED( _alignment )
#endif

/* Provided for backward compatibility */
#define RTEMS_COMPILER_PACKED_ATTRIBUTE RTEMS_PACKED

#if defined(RTEMS_DEBUG) && !defined(RTEMS_SCHEDSIM)
  #define _Assert_Unreachable() _Assert( 0 )
#else
  #define _Assert_Unreachable() do { } while ( 0 )
#endif

/**
 * @brief Tells the compiler that this program point is unreachable.
 */
#if defined(__GNUC__) && !defined(RTEMS_SCHEDSIM)
  #define RTEMS_UNREACHABLE() \
    do { \
      __builtin_unreachable(); \
      _Assert_Unreachable(); \
    } while ( 0 )
#else
  #define RTEMS_UNREACHABLE() _Assert_Unreachable()
#endif

/**
 * @brief Tells the compiler that this function expects printf()-like
 * arguments.
 */
#if defined(__GNUC__)
  #define RTEMS_PRINTFLIKE( _format_pos, _ap_pos ) \
    __attribute__((__format__(__printf__, _format_pos, _ap_pos)))
#else
  #define RTEMS_PRINTFLIKE( _format_pos, _ap_pos )
#endif

/**
 * @brief Tells the compiler that this function is a memory allocation function
 * similar to malloc().
 */
#if defined(__GNUC__)
  #define RTEMS_MALLOCLIKE __attribute__((__malloc__))
#else
  #define RTEMS_MALLOCLIKE
#endif

/**
 * @brief Tells the compiler the memory allocation size parameter of this
 * function similar to malloc().
 */
#if defined(__GNUC__)
  #define RTEMS_ALLOC_SIZE( _index ) __attribute__((__alloc_size__(_index)))
#else
  #define RTEMS_ALLOC_SIZE( _index )
#endif

/**
 * @brief Tells the compiler the memory allocation item count and item size
 * parameter of this function similar to calloc().
 */
#if defined(__GNUC__)
  #define RTEMS_ALLOC_SIZE_2( _count_index, _size_index ) \
     __attribute__((__alloc_size__(_count_index, _size_index)))
#else
  #define RTEMS_ALLOC_SIZE_2( _count_index, _size_index )
#endif

/**
 * @brief Tells the compiler the memory allocation alignment parameter of this
 * function similar to aligned_alloc().
 */
#if defined(__GNUC__)
  #define RTEMS_ALLOC_ALIGN( _index ) __attribute__((__alloc_align__(_index)))
#else
  #define RTEMS_ALLOC_ALIGN( _index )
#endif

/**
 * @brief Tells the compiler that the result of this function should be used.
 */
#if defined(__GNUC__)
  #define RTEMS_WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))
#else
  #define RTEMS_WARN_UNUSED_RESULT
#endif

/**
 * @brief Obfuscates the variable so that the compiler cannot perform
 * optimizations based on the variable value.
 *
 * The variable must be simple enough to fit into a register.
 */
#if defined(__GNUC__)
  #define RTEMS_OBFUSCATE_VARIABLE( _var ) __asm__("" : "+r" (_var))
#else
  #define RTEMS_OBFUSCATE_VARIABLE( _var ) (void) (_var)
#endif

/**
 * @brief Declares a global symbol with the specified name.
 *
 * This macro must be placed at file scope.
 *
 * The name must be a valid designator.
 */
#define RTEMS_DECLARE_GLOBAL_SYMBOL( _name ) \
  extern char _name[]

/**
 * @brief Constructs a symbol name.
 *
 * @param _name The user defined name of the symbol.  The name shall be a valid
 *   designator.  On the name a macro expansion is performed.
 */
#if defined(__USER_LABEL_PREFIX__)
  #define RTEMS_SYMBOL_NAME( _name ) \
    RTEMS_XCONCAT( __USER_LABEL_PREFIX__, _name )
#else
  /* Helper to perform the macro expansion */
  #define _RTEMS_SYMBOL_NAME( _name ) _name

  #define RTEMS_SYMBOL_NAME( _name ) _RTEMS_SYMBOL_NAME( _name )
#endif

/**
 * @brief Defines a global symbol with the specified name and value.
 *
 * This macro shall be placed at file scope.
 *
 * @param _name The user defined name of the symbol.  The name shall be a valid
 *   designator.  On the name a macro expansion is performed and afterwards it
 *   is stringified.
 * @param _value The value of the symbol.  On the value a macro expansion is
 *   performed and afterwards it is stringified.  It shall expand to an integer
 *   expression understood by the assembler.
 */
#if defined(__GNUC__)
  #define RTEMS_DEFINE_GLOBAL_SYMBOL( _name, _value ) \
    __asm__( \
      "\t.globl " RTEMS_XSTRING( RTEMS_SYMBOL_NAME( _name ) ) \
      "\n\t.set " RTEMS_XSTRING( RTEMS_SYMBOL_NAME( _name ) ) \
      ", " RTEMS_STRING( _value ) "\n" \
    )
#else
  #define RTEMS_DEFINE_GLOBAL_SYMBOL( _name, _value )
#endif

/**
 * @brief Returns the value of the specified integral expression and tells the
 * compiler that the predicted value is true (1).
 *
 * @param[in] _exp The expression.
 *
 * @return The value of the expression.
 */
#if defined(__GNUC__)
  #define RTEMS_PREDICT_TRUE( _exp ) __builtin_expect( ( _exp ), 1 )
#else
  #define RTEMS_PREDICT_TRUE( _exp ) ( _exp )
#endif

/**
 * @brief Returns the value of the specified integral expression and tells the
 * compiler that the predicted value is false (0).
 *
 * @param[in] _exp The expression.
 *
 * @return The value of the expression.
 */
#if defined(__GNUC__)
  #define RTEMS_PREDICT_FALSE( _exp ) __builtin_expect( ( _exp ), 0 )
#else
  #define RTEMS_PREDICT_FALSE( _exp ) ( _exp )
#endif

/**
 * @brief Returns the return address of the current function.
 *
 * @return The return address.
 */
#if defined(__GNUC__)
  #define RTEMS_RETURN_ADDRESS() __builtin_return_address( 0 )
#else
  #define RTEMS_RETURN_ADDRESS() NULL
#endif

#if __cplusplus >= 201103L
  #define RTEMS_STATIC_ASSERT(cond, msg) \
    static_assert(cond, # msg)
#elif __STDC_VERSION__ >= 201112L
  #define RTEMS_STATIC_ASSERT(cond, msg) \
    _Static_assert(cond, # msg)
#else
  #define RTEMS_STATIC_ASSERT(cond, msg) \
    struct rtems_static_assert_ ## msg \
      { int rtems_static_assert_ ## msg : (cond) ? 1 : -1; }
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
  __typeof__(_ptr_level(union { int z; __typeof__(_ptr_type) x; }){0}.x)

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

/**
 * @brief Evaluates to true if the members of two types have the same type.
 *
 * @param[in] _t_lhs Left hand side type.
 * @param[in] _m_lhs Left hand side member.
 * @param[in] _t_rhs Right hand side type.
 * @param[in] _m_rhs Right hand side member.
 */
#ifdef __GNUC__
  #define RTEMS_HAVE_MEMBER_SAME_TYPE( _t_lhs, _m_lhs, _t_rhs, _m_rhs ) \
    __builtin_types_compatible_p( \
      __typeof__( ( (_t_lhs *) 0 )->_m_lhs ), \
      __typeof__( ( (_t_rhs *) 0 )->_m_rhs ) \
    )
#else
  #define RTEMS_HAVE_MEMBER_SAME_TYPE( _t_lhs, _m_lhs, _t_rhs, _m_rhs ) \
    true
#endif

/**
 * @brief Concatenates _x and _y without expanding.
 */
#define RTEMS_CONCAT( _x, _y ) _x##_y

/**
 * @brief Concatenates expansion of _x and expansion of _y.
 */
#define RTEMS_XCONCAT( _x, _y ) RTEMS_CONCAT( _x, _y )

/**
 * @brief Stringifies _x  without expanding.
 */
#define RTEMS_STRING( _x ) #_x

/**
 * @brief Stringifies expansion of _x.
 */
#define RTEMS_XSTRING( _x ) RTEMS_STRING( _x )

#ifndef ASM
  #ifdef RTEMS_DEPRECATED_TYPES
    typedef bool boolean RTEMS_DEPRECATED;
    typedef float single_precision RTEMS_DEPRECATED;
    typedef double double_precision RTEMS_DEPRECATED;
  #endif

  /**
   * XXX: Eventually proc_ptr needs to disappear!!!
   */
  typedef void * proc_ptr RTEMS_DEPRECATED;
#endif

/** @} */

#endif /* _RTEMS_BASEDEFS_H */
