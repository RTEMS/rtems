/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ofw
 *
 * RTEMS FDT implementation of OpenFirmWare Interface.
 *
 * RTEMS OFW is a FDT only implementation of the OpenFirmWare interface.
 * This API is created to be compatible with FreeBSD OpenFirmWare interface.
 * The main intention is to make porting of FreeBSD drivers to RTEMS easier.
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

#ifndef _OFW_H
#define _OFW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <stdint.h>
#include <sys/types.h>

/**
 * Represents a node in the device tree. The offset from fdtp to node is used
 * instead of fdt offset to make sure this is compatible with OF interface in
 * FreeBSD.
 */
typedef uint32_t  phandle_t;
/**
 * Represents the effective phandle of the node.
 */
typedef uint32_t  ihandle_t;
/**
 * Represents the data type of the buffer.
 */
typedef uint32_t  pcell_t;

/**
 * @struct rtems_ofw_memory_area
 *
 * This is used to represent elements(FDT properties) that define
 * region of memory address.
 */
typedef struct {
  /** The start address of the memory region. */
  uint32_t start;
  /** The size of the memory region. */
  uint32_t size;
} rtems_ofw_memory_area;

/**
 * @struct rtems_ofw_ranges
 *
 * This is used to represent the ranges property in the device tree.
 */
typedef struct {
  /** The physical address within the child bus address space */
  uint32_t child_bus;
  /** The physical address within the parent bus address space */
  uint32_t parent_bus;
  /** The size of the range in the child's address space */
  uint32_t size;
} rtems_ofw_ranges;

/**
 * @brief Gets the node that is next to @a node.
 *
 * @param[in] node Node offset
 *
 * @retval Peer node offset Successful operation.
 * @retval 0 No peer node or invalid node handle
 */
phandle_t rtems_ofw_peer(
  phandle_t   node
);

/**
 * @brief Gets the node that is the child of @a node.
 *
 * @param[in] node Node offset
 *
 * @retval child node offset Successful operation.
 * @retval 0 No child node or invalid node handle
 */
phandle_t rtems_ofw_child(
  phandle_t   node
);

/**
 * @brief Gets the node that is the parent of @a node.
 *
 * @param[in] node Node offset
 *
 * @retval child node offset Successful operation.
 * @retval 0 No child node or invalid node handle
 */
phandle_t rtems_ofw_parent(
  phandle_t   node
);

/**
 * @brief Gets the length of the property mentioned in @a propname.
 *
 * @param[in] node Node offset
 * @param[in] prop Property name
 *
 * @retval -1 Invalid node or property
 * @retval  Length of property on successful operation
 */
ssize_t rtems_ofw_get_prop_len(
  phandle_t   node,
  const char *propname
);

/**
 * @brief Gets the value of property mentioned in @a propname.
 *
 * @param[in] node Node offset
 * @param[in] prop Property name
 * @param[out] buf The property value gets stored in this buffer (Pre-allocated)
 * @param[in] len Length of the buffer
 
 * @retval -1 Invalid node or property
 * @retval  Length of property on successful operation
 */
ssize_t rtems_ofw_get_prop(
  phandle_t    node,
  const char  *propname,
  void        *buf,
  size_t       len
);

/**
 * @brief Gets the value of property mentioned in @a prop.
 * 
 * Gets the value of property of @a node and converts the value into the host's
 * endianness.
 *
 * @param[in] node Node offset
 * @param[in] prop Property name
 * @param[out] buf The property value gets stored in this buffer(Pre-allocated)
 * after converted to the host's endianness
 * @param[in] len Length of the buffer
 *
 * @retval -1 Invalid node or property
 * @retval  Length of property on successful operation
 */
ssize_t rtems_ofw_get_enc_prop(
  phandle_t    node,
  const char  *prop,
  pcell_t     *buf,
  size_t       len
);

/**
 * @brief Checks if the property @a propname is present in @a node.
 *
 * @param[in] node Node offset
 * @param[in] propname Property name
 *
 *  @retval  0 Property not present.
 *  @retval  1 Property is present.
 */
int rtems_ofw_has_prop(
  phandle_t    node,
  const char  *propname
);

/**
 * @brief Searches for property @a propname in @a node.
 * 
 * Searches the node and its parent recursively for the property and fills the
 * buffer with the first found value.
 *
 * @param[in] node Node offset
 * @param[in] propname Property name
 * @param[out] buf The property value gets stored in this buffer (Pre-allocated)
 * @param[in] len Length of the buffer
 *
 * @retval  Length of the property if property is found.
 * @retval  -1 Property is not found.
 */
ssize_t rtems_ofw_search_prop(
  phandle_t    node,
  const char  *propname,
  void        *buf,
  size_t       len
);

