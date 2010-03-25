/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Generic BSP interrupt support implementation.
 */

/*
 * Based on concepts of Pavel Pisa, Till Straumann and Eric Valette.
 *
 * Copyright (c) 2008, 2009
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

#include <stdlib.h>

#include <bsp/irq-generic.h>

#ifdef BSP_INTERRUPT_USE_INDEX_TABLE
  bsp_interrupt_handler_index_type bsp_interrupt_handler_index_table
    [BSP_INTERRUPT_VECTOR_NUMBER];
#endif

bsp_interrupt_handler_entry bsp_interrupt_handler_table
  [BSP_INTERRUPT_HANDLER_TABLE_SIZE];

/* The last entry indicates if everything is initialized */
static uint8_t bsp_interrupt_handler_unique_table
  [(BSP_INTERRUPT_HANDLER_TABLE_SIZE + 7 + 1) / 8];

static rtems_id bsp_interrupt_mutex = RTEMS_ID_NONE;

static void bsp_interrupt_handler_empty(void *arg)
{
  rtems_vector_number vector = (rtems_vector_number) arg;

  bsp_interrupt_handler_default(vector);
}

static inline bool bsp_interrupt_is_handler_unique(rtems_vector_number index)
{
  rtems_vector_number i = index / 8;
  rtems_vector_number s = index % 8;
  return (bsp_interrupt_handler_unique_table [i] >> s) & 0x1;
}

static inline void bsp_interrupt_set_handler_unique(
  rtems_vector_number index,
  bool unique
)
{
  rtems_vector_number i = index / 8;
  rtems_vector_number s = index % 8;
  if (unique) {
    bsp_interrupt_handler_unique_table [i] |= (uint8_t) (0x1U << s);
  } else {
    bsp_interrupt_handler_unique_table [i] &= (uint8_t) ~(0x1U << s);
  }
}

static inline bool bsp_interrupt_is_initialized(void)
{
  return bsp_interrupt_is_handler_unique(BSP_INTERRUPT_HANDLER_TABLE_SIZE);
}

static inline void bsp_interrupt_set_initialized(void)
{
  bsp_interrupt_set_handler_unique(BSP_INTERRUPT_HANDLER_TABLE_SIZE, true);
}

static inline bool bsp_interrupt_is_empty_handler_entry(
  const bsp_interrupt_handler_entry *e
)
{
  return e->handler == bsp_interrupt_handler_empty;
}

static inline void bsp_interrupt_clear_handler_entry(
  bsp_interrupt_handler_entry *e
)
{
  e->handler = bsp_interrupt_handler_empty;
  e->arg = NULL;
  e->info = NULL;
  e->next = NULL;
}

static inline bool bsp_interrupt_allocate_handler_index(
  rtems_vector_number vector,
  rtems_vector_number *index
)
{
  #ifdef BSP_INTERRUPT_USE_INDEX_TABLE
    rtems_vector_number i = 0;

    /* The first entry will remain empty */
    for (i = 1; i < BSP_INTERRUPT_HANDLER_TABLE_SIZE; ++i) {
      const bsp_interrupt_handler_entry *e = &bsp_interrupt_handler_table [i];
      if (bsp_interrupt_is_empty_handler_entry(e)) {
        *index = i;
        return true;
      }
    }

    return false;
  #else
    *index = vector;
    return true;
  #endif
}

static bsp_interrupt_handler_entry *bsp_interrupt_allocate_handler_entry(void)
{
  #ifdef BSP_INTERRUPT_NO_HEAP_USAGE
    rtems_vector_number index = 0;
    if (bsp_interrupt_allocate_handler_index(0, &index)) {
      return &bsp_interrupt_handler_table [index];
    } else {
      return NULL;
    }
  #else
    return malloc(sizeof(bsp_interrupt_handler_entry));
  #endif
}

static void bsp_interrupt_free_handler_entry(bsp_interrupt_handler_entry *e)
{
  #ifdef BSP_INTERRUPT_NO_HEAP_USAGE
    bsp_interrupt_clear_handler_entry(e);
  #else
    free(e);
  #endif
}

