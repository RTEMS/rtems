/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief This header file provides interfaces of the Interrupt Manager
 *   implementation.
 */

/*
 * Copyright (C) 2016 Chris Johns <chrisj@rtems.org>
 *
 * Copyright (C) 2008, 2021 embedded brains GmbH & Co. KG
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
 * The API is based on concepts of Pavel Pisa, Till Straumann and Eric Valette.
 */

#ifndef LIBBSP_SHARED_IRQ_GENERIC_H
#define LIBBSP_SHARED_IRQ_GENERIC_H

#include <stdbool.h>

#include <rtems/irq-extension.h>
#include <rtems/score/assert.h>

#ifdef RTEMS_SMP
  #include <rtems/score/atomic.h>
#endif

#include <bsp/irq.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if !defined(BSP_INTERRUPT_VECTOR_COUNT)
  #error "BSP_INTERRUPT_VECTOR_COUNT shall be defined"
#endif

#if defined(BSP_INTERRUPT_USE_INDEX_TABLE) && !defined(BSP_INTERRUPT_DISPATCH_TABLE_SIZE)
  #error "if you define BSP_INTERRUPT_USE_INDEX_TABLE, you have to define BSP_INTERRUPT_DISPATCH_TABLE_SIZE etc. as well"
#endif

#ifndef BSP_INTERRUPT_DISPATCH_TABLE_SIZE
  #define BSP_INTERRUPT_DISPATCH_TABLE_SIZE BSP_INTERRUPT_VECTOR_COUNT
#endif

#define bsp_interrupt_assert(e) _Assert(e)

/**
 * @brief Each member of this table references the first installed entry at the
 *   corresponding interrupt vector or is NULL.
 */
extern rtems_interrupt_entry *bsp_interrupt_dispatch_table[];

#ifdef BSP_INTERRUPT_USE_INDEX_TABLE
  #if BSP_INTERRUPT_DISPATCH_TABLE_SIZE < 0x100
    typedef uint8_t bsp_interrupt_dispatch_index_type;
  #elif BSP_INTERRUPT_DISPATCH_TABLE_SIZE < 0x10000
    typedef uint16_t bsp_interrupt_dispatch_index_type;
  #else
    typedef uint32_t bsp_interrupt_dispatch_index_type;
  #endif
  extern bsp_interrupt_dispatch_index_type bsp_interrupt_dispatch_index_table [];
#endif

static inline rtems_vector_number bsp_interrupt_dispatch_index(
  rtems_vector_number vector
)
{
  #ifdef BSP_INTERRUPT_USE_INDEX_TABLE
    return bsp_interrupt_dispatch_index_table [vector];
  #else
    return vector;
  #endif
}

/**
 * @defgroup RTEMSImplClassicIntr Interrupt Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Interrupt Manager implementation.
 *
 * The Interrupt Manager implementation manages a sequence of interrupt vector
 * numbers greater than or equal to zero and less than
 * ``BSP_INTERRUPT_VECTOR_COUNT``.  It provides methods to install, remove, and
 * dispatch interrupt entries for each vector number, see
 * bsp_interrupt_dispatch_entries().
 *
 * The entry points to a list of interrupt entries are stored in a table
 * (= dispatch table).
 *
 * You have to configure the Interrupt Manager implementation in the <bsp/irq.h> file
 * for each BSP.  For a minimum configuration you have to provide
 * ``BSP_INTERRUPT_VECTOR_COUNT``.
 *
 * For boards with small memory requirements you can define
 * ``BSP_INTERRUPT_USE_INDEX_TABLE``.  With an enabled index table the
 * dispatch table will be accessed via a small index table.  You can define the
 * size of the dispatch table with ``BSP_INTERRUPT_DISPATCH_TABLE_SIZE``.
 *
 * You have to provide some special routines in your BSP (follow the links for
 * the details):
 * - bsp_interrupt_facility_initialize()
 * - bsp_interrupt_vector_enable()
 * - bsp_interrupt_vector_disable()
 * - bsp_interrupt_handler_default()
 *
 * Optionally, the BSP may define the following macros to customize the vector
 * installation after installing the first entry and the vector removal before
 * removing the last entry:
 * - bsp_interrupt_vector_install()
 * - bsp_interrupt_vector_remove()
 *
 * The following now deprecated functions are provided for backward
 * compatibility:
 * - BSP_get_current_rtems_irq_handler()
 * - BSP_install_rtems_irq_handler()
 * - BSP_install_rtems_shared_irq_handler()
 * - BSP_remove_rtems_irq_handler()
 * - BSP_rtems_irq_mngt_set()
 * - BSP_rtems_irq_mngt_get()
 *
 * @{
 */

