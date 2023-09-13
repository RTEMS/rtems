/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreTLS
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreTLS.
 */

/*
 * Copyright (C) 2014, 2023 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_TLS_H
#define _RTEMS_SCORE_TLS_H

#include <rtems/score/cpuimpl.h>

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreTLS Thread-Local Storage (TLS)
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the implementation to support thread-local
 *   storage (TLS).
 *
 * Variants I and II are according to Ulrich Drepper, "ELF Handling For
 * Thread-Local Storage".
 *
 * @{
 */

/**
 * @brief Represents the TLS configuration.
 */
typedef struct {
  /**
   * @brief This member is initialized to _TLS_Data_begin.
   */
  const char *data_begin;

  /**
   * @brief This member is initialized to _TLS_Data_size.
   */
  const char *data_size;

  /**
   * @brief This member is initialized to _TLS_BSS_begin.
   */
  const char *bss_begin;

  /**
   * @brief This member is initialized to _TLS_BSS_size.
   */
  const char *bss_size;

  /**
   * @brief This member is initialized to _TLS_Size.
   */
  const char *size;

  /**
   * @brief This member is initialized to _TLS_Alignment.
   */
  const char *alignment;
} TLS_Configuration;

/**
 * @brief Provides the TLS configuration.
 *
 * Directly using symbols with an arbitrary absolute address such as
 * _TLS_Alignment may not work with all code models (for example the AArch64
 * tiny and small code models).  Store the addresses in a read-only object.
 * Using the volatile qualifier ensures that the compiler actually loads the
 * address from the object.
 */
extern const volatile TLS_Configuration _TLS_Configuration;

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
#else /* !__i386__ */
  TLS_Dynamic_thread_vector *dtv;
/*
 * GCC under AArch64/LP64 expects a 16 byte TCB at the beginning of the TLS
 * data segment and indexes into it accordingly for TLS variable addresses.
 */
#if CPU_SIZEOF_POINTER == 4 || defined(AARCH64_MULTILIB_ARCH_V8)
  uintptr_t reserved;
#endif
#endif /* __i386__ */
} TLS_Thread_control_block;

typedef struct {
  uintptr_t module;
  uintptr_t offset;
} TLS_Index;

/**
 * @brief Gets the size of the thread control block area in bytes.
 *
 * @param config is the TLS configuration.
 *
 * @return Returns the size of the thread control block area in bytes.
 */
static inline uintptr_t _TLS_Get_thread_control_block_area_size(
  const volatile TLS_Configuration *config
)
{
#if CPU_THREAD_LOCAL_STORAGE_VARIANT == 11
  uintptr_t alignment;

  alignment = (uintptr_t) config->alignment;

  return RTEMS_ALIGN_UP( sizeof( TLS_Thread_control_block ), alignment );
#else
  (void) config;
  return sizeof( TLS_Thread_control_block );
#endif
}

/**
 * @brief Gets the allocation size of the thread-local storage area in bytes.
 *
 * @return Returns the allocation size of the thread-local storage area in
 *   bytes.
 */
uintptr_t _TLS_Get_allocation_size( void );

/**
 * @brief Initializes the thread-local storage data.
 *
 * @param config is the TLS configuration.
 *
 * @param[out] tls_data is the thread-local storage data to initialize.
 */
static inline void _TLS_Copy_and_clear(
  const volatile TLS_Configuration *config,
  void                             *tls_data
)
{
  tls_data =
    memcpy( tls_data, config->data_begin, (uintptr_t) config->data_size );

  memset(
    (char *) tls_data +
      (uintptr_t) config->bss_begin - (uintptr_t) config->data_begin,
    0,
    (uintptr_t) config->bss_size
  );
}

/**
 * @brief Initializes the thread control block and the dynamic thread vector.
 *
 * @param tls_data is the thread-local storage data address.
 *
 * @param[out] tcb is the thread control block to initialize.
 *
 * @param[out] dtv is the dynamic thread vector to initialize.
 */
static inline void _TLS_Initialize_TCB_and_DTV(
  void                      *tls_data,
  TLS_Thread_control_block  *tcb,
  TLS_Dynamic_thread_vector *dtv
)
{
#ifdef __i386__
  (void) dtv;
  tcb->tcb = tcb;
#else
  tcb->dtv = dtv;
  dtv->generation_number = 1;
  dtv->tls_blocks[0] = tls_data;
#endif
}

/**
 * @brief Initializes the thread-local storage area.
 *
 * @param tls_area[out] is the thread-local storage area to initialize.
 *
 * @return Where the architectures uses Variant I and the TLS offsets emitted
 *   by the linker neglect the TCB, returns the address of the thread-local
 *   storage data.  Otherwise, returns the address of the thread control block.
 */
static inline void *_TLS_Initialize_area( void *tls_area )
{
  const volatile TLS_Configuration *config;
  uintptr_t                         alignment;
  void                             *tls_data;
  TLS_Thread_control_block         *tcb;
  TLS_Dynamic_thread_vector        *dtv;
  void                             *return_value;
#if CPU_THREAD_LOCAL_STORAGE_VARIANT == 11
  uintptr_t                         tcb_size;
#endif
#if CPU_THREAD_LOCAL_STORAGE_VARIANT == 20
  uintptr_t                         size;
  uintptr_t                         alignment_2;
#endif

  config = &_TLS_Configuration;
  alignment = (uintptr_t) config->alignment;

#ifdef __i386__
  dtv = NULL;
#else
  dtv = (TLS_Dynamic_thread_vector *) tls_area;
  tls_area = (char *) tls_area + sizeof( *dtv );
#endif

#if CPU_THREAD_LOCAL_STORAGE_VARIANT == 10
  tls_data = (void *)
    RTEMS_ALIGN_UP( (uintptr_t) tls_area + sizeof( *tcb ), alignment );
  tcb = (TLS_Thread_control_block *) ((char *) tls_data - sizeof( *tcb ));
  return_value = tls_data;
#elif CPU_THREAD_LOCAL_STORAGE_VARIANT == 11
  tcb_size = RTEMS_ALIGN_UP( sizeof( *tcb ), alignment );
  tls_data = (void *)
    RTEMS_ALIGN_UP( (uintptr_t) tls_area + tcb_size, alignment );
  tcb = (TLS_Thread_control_block *) ((char *) tls_data - tcb_size);
  return_value = tcb;
#elif CPU_THREAD_LOCAL_STORAGE_VARIANT == 20
  alignment_2 = RTEMS_ALIGN_UP( alignment, CPU_SIZEOF_POINTER );
  tls_area = (void *) RTEMS_ALIGN_UP( (uintptr_t) tls_area, alignment_2 );
  size = (uintptr_t) config->size;
  tcb = (TLS_Thread_control_block *)
    ((char *) tls_area + RTEMS_ALIGN_UP( size, alignment_2 ));
  tls_data = (char *) tcb - RTEMS_ALIGN_UP( size, alignment );
  return_value = tcb;
#else
#error "unexpected CPU_THREAD_LOCAL_STORAGE_VARIANT value"
#endif

  _TLS_Initialize_TCB_and_DTV( tls_data, tcb, dtv );
  _TLS_Copy_and_clear( config, tls_data );

  return return_value;
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_TLS_H */
