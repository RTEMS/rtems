/**
 * @file
 * 
 * @brief Atomic Operations CPU API
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

#endif /* _RTEMS_SCORE_CPUSTDATOMIC_H */
