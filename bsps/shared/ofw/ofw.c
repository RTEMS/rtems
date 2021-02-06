/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ofw
 */

/*
 * Copyright (C) 2020 Niteesh Babu G S <niteesh.gs@gmail.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp/fdt.h>
#include <sys/param.h>
#include <ofw/ofw.h>
#include <libfdt.h>
#include <assert.h>
#include <rtems/sysinit.h>
#include <ofw/ofw_test.h>

static void *fdtp = NULL;

static phandle_t rtems_fdt_offset_to_phandle( int offset )
{
  if (offset < 0) {
    return 0;
  }

  return (phandle_t)offset + fdt_off_dt_struct(fdtp);
}

static int rtems_fdt_phandle_to_offset( phandle_t handle )
{
  int off;
  int fdt_off;

  off = (int) handle;
  fdt_off = fdt_off_dt_struct(fdtp);

  if (off < fdt_off) {
    return -1;

  }

  return off - fdt_off;
}

void rtems_ofw_init( void ) {
  int rv;
  const void *fdt;
  
  fdt = bsp_fdt_get();

  rv = fdt_check_header(fdt);

  /*
   * If the FDT is invalid exit through fatal.
   */
  if (rv != 0) {
    rtems_fatal_error_occurred(RTEMS_NOT_CONFIGURED);
  }

  fdtp = (void *)fdt;
}

RTEMS_SYSINIT_ITEM(
  rtems_ofw_init,
  RTEMS_SYSINIT_BSP_PRE_DRIVERS,
  RTEMS_SYSINIT_ORDER_FIRST
);

phandle_t rtems_ofw_peer( phandle_t node )
{
  int offset;

  if (node == 0) {
    int root = fdt_path_offset(fdtp, "/");
    return rtems_fdt_offset_to_phandle(root);
  }

  offset = rtems_fdt_phandle_to_offset(node);
  if (offset < 0) {
    return 0;
  }

  offset = fdt_next_subnode(fdtp, offset);
  return rtems_fdt_offset_to_phandle(offset);
}

phandle_t rtems_ofw_child( phandle_t node )
{
  int offset;

  offset = rtems_fdt_phandle_to_offset(node);

  if (offset < 0) {
    return 0;
  }

  offset = fdt_first_subnode(fdtp, offset);
  return rtems_fdt_offset_to_phandle(offset);
}

phandle_t rtems_ofw_parent( phandle_t node )
{
  int offset;

  offset = rtems_fdt_phandle_to_offset(node);

  if (offset < 0) {
    return 0;
  }

  offset = fdt_parent_offset(fdtp, offset);
  return rtems_fdt_offset_to_phandle(offset);
}

ssize_t rtems_ofw_get_prop_len(
  phandle_t node,
  const char *propname
)
{
  int offset;
  int len;
  const void *prop;

  offset = rtems_fdt_phandle_to_offset(node);

  if (offset < 0) {
    return -1;
  }

  prop = fdt_getprop(fdtp, offset, propname, &len);

  if (prop == NULL && strcmp(propname, "name") == 0) {
    fdt_get_name(fdtp, offset, &len);
    return len + 1;
  }

  if (prop == NULL && offset == fdt_path_offset(fdtp, "/chosen")) {
    if (strcmp(propname, "fdtbootcpu") == 0)
      return sizeof(pcell_t);
    if (strcmp(propname, "fdtmemreserv") == 0)
      return 2 * sizeof(uint64_t) * fdt_num_mem_rsv(fdtp);
  }

  if (prop == NULL) {
    return -1;
  }

  return len;
}

