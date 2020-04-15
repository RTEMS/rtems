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
 * @ingroup rtl
 *
 * @brief RTEMS Module Loading Debugger Interface.
 *
 * Inspection of run-time linkers in NetBSD and Android show a common type of
 * structure that is used to interface to GDB. The NetBSD definition of this
 * interface is being used and is defined in <link.h>. It defines a protocol
 * that is used by GDB to inspect the state of dynamic libraries. I have not
 * checked GDB code at when writing this comment but I suspect GDB sets a break
 * point on the r_brk field of _rtld_debug and it has code that detects this
 * break point being hit. When this happens it reads the state and performs the
 * operation based on the r_state field.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <link.h>
#include <rtems/rtl/rtl.h>
#include <rtems/rtl/rtl-trace.h>
#include <rtems/rtl/rtl-obj-fwd.h>

struct r_debug  _rtld_debug;

void
_rtld_debug_state (void)
{
  /*
   * Empty. GDB only needs to hit this location.
   */
}

int
_rtld_linkmap_add (rtems_rtl_obj* obj)
{
  struct link_map* l = obj->linkmap;
  struct link_map* prev;
  uint32_t         obj_num = obj->obj_num;
  int              i;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_DETAIL))
    printf ("rtl: linkmap_add\n");

  for (i = 0; i < obj_num; ++i)
  {
    l[i].sec_addr[rap_text] = obj->text_base;
    l[i].sec_addr[rap_const] = obj->const_base;
    l[i].sec_addr[rap_data] = obj->data_base;
    l[i].sec_addr[rap_bss] = obj->bss_base;
  }

  if (_rtld_debug.r_map == NULL)
  {
    _rtld_debug.r_map = l;
  }
  else
  {
    for (prev = _rtld_debug.r_map; prev->l_next != NULL; prev = prev->l_next);
    l->l_prev = prev;
    prev->l_next = l;
  }

  return true;
}

void
_rtld_linkmap_delete (rtems_rtl_obj* obj)
{
  struct link_map* l = obj->linkmap;

  /*
   *  link_maps are allocated together if not 1
   */
  struct link_map* e = l + obj->obj_num - 1;

  if (l->l_prev == NULL)
  {
    if ((_rtld_debug.r_map = e->l_next) != NULL)
     _rtld_debug.r_map->l_prev = NULL;
  }
  else
  {
    if ((l->l_prev->l_next = e->l_next) != NULL)
      e->l_next->l_prev = l->l_prev;
  }
}
