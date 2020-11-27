/**
 * @file
 * 
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreAtomicCPU.
 */

/*
 * COPYRIGHT (c) 2013 Deng Hengyi.
 * Copyright (c) 2015 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUSTDATOMIC_H
#define _RTEMS_SCORE_CPUSTDATOMIC_H

#include <rtems/score/basedefs.h>

/**
 * @defgroup RTEMSScoreAtomicCPU C11/C++11 Atomic Operations
 *
 * @ingroup RTEMSScoreAtomic
 *
 * @brief This group contains the atomic operations implementation using
 *   functions provided by the C11/C++11.
 *
 * @{
 */

#ifdef RTEMS_SMP
  #if defined(__cplusplus) \
    && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 9))
    /*
     * The GCC 4.9 ships its own <stdatomic.h> which is not C++ compatible.  The
     * suggested solution was to include <atomic> in case C++ is used.  This works
     * at least with GCC 4.9.  See also:
     *
     * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=60932
     * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=60940
     */
    #include <atomic>
    #define _RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC
  #else
    #include <stdatomic.h>
    #define _RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC
  #endif
#else
  #include <rtems/score/isrlevel.h>
#endif

#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)

typedef std::atomic_uint CPU_atomic_Uint;

typedef std::atomic_ulong CPU_atomic_Ulong;

typedef std::atomic_uintptr_t CPU_atomic_Uintptr;

typedef std::atomic_flag CPU_atomic_Flag;

typedef std::memory_order CPU_atomic_Order;

#define CPU_ATOMIC_ORDER_RELAXED std::memory_order_relaxed

#define CPU_ATOMIC_ORDER_ACQUIRE std::memory_order_acquire

#define CPU_ATOMIC_ORDER_RELEASE std::memory_order_release

#define CPU_ATOMIC_ORDER_ACQ_REL std::memory_order_acq_rel

#define CPU_ATOMIC_ORDER_SEQ_CST std::memory_order_seq_cst

#define CPU_ATOMIC_INITIALIZER_UINT( value ) ATOMIC_VAR_INIT( value )

#define CPU_ATOMIC_INITIALIZER_ULONG( value ) ATOMIC_VAR_INIT( value )

#define CPU_ATOMIC_INITIALIZER_UINTPTR( value ) ATOMIC_VAR_INIT( value )

#define CPU_ATOMIC_INITIALIZER_FLAG ATOMIC_FLAG_INIT

#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)

typedef atomic_uint CPU_atomic_Uint;

typedef atomic_ulong CPU_atomic_Ulong;

typedef atomic_uintptr_t CPU_atomic_Uintptr;

typedef atomic_flag CPU_atomic_Flag;

typedef memory_order CPU_atomic_Order;

#define CPU_ATOMIC_ORDER_RELAXED memory_order_relaxed

#define CPU_ATOMIC_ORDER_ACQUIRE memory_order_acquire

#define CPU_ATOMIC_ORDER_RELEASE memory_order_release

#define CPU_ATOMIC_ORDER_ACQ_REL memory_order_acq_rel

#define CPU_ATOMIC_ORDER_SEQ_CST memory_order_seq_cst

#define CPU_ATOMIC_INITIALIZER_UINT( value ) ATOMIC_VAR_INIT( value )

#define CPU_ATOMIC_INITIALIZER_ULONG( value ) ATOMIC_VAR_INIT( value )

#define CPU_ATOMIC_INITIALIZER_UINTPTR( value ) ATOMIC_VAR_INIT( value )

#define CPU_ATOMIC_INITIALIZER_FLAG ATOMIC_FLAG_INIT

#else

typedef unsigned int CPU_atomic_Uint;

typedef unsigned long CPU_atomic_Ulong;

typedef uintptr_t CPU_atomic_Uintptr;

typedef bool CPU_atomic_Flag;

typedef int CPU_atomic_Order;

#define CPU_ATOMIC_ORDER_RELAXED 0

#define CPU_ATOMIC_ORDER_ACQUIRE 2

#define CPU_ATOMIC_ORDER_RELEASE 3

#define CPU_ATOMIC_ORDER_ACQ_REL 4

#define CPU_ATOMIC_ORDER_SEQ_CST 5

#define CPU_ATOMIC_INITIALIZER_UINT( value ) ( value )

#define CPU_ATOMIC_INITIALIZER_ULONG( value ) ( value )

#define CPU_ATOMIC_INITIALIZER_UINTPTR( value ) ( value )

