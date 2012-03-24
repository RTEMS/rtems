/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Generic BSP interrupt support API.
 */

/*
 * Based on concepts of Pavel Pisa, Till Straumann and Eric Valette.
 *
 * Copyright (c) 2008, 2009, 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_SHARED_IRQ_GENERIC_H
#define LIBBSP_SHARED_IRQ_GENERIC_H

#include <stdbool.h>

#include <rtems/irq-extension.h>

#include <bsp/irq.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if !defined(BSP_INTERRUPT_VECTOR_MIN) || !defined(BSP_INTERRUPT_VECTOR_MAX) || (BSP_INTERRUPT_VECTOR_MAX + 1) < BSP_INTERRUPT_VECTOR_MIN
  #error "invalid BSP_INTERRUPT_VECTOR_MIN or BSP_INTERRUPT_VECTOR_MAX"
#endif

#if defined(BSP_INTERRUPT_USE_INDEX_TABLE) && !defined(BSP_INTERRUPT_HANDLER_TABLE_SIZE)
  #error "if you define BSP_INTERRUPT_USE_INDEX_TABLE, you have to define BSP_INTERRUPT_HANDLER_TABLE_SIZE etc. as well"
#endif

#if defined(BSP_INTERRUPT_NO_HEAP_USAGE) && !defined(BSP_INTERRUPT_USE_INDEX_TABLE)
  #error "if you define BSP_INTERRUPT_NO_HEAP_USAGE, you have to define BSP_INTERRUPT_USE_INDEX_TABLE etc. as well"
#endif

#define BSP_INTERRUPT_VECTOR_NUMBER \
  (BSP_INTERRUPT_VECTOR_MAX - BSP_INTERRUPT_VECTOR_MIN + 1)

#ifndef BSP_INTERRUPT_HANDLER_TABLE_SIZE
  #define BSP_INTERRUPT_HANDLER_TABLE_SIZE BSP_INTERRUPT_VECTOR_NUMBER
#endif

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
    return bsp_interrupt_handler_index_table [vector - BSP_INTERRUPT_VECTOR_MIN];
  #else
    return vector - BSP_INTERRUPT_VECTOR_MIN;
  #endif
}

/**
 * @defgroup bsp_interrupt BSP Interrupt Support
 *
 * @ingroup rtems_interrupt_extension
 *
 * The BSP interrupt support manages a sequence of interrupt vector numbers
 * ranging from @ref BSP_INTERRUPT_VECTOR_MIN to @ref BSP_INTERRUPT_VECTOR_MAX
 * including the end points.  It provides methods to
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
 * @ref BSP_INTERRUPT_VECTOR_MIN and @ref BSP_INTERRUPT_VECTOR_MAX.
 *
 * For boards with small memory requirements you can define
 * @ref BSP_INTERRUPT_USE_INDEX_TABLE.  With an enabled index table the handler
 * table will be accessed via a small index table.  You can define the size of
 * the handler table with @ref BSP_INTERRUPT_HANDLER_TABLE_SIZE.
 *
 * Normally new list entries are allocated from the heap.  You may define
 * @ref BSP_INTERRUPT_NO_HEAP_USAGE, if you do not want to use the heap.  For
 * this option you have to define @ref BSP_INTERRUPT_USE_INDEX_TABLE as well.
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
    return (rtems_vector_number) BSP_INTERRUPT_VECTOR_MIN <= vector
      && vector <= (rtems_vector_number) BSP_INTERRUPT_VECTOR_MAX;
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
 * interrupt handlers.  The BSP specific bsp_interrupt_facility_initialize()
 * function will be called after all internals are initialized.  Initialization
 * is complete if everything was successful.
 */
rtems_status_code bsp_interrupt_initialize(void);

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
 * is installed for the vector in bsp_interrupt_handler_install().  It is
 * guaranteed that the vector number is within the BSP_INTERRUPT_VECTOR_MIN and
 * BSP_INTERRUPT_VECTOR_MAX range.
 *
 * @note You must not install or remove an interrupt handler in this function.
 * This may result in a deadlock.
 *
 * @return On success RTEMS_SUCCESSFUL shall be returned.
 */
rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector);

/**
 * @brief Disables the interrupt vector with number @a vector.
 *
 * This function shall disable the vector at the corresponding facility (in
 * most cases the interrupt controller).  It will be called then the last
 * handler is removed for the vector in bsp_interrupt_handler_remove().  It is
 * guaranteed that the vector number is within the BSP_INTERRUPT_VECTOR_MIN and
 * BSP_INTERRUPT_VECTOR_MAX range.
 *
 * @note You must not install or remove an interrupt handler in this function.
 * This may result in a deadlock.
 *
 * @return On success RTEMS_SUCCESSFUL shall be returned.
 */
rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector);

/**
 * @brief Sequencially calls all interrupt handlers for the vector number @a
 * vector.
 *
 * If the vector number is out of range or the handler list is empty
 * bsp_interrupt_handler_default() will be called with argument @a vector.
 *
 * You can call this function within every context which can be disabled via
 * rtems_interrupt_disable().
 */
static inline void bsp_interrupt_handler_dispatch(rtems_vector_number vector)
{
  if (bsp_interrupt_is_valid_vector(vector)) {
    bsp_interrupt_handler_entry *e =
      &bsp_interrupt_handler_table [bsp_interrupt_handler_index(vector)];

    do {
      (*e->handler)(e->arg);
      e = e->next;
    } while (e != NULL);
  } else {
    bsp_interrupt_handler_default(vector);
  }
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_IRQ_GENERIC_H */
