/**
 * @file
 *
 * @ingroup RTEMSScoreTLS
 *
 * @brief Thread-Local Storage (TLS)
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TLS_H
#define _RTEMS_SCORE_TLS_H

#include <rtems/score/cpu.h>

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreTLS Thread-Local Storage (TLS)
 *
 * @ingroup RTEMSScore
 *
 * @brief Thread-local storage (TLS) support.
 *
 * Variants I and II are according to Ulrich Drepper, "ELF Handling For
 * Thread-Local Storage".
 *
 * @{
 */

extern char _TLS_Data_begin[];

extern char _TLS_Data_end[];

extern char _TLS_Data_size[];

extern char _TLS_BSS_begin[];

extern char _TLS_BSS_end[];

extern char _TLS_BSS_size[];

extern char _TLS_Size[];

extern char _TLS_Alignment[];

typedef struct {
  /*
   * FIXME: Not sure if the generation number type is correct for all
   * architectures.
  */
  uint32_t generation_number;

  void *tls_blocks[1];
} TLS_Dynamic_thread_vector;

typedef struct TLS_Thread_control_block {
#ifdef __i386__
  struct TLS_Thread_control_block *tcb;
#else
  TLS_Dynamic_thread_vector *dtv;
  uintptr_t reserved;
#endif
} TLS_Thread_control_block;

typedef struct {
  uintptr_t module;
  uintptr_t offset;
} TLS_Index;

/**
 * @brief Gets the TLS size.
 *
 * @return the TLS size.
 */
static inline uintptr_t _TLS_Get_size( void )
{
  /*
   * Do not use _TLS_Size here since this will lead GCC to assume that this
   * symbol is not 0 and the tests for 0 will be optimized away.
   */
  return (uintptr_t) _TLS_BSS_end - (uintptr_t) _TLS_Data_begin;
}

/**
 * @brief Returns the value aligned up to the heap alignment.
 *
 * @param val The value to align.
 *
 * @return The value aligned to the heap alignment.
 */
static inline uintptr_t _TLS_Heap_align_up( uintptr_t val )
{
  uintptr_t msk = CPU_HEAP_ALIGNMENT - 1;

  return (val + msk) & ~msk;
}

/**
 * @brief Returns the size of the thread control block area size for this
 *      alignment, or the minimum size if alignment is too small.
 *
 * @param alignment The alignment for the operation.
 *
 * @return The size of the thread control block area.
 */
static inline uintptr_t _TLS_Get_thread_control_block_area_size(
  uintptr_t alignment
)
{
  return alignment <= sizeof(TLS_Thread_control_block) ?
    sizeof(TLS_Thread_control_block) : alignment;
}

/**
 * @brief Returns the actual size that has to be allocated for this size and
 *      alignment.
 *
 * @param size The size for the operation.
 * @param alignment The alignment for the operation.
 *
 * @return The actual allocation size.
 */
static inline uintptr_t _TLS_Get_allocation_size(
  uintptr_t size,
  uintptr_t alignment
)
{
  uintptr_t allocation_size = 0;

  allocation_size += _TLS_Heap_align_up( size );
  allocation_size += _TLS_Get_thread_control_block_area_size( alignment );

#ifndef __i386__
  allocation_size += sizeof(TLS_Dynamic_thread_vector);
#endif

  return allocation_size;
}

/**
 * @brief Copies TLS size bytes from the address tls_area and returns a pointer
 *      to the start of the area after clearing it.
 *
 * @param tls_area The starting address of the area to clear.
 *
 * @return The pointer to the beginning of the cleared section.
 */
static inline void *_TLS_Copy_and_clear( void *tls_area )
{
  tls_area = memcpy(
    tls_area,
    _TLS_Data_begin,
    (size_t) ((uintptr_t)_TLS_Data_size)
  );


  memset(
    (char *) tls_area + (size_t)((intptr_t) _TLS_BSS_begin) -
      (size_t)((intptr_t) _TLS_Data_begin),
    0,
    ((size_t) (intptr_t)_TLS_BSS_size)
  );

  return tls_area;
}

