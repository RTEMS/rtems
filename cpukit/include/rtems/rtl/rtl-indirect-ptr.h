/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2012, 2018 Chris Johns <chrisj@rtems.org>
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
/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Indirect Pointer Management allows memory
 *        compaction in the allocator.
 */

#if !defined (_RTEMS_RTL_INDIRECT_PTR_H_)
#define _RTEMS_RTL_INDIRECT_PTR_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <rtems/chain.h>

/**
 * The RTL Indirect pointer.
 */
struct rtems_rtl_ptr {
  rtems_chain_node node;     /**< Indirect pointers are held on lists. */
  void*            pointer;  /**< The actual pointer. */
};

typedef struct rtems_rtl_ptr rtems_rtl_ptr;

/**
 * The RTL Indirect size and pointer.
 */
struct rtems_rtl_sptr {
  rtems_rtl_ptr  ptr;      /**< The indirect pointer. */
  size_t         size;     /**< The size of the memory block. */
};

typedef struct rtems_rtl_sptr rtems_rtl_sptr;

/**
 * A chain of indirect pointers for users to chain in applications.
 *
 * @note The chain the pointer is on is internal to the allocator and cannot be
 *       used by applications.
 */
struct rtems_rtl_ptr_chain {
  rtems_chain_node node;  /**< Chain of indirect pointers. */
  rtems_rtl_ptr    ptr;   /**< The indirect pointer. */
};

typedef struct rtems_rtl_ptr_chain rtems_rtl_ptr_chain;

/**
 * A chain of indirect sized pointers for users to chain in applications.
 *
 * @note The chain the pointer is on is internal to the allocator and cannot be
 *       used by applications.
 */
struct rtems_rtl_sptr_chain {
  rtems_chain_node node; /**< Chain of indirect pointers. */
  rtems_rtl_sptr   ptr;  /**< The indirect pointer. */
};

typedef struct rtems_rtl_sptr_chain rtems_rtl_sptr_chain;

/**
 * Get the pointer given an indirect handle.
 *
 * @param handle The handle the pointer is returned from.
 * @return void* The pointer held in the handle.
 */
static inline void* rtems_rtl_ptr_get (rtems_rtl_ptr* handle)
{
  return handle->pointer;
}

/**
 * Set the pointer given an indirect handle and the pointer.
 *
 * @param handle The handle the pointer is returned from.
 * @param pointer The pointer to set in the handle.
 */
static inline void rtems_rtl_ptr_set (rtems_rtl_ptr* handle, void* pointer)
{
  handle->pointer = pointer;
}

/**
 * Initialise the indirect handle.
 *
 * @param handle The handle to initialise.
 */
static inline void rtems_rtl_ptr_init (rtems_rtl_ptr* handle)
{
  rtems_chain_set_off_chain (&handle->node);
  handle->pointer = NULL;
}

/**
 * Is the indirect handle NULL ?
 *
 * @param handle The handle to test.
 * @return bool True if the pointer is NULL.
 */
static inline bool rtems_rtl_ptr_null (rtems_rtl_ptr* handle)
{
  return handle->pointer == NULL;
}

/**
 * Move the allocated pointer from one handle to another. The source handle is
 * cleared and removed from the list of handles.
 *
 * @param src The source handle to move the pointer from.
 * @param dst The destination handle to receive the pointer.
 */
static inline void rtems_rtl_ptr_move (rtems_rtl_ptr* dst, rtems_rtl_ptr* src)
{
  /*
   * We do not know which chain the src handle resides on so insert the dst
   * handle after the src handle then extract the src handle.
   */
  rtems_chain_insert_unprotected (&src->node, &dst->node);
  rtems_chain_extract_unprotected (&src->node);
  dst->pointer = src->pointer;
  rtems_rtl_ptr_init (src);
}

/**
 * Return the pointer as the type provided.
 *
 * @param _h The handle.
 * @param _t The type.
 */
#define rtems_rtl_ptr_type_get(_h, _t) ((_t*) rtems_rtl_ptr_get (_h))

/**
 * Get the pointer given an indirect handle.
 *
 * @param handle The handle the pointer is returned from.
 * @return void* The pointer held in the handle.
 */
static inline void* rtems_rtl_sptr_get (rtems_rtl_sptr* handle)
{
  return rtems_rtl_ptr_get (&handle->ptr);
}

/**
 * Set the pointer given an indirect handle and the pointer.
 *
 * @param handle The handle the pointer is returned from.
 * @param pointer The pointer to set in the handle.
 */
static inline void rtems_rtl_sptr_set (rtems_rtl_sptr* handle, void* pointer)
{
  rtems_rtl_ptr_set (&handle->ptr, pointer);
}

/**
 * Initialise the indirect handle.
 *
 * @param handle The handle to initialise.
 */
static inline void rtems_rtl_sptr_init (rtems_rtl_sptr* handle)
{
  rtems_rtl_ptr_init (&handle->ptr);
  handle->size = 0;
}

/**
 * Is the indirect handle NULL ?
 *
 * @param handle The handle to test.
 * @return bool True if the pointer is NULL.
 */
static inline bool rtems_rtl_sptr_null (rtems_rtl_sptr* handle)
{
  return rtems_rtl_ptr_null (&handle->ptr);
}

/**
 * Move the allocated pointer from one handle to another. The source handle is
 * cleared and removed from the list of handles.
 *
 * @param src The source handle to move the pointer from.
 * @param dst The destination handle to receive the pointer.
 */
static inline void rtems_rtl_sptr_move (rtems_rtl_sptr* dst, rtems_rtl_sptr* src)
{
  rtems_rtl_ptr_move (&dst->ptr, &src->ptr);
  dst->size = src->size;
  src->size = 0;
}

/**
 * Get the size.
 *
 * @param handle The handle to get the size from.
 * @return size_t The size_t.
 */
static inline size_t rtems_rtl_sptr_get_size (rtems_rtl_sptr* handle)
{
  return handle->size;
}

/**
 * Set the size.
 *
 * @param handle The handle to set the size.
 * @param size The size to set..
 */
static inline void rtems_rtl_sptr_set_size (rtems_rtl_sptr* handle, size_t size)
{
  handle->size = size;
}

/**
 * Return the pointer as the type provided.
 *
 * @param _h The handle.
 * @param _t The type.
 */
#define rtems_rtl_sptr_type_get(_h, _t) ((_t*) rtems_rtl_sptr_get (_h))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