static rtems_status_code bsp_interrupt_lock(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  if (_System_state_Is_up(_System_state_Get())) {
    if (bsp_interrupt_mutex == RTEMS_ID_NONE) {
      rtems_id mutex = RTEMS_ID_NONE;
      rtems_interrupt_level level;

      /* Create a mutex */
      sc = rtems_semaphore_create (
        rtems_build_name('I', 'N', 'T', 'R'),
        1,
        RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
        0,
        &mutex
      );
      if (sc != RTEMS_SUCCESSFUL) {
        return sc;
      }

      /* Assign the mutex */
      rtems_interrupt_disable(level);
      if (bsp_interrupt_mutex == RTEMS_ID_NONE) {
        /* Nobody else assigned the mutex in the meantime */

        bsp_interrupt_mutex = mutex;
        rtems_interrupt_enable(level);
      } else {
        /* Somebody else won */

        rtems_interrupt_enable(level);
        sc = rtems_semaphore_delete(mutex);
        if (sc != RTEMS_SUCCESSFUL) {
          return sc;
        }
      }
    }
    return rtems_semaphore_obtain(
      bsp_interrupt_mutex,
      RTEMS_WAIT,
      RTEMS_NO_TIMEOUT
    );
  } else {
    return RTEMS_SUCCESSFUL;
  }
}

static rtems_status_code bsp_interrupt_unlock(void)
{
  if (bsp_interrupt_mutex != RTEMS_ID_NONE) {
    return rtems_semaphore_release(bsp_interrupt_mutex);
  } else {
    return RTEMS_SUCCESSFUL;
  }
}

rtems_status_code bsp_interrupt_initialize(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  size_t i = 0;

  sc = bsp_interrupt_lock();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* We need one semaphore */
  if (_System_state_Is_before_initialization(_System_state_Get())) {
    Configuration.work_space_size += sizeof(Semaphore_Control);
    ++Configuration_RTEMS_API.maximum_semaphores;
  }

  if (bsp_interrupt_is_initialized()) {
    bsp_interrupt_unlock();
    return RTEMS_INTERNAL_ERROR;
  }

  /* Initialize handler table */
  for (i = 0; i < BSP_INTERRUPT_HANDLER_TABLE_SIZE; ++i) {
    bsp_interrupt_handler_table [i].handler = bsp_interrupt_handler_empty;
    bsp_interrupt_handler_table [i].arg = (void *) i;
  }

  sc = bsp_interrupt_facility_initialize();
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_interrupt_unlock();
    return sc;
  }

  bsp_interrupt_set_initialized();

  sc = bsp_interrupt_unlock();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  return RTEMS_SUCCESSFUL;
}

/**
 * @brief Installs an interrupt handler.
 *
 * @ingroup bsp_interrupt
 *
 * @return In addition to the standard status codes this function returns:
 * - If the BSP interrupt support is not initialized RTEMS_INTERNAL_ERROR will
 * be returned.
 * - If not enough memory for a new handler is available RTEMS_NO_MEMORY will
 * be returned
 *
 * @see rtems_interrupt_handler_install()
 */