/**
 * @brief Initializes the dynamic thread vector.
 *
 * @param tls_block The tls block for @a dtv.
 * @param tcb The thread control block for @a dtv.
 * @param[out] dtv The dynamic thread vector to initialize.
 *
 * @return Pointer to an area that was copied and cleared from tls_block
 *       onwards (@see _TLS_Copy_and_clear).
 */
static inline void *_TLS_Initialize(
  void *tls_block,
  TLS_Thread_control_block *tcb,
  TLS_Dynamic_thread_vector *dtv
)
{
#ifdef __i386__
  (void) dtv;
  tcb->tcb = tcb;
#else
  tcb->dtv = dtv;
  dtv->generation_number = 1;
  dtv->tls_blocks[0] = tls_block;
#endif

  return _TLS_Copy_and_clear( tls_block );
}

/**
 * @brief Initializes a dynamic thread vector beginning at the given starting
 *      address.
 *
 * Use Variant I, TLS offsets emitted by linker takes the TCB into account.
 *
 * @param tls_area The tls area for the initialization.
 *
 * @return Pointer to an area that was copied and cleared from tls_block
 *       onwards (@see _TLS_Copy_and_clear).
 */
static inline void *_TLS_TCB_at_area_begin_initialize( void *tls_area )
{
  void *tls_block = (char *) tls_area
    + _TLS_Get_thread_control_block_area_size( (uintptr_t) _TLS_Alignment );
  TLS_Thread_control_block *tcb = tls_area;
  uintptr_t aligned_size = _TLS_Heap_align_up( (uintptr_t) _TLS_Size );
  TLS_Dynamic_thread_vector *dtv = (TLS_Dynamic_thread_vector *)
    ((char *) tls_block + aligned_size);

  return _TLS_Initialize( tls_block, tcb, dtv );
}

/**
 * @brief Initializes a dynamic thread vector with the area before a given
 * starting address as thread control block.
 *
 * Use Variant I, TLS offsets emitted by linker neglects the TCB.
 *
 * @param tls_area The tls area for the initialization.
 *
 * @return Pointer to an area that was copied and cleared from tls_block
 *       onwards (@see _TLS_Copy_and_clear).
 */
static inline void *_TLS_TCB_before_TLS_block_initialize( void *tls_area )
{
  void *tls_block = (char *) tls_area
    + _TLS_Get_thread_control_block_area_size( (uintptr_t) _TLS_Alignment );
  TLS_Thread_control_block *tcb = (TLS_Thread_control_block *)
    ((char *) tls_block - sizeof(*tcb));
  uintptr_t aligned_size = _TLS_Heap_align_up( (uintptr_t) _TLS_Size );
  TLS_Dynamic_thread_vector *dtv = (TLS_Dynamic_thread_vector *)
    ((char *) tls_block + aligned_size);

  return _TLS_Initialize( tls_block, tcb, dtv );
}

/**
 * @brief Initializes a dynamic thread vector with the area after a given
 * starting address as thread control block.
 *
 * Use Variant II
 *
 * @param tls_area The tls area for the initialization.
 *
 * @return Pointer to an area that was copied and cleared from tls_block
 *       onwards (@see _TLS_Copy_and_clear).
 */
static inline void *_TLS_TCB_after_TLS_block_initialize( void *tls_area )
{
  uintptr_t size = (uintptr_t) _TLS_Size;
  uintptr_t tls_align = (uintptr_t) _TLS_Alignment;
  uintptr_t tls_mask = tls_align - 1;
  uintptr_t heap_align = _TLS_Heap_align_up( tls_align );
  uintptr_t heap_mask = heap_align - 1;
  TLS_Thread_control_block *tcb = (TLS_Thread_control_block *)
    ((char *) tls_area + ((size + heap_mask) & ~heap_mask));
  void *tls_block = (char *) tcb - ((size + tls_mask) & ~tls_mask);
  TLS_Dynamic_thread_vector *dtv = (TLS_Dynamic_thread_vector *)
    ((char *) tcb + sizeof(*tcb));

  _TLS_Initialize( tls_block, tcb, dtv );

  return tcb;
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_TLS_H */