#ifdef BSP_INTERRUPT_CUSTOM_VALID_VECTOR
  bool bsp_interrupt_is_valid_vector(rtems_vector_number vector);
#else
  /**
   * @brief Returns true if the interrupt vector with number @a vector is
   * valid.
   */
  static inline bool bsp_interrupt_is_valid_vector(rtems_vector_number vector)
  {
    return vector < (rtems_vector_number) BSP_INTERRUPT_VECTOR_COUNT;
  }
#endif

/**
 * @brief Default interrupt handler.
 *
 * This routine will be called from bsp_interrupt_handler_dispatch() with the
 * current vector number @a vector when the handler list for this vector is
 * empty or the vector number is out of range.
 *
 * @note This function must cope with arbitrary vector numbers @a vector.
 */
void bsp_interrupt_handler_default(rtems_vector_number vector);

/**
 * @brief Initialize Interrupt Manager implementation.
 *
 * You must call this function before you can install, remove and dispatch
 * interrupt entries.  There is no protection against concurrent
 * initialization.  This function must be called at most once.  The BSP
 * specific bsp_interrupt_facility_initialize() function will be called after
 * all internals are initialized.  If the BSP specific initialization fails,
 * then this is a fatal error.  The fatal error source is
 * RTEMS_FATAL_SOURCE_BSP and the fatal error code is
 * BSP_FATAL_INTERRUPT_INITIALIZATION.
 */
void bsp_interrupt_initialize(void);

/**
 * @brief BSP specific initialization.
 *
 * This routine will be called form bsp_interrupt_initialize() and shall do the
 * following:
 * - Initialize the facilities that call bsp_interrupt_handler_dispatch().  For
 * example on PowerPC the external exception handler.
 * - Initialize the interrupt controller.  You shall set the interrupt
 * controller in a state such that interrupts are disabled for all vectors.
 * The vectors will be enabled with your bsp_interrupt_vector_enable() function
 * and disabled via your bsp_interrupt_vector_disable() function.  These
 * functions have to work afterwards.
 */
void bsp_interrupt_facility_initialize(void);

/**
 * @brief Gets the attributes of the interrupt vector.
 *
 * @param vector is the interrupt vector number.  It shall be valid.
 *
 * @param[out] attributes is the pointer to an rtems_interrupt_attributes
 *   object.  When the function call is successful, the attributes of the
 *   interrupt vector will be stored in this object.  The pointer shall not be
 *   NULL.  The object shall be cleared to zero by the caller.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 */
rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
);

/**
 * @brief Checks if the interrupt is enabled.
 *
 * The function checks if the interrupt associated with the interrupt vector
 * specified by ``vector`` was enabled for the processor executing the function
 * call at some time point during the call.
 *
 * @param vector is the interrupt vector number.  It shall be valid.
 *
 * @param[out] enabled is the pointer to a ``bool`` object.  It shall not be
 *   ``NULL``.  When the function call is successful, the enabled status of
 *   the interrupt associated with the interrupt vector specified by ``vector``
 *   will be stored in this object.  When the interrupt was enabled for the
 *   processor executing the function call at some time point during the call,
 *   the object will be set to true, otherwise to false.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 */
rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
);

/**
 * @brief Enables the interrupt vector.
 *
 * This function shall enable the vector at the corresponding facility (in most
 * cases the interrupt controller).  It will be called then the first entry
 * is installed for the vector in rtems_interrupt_entry_install() for example.
 *
 * @note The implementation should use
 * bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) ) to validate
 * the vector number in ::RTEMS_DEBUG configurations.
 *
 * @param vector is the interrupt vector number.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_UNSATISFIED The request to enable the interrupt vector has
 *   not been satisfied.  The presence of this error condition is
 *   implementation-defined.  The interrupt vector attributes obtained by
 *   rtems_interrupt_get_attributes() should indicate if it is possible to
 *   enable a particular interrupt vector.
 */
rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number vector );

/**
 * @brief Disables the interrupt vector.
 *
 * This function shall disable the vector at the corresponding facility (in
 * most cases the interrupt controller).  It will be called then the last
 * entry is removed for the vector in rtems_interrupt_entry_remove() for
 * example.
 *
 * @note The implementation should use
 * bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) ) to validate
 * the vector number in ::RTEMS_DEBUG configurations.
 *
 * @param vector is the interrupt vector number.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_UNSATISFIED The request to disable the interrupt vector has
 *   not been satisfied.  The presence of this error condition is
 *   implementation-defined.  The interrupt vector attributes obtained by
 *   rtems_interrupt_get_attributes() should indicate if it is possible to
 *   disable a particular interrupt vector.
 */
rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number vector );

/**
 * @brief Checks if the interrupt is pending.
 *
 * The function checks if the interrupt associated with the interrupt vector
 * specified by ``vector`` was pending for the processor executing the function
 * call at some time point during the call.
 *
 * @param vector is the interrupt vector number.  It shall be valid.
 *
 * @param[out] pending is the pointer to a ``bool`` object.  It shall not be
 *   ``NULL``.  When the function call is successful, the pending status of
 *   the interrupt associated with the interrupt vector specified by ``vector``
 *   will be stored in this object.  When the interrupt was pending for the
 *   processor executing the function call at some time point during the call,
 *   the object will be set to true, otherwise to false.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_UNSATISFIED The request to get the pending status has not
 *   been satisfied.
 */
rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
);

/**
 * @brief Causes the interrupt vector.
 *
 * @param vector is the number of the interrupt vector to cause.  It shall be
 *   valid.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_UNSATISFIED The request to raise the interrupt vector has
 *   not been satisfied.  The presence of this error condition is
 *   implementation-defined.  The interrupt vector attributes obtained by
 *   rtems_interrupt_get_attributes() should indicate if it is possible to
 *   raise a particular interrupt vector.
 */
rtems_status_code bsp_interrupt_raise( rtems_vector_number vector );

#if defined(RTEMS_SMP)
/**
 * @brief Causes the interrupt vector on the processor.
 *
 * @param vector is the number of the interrupt vector to cause.  It shall be
 *   valid.
 *
 * @param cpu_index is the index of the target processor of the interrupt
 *   vector to cause.  It shall be valid.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_UNSATISFIED The request to cause the interrupt vector has
 *   not been satisfied.  The presence of this error condition is
 *   implementation-defined.  The interrupt vector attributes obtained by
 *   rtems_interrupt_get_attributes() should indicate if it is possible to
 *   raise a particular interrupt vector on a specific processor.
 */
rtems_status_code bsp_interrupt_raise_on(
  rtems_vector_number vector,
  uint32_t            cpu_index
);
#endif

/**
 * @brief Clears the interrupt vector.
 *
 * @param vector is the number of the interrupt vector to clear.  It shall be
 *   valid.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_UNSATISFIED The request to cause the interrupt vector has
 *   not been satisfied.  The presence of this error condition is
 *   implementation-defined.  The interrupt vector attributes obtained by
 *   rtems_interrupt_get_attributes() should indicate if it is possible to
 *   clear a particular interrupt vector.
 */
rtems_status_code bsp_interrupt_clear( rtems_vector_number vector );

#if defined(RTEMS_SMP)
/**
 * @brief Handles a spurious interrupt.
 *
 * @param vector is the vector number.
 */
void bsp_interrupt_spurious( rtems_vector_number vector );
#endif

/**
 * @brief Loads the interrupt entry with atomic acquire semantic.
 *
 * @param ptr is the pointer to an ::rtems_interrupt_entry pointer.
 *
 * @return Returns the pointer value.
 */
static inline rtems_interrupt_entry *bsp_interrupt_entry_load_acquire(
  rtems_interrupt_entry * const *ptr
)
{
#if defined(RTEMS_SMP)
  return (rtems_interrupt_entry *) _Atomic_Load_uintptr(
    (const Atomic_Uintptr *) ptr,
    ATOMIC_ORDER_ACQUIRE
  );
#else
  return *ptr;
#endif
}