#define CPU_ATOMIC_INITIALIZER_FLAG false

#endif

/**
 * @brief Sets up a cpu fence.
 *
 * @param[out] order The order for the fence.
 */
static inline void _CPU_atomic_Fence( CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  std::atomic_thread_fence( order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  atomic_thread_fence( order );
#else
  (void) order;
  RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

/**
 * @brief Initializes Uint.
 *
 * @param[out] obj The CPU atomic Uint to initialize.
 * @param desired The desired value for @a obj.
 */
static inline void _CPU_atomic_Init_uint( CPU_atomic_Uint *obj, unsigned int desired )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  obj->store( desired );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  atomic_init( obj, desired );
#else
  *obj = desired;
#endif
}

/**
 * @brief Initializes Ulong.
 *
 * @param[out] obj The CPU atomic Ulong to initialize.
 * @param desired The desired value for @a obj.
 */
static inline void _CPU_atomic_Init_ulong( CPU_atomic_Ulong *obj, unsigned long desired )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  obj->store( desired );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  atomic_init( obj, desired );
#else
  *obj = desired;
#endif
}

/**
 * @brief Initializes Uintptr.
 *
 * @param[out] obj The CPU atomic Uintptr to initialize.
 * @param desired The desired value for @a obj.
 */
static inline void _CPU_atomic_Init_uintptr( CPU_atomic_Uintptr *obj, uintptr_t desired )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  obj->store( desired );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  atomic_init( obj, desired );
#else
  *obj = desired;
#endif
}

/**
 * @brief Loads value of Uint considering the order.
 *
 * @param obj The CPU atomic Uint to get the value from.
 * @param order The atomic order for getting the value.
 *
 * @return The value of @a obj considering the @a order.
 */