ssize_t rtems_ofw_get_prop(
  phandle_t    node,
  const char  *propname,
  void        *buf,
  size_t       bufsize
)
{
  const void *prop;
  int offset;
  int len;
  uint32_t cpuid;

  offset = rtems_fdt_phandle_to_offset(node);

  if (offset < 0) {
    return -1;
  }

  prop = fdt_getprop(fdtp, offset, propname, &len);

  if (prop == NULL && strcmp(propname, "name") == 0) {
    prop = fdt_get_name(fdtp, offset, &len);

    /* Node name's are 1-31 chars in length consisting of only
     * ascii chars and are null terminated */
    strlcpy(buf, prop, bufsize);

    /* Return the length of the name including the null byte
     * rather than the amount copied.
     * This is the behaviour in libBSD ofw_fdt_getprop
     */
    return len + 1;
  }

  if (prop == NULL && offset == fdt_path_offset(fdtp, "/chosen")) {
    if (strcmp(propname, "fdtbootcpu") == 0) {
      cpuid = cpu_to_fdt32(fdt_boot_cpuid_phys(fdtp));
      len = sizeof(cpuid);
      prop = &cpuid;
    }
    if (strcmp(propname, "fdtmemreserv") == 0) {
      prop = (char *)fdtp + fdt_off_mem_rsvmap(fdtp);
      len = sizeof(uint64_t)*2*fdt_num_mem_rsv(fdtp);
    }
  }

  if (prop == NULL) {
    return -1;
  }

  bcopy(prop, buf, MIN(len, bufsize));

  return len;
}

ssize_t rtems_ofw_get_enc_prop(
  phandle_t    node,
  const char  *prop,
  pcell_t     *buf,
  size_t       len
)
{
  ssize_t rv;

  assert(len % sizeof(pcell_t) == 0);
  rv = rtems_ofw_get_prop(node, prop, buf, len);

  if (rv < 0) {
    return rv;
  }

  for (int i = 0; i < (len / 4); i++) {
    buf[i] = fdt32_to_cpu(buf[i]);
  }

  return rv;
}

int rtems_ofw_has_prop(
  phandle_t    node,
  const char  *propname
)
{
  ssize_t rv;

  rv = rtems_ofw_get_prop_len(node, propname);
  return rv >= 0 ? 1 : 0;
}

ssize_t rtems_ofw_search_prop(
  phandle_t    node,
  const char  *propname,
  void        *buf,
  size_t       len
)
{
  ssize_t rv;

  for (; node != 0; node = rtems_ofw_parent(node)) {
    if ((rv = rtems_ofw_get_prop(node, propname, buf, len) != -1)) {
      return rv;
    }
  }

  return -1;
}

ssize_t rtems_ofw_search_enc_prop(
  phandle_t    node,
  const char  *propname,
  pcell_t     *buf,
  size_t       len
)
{
  ssize_t rv;

  for (; node != 0; node = rtems_ofw_parent(node)) {
    if ((rv = rtems_ofw_get_enc_prop(node, propname, buf, len) != -1)) {
      return rv;
    }
  }

  return -1;
}

ssize_t rtems_ofw_get_prop_alloc(
  phandle_t    node,
  const char  *propname,
  void       **buf
)
{
  ssize_t len;

  *buf = NULL;
  if ((len = rtems_ofw_get_prop_len(node, propname)) == -1) {
    return -1;
  }

  if (len > 0) {
    *buf = malloc(len);
    if (*buf == NULL) {
      return -1;
    }

    if (rtems_ofw_get_prop(node, propname, *buf, len) == -1) {
      rtems_ofw_free(*buf);
      *buf = NULL;
      return -1;
    }
  }

  return len;
}

ssize_t rtems_ofw_get_prop_alloc_multi(
  phandle_t    node,
  const char  *propname,
  int          elsz,
  void       **buf
)
{
  ssize_t len;

  *buf = NULL;
  if ((len = rtems_ofw_get_prop_len(node, propname)) == -1 ||
      (len % elsz != 0)) {
    return -1;
  }

  if (len > 0) {
    *buf = malloc(len);
    if (*buf == NULL) {
      return -1;
    }

    if (rtems_ofw_get_prop(node, propname, *buf, len) == -1) {
      rtems_ofw_free(*buf);
      *buf = NULL;
      return -1;
    }
  }

  return (len / elsz);
}