/**
 * @brief Stores the interrupt entry with atomic release semantic.
 *
 * @param[out] ptr is the pointer to an ::rtems_interrupt_entry pointer.
 *
 * @param value is the pointer value.
 */
static inline void bsp_interrupt_entry_store_release(
  rtems_interrupt_entry **ptr,
  rtems_interrupt_entry  *value
)
{
#if defined(RTEMS_SMP)
  _Atomic_Store_uintptr(
    (Atomic_Uintptr *) ptr,
    (Atomic_Uintptr) value,
    ATOMIC_ORDER_RELEASE
  );
#else
  rtems_interrupt_level level;

  rtems_interrupt_local_disable( level );
  *ptr = value;
  rtems_interrupt_local_enable( level );
#endif
}

/**
 * @brief Loads the first interrupt entry installed at the interrupt vector.
 *
 * @param vector is the vector number.
 *
 * @return Returns the first entry or NULL.
 */
static inline rtems_interrupt_entry *bsp_interrupt_entry_load_first(
  rtems_vector_number vector
)
{
  rtems_vector_number index;

  index = bsp_interrupt_dispatch_index( vector );

  return bsp_interrupt_entry_load_acquire(
    &bsp_interrupt_dispatch_table[ index ]
  );
}

/**
 * @brief Sequentially calls all interrupt handlers of the entry its
 *   successors.
 *
 * In uniprocessor configurations, you can call this function within every
 * context which can be disabled via rtems_interrupt_local_disable().
 *
 * In SMP configurations, you can call this function in every context.
 *
 * @param entry is the first entry.
 */
static inline void bsp_interrupt_dispatch_entries(
  const rtems_interrupt_entry *entry
)
{
  do {
    ( *entry->handler )( entry->arg );
    entry = bsp_interrupt_entry_load_acquire( &entry->next );
  } while ( RTEMS_PREDICT_FALSE( entry != NULL ) );
}

/**
 * @brief Sequentially calls all interrupt handlers installed at the vector.
 *
 * This function does not validate the vector number.  If the vector number is
 * out of range, then the behaviour is undefined.
 *
 * The function assumes that no interrupt entries are installed at the vector.
 * In this case, no operation is performed.
 *
 * In uniprocessor configurations, you can call this function within every
 * context which can be disabled via rtems_interrupt_local_disable().
 *
 * In SMP configurations, you can call this function in every context.
 *
 * @param vector is the vector number.
 */
static inline void bsp_interrupt_handler_dispatch_unlikely(
  rtems_vector_number vector
)
{
  const rtems_interrupt_entry *entry;

  entry = bsp_interrupt_entry_load_first( vector );

  if ( RTEMS_PREDICT_FALSE( entry != NULL ) ) {
    bsp_interrupt_dispatch_entries( entry );
  }
}

/**
 * @brief Sequentially calls all interrupt handlers installed at the vector.
 *
 * This function does not validate the vector number.  If the vector number is
 * out of range, then the behaviour is undefined.
 *
 * In uniprocessor configurations, you can call this function within every
 * context which can be disabled via rtems_interrupt_local_disable().
 *
 * In SMP configurations, you can call this function in every context.
 *
 * @param vector is the vector number.
 */
static inline void bsp_interrupt_handler_dispatch_unchecked(
  rtems_vector_number vector
)
{
  const rtems_interrupt_entry *entry;

  entry = bsp_interrupt_entry_load_first( vector );

  if ( RTEMS_PREDICT_TRUE( entry != NULL ) ) {
    bsp_interrupt_dispatch_entries( entry );
  } else {
#if defined(RTEMS_SMP)
    bsp_interrupt_spurious( vector );
#else
    bsp_interrupt_handler_default( vector );
#endif
  }
}

/**
 * @brief Sequentially calls all interrupt handlers installed at the vector.
 *
 * If the vector number is out of range or the interrupt entry list is empty,
 * then bsp_interrupt_handler_default() will be called with the vector number
 * as argument.
 *
 * In uniprocessor configurations, you can call this function within every
 * context which can be disabled via rtems_interrupt_local_disable().
 *
 * In SMP configurations, you can call this function in every context.
 *
 * @param vector is the vector number.
 */