static inline unsigned int _CPU_atomic_Load_uint( const CPU_atomic_Uint *obj, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->load( order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_load_explicit( obj, order );
#else
  unsigned int val;

  (void) order;
  val = *obj;
  RTEMS_COMPILER_MEMORY_BARRIER();

  return val;
#endif
}

/**
 * @brief Loads value of Ulong considering the order.
 *
 * @param obj The CPU atomic Ulong to get the value from.
 * @param order The atomic order for getting the value.
 *
 * @return The value of @a obj considering the @a order.
 */
static inline unsigned long _CPU_atomic_Load_ulong( const CPU_atomic_Ulong *obj, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->load( order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_load_explicit( obj, order );
#else
  unsigned long val;

  (void) order;
  val = *obj;
  RTEMS_COMPILER_MEMORY_BARRIER();

  return val;
#endif
}

/**
 * @brief Loads value of Uintptr considering the order.
 *
 * @param obj The CPU atomic Uintptr to get the value from.
 * @param order The atomic order for getting the value.
 *
 * @return The value of @a obj considering the @a order.
 */
static inline uintptr_t _CPU_atomic_Load_uintptr( const CPU_atomic_Uintptr *obj, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->load( order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_load_explicit( obj, order );
#else
  uintptr_t val;

  (void) order;
  val = *obj;
  RTEMS_COMPILER_MEMORY_BARRIER();

  return val;
#endif
}

/**
 * @brief Stores a value to Uint considering the order.
 *
 * @param[out] obj The CPU atomic Uint to store a value in.
 * @param desired The desired value for @a obj.
 * @param order The atomic order for storing the value.
 */
static inline void _CPU_atomic_Store_uint( CPU_atomic_Uint *obj, unsigned int desired, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  obj->store( desired );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  atomic_store_explicit( obj, desired, order );
#else
  (void) order;
  RTEMS_COMPILER_MEMORY_BARRIER();
  *obj = desired;
#endif
}

/**
 * @brief Stores a value to Ulong considering the order.
 *
 * @param[out] obj The CPU atomic Ulong to store a value in.
 * @param desired The desired value for @a obj.
 * @param order The atomic order for storing the value.
 */
static inline void _CPU_atomic_Store_ulong( CPU_atomic_Ulong *obj, unsigned long desired, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  obj->store( desired );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  atomic_store_explicit( obj, desired, order );
#else
  (void) order;
  RTEMS_COMPILER_MEMORY_BARRIER();
  *obj = desired;
#endif
}

/**
 * @brief Stores a value to Uintptr considering the order.
 *
 * @param[out] obj The CPU atomic Uintptr to store a value in.
 * @param desired The desired value for @a obj.
 * @param order The atomic order for storing the value.
 */
static inline void _CPU_atomic_Store_uintptr( CPU_atomic_Uintptr *obj, uintptr_t desired, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  obj->store( desired );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  atomic_store_explicit( obj, desired, order );
#else
  (void) order;
  RTEMS_COMPILER_MEMORY_BARRIER();
  *obj = desired;
#endif
}

/**
 * @brief Fetches current value of Uint and adds a value to the stored value.
 *
 * @param[in, out] obj The CPU atomic Uint to get the value from and add @a arg to.
 * @param arg The value to add to @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the addition of @a arg.
 */
static inline unsigned int _CPU_atomic_Fetch_add_uint( CPU_atomic_Uint *obj, unsigned int arg, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->fetch_add( arg, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_fetch_add_explicit( obj, arg, order );
#else
  unsigned int val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val + arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Ulong and adds a value to the stored value.
 *
 * @param[in, out] obj The CPU atomic Ulong to get the value from and add @a arg to.
 * @param arg The value to add to @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the addition of @a arg.
 */
static inline unsigned long _CPU_atomic_Fetch_add_ulong( CPU_atomic_Ulong *obj, unsigned long arg, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->fetch_add( arg, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_fetch_add_explicit( obj, arg, order );
#else
  unsigned long val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val + arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uintptr and adds a value to the stored value.
 *
 * @param[in, out] obj The CPU atomic Uintptr to get the value from and add @a arg to.
 * @param arg The value to add to @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the addition of @a arg.
 */
static inline uintptr_t _CPU_atomic_Fetch_add_uintptr( CPU_atomic_Uintptr *obj, uintptr_t arg, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->fetch_add( arg, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_fetch_add_explicit( obj, arg, order );
#else
  uintptr_t val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val + arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uint and subtracts a value from the stored value.
 *
 * @param[in, out] obj The CPU atomic Uint to get the value from and subtract @a arg from.
 * @param arg The value to subtract from @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the subtraction of @a arg.
 */
static inline unsigned int _CPU_atomic_Fetch_sub_uint( CPU_atomic_Uint *obj, unsigned int arg, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->fetch_sub( arg, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_fetch_sub_explicit( obj, arg, order );
#else
  unsigned int val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val - arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Ulong and subtracts a value from the stored value.
 *
 * @param[in, out] obj The CPU atomic Ulong to get the value from and subtract @a arg from.
 * @param arg The value to subtract from @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the subtraction of @a arg.
 */
static inline unsigned long _CPU_atomic_Fetch_sub_ulong( CPU_atomic_Ulong *obj, unsigned long arg, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->fetch_sub( arg, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_fetch_sub_explicit( obj, arg, order );
#else
  unsigned long val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val - arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uintptr and subtracts a value from the stored value.
 *
 * @param[in, out] obj The CPU atomic Uintptr to get the value from and subtract @a arg from.
 * @param arg The value to subtract from @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the subtraction of @a arg.
 */
static inline uintptr_t _CPU_atomic_Fetch_sub_uintptr( CPU_atomic_Uintptr *obj, uintptr_t arg, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->fetch_sub( arg, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_fetch_sub_explicit( obj, arg, order );
#else
  uintptr_t val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val - arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uint and ORs a value with the stored value.
 *
 * @param[in, out] obj The CPU atomic Uint to get the value from and OR @a arg to.
 * @param arg The value to OR with @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the OR operation with @a arg.
 */
static inline unsigned int _CPU_atomic_Fetch_or_uint( CPU_atomic_Uint *obj, unsigned int arg, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->fetch_or( arg, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_fetch_or_explicit( obj, arg, order );
#else
  unsigned int val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val | arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Ulong and ORs a value with the stored value.
 *
 * @param[in, out] obj The CPU atomic Ulong to get the value from and OR @a arg to.
 * @param arg The value to OR with @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the OR operation with @a arg.
 */
static inline unsigned long _CPU_atomic_Fetch_or_ulong( CPU_atomic_Ulong *obj, unsigned long arg, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->fetch_or( arg, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_fetch_or_explicit( obj, arg, order );
#else
  unsigned long val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val | arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uintptr and ORs a value with the stored value.
 *
 * @param[in, out] obj The CPU atomic Uintptr to get the value from and OR @a arg to.
 * @param arg The value to OR with @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the OR operation with @a arg.
 */
static inline uintptr_t _CPU_atomic_Fetch_or_uintptr( CPU_atomic_Uintptr *obj, uintptr_t arg, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->fetch_or( arg, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_fetch_or_explicit( obj, arg, order );
#else
  uintptr_t val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val | arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uint and ANDs a value with the stored value.
 *
 * @param[in, out] obj The CPU atomic Uint to get the value from and AND @a arg to.
 * @param arg The value to AND with @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the AND operation with @a arg.
 */
static inline unsigned int _CPU_atomic_Fetch_and_uint( CPU_atomic_Uint *obj, unsigned int arg, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->fetch_and( arg, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_fetch_and_explicit( obj, arg, order );
#else
  unsigned int val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val & arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Ulong and ANDs a value with the stored value.
 *
 * @param[in, out] obj The CPU atomic Ulong to get the value from and AND @a arg to.
 * @param arg The value to AND with @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the AND operation with @a arg.
 */
static inline unsigned long _CPU_atomic_Fetch_and_ulong( CPU_atomic_Ulong *obj, unsigned long arg, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->fetch_and( arg, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_fetch_and_explicit( obj, arg, order );
#else
  unsigned long val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val & arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uintptr and ANDs a value with the stored value.
 *
 * @param[in, out] obj The CPU atomic Uintptr to get the value from and AND @a arg to.
 * @param arg The value to AND with @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the AND operation with @a arg.
 */
static inline uintptr_t _CPU_atomic_Fetch_and_uintptr( CPU_atomic_Uintptr *obj, uintptr_t arg, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->fetch_and( arg, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_fetch_and_explicit( obj, arg, order );
#else
  uintptr_t val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = val & arg;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uint and sets its value.
 *
 * @param[in, out] obj The CPU atomic Uint to get the value from and set the value to @a desired.
 * @param arg The value to set for @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the exchange with @a desired.
 */
static inline unsigned int _CPU_atomic_Exchange_uint( CPU_atomic_Uint *obj, unsigned int desired, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->exchange( desired, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_exchange_explicit( obj, desired, order );
#else
  unsigned int val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = desired;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Ulong and sets its value.
 *
 * @param[in, out] obj The CPU atomic Ulong to get the value from and set the value to @a desired.
 * @param arg The value to set for @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the exchange with @a desired.
 */
static inline unsigned long _CPU_atomic_Exchange_ulong( CPU_atomic_Ulong *obj, unsigned long desired, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->exchange( desired, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_exchange_explicit( obj, desired, order );
#else
  unsigned long val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = desired;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Fetches current value of Uintptr and sets its value.
 *
 * @param[in, out] obj The CPU atomic Uintptr to get the value from and set the value to @a desired.
 * @param arg The value to set for @a obj.
 * @param order The atomic order for the operation.
 *
 * @return The value of @a obj prior to the exchange with @a desired.
 */
static inline uintptr_t _CPU_atomic_Exchange_uintptr( CPU_atomic_Uintptr *obj, uintptr_t desired, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->exchange( desired, order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_exchange_explicit( obj, desired, order );
#else
  uintptr_t val;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  val = *obj;
  *obj = desired;
  _ISR_Local_enable( level );

  return val;
#endif
}

/**
 * @brief Checks if value of Uint is as expected.
 *
 * This method checks if the value of @a obj is equal to the value of @a expected.  If
 * this is the case, the value of @a obj is changed to @a desired.  Otherwise, the value
 * of @a obj is changed to @a expected.
 *
 * @param[in, out] obj The CPU atomic Uint to operate upon.
 * @param[in, out] expected The expected value of @a obj.  If @a obj has a different
 *      value, @a expected is changed to the actual value of @a obj.
 * @param desired The new value of @a obj if the old value of @a obj was as expected.
 * @param succ The order if it is successful.
 * @param fail The order if it fails.
 *
 * @retval true The old value of @a obj was as expected.
 * @retval false The old value of @a obj was not as expected.
 */
static inline bool _CPU_atomic_Compare_exchange_uint( CPU_atomic_Uint *obj, unsigned int *expected, unsigned int desired, CPU_atomic_Order succ, CPU_atomic_Order fail )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->compare_exchange_strong( *expected, desired, succ, fail );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_compare_exchange_strong_explicit( obj, expected, desired, succ, fail );
#else
  bool success;
  ISR_Level level;
  unsigned int actual;

  (void) succ;
  (void) fail;
  _ISR_Local_disable( level );
  actual = *obj;
  success = ( actual == *expected );
  if ( success ) {
    *obj = desired;
  } else {
    *expected = actual;
  }
  _ISR_Local_enable( level );

  return success;
#endif
}

/**
 * @brief Checks if value of Ulong is as expected.
 *
 * This method checks if the value of @a obj is equal to the value of @a expected.  If
 * this is the case, the value of @a obj is changed to @a desired.  Otherwise, the value
 * of @a obj is changed to @a expected.
 *
 * @param[in, out] obj The CPU atomic Ulong to operate upon.
 * @param[in, out] expected The expected value of @a obj.  If @a obj has a different
 *      value, @a expected is changed to the actual value of @a obj.
 * @param desired The new value of @a obj if the old value of @a obj was as expected.
 * @param succ The order if it is successful.
 * @param fail The order if it fails.
 *
 * @retval true The old value of @a obj was as expected.
 * @retval false The old value of @a obj was not as expected.
 */
static inline bool _CPU_atomic_Compare_exchange_ulong( CPU_atomic_Ulong *obj, unsigned long *expected, unsigned long desired, CPU_atomic_Order succ, CPU_atomic_Order fail )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->compare_exchange_strong( *expected, desired, succ, fail );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_compare_exchange_strong_explicit( obj, expected, desired, succ, fail );
#else
  bool success;
  ISR_Level level;
  unsigned long actual;

  (void) succ;
  (void) fail;
  _ISR_Local_disable( level );
  actual = *obj;
  success = ( actual == *expected );
  if ( success ) {
    *obj = desired;
  } else {
    *expected = actual;
  }
  _ISR_Local_enable( level );

  return success;
#endif
}

/**
 * @brief Checks if value of Uintptr is as expected.
 *
 * This method checks if the value of @a obj is equal to the value of @a expected.  If
 * this is the case, the value of @a obj is changed to @a desired.  Otherwise, the value
 * of @a obj is changed to @a expected.
 *
 * @param[in, out] obj The CPU atomic Uintptr to operate upon.
 * @param[in, out] expected The expected value of @a obj.  If @a obj has a different
 *      value, @a expected is changed to the actual value of @a obj.
 * @param desired The new value of @a obj if the old value of @a obj was as expected.
 * @param succ The order if it is successful.
 * @param fail The order if it fails.
 *
 * @retval true The old value of @a obj was as expected.
 * @retval false The old value of @a obj was not as expected.
 */
static inline bool _CPU_atomic_Compare_exchange_uintptr( CPU_atomic_Uintptr *obj, uintptr_t *expected, uintptr_t desired, CPU_atomic_Order succ, CPU_atomic_Order fail )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->compare_exchange_strong( *expected, desired, succ, fail );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_compare_exchange_strong_explicit( obj, expected, desired, succ, fail );
#else
  bool success;
  ISR_Level level;
  uintptr_t actual;

  (void) succ;
  (void) fail;
  _ISR_Local_disable( level );
  actual = *obj;
  success = ( actual == *expected );
  if ( success ) {
    *obj = desired;
  } else {
    *expected = actual;
  }
  _ISR_Local_enable( level );

  return success;
#endif
}

/**
 * @brief Clears the atomic flag.
 *
 * @param[out] obj The atomic flag to be cleared.
 * @param order The atomic order for the operation.
 */
static inline void _CPU_atomic_Flag_clear( CPU_atomic_Flag *obj, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  obj->clear( order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  atomic_flag_clear_explicit( obj, order );
#else
  (void) order;
  *obj = false;
#endif
}

/**
 * @brief Returns current flag state and sets it.
 *
 * @param[in, out] obj The atomic flag to be set.
 * @param order The atomic order for the operation.
 *
 * @retval true @a obj was set prior to this operation.
 * @retval false @a obj was not set prior to this operation.
 */
static inline bool _CPU_atomic_Flag_test_and_set( CPU_atomic_Flag *obj, CPU_atomic_Order order )
{
#if defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_ATOMIC)
  return obj->test_and_set( order );
#elif defined(_RTEMS_SCORE_CPUSTDATOMIC_USE_STDATOMIC)
  return atomic_flag_test_and_set_explicit( obj, order );
#else
  bool flag;
  ISR_Level level;

  (void) order;
  _ISR_Local_disable( level );
  flag = *obj;
  *obj = true;
  _ISR_Local_enable( level );

  return flag;
#endif
}

/** @} */

#endif /* _RTEMS_SCORE_CPUSTDATOMIC_H */
