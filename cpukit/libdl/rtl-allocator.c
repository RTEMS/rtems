/*
 *  COPYRIGHT (c) 2012, 2018 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Allocator
 */

#include <stdio.h>
#include <string.h>

#include <rtems/rtl/rtl.h>
#include "rtl-alloc-heap.h"
#include <rtems/rtl/rtl-trace.h>

/**
 * Tags as symbols for tracing.
 */
#if RTEMS_RTL_TRACE
static const char* tag_labels[6] =
{
  "OBJECT",
  "SYMBOL",
  "EXTERNAL",
  "READ",
  "READ_WRITE",
  "READ_EXEC",
};
#define rtems_rtl_trace_tag_label(_l) tag_labels[_l]
#else
#define rtems_rtl_trace_tag_label(_l) ""
#endif

void
rtems_rtl_alloc_initialise (rtems_rtl_alloc_data* data)
{
  int c;
  data->allocator = rtems_rtl_alloc_heap;
  for (c = 0; c < RTEMS_RTL_ALLOC_TAGS; ++c)
    rtems_chain_initialize_empty (&data->indirects[c]);
}

void*
rtems_rtl_alloc_new (rtems_rtl_alloc_tag tag, size_t size, bool zero)
{
  rtems_rtl_data* rtl = rtems_rtl_lock ();
  void*           address = NULL;

  /*
   * Obtain memory from the allocator. The address field is set by the
   * allocator.
   */
  if (rtl)
    rtl->allocator.allocator (true, tag, &address, size);

  rtems_rtl_unlock ();

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ALLOCATOR))
    printf ("rtl: alloc: new: %s addr=%p size=%zu\n",
            rtems_rtl_trace_tag_label (tag), address, size);

  /*
   * Only zero the memory if asked to and the allocation was successful.
   */
  if (address && zero)
    memset (address, 0, size);

  return address;
}

void
rtems_rtl_alloc_del (rtems_rtl_alloc_tag tag, void* address)
{
  rtems_rtl_data* rtl = rtems_rtl_lock ();

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ALLOCATOR))
    printf ("rtl: alloc: del: %s addr=%p\n",
            rtems_rtl_trace_tag_label (tag), address);

  if (rtl && address)
    rtl->allocator.allocator (false, tag, &address, 0);

  rtems_rtl_unlock ();
}

rtems_rtl_allocator
rtems_rtl_alloc_hook (rtems_rtl_allocator handler)
{
  rtems_rtl_data*     rtl = rtems_rtl_lock ();
  rtems_rtl_allocator previous = rtl->allocator.allocator;
  rtl->allocator.allocator = handler;
  rtems_rtl_unlock ();
  return previous;
}

void
rtems_rtl_alloc_indirect_new (rtems_rtl_alloc_tag tag,
                              rtems_rtl_ptr*      handle,
                              size_t              size)
{
  rtems_rtl_data* rtl = rtems_rtl_lock ();

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ALLOCATOR))
  {
    if (!rtems_rtl_ptr_null (handle))
      printf ("rtl: alloc: inew: %s handle=%p: not null\n",
              rtems_rtl_trace_tag_label (tag), handle);
    printf ("rtl: alloc: inew: %s handle=%p size=%zd\n",
            rtems_rtl_trace_tag_label (tag), handle, size);
  }

  if (rtl)
  {
    rtems_rtl_alloc_data* allocator = &rtl->allocator;
    handle->pointer = rtems_rtl_alloc_new (tag, size, false);
    if (!rtems_rtl_ptr_null (handle))
      rtems_chain_append_unprotected (&allocator->indirects[tag],
                                      &handle->node);
  }

  rtems_rtl_unlock ();
}

void
rtems_rtl_alloc_indirect_del (rtems_rtl_alloc_tag tag,
                              rtems_rtl_ptr*      handle)
{
  rtems_rtl_data* rtl = rtems_rtl_lock ();

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_ALLOCATOR))
  {
    if (rtems_rtl_ptr_null (handle))
      printf ("rtl: alloc: idel: %s handle=%p: is null\n",
              rtems_rtl_trace_tag_label (tag), handle);
    printf ("rtl: alloc: idel: %s handle=%p\n",
            rtems_rtl_trace_tag_label (tag), handle);
  }

  if (rtl && !rtems_rtl_ptr_null (handle))
  {
    rtems_chain_extract_unprotected (&handle->node);
    rtems_rtl_alloc_del (tag, &handle->pointer);
  }
}

bool
rtems_rtl_alloc_module_new (void** text_base, size_t text_size,
                            void** const_base, size_t const_size,
                            void** eh_base, size_t eh_size,
                            void** data_base, size_t data_size,
                            void** bss_base, size_t bss_size)
{
  *text_base = *const_base = *data_base = *bss_base = NULL;

  if (text_size)
  {
    *text_base = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_READ_EXEC,
                                      text_size, false);
    if (!*text_base)
    {
      return false;
    }
  }

  if (const_size)
  {
    *const_base = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_READ,
                                       const_size, false);
    if (!*const_base)
    {
      rtems_rtl_alloc_module_del (text_base, const_base, eh_base,
                                  data_base, bss_base);
      return false;
    }
  }

  if (eh_size)
  {
    *eh_base = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_READ,
                                    eh_size, false);
    if (!*eh_base)
    {
      rtems_rtl_alloc_module_del (text_base, const_base, eh_base,
                                  data_base, bss_base);
      return false;
    }
  }

  if (data_size)
  {
    *data_base = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_READ_WRITE,
                                      data_size, false);
    if (!*data_base)
    {
      rtems_rtl_alloc_module_del (text_base, const_base, eh_base,
                                  data_base, bss_base);
      return false;
    }
  }

  if (bss_size)
  {
    *bss_base = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_READ_WRITE,
                                     bss_size, false);
    if (!*bss_base)
    {
      rtems_rtl_alloc_module_del (text_base, const_base, eh_base,
                                  data_base, bss_base);
      return false;
    }
  }

  return true;
}

void
rtems_rtl_alloc_module_del (void** text_base,
                            void** const_base,
                            void** eh_base,
                            void** data_base,
                            void** bss_base)
{
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_READ_WRITE, *bss_base);
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_READ_WRITE, *data_base);
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_READ, *eh_base);
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_READ, *const_base);
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_READ_EXEC, *text_base);
  *text_base = *const_base = *eh_base = *data_base = *bss_base = NULL;
}