static inline void bsp_interrupt_handler_dispatch( rtems_vector_number vector )
{
  if ( bsp_interrupt_is_valid_vector( vector ) ) {
    bsp_interrupt_handler_dispatch_unchecked( vector );
  } else {
    bsp_interrupt_handler_default( vector );
  }
}

/** @} */

/**
 * @brief Acquires the interrupt support lock.
 *
 * The interrupt support lock is a mutex.  The mutex is only acquired if the
 * system is the ::SYSTEM_STATE_UP state.
 */
void bsp_interrupt_lock(void);

/**
 * @brief Releases the interrupt support lock.
 *
 * The mutex is only released if the system is the ::SYSTEM_STATE_UP state.
 */
void bsp_interrupt_unlock(void);

/**
 * @brief Checks the vector and routine.  When the checks were successful, the
 *   interrupt support lock will be obtained.
 *
 * @param vector is the interrupt vector number to check.
 *
 * @param routine is the routine to check.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INCORRECT_STATE The interrupt support was not initialized.
 *
 * @retval ::RTEMS_CALLED_FROM_ISR The function was called from within
 *   interrupt context.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``routine`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no interrupt vector associated with the
 *   number specified by ``vector``.
 */
rtems_status_code bsp_interrupt_check_and_lock(
  rtems_vector_number     vector,
  rtems_interrupt_handler handler
);

/* For internal use only */
rtems_interrupt_entry *bsp_interrupt_entry_find(
  rtems_vector_number      vector,
  rtems_interrupt_handler  routine,
  void                    *arg,
  rtems_interrupt_entry ***previous_next
);

/* For internal use only */
void bsp_interrupt_entry_remove(
  rtems_vector_number     vector,
  rtems_interrupt_entry  *entry,
  rtems_interrupt_entry **previous_next
);

/**
 * @brief This table contains a bit map which indicates if an entry is unique
 *   or shared.
 *
 * If the bit associated with a vector is set, then the entry is unique,
 * otherwise it may be shared.  If the bit with index
 * #BSP_INTERRUPT_DISPATCH_TABLE_SIZE is set, then the interrupt support is
 * initialized, otherwise it is not initialized.
 */
extern uint8_t bsp_interrupt_handler_unique_table[];

/**
 * @brief Checks if the handler entry associated with the hander index is
 *   unique.
 *
 * @param index is the handler index to check.
 *
 * @return Returns true, if handler entry associated with the hander index is
 *   unique, otherwise false.
 */
static inline bool bsp_interrupt_is_handler_unique( rtems_vector_number index )
{
  rtems_vector_number table_index;
  uint8_t             bit;

  table_index = index / 8;
  bit = (uint8_t) ( 1U << ( index % 8 ) );

  return ( bsp_interrupt_handler_unique_table[ table_index ] & bit ) != 0;
}

/**
 * @brief Sets the unique status of the handler entry.
 *
 * @param index is the handler index.
 *
 * @param unique is the unique status to set.
 */
static inline void bsp_interrupt_set_handler_unique(
  rtems_vector_number index,
  bool                unique
)
{
  rtems_vector_number table_index;
  uint8_t             bit;

  table_index = index / 8;
  bit = (uint8_t) ( 1U << ( index % 8 ) );

  if (unique) {
    bsp_interrupt_handler_unique_table[ table_index ] |= bit;
  } else {
    bsp_interrupt_handler_unique_table[ table_index ] &= ~bit;
  }
}

/**
 * @brief Checks if the interrupt support is initialized.
 *
 * @return Returns true, if the interrupt support is initialized, otherwise
 *   false.
 */
static inline bool bsp_interrupt_is_initialized( void )
{
  return bsp_interrupt_is_handler_unique( BSP_INTERRUPT_DISPATCH_TABLE_SIZE );
}

/**
 * @brief Gets a reference to the interrupt handler table slot associated with
 *   the index.
 *
 * @return Returns a reference to the interrupt handler table slot associated
 *   with the index.
 */
rtems_interrupt_entry **bsp_interrupt_get_dispatch_table_slot(
  rtems_vector_number index
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_IRQ_GENERIC_H */