static rtems_status_code bsp_interrupt_handler_install(
  rtems_vector_number vector,
  const char *info,
  rtems_option options,
  rtems_interrupt_handler handler,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_interrupt_level level;
  rtems_vector_number index = 0;
  bsp_interrupt_handler_entry *head = NULL;
  bsp_interrupt_handler_entry *tail = NULL;
  bsp_interrupt_handler_entry *current = NULL;
  bsp_interrupt_handler_entry *match = NULL;
  bool enable_vector = false;

  /* Check parameters and system state */
  if (!bsp_interrupt_is_initialized()) {
    return RTEMS_INTERNAL_ERROR;
  } else if (!bsp_interrupt_is_valid_vector(vector)) {
    return RTEMS_INVALID_ID;
  } else if (handler == NULL) {
    return RTEMS_INVALID_ADDRESS;
  } else if (rtems_interrupt_is_in_progress()) {
    return RTEMS_CALLED_FROM_ISR;
  }

  /* Lock */
  sc = bsp_interrupt_lock();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Get handler table index */
  index = bsp_interrupt_handler_index(vector);

  /* Get head entry of the handler list for current vector */
  head = &bsp_interrupt_handler_table [index];

  if (bsp_interrupt_is_empty_handler_entry(head)) {
    /*
     * No real handler installed yet.  So allocate a new index in
     * the handler table and fill the entry with life.
     */
    if (bsp_interrupt_allocate_handler_index(vector, &index)) {
      rtems_interrupt_disable(level);
      bsp_interrupt_handler_table [index].handler = handler;
      bsp_interrupt_handler_table [index].arg = arg;
      #ifdef BSP_INTERRUPT_USE_INDEX_TABLE
        bsp_interrupt_handler_index_table [vector] = index;
      #endif
      rtems_interrupt_enable(level);
      bsp_interrupt_handler_table [index].info = info;
    } else {
      /* Handler table is full */
      bsp_interrupt_unlock();
      return RTEMS_NO_MEMORY;
    }

    /* This is the first handler so enable the vector later */
    enable_vector = true;
  } else {
    /* Ensure that a unique handler remains unique */
    if (
      RTEMS_INTERRUPT_IS_UNIQUE(options)
        || bsp_interrupt_is_handler_unique(index)
    ) {
      /*
       * Tried to install a unique handler on a not empty
       * list or there is already a unique handler installed.
       */
      bsp_interrupt_unlock();
      return RTEMS_RESOURCE_IN_USE;
    }

    /*
     * Search for the list tail and check if the handler is already
     * installed.
     */
    current = head;
    do {
      if (current->handler == handler && current->arg == arg) {
        match = current;
      }
      tail = current;
      current = current->next;
    } while (current != NULL);

    /* Ensure the handler is not already installed */
    if (match != NULL) {
      /* The handler is already installed */
      bsp_interrupt_unlock();
      return RTEMS_TOO_MANY;
    }

    /* Allocate a new entry */
    current = bsp_interrupt_allocate_handler_entry();
    if (current == NULL) {
      /* Not enough memory */
      bsp_interrupt_unlock();
      return RTEMS_NO_MEMORY;
    }

    /* Set entry */
    current->handler = handler;
    current->arg = arg;
    current->info = info;
    current->next = NULL;

    /* Link to list tail */
    rtems_interrupt_disable(level);
    tail->next = current;
    rtems_interrupt_enable(level);
  }

  /* Make the handler unique if necessary */
  bsp_interrupt_set_handler_unique(index, RTEMS_INTERRUPT_IS_UNIQUE(options));

  /* Enable the vector if necessary */
  if (enable_vector) {
    sc = bsp_interrupt_vector_enable(vector);
    if (sc != RTEMS_SUCCESSFUL) {
      bsp_interrupt_unlock();
      return sc;
    }
  }

  /* Unlock */
  sc = bsp_interrupt_unlock();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  return RTEMS_SUCCESSFUL;
}

/**
 * @brief Removes an interrupt handler.
 *
 * @ingroup bsp_interrupt
 *
 * @return In addition to the standard status codes this function returns
 * RTEMS_INTERNAL_ERROR if the BSP interrupt support is not initialized.
 *
 * @see rtems_interrupt_handler_remove().
 */