/**
 * @brief Searches for property @a propname in @a node.
 * 
 * Searches the node and its parent recursively for the property and fills the 
 * buffer with the first value found after converting it to the endianness of
 * the host.
 *
 * @param[in] node Node offset
 * @param[in] propname Property name
 * @param[out] buf The property value gets stored in this buffer(Pre-allocated)
 * after converted to the host's endianness
 * @param[in] len Length of the buffer
 *
 * @retval  Length of the property if property is found.
 * @retval  -1 Property is not found.
 */
ssize_t rtems_ofw_search_enc_prop(
  phandle_t    node,
  const char  *propname,
  pcell_t     *buf,
  size_t       len
);

/**
 * @brief Gets the value of property mentioned in @a propname.
 *
 * Same as rtems_ofw_getprop, but the @a buf is allocated in this routine and
 * the user is responsible for freeing it.
 * 
 * @param[in] node Node offset
 * @param[in] propname Property name
 * @param[out] buf The buffer is allocated in this routine and user is
 * responsible for freeing.
 *
 * @retval  -1 Property is not found.
 * @retval  Length of the property if property is found.
 */
ssize_t rtems_ofw_get_prop_alloc(
  phandle_t    node,
  const char  *propname,
  void       **buf
);

/**
 * @brief Gets multiple values of the property @a propname.
 * 
 * Same as rtems_ofw_getprop_alloc but it can read properties with multiple
 * values.
 * For eg: reg = <0x1000 0x10 0x2000 0x20>
 *
 * @param[in] node Node offset
 * @param[in] propname Property name
 * @param[in] elsz Size of the single value
 * @param[out] buf The buffer is allocated in this routine and user is
 * responsible for freeing.
 *
 * @retval  -1 Property is not found.
 * @retval   Number of values read.
 */
ssize_t rtems_ofw_get_prop_alloc_multi(
  phandle_t    node,
  const char  *propname,
  int          elsz,
  void       **buf
);

/**
 * @brief Gets the value of property mentioned in @a propname.
 * 
 * Same as rtems_ofw_getprop_alloc but the value stored in the buffer is
 * converted into the host's endianness.
 *
 * @param[in] node Node offset
 * @param[in] propname Property name
 * @param[out] buf The buffer is allocated in this routine and user is
 * responsible for freeing.
 *
 * @retval  -1 Property is not found.
 * @retval  Length of the property if property is found.
 */
ssize_t rtems_ofw_get_enc_prop_alloc(
  phandle_t    node,
  const char  *propname,
  void       **buf
);

/**
 * @brief Gets multiple values of the property @a propname.
 *
 * Same as rtems_ofw_getprop_alloc_multi but the values stored in the buffer
 * are converted to the host's endianness.
 * 
 * @param[in] node Node offset
 * @param[in] propname Property name
 * @param[in] elsz Size of the single value
 * @param[out] buf The buffer is allocated in this routine and user is
 * responsible for freeing.
 *
 * @retval  -1 Property is not found.
 * @retval   Number of values read.
 */
ssize_t rtems_ofw_get_enc_prop_alloc_multi(
  phandle_t     node,
  const char   *propname,
  int           elsz,
  void        **buf
);

/**
 * @brief Free's the buffers allocated by the rtems_ofw_*_alloc functions.
 *
 * @param[in] buf Buffer to be freed
 */
void rtems_ofw_free(
  void   *buf
);

/**
 * @brief Finds the next property of @a node.
 * 
 * Finds the next property of the node and when @a propname is NULL it returns
 * the value in the first property.
 *
 * @param[in] node Node offset
 * @param[in] previous Previous property name
 * @param[out] buf The value of the next property gets stored in this buffer
 * (Pre-allocated)
 * @param[in] len Length of the buffer
 *
 * @retval  -1 node or previous property does not exist
 * @retval   0 no more properties
 * @retval   1 success
 */
int rtems_ofw_next_prop(
  phandle_t    node,
  const char  *previous,
  char        *buf,
  size_t       len
);

/**
 * @brief Sets the property @name of @a node to @a buf.
 *
 * @param[in] node Node offset
 * @param[in] name Property name
 * @param[in] buf Buffer contains the value to be set.
 * @param[in] len Length of the buffer
 *
 * @retval  -1 node does not exist
 * @retval   0 on success
 * @retval   libFDT error codes on unsuccessful setting operation
 */
int rtems_ofw_set_prop(
  phandle_t    node,
  const char  *name,
  const void  *buf,
  size_t       len
);

/**
 * @brief Converts a device specifier to a fully qualified path name.
 *
 * @param[in] dev device specifier
 * @param[out] buf The path is filled into the buffer(Pre-allocated)
 * @param[in] length of the buffer
 *
 * @retval  -1 always. Unimplemented.
 */