ssize_t rtems_ofw_get_enc_prop_alloc(
  phandle_t    node,
  const char  *propname,
  void       **buf
)
{
  ssize_t len;

  *buf = NULL;
  if ((len = rtems_ofw_get_prop_len(node, propname)) == -1) {
    return -1;
  }

  if (len > 0) {
    *buf = malloc(len);
    if (*buf == NULL) {
      return -1;
    }

    if (rtems_ofw_get_enc_prop(node, propname, *buf, len) == -1) {
      rtems_ofw_free(*buf);
      *buf = NULL;
      return -1;
    }
  }

  return len;
}

ssize_t rtems_ofw_get_enc_prop_alloc_multi(
  phandle_t     node,
  const char   *propname,
  int           elsz,
  void        **buf
)
{
  ssize_t len;

  *buf = NULL;
  if ((len = rtems_ofw_get_prop_len(node, propname)) == -1 ||
      (len % elsz != 0)) {
    return -1;
  }

  if (len > 0) {
    *buf = malloc(len);
    if (*buf == NULL) {
      return -1;
    }

    if (rtems_ofw_get_enc_prop(node, propname, *buf, len) == -1) {
      rtems_ofw_free(*buf);
      *buf = NULL;
      return -1;
    }
  }

  return (len / elsz);
}

void rtems_ofw_free( void *buf )
{
  free(buf);
}

int rtems_ofw_next_prop(
  phandle_t    node,
  const char  *previous,
  char        *buf,
  size_t       len
)
{
  const void *name;
  const void *prop;
  int offset;

  offset = rtems_fdt_phandle_to_offset(node);

  if (offset < 0) {
    return -1;
  }

  if (previous == NULL) {
    offset = fdt_first_property_offset(fdtp, offset);
  } else {
    fdt_for_each_property_offset(offset, fdtp, offset) {
      prop = fdt_getprop_by_offset(fdtp, offset, (const char **)&name, NULL);
      if (prop == NULL)
        return -1;

      if (strcmp(previous, name) != 0)
        continue;

      offset = fdt_next_property_offset(fdtp, offset);
      break;
    }
  }

  if (offset < 0)
    return 0;

  prop = fdt_getprop_by_offset(fdtp, offset, (const char **)&name, &offset);
  if (prop == NULL)
    return -1;

  strncpy(buf, name, len);

  return 1;
}

int rtems_ofw_set_prop(
  phandle_t    node,
  const char  *name,
  const void  *buf,
  size_t       len
)
{
  int offset;

  offset = rtems_fdt_phandle_to_offset(node);

  if (offset < 0)
    return -1;

  if (fdt_setprop_inplace(fdtp, offset, name, buf, len) != 0)
    return (fdt_setprop(fdtp, offset, name, buf, len));

  return 0;
}

phandle_t rtems_ofw_find_device( const char *path )
{
  int offset;

  offset = fdt_path_offset(fdtp, path);
  if (offset < 0)
    return -1;

  return rtems_fdt_offset_to_phandle(offset);
}

static phandle_t rtems_ofw_get_effective_phandle(
  phandle_t node,
  phandle_t xref
)
{
  phandle_t child;
  phandle_t ref;
  int node_offset;

  node_offset = fdt_path_offset(fdtp, "/");

  while ((node_offset = fdt_next_node(fdtp, node_offset, NULL)) > 0) {
    child = rtems_fdt_offset_to_phandle(node_offset);

    if (rtems_ofw_get_enc_prop(child, "phandle", &ref, sizeof(ref)) == -1 &&
        rtems_ofw_get_enc_prop(child, "ibm,phandle", &ref, sizeof(ref)) == -1 &&
        rtems_ofw_get_enc_prop(child, "linux,phandle", &ref, sizeof(ref)) == -1
    ) {
      continue;
    }

    if (ref == xref)
      return child;
  }

  return -1;
}

phandle_t rtems_ofw_node_from_xref( phandle_t xref )
{
  phandle_t node;

  if ((node = rtems_ofw_get_effective_phandle(rtems_ofw_peer(0), xref)) == -1)
    return xref;

  return node;
}

