/**
 * @file
 *
 * @ingroup ScoreTLS
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
 * @defgroup ScoreTLS Thread-Local Storage (TLS)
 *
 * @ingroup Score
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

typedef struct {
  TLS_Dynamic_thread_vector *dtv;
  uintptr_t reserved;
} TLS_Thread_control_block;

typedef struct {
  uintptr_t module;
  uintptr_t offset;
} TLS_Index;

static inline uintptr_t _TLS_Get_size( void )
{
  /*
   * Do not use _TLS_Size here since this will lead GCC to assume that this
   * symbol is not 0 and the tests for 0 will be optimized away.
   */
  return (uintptr_t) _TLS_BSS_end - (uintptr_t) _TLS_Data_begin;
}

static inline uintptr_t _TLS_Heap_align_up( uintptr_t val )
{
  uintptr_t msk = CPU_HEAP_ALIGNMENT - 1;

  return (val + msk) & ~msk;
}

static inline uintptr_t _TLS_Get_thread_control_block_area_size(
  uintptr_t alignment
)
{
  return alignment <= sizeof(TLS_Thread_control_block) ?
    sizeof(TLS_Thread_control_block) : alignment;
}

static inline uintptr_t _TLS_Get_allocation_size(
  uintptr_t size,
  uintptr_t alignment
)
{
  uintptr_t aligned_size = _TLS_Heap_align_up( size );

  return _TLS_Get_thread_control_block_area_size( alignment )
    + aligned_size + sizeof(TLS_Dynamic_thread_vector);
}

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

static inline void *_TLS_Initialize(
  void *tls_block,
  TLS_Thread_control_block *tcb,
  TLS_Dynamic_thread_vector *dtv
)
{
  tcb->dtv = dtv;
  dtv->generation_number = 1;
  dtv->tls_blocks[0] = tls_block;

  return _TLS_Copy_and_clear( tls_block );
}

/* Use Variant I, TLS offsets emitted by linker takes the TCB into account */
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

/* Use Variant I, TLS offsets emitted by linker neglects the TCB */
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

/* Use Variant II */
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
