/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief This header file provides interfaces of the generic interrupt
 *   controller support.
 */

/*
 * Copyright (C) 2016 Chris Johns <chrisj@rtems.org>
 *
 * Copyright (C) 2008, 2017 embedded brains GmbH (http://www.embedded-brains.de)
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

#if defined(BSP_INTERRUPT_USE_INDEX_TABLE) && !defined(BSP_INTERRUPT_HANDLER_TABLE_SIZE)
  #error "if you define BSP_INTERRUPT_USE_INDEX_TABLE, you have to define BSP_INTERRUPT_HANDLER_TABLE_SIZE etc. as well"
#endif

#ifndef BSP_INTERRUPT_HANDLER_TABLE_SIZE
  #define BSP_INTERRUPT_HANDLER_TABLE_SIZE BSP_INTERRUPT_VECTOR_COUNT
#endif

/* Internal macros for SMP support, do not use externally */
#ifdef RTEMS_SMP
  #define bsp_interrupt_disable(level) do { (void) level; } while (0)
  #define bsp_interrupt_enable(level) do { } while (0)
  #define bsp_interrupt_fence(order) _Atomic_Fence(order)
#else
  #define bsp_interrupt_disable(level) rtems_interrupt_disable(level)
  #define bsp_interrupt_enable(level) rtems_interrupt_enable(level)
  #define bsp_interrupt_fence(order) do { } while (0)
#endif

#define bsp_interrupt_assert(e) _Assert(e)

struct bsp_interrupt_handler_entry {
  rtems_interrupt_handler handler;
  void *arg;
  const char *info;
  struct bsp_interrupt_handler_entry *next;
};

typedef struct bsp_interrupt_handler_entry bsp_interrupt_handler_entry;

extern bsp_interrupt_handler_entry bsp_interrupt_handler_table [];

#ifdef BSP_INTERRUPT_USE_INDEX_TABLE
  #if BSP_INTERRUPT_HANDLER_TABLE_SIZE < 0x100
    typedef uint8_t bsp_interrupt_handler_index_type;
  #elif BSP_INTERRUPT_HANDLER_TABLE_SIZE < 0x10000
    typedef uint16_t bsp_interrupt_handler_index_type;
  #else
    typedef uint32_t bsp_interrupt_handler_index_type;
  #endif
  extern bsp_interrupt_handler_index_type bsp_interrupt_handler_index_table [];
#endif

static inline rtems_vector_number bsp_interrupt_handler_index(
  rtems_vector_number vector
)
{
  #ifdef BSP_INTERRUPT_USE_INDEX_TABLE
    return bsp_interrupt_handler_index_table [vector];
  #else
    return vector;
  #endif
}

/**
 * @defgroup bsp_interrupt BSP Interrupt Support
 *
 * @ingroup RTEMSBSPsShared
 *
 * @brief Generic BSP Interrupt Support
 *
 * The BSP interrupt support manages a sequence of interrupt vector numbers
 * greater than or equal to zero and less than @ref BSP_INTERRUPT_VECTOR_COUNT
 * It provides methods to
 * @ref bsp_interrupt_handler_install() "install",
 * @ref bsp_interrupt_handler_remove() "remove" and
 * @ref bsp_interrupt_handler_dispatch() "dispatch" interrupt handlers for each
 * vector number.  It implements parts of the RTEMS interrupt manager.
 *
 * The entry points to a list of interrupt handlers are stored in a table
 * (= handler table).
 *
 * You have to configure the BSP interrupt support in the <bsp/irq.h> file
 * for each BSP.  For a minimum configuration you have to provide
 * @ref BSP_INTERRUPT_VECTOR_COUNT.
 *
 * For boards with small memory requirements you can define
 * @ref BSP_INTERRUPT_USE_INDEX_TABLE.  With an enabled index table the handler
 * table will be accessed via a small index table.  You can define the size of
 * the handler table with @ref BSP_INTERRUPT_HANDLER_TABLE_SIZE.
 *
 * You have to provide some special routines in your BSP (follow the links for
 * the details):
 * - bsp_interrupt_facility_initialize()
 * - bsp_interrupt_vector_enable()
 * - bsp_interrupt_vector_disable()
 * - bsp_interrupt_handler_default()
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
 * @brief Initialize BSP interrupt support.
 *
 * You must call this function before you can install, remove and dispatch
 * interrupt handlers.  There is no protection against concurrent
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
 *
 * @return On success RTEMS_SUCCESSFUL shall be returned.
 */
rtems_status_code bsp_interrupt_facility_initialize(void);

/**
 * @brief Enables the interrupt vector with number @a vector.
 *
 * This function shall enable the vector at the corresponding facility (in most
 * cases the interrupt controller).  It will be called then the first handler
 * is installed for the vector in bsp_interrupt_handler_install() for example.
 *
 * @note The implementation should use
 * bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector)) to valdiate the
 * vector number.
 *
 * @note You must not install or remove an interrupt handler in this function.
 * This may result in a deadlock.
 */
void bsp_interrupt_vector_enable(rtems_vector_number vector);

/**
 * @brief Disables the interrupt vector with number @a vector.
 *
 * This function shall disable the vector at the corresponding facility (in
 * most cases the interrupt controller).  It will be called then the last
 * handler is removed for the vector in bsp_interrupt_handler_remove() for
 * example.
 *
 * @note The implementation should use
 * bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector)) to valdiate the
 * vector number.
 *
 * @note You must not install or remove an interrupt handler in this function.
 * This may result in a deadlock.
 */
void bsp_interrupt_vector_disable(rtems_vector_number vector);

/**
 * @brief Sequentially calls all interrupt handlers installed at the vector.
 *
 * This function does not validate the vector number.  If the vector number is
 * out of range, then the behaviour is undefined.
 *
 * You can call this function within every context which can be disabled via
 * rtems_interrupt_local_disable().
 *
 * @param vector is the vector number.
 */
static inline void bsp_interrupt_handler_dispatch_unchecked(
  rtems_vector_number vector
)
{
  const bsp_interrupt_handler_entry *e;

  e = &bsp_interrupt_handler_table[ bsp_interrupt_handler_index( vector ) ];

  do {
    rtems_interrupt_handler handler;
    void *arg;

    arg = e->arg;
    bsp_interrupt_fence( ATOMIC_ORDER_ACQUIRE );
    handler = e->handler;
    ( *handler )( arg );

    e = e->next;
  } while ( e != NULL );
}

/**
 * @brief Sequentially calls all interrupt handlers installed at the vector.
 *
 * If the vector number is out of range or the handler list is empty
 * bsp_interrupt_handler_default() will be called with the vector number as
 * argument.
 *
 * You can call this function within every context which can be disabled via
 * rtems_interrupt_local_disable().
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

/**
 * @brief Is interrupt handler empty.
 *
 * This routine returns true if the handler is empty and has not been
 * initialised else false is returned. The interrupt lock is not used
 * so this call can be used from within interrupts.
 *
 * @return If empty true shall be returned else false is returned.
 */
bool bsp_interrupt_handler_is_empty(rtems_vector_number vector);

/** @} */

/* For internal use only */
void bsp_interrupt_lock(void);

/* For internal use only */
void bsp_interrupt_unlock(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_IRQ_GENERIC_H */