phandle_t rtems_ofw_xref_from_node( phandle_t node )
{
  phandle_t ref;

    if (rtems_ofw_get_enc_prop(node, "phandle", &ref, sizeof(ref)) == -1 &&
        rtems_ofw_get_enc_prop(node, "ibm,phandle", &ref, sizeof(ref)) == -1 &&
        rtems_ofw_get_enc_prop(node, "linux,phandle", &ref, sizeof(ref)) == -1)
    {
      return node;
    }

    return ref;
}

phandle_t rtems_ofw_instance_to_package( ihandle_t instance )
{
  return rtems_ofw_node_from_xref(instance);
}

ssize_t rtems_ofw_package_to_path(
  phandle_t   node,
  char       *buf,
  size_t      len
)
{
  int offset;
  int rv;

  offset = rtems_fdt_phandle_to_offset(node);

  rv = fdt_get_path(fdtp, offset, buf, len);
  if (rv != 0)
    return -1;

  return rv;
}

ssize_t rtems_ofw_instance_to_path(
  ihandle_t  instance,
  char      *buf,
  size_t     len
)
{
  int offset;
  int rv;

  offset = rtems_ofw_instance_to_package(instance);
  offset = rtems_fdt_phandle_to_offset(offset);

  rv = fdt_get_path(fdtp, offset, buf, len);
  if (rv != 0)
    return -1;

  return rv;
}

int rtems_ofw_get_reg(
  phandle_t              node,
  rtems_ofw_memory_area *buf,
  size_t                 size
)
{
  int len;
  int offset;
  int nranges;
  int nregs;
  phandle_t parent;
  rtems_ofw_ranges range;
  const rtems_ofw_ranges *ptr;

  len = rtems_ofw_get_enc_prop(node, "reg", (pcell_t *)buf, size);
  if (len <= 0) {
    return len;
  }

  nregs = MIN(len, size) / sizeof(rtems_ofw_memory_area);

  for (parent = rtems_ofw_parent(node); parent > 0;
      parent = rtems_ofw_parent(parent)) {

    offset = rtems_fdt_phandle_to_offset(parent);
    ptr = fdt_getprop(fdtp, offset, "ranges", &len);

    if (ptr == NULL) {
      break;
    }

    nranges = len / sizeof(rtems_ofw_ranges);

    offset = 0;
    for (int i=0; i < nregs; i++) {
      for (int j=0; j < nranges; j++) {
        
        range.parent_bus = fdt32_to_cpu(ptr[j].parent_bus);
        range.child_bus = fdt32_to_cpu(ptr[j].child_bus);
        range.size = fdt32_to_cpu(ptr[j].size);

        if (buf[i].start >= range.child_bus &&
            buf[i].start < range.child_bus + range.size) {
          offset = range.parent_bus - range.child_bus;
          break;
        }

      }
      buf[i].start += offset;
    }
  }

  return nregs;
}

int rtems_ofw_get_interrupts(
  phandle_t            node,
  rtems_vector_number *buf,
  size_t               size
)
{
  int rv;

  rv = rtems_ofw_get_enc_prop(node, "interrupts", buf, size);

  if (rv <= 0) {
    return rv;
  }

  return MIN(size, rv) / sizeof(rtems_vector_number);
}

bool rtems_ofw_node_status( phandle_t node )
{
  int len;
  const char buf[10];

  len = rtems_ofw_get_prop(node, "status", (void *)&buf[0], sizeof(buf));
  if ((len == -1) ||
      (strncmp(buf, "okay", MIN(5, len)) == 0) ||
      (strncmp(buf, "ok", MIN(3, len)) == 0)) {
      return true;
  }

  return false;
}

phandle_t rtems_ofw_find_device_by_compat( const char *compat )
{
  int offset;

  offset = fdt_node_offset_by_compatible(fdtp, -1, compat);
  return rtems_fdt_offset_to_phandle(offset);
}