static rtems_status_code bsp_interrupt_handler_remove(
  rtems_vector_number vector,
  rtems_interrupt_handler handler,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_interrupt_level level;
  rtems_vector_number index = 0;
  bsp_interrupt_handler_entry *head = NULL;
  bsp_interrupt_handler_entry *current = NULL;
  bsp_interrupt_handler_entry *previous = NULL;
  bsp_interrupt_handler_entry *match = NULL;

  /* Check parameters and system state */
  if (!bsp_interrupt_is_initialized()) {
    return RTEMS_INTERNAL_ERROR;
  } else if (!bsp_interrupt_is_valid_vector(vector)) {
    return RTEMS_INVALID_ID;
  } else if (handler == NULL) {
    return RTEMS_INVALID_ADDRESS;
  } else if (rtems_interrupt_is_in_progress()) {
    return RTEMS_CALLED_FROM_ISR;
  }

  /* Lock */
  sc = bsp_interrupt_lock();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Get handler table index */
  index = bsp_interrupt_handler_index(vector);

  /* Get head entry of the handler list for current vector */
  head = &bsp_interrupt_handler_table [index];

  /* Search for a matching entry */
  current = head;
  do {
    if (current->handler == handler && current->arg == arg) {
      match = current;
      break;
    }
    previous = current;
    current = current->next;
  } while (current != NULL);

  /* Remove the matching entry */
  if (match != NULL) {
    if (match->next != NULL) {
      /*
       * The match has a successor.  A successor is always
       * allocated.  So replace the match with its successor
       * and free the successor entry.
       */
      current = match->next;

      rtems_interrupt_disable(level);
      *match = *current;
      rtems_interrupt_enable(level);

      bsp_interrupt_free_handler_entry(current);
    } else if (match == head) {
      /*
       * The match is the list head and has no successor.
       * The list head is stored in a static table so clear
       * this entry.  Since now the list is empty disable the
       * vector.
       */

      /* Disable the vector */
      sc = bsp_interrupt_vector_disable(vector);

      /* Clear entry */
      rtems_interrupt_disable(level);
      bsp_interrupt_clear_handler_entry(head);
      #ifdef BSP_INTERRUPT_USE_INDEX_TABLE
        bsp_interrupt_handler_index_table [vector] = 0;
      #endif
      rtems_interrupt_enable(level);

      /* Allow shared handlers */
      bsp_interrupt_set_handler_unique(index, false);

      /* Check status code */
      if (sc != RTEMS_SUCCESSFUL) {
        bsp_interrupt_unlock();
        return sc;
      }
    } else {
      /*
       * The match is the list tail and has a predecessor.
       * So terminate the predecessor and free the match.
       */
      rtems_interrupt_disable(level);
      previous->next = NULL;
      rtems_interrupt_enable(level);

      bsp_interrupt_free_handler_entry(match);
    }
  } else {
    /* No matching entry found */
    bsp_interrupt_unlock();
    return RTEMS_UNSATISFIED;
  }

  /* Unlock */
  sc = bsp_interrupt_unlock();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  return RTEMS_SUCCESSFUL;
}

/**
 * @brief Iterates over all installed interrupt handler of a vector.
 *
 * @ingroup bsp_interrupt
 *
 * @return In addition to the standard status codes this function returns
 * RTEMS_INTERNAL_ERROR if the BSP interrupt support is not initialized.
 *
 * @see rtems_interrupt_handler_iterate().
 */
static rtems_status_code bsp_interrupt_handler_iterate(
  rtems_vector_number vector,
  rtems_interrupt_per_handler_routine routine,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  bsp_interrupt_handler_entry *current = NULL;
  rtems_option options = 0;
  rtems_vector_number index = 0;

  /* Check parameters and system state */
  if (!bsp_interrupt_is_initialized()) {
    return RTEMS_INTERNAL_ERROR;
  } else if (!bsp_interrupt_is_valid_vector(vector)) {
    return RTEMS_INVALID_ID;
  } else if (rtems_interrupt_is_in_progress()) {
    return RTEMS_CALLED_FROM_ISR;
  }

  /* Lock */
  sc = bsp_interrupt_lock();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Interate */
  index = bsp_interrupt_handler_index(vector);
  current = &bsp_interrupt_handler_table [index];
  if (!bsp_interrupt_is_empty_handler_entry(current)) {
    do {
      options = bsp_interrupt_is_handler_unique(index) ?
        RTEMS_INTERRUPT_UNIQUE : RTEMS_INTERRUPT_SHARED;
      routine(arg, current->info, options, current->handler, current->arg);
      current = current->next;
    } while (current != NULL);
  }

  /* Unlock */
  sc = bsp_interrupt_unlock();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_interrupt_handler_install(
  rtems_vector_number vector,
  const char *info,
  rtems_option options,
  rtems_interrupt_handler handler,
  void *arg
)
{
  return bsp_interrupt_handler_install(vector, info, options, handler, arg);
}

rtems_status_code rtems_interrupt_handler_remove(
  rtems_vector_number vector,
  rtems_interrupt_handler handler,
  void *arg
)
{
  return bsp_interrupt_handler_remove(vector, handler, arg);
}

rtems_status_code rtems_interrupt_handler_iterate(
  rtems_vector_number vector,
  rtems_interrupt_per_handler_routine routine,
  void *arg
)
{
  return bsp_interrupt_handler_iterate(vector, routine, arg);
}