ssize_t rtems_ofw_canon(
  const char  *dev,
  char        *buf,
  size_t       len
);

/**
 * @brief Finds the node at the given path
 *
 * @param[in] path to the node from root
 *
 * @retval  -1 node does not exist
 * @retval   node handle on success
 */
phandle_t rtems_ofw_find_device(
  const char  *path
);

/**
 * @brief This routine converts effective phandle @a xref to node offset.
 *
 * @param[in] xref Node phandle
 *
 * @retval Node offset on success
 * @retval Node phandle on failure
 */
phandle_t rtems_ofw_node_from_xref(
  phandle_t   xref
);

/**
 * @brief This routine converts node offset to effective phandle of @a node.
 *
 * @retval Node phandle on success
 * @retval Node offset on failure
 */
phandle_t rtems_ofw_xref_from_node(
  phandle_t   node
);

/*
 * instance handles(ihandle_t) as same as phandles in the FDT implementation
 * of OF interface.
 */

/**
 * @brief Converts @a instance handle to phandle.
 *
 * instance are same as node offsets in FDT.
 * 
 * @param[in] instance Node offset
 * 
 * @retval phandle of node on success.
 * @retval instance of node on failure.
 */
phandle_t rtems_ofw_instance_to_package(
  ihandle_t   instance
);

/**
 * @brief Find the node's path from phandle.
 * 
 * @param[in] node Node offset
 * @param[out] buf Path is filled into this buffer(Pre-allocated).
 * @param[in] len Length of the buffer.
 *
 * @retval -1 always. Unimplemented.
 */
ssize_t rtems_ofw_package_to_path(
  phandle_t   node,
  char       *buf,
  size_t      len
);

/**
 * @brief Find the node's path from ihandle.
 * 
 * @param[in] instance Node offset
 * @param[out] buf Path is filled into this buffer(Pre-allocated).
 * @param[in] len Length of the buffer.
 *
 * @retval -1 always. Unimplemented.
 */
ssize_t rtems_ofw_instance_to_path(
  ihandle_t  instance,
  char      *buf,
  size_t     len
);

/**
 * @brief Queries the node's reg value.
 *
 * This routine fills the buffer @a buf with the values in reg property of node
 * @a node. It reads all the values of the property and fills the buffer to max
 * size.
 * This routine is local to RTEMS OFW and does not have an corresponding
 * FreeBSD OFW pair.
 *
 * @param[in] node Node offset
 * @param[out] buf Register values are stored in this buffer(Pre-allocated).
 * @param[in] size Length of the buffer.
 *
 * @retval -1 If reg property is missing.
 * @retval  Length of the reg property in bytes.
 */
int rtems_ofw_get_reg(
  phandle_t              node,
  rtems_ofw_memory_area *buf,
  size_t                 size
);

/**
 * @brief Queries the node's interrupt value.
 *
 * This routine fills the buffer @a buf with the interrupt number. In case of
 * multiple numbers it fills the buffer to its full extent.
 * This routine is local to RTEMS OFW and does not have an corresponding
 * FreeBSD OFW pair.
 *
 * @param[in] node Node offset
 * @param[out] buf Interrupt values are stored in this buffer(Pre-allocated).
 * @param[in] size Length of the buffer.
 *
 * @retval -1 If interrupts property is missing.
 * @retval  The number of interrupt numbers read.
 */
int rtems_ofw_get_interrupts(
  phandle_t            node,
  rtems_vector_number *buf,
  size_t               size
);

/**
 * @brief Queries the node's status.
 *
 * This routine is local to RTEMS OFW and does not have an corresponding
 * FreeBSD OFW pair.
 *
 * @param[in] node Node offset
 *
 * @retval true  Status is OK or empty.
 * @retval false Status is not OK or missing.
 */
bool rtems_ofw_node_status( phandle_t node );

/**
 * @brief Gets node phandle from compatible property.
 *
 * This routine is local to RTEMS OFW and does not have an corresponding
 * FreeBSD OFW pair.
 *
 * @param[in] compat Compatible string
 *
 * @retval 0 Node with @a compat as compatible string not found
 * @retval Node phandle on success.
 */
phandle_t rtems_ofw_find_device_by_compat( const char *compat );

/**
 * @brief check a nodes compatible property.
 *
 * This routine is local to RTEMS OFW and does not have an corresponding
 * FreeBSD OFW pair.
 *
 * Return true if @a compat equals @a node compatible property
 *
 * @param[in] node phandle of node
 * @param[in] compat Compatible string
 *
 * @retval 1 If node contains the @a compat as a element in compatible
 * property.
 * @retval 0 Otherwise.
 */
bool rtems_ofw_is_node_compatible( phandle_t node, const char *compat );

#ifdef __cplusplus
}
#endif

#endif /* _OFW_H */
