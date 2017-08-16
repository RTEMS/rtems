/*
 *  COPYRIGHT (c) 2013-2017 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Interface based on the libdft:
 *    libfdt - Flat Device Tree manipulation
 *    Copyright (C) 2006 David Gibson, IBM Corporation.
 */
/**
 * @file
 *
 * @ingroup rtems_fdt
 *
 * @brief RTEMS Flattened Device Tree
 *
 * Support for loading, managing and accessing FDT blobs in RTEMS.
 */

#if !defined (_RTEMS_FDT_H_)
#define _RTEMS_FDT_H_

#include <rtems.h>
#include <rtems/chain.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * A blob.
 */
struct rtems_fdt_blob;
typedef struct rtems_fdt_blob rtems_fdt_blob;

/**
 * A blob handle is a way to manage access to the FDT blobs. The blob is
 * referenced via the handle to allow searches across loaded DTB's to return
 * the referenced DTB.
 */
typedef struct
{
  rtems_fdt_blob* blob;  /**< The blob the handle references. */
} rtems_fdt_handle;

/*
 * The following are mappings to the standard FDT calls.
 */

/**
 * RTEMS_FDT_ERR_NOTFOUND: The requested node or property does not exist
 */
#define RTEMS_FDT_ERR_NOTFOUND  1
/**
 * RTEMS_FDT_ERR_EXISTS: Attemped to create a node or property which already
 * exists */
#define RTEMS_FDT_ERR_EXISTS    2
/**
 * RTEMS_FDT_ERR_NOSPACE: Operation needed to expand the device tree, but its
 * buffer did not have sufficient space to contain the expanded tree. Use
 * rtems_fdt_open_into() to move the device tree to a buffer with more space.
 */
#define RTEMS_FDT_ERR_NOSPACE    3

/* Error codes: codes for bad parameters */
/**
 * RTEMS_FDT_ERR_BADOFFSET: Function was passed a structure block offset which
 * is out-of-bounds, or which points to an unsuitable part of the structure for
 * the operation.
 */
#define RTEMS_FDT_ERR_BADOFFSET  4
/**
 * RTEMS_FDT_ERR_BADPATH: Function was passed a badly formatted path
 * (e.g. missing a leading / for a function which requires an absolute path)
*/
#define RTEMS_FDT_ERR_BADPATH    5
/**
 * RTEMS_FDT_ERR_BADPHANDLE: Function was passed an invalid phandle value.
 * phandle values of 0 and -1 are not permitted.
 */
#define RTEMS_FDT_ERR_BADPHANDLE  6
/**
 * RTEMS_FDT_ERR_BADSTATE: Function was passed an incomplete device tree
 * created by the sequential-write functions, which is not sufficiently
 * complete for the requested operation.
 */
#define RTEMS_FDT_ERR_BADSTATE  7

/* Error codes: codes for bad device tree blobs */

/**
 * RTEMS_FDT_ERR_TRUNCATED: Structure block of the given device tree ends
 * without an RTEMS_FDT_END tag.
 */
#define RTEMS_FDT_ERR_TRUNCATED  8
/**
 * RTEMS_FDT_ERR_BADMAGIC: Given "device tree" appears not to be a device tree
 * at all - it is missing the flattened device tree magic number.
 */
#define RTEMS_FDT_ERR_BADMAGIC  9
/** RTEMS_FDT_ERR_BADVERSION: Given device tree has a version which can't be
 * handled by the requested operation.  For read-write functions, this may mean
 * that rtems_fdt_open_into() is required to convert the tree to the expected
 * version.
 */
#define RTEMS_FDT_ERR_BADVERSION  10
/**
 * RTEMS_FDT_ERR_BADSTRUCTURE: Given device tree has a corrupt structure block
 * or other serious error (e.g. misnested nodes, or subnodes preceding
 * properties).
 */
#define RTEMS_FDT_ERR_BADSTRUCTURE  11
/**
 * RTEMS_FDT_ERR_BADLAYOUT: For read-write functions, the given device tree has
 * it's sub-blocks in an order that the function can't handle (memory reserve
 * map, then structure, then strings).  Use rtems_fdt_open_into() to reorganize
 * the tree into a form suitable for the read-write operations.
 */
#define RTEMS_FDT_ERR_BADLAYOUT  12
/**
 * "Can't happen" error indicating a bug in libfdt
 */
#define RTEMS_FDT_ERR_INTERNAL  13

/* RTEMS error codes. */

/**
 * Invalid handle.
 */
#define RTEMS_FDT_ERR_INVALID_HANDLE 100
/**
 * No memory.
 */
#define RTEMS_FDT_ERR_NO_MEMORY      101
/**
 * File not found.
 */
#define RTEMS_FDT_ERR_NOT_FOUND      102
/**
 * Cannot read the DTB into memory.
 */
#define RTEMS_FDT_ERR_READ_FAIL      103
/**
 * The blob cannot be unloaded as it is referenced.
 */
#define RTEMS_FDT_ERR_REFERENCED     104

#define RTEMS_FDT_ERR_RTEMS_MIN      100
#define RTEMS_FDT_ERR_MAX            104

/**
 * Initialise a handle to a default state.
 *
 * @param handle The handle to initialise.
 */
void rtems_fdt_init_handle (rtems_fdt_handle* handle);

/**
 * Duplicate a handle. The copy must be released.
 *
 * @param from Duplicate from this handle.
 * @param to   Duplicate to this handle.
 */
void rtems_fdt_dup_handle (rtems_fdt_handle* from, rtems_fdt_handle* to);

/**
 * Release a blob from a handle and clear it.
 *
 * @param handle The handle to check.
 */
void rtems_fdt_release_handle (rtems_fdt_handle* handle);

/**
 * Check if a handle had a valid blob assigned.
 *
 * @param handle The handle to check.
 * @retval true The handle has a valid blob.
 * @retval false The handle does not have a valid blob.
 */
bool rtems_fdt_valid_handle (const rtems_fdt_handle* handle);

/**
 * Find a tree node by its full path looking across of loaded blobs.. Each path
 * component may omit the unit address portion, but the results of this are
 * undefined if any such path component is ambiguous (that is if there are
 * multiple nodes at the relevant level matching the given component,
 * differentiated only by unit address).
 *
 * If the handle points to a valid blob it is release and the search starts
 * from the first blob.
 *
 * @param handle The FDT handle assigned to the blob if found else left invalid.
 * @param path Full path of the node to locate.
 * @param int If less than 0 an error code else the node offset is returned.
 */
int rtems_fdt_find_path_offset (rtems_fdt_handle* handle, const char* path);

/**
 * Load a device tree blob or DTB file into memory and register it on the chain
 * of blobs.
 *
 * @param filename The name of the blob file to load.
 * @param handle The handle returns the reference to the blob once load.
 * @return int If less than 0 it is an error code else it is the blob descriptor.
 */
int rtems_fdt_load (const char* const filename, rtems_fdt_handle* handle);

/**
 * Register a device tree blob or DTB on to the chain of blobs.
 *
 * @param blob_desc A pointer to the blob.
 * @param handle The handle returns the reference to the blob once load.
 * @return int If less than 0 it is an error code else it is the blob descriptor.
 */
int rtems_fdt_register (const void* blob, rtems_fdt_handle* handle);

/**
 * Unload a device tree blob or DTB file and release any memory allocated when
 * loading. The blob is removed from the list of registered.
 *
 * @param blob_desc A valid blob descriptor.
 * @return int If less than 0 it is an error code else 0 is return on success.
 */
int rtems_fdt_unload (rtems_fdt_handle* handle);

/**
 * Returns the number of entries in the device tree blob's memory
 * reservation map.  This does not include the terminating 0,0 entry
 * or any other (0,0) entries reserved for expansion.
 *
 * @param blob_desc A valid blob descriptor.
 * @return int The number of entries.
 */
int rtems_fdt_num_mem_rsv (rtems_fdt_handle* handle);

/**
 * Retrieve one memory reserve map entry. On success, *address and *size will
 * contain the address and size of the n-th reserve map entry from the device
 * tree blob, in native-endian format.
 *
 * @param blob_desc A valid blob descriptor.
 * @param address Pointer to 64-bit variables to hold the addresses.
 * @param size Pointer to 64-bit variables to hold the size.
 * @return int If less than 0 it is an error code else 0 is returned on
 *             success.
 */
int rtems_fdt_get_mem_rsv (rtems_fdt_handle* handle,
                           int               n,
                           uint64_t*         address,
                           uint64_t*         size);

/**
 * Find a subnode based on substring. Identical to rtems_fdt_subnode_offset(),
 * but only examine the first namelen characters of name for matching the
 * subnode name. This is useful for finding subnodes based on a portion of a
 * larger string, such as a full path.
 *
 * @param blob_desc A valid blob descriptor.
 * @param arentoffset Structure block offset of a node
 * @param name Name of the subnode to locate.
 * @param namelen Number of characters of name to consider.
 * @return int If less than 0 it is an error code else the node offset is
 *             returned.
 */
int rtems_fdt_subnode_offset_namelen (rtems_fdt_handle* handle,
                                      int               parentoffset,
                                      const char* const name,
                                      int               namelen);

/**
 * Find a subnode of a given node at structure block offset parentoffset with
 * the given name. The name may include a unit address, in which case
 * rtems_fdt_subnode_offset() will find the subnode with that unit address, or
 * the unit address may be omitted, in which case rtems_fdt_subnode_offset()
 * will find an arbitrary subnode whose name excluding unit address matches the
 * given name.
 *
 * @param blob_desc A valid blob descriptor.
 * @param parentoffset Structure block offset of a node.
 * @param name The name of the subnode to locate.
 * @return int If less than 0 it is an error code else the subnode offset is
 *             returned.
 */
int rtems_fdt_subnode_offset (rtems_fdt_handle* handle,
                              int               parentoffset,
                              const char* const name);

/**
 * Find a tree node by its full path. Each path component may omit the unit
 * address portion, but the results of this are undefined if any such path
 * component is ambiguous (that is if there are multiple nodes at the relevant
 * level matching the given component, differentiated only by unit address).
 *
 * @param handle The FDT handle to the current blob.
 * @param path Full path of the node to locate.
 * @param int If less than 0 an error code else the node offset is returned.
 */
int rtems_fdt_path_offset (rtems_fdt_handle* handle, const char* path);

/**
 * Retrieve the name of a given node (including unit address) of the device
 * tree node at structure block offset @nodeoffset.  If @length is non-NULL,
 * the length of this name is also returned, in the integer pointed to by
 * @length.
 *
 * @param handle The FDT handle to the current blob.
 * @param nodeoffset Structure block offset of the starting node.
 * @param length Pointer to an integer variable (will be overwritten) or NULL.
 * @return const char* The node's name on success or NULL on error. The length
 *                     if non-NULL will hold the error code.
 */
const char* rtems_fdt_get_name (rtems_fdt_handle* handle,
                                int               nodeoffset,
                                int*              length);

/**
 * Get property value based on substring. Identical to rtems_fdt_getprop(), but
 * only examine the first namelen characters of name for matching the property
 * name.
 *
 * @param handle The FDT handle to the current blob.
 * @param nodeoffset Offset of the node whose property to find
 * @param name The name of the property to find
 * @param namelen The number of characters of name to consider
 * @param length A pointer to an integer variable (will be overwritten) or
 *               NULL.
 * @return const void* The node's property on success or NULL on error. The
 *                     length if non-NULL will hold the error code.
 */
const void *rtems_fdt_getprop_namelen (rtems_fdt_handle* handle,
                                       int               nodeoffset,
                                       const char* const name,
                                       int               namelen,
                                       int*              length);

/**
 * Retrieve the value of a given property. Retrieves a pointer to the value of
 * the property named 'name' of the node at offset nodeoffset (this will be a
 * pointer to within the device blob itself, not a copy of the value).  If lenp
 * is non-NULL, the length of the property value is also returned, in the
 * integer pointed to by @length.
 *
 * @param handle The FDT handle to the current blob.
 * @param nodeoffset The offset of the node whose property to find.
 * @param name The name of the property to find.
 * @param length A pointer to an integer variable (will be overwritten) or
 *               NULL.
 * @return const void* The node's property on success or NULL on error. The
 *                     length if non-NULL will hold the error code.
 */
const void *rtems_fdt_getprop (rtems_fdt_handle* handle,
                               int               nodeoffset,
                               const char* const name,
                               int*              length);

/**
 * Retrieve the phandle of a given of the device tree node at structure block
 * offset nodeoffset.
 *
 * @param handle The FDT handle to the current blob.
 * @oaram nodeoffset The structure block offset of the node.
 * return uint32_t The phandle of the node at nodeoffset, on success (!= 0, !=
 *                  -1) 0, if the node has no phandle, or another error occurs.
 */
uint32_t rtems_fdt_get_phandle (rtems_fdt_handle* handle, int nodeoffset);

/**
 * Get alias based on substring. Identical to rtems_fdt_get_alias(), but only
 * examine the first namelen characters of name for matching the alias name.
 *
 * @param handle The FDT handle to the current blob.
 * @param name The name of the alias th look up.
 * @param namelen The number of characters of name to consider.
 * @return const char* The alias or NULL.
 */
const char *rtems_fdt_get_alias_namelen (rtems_fdt_handle* handle,
                                         const char* const name,
                                         int               namelen);

/**
 * Retreive the path referenced by a given alias. That is, the value of the
 * property named 'name' in the node /aliases.
 *
 * @param handle The FDT handle to the current blob.
 * @param name The name of the alias to look up.
 * @return const char* A pointer to the expansion of the alias named 'name', of
 *                     it exists NULL, if the given alias or the /aliases node
 *                     does not exist
 */
const char* rtems_fdt_get_alias (rtems_fdt_handle* handle, const char* name);

/**
 * Determine the full path of a node. This function is expensive, as it must
 * scan the device tree structure from the start to nodeoffset. It computes the
 * full path of the node at offset nodeoffset, and records that path in the
 * buffer at buf.
 *
 * @param handle The FDT handle to the current blob.
 * @param nodeoffset The offset of the node whose path to find.
 * @param buf A character buffer to contain the returned path (will be
 *            overwritten)
 * @param buflen The size of the character buffer at buf.
 * @return int 0 on success of an error code.
 */
int rtems_fdt_get_path (rtems_fdt_handle* handle,
                        int               nodeoffset,
                        char*             buf,
                        int               buflen);

/**
 * Find a specific ancestor of a node at a specific depth from the root (where
 * the root itself has depth 0, its immediate subnodes depth 1 and so forth).
 * So rtems_fdt_supernode_atdepth_offset(blob, nodeoffset, 0, NULL); will
 * always return 0, the offset of the root node. If the node at nodeoffset has
 * depth D, then:
 *  rtems_fdt_supernode_atdepth_offset(blob, nodeoffset, D, NULL);
 * will return nodeoffset itself.
 *
 * @param handle The FDT handle to the current blob.
 * @param nodeoffset The offset of the node whose parent to find.
 * @param supernodedepth The depth of the ancestor to find.
 * @oaram nodedepth The pointer to an integer variable (will be overwritten) or
 *                  NULL
 * @return int If less than 0 an error else the node offset.
 */
int rtems_fdt_supernode_atdepth_offset (rtems_fdt_handle* handle,
                                        int               nodeoffset,
                                        int               supernodedepth,
                                        int*              nodedepth);

/**
 * Find the depth of a given node. The root node has depth 0, its immediate
 * subnodes depth 1 and so forth.
 *
 * @note This function is expensive, as it must scan the device tree structure
 * from the start to nodeoffset.
 *
 * @param handle The FDT handle to the current blob.
 * @param nodeoffset The offset of the node whose parent to find.
 * @return int If less than 0 an error else the node offset.
 */
int rtems_fdt_node_depth (rtems_fdt_handle* handle, int nodeoffset);

/**
 * Find the parent of a given node. This funciton locates the parent node of a
 * given node (that is, it finds the offset of the node which contains the node
 * at nodeoffset as a subnode).
 *
 * @note This function is expensive, as it must scan the device tree structure
 * from the start to nodeoffset, *twice*.
 *
 * @param handle The FDT handle to the current blob.
 * @param nodeoffset The offset of the node whose parent to find.
 * @return int If less than 0 an error else the node offset.
 */
int rtems_fdt_parent_offset (rtems_fdt_handle* handle, int nodeoffset);

/**
 * Find nodes with a given property value. This funtion returns the offset of
 * the first node after startoffset, which has a property named propname whose
 * value is of length proplen and has value equal to propval; or if startoffset
 * is -1, the very first such node in the tree.
 *
 * To iterate through all nodes matching the criterion, the following idiom can
 * be used:
 *  offset = rtemsfdt_node_offset_by_prop_value(blob, -1, propname,
 *                                              propval, proplen);
 *  while (offset != -RTEMS_FDT_ERR_NOTFOUND) {
 *    // other code here
 *    offset = rtems_fdt_node_offset_by_prop_value(fdt, offset, propname,
 *                                                 propval, proplen);
 *  }
 *
 * @note The -1 in the first call to the function, if 0 is used here
 * instead, the function will never locate the root node, even if it
 * matches the criterion.
 *
 * @param handle The FDT handle to the current blob.
 * @param startoffset Only find nodes after this offset.
 * @param propname The property name to check.
 * @param propval The property value to search for.
 * @param proplen The length of the value in propval.
 * @return int The structure block offset of the located node (>= 0,
 *             >startoffset), on success and an error code is less
 *             than 0.
 */
int rtems_fdt_node_offset_by_prop_value (rtems_fdt_handle* handle,
                                         int               startoffset,
                                         const char* const propname,
                                         const void*       propval,
                                         int               proplen);

/**
 * Find the node with a given phandle returning the offset of the node which
 * has the given phandle value. If there is more than one node in the tree
 * with the given phandle (an invalid tree), results are undefined.
 *
 * @param handle The FDT handle to the current blob.
 * @param phandle The phandle value.
 * @return int If less than 0 an error else the node offset.
 */
int rtems_fdt_node_offset_by_phandle (rtems_fdt_handle* handle,
                                      uint32_t          phandle);

/**
 * Check a node's compatible property returning 0 if the given node contains a
 * 'compatible' property with the given string as one of its elements, it
 * returns non-zero otherwise, or on error.
 *
 * @param handle The FDT handle to the current blob.
 * @param nodeoffset The offset of a tree node.
 * @param compatible The string to match against.
 * @retval 0, if the node has a 'compatible' property listing the given string.
 * @retval 1, if the node has a 'compatible' property, but it does not list the
 *            given string
 */
int rtems_fdt_node_check_compatible (rtems_fdt_handle* handle,
                                     int               nodeoffset,
                                     const char* const compatible);

/**
 * Find nodes with a given 'compatible' value returning the offset of the first
 * node after startoffset, which has a 'compatible' property which lists the
 * given compatible string; or if startoffset is -1, the very first such node
 * in the tree.
 *
 * To iterate through all nodes matching the criterion, the following idiom can
 * be used:
 *
 *  offset = rtems_fdt_node_offset_by_compatible(blob, -1, compatible);
 *  while (offset != -RTEMS_FDT_ERR_NOTFOUND) {
 *    // other code here
 *    offset = rtems_fdt_node_offset_by_compatible(blob, offset, compatible);
 *  }
 *
 * @note The -1 in the first call to the function, if 0 is used here instead,
 * the function will never locate the root node, even if it matches the
 * criterion.
 *
 * @param handle The FDT handle to the current blob.
 * @param startoffset Only find nodes after this offset.
 * @param compatible The 'compatible' string to match against.
 * @return int If less than 0 an error else the node offset.
 */
int rtems_fdt_node_offset_by_compatible(rtems_fdt_handle* handle,
                                        int               startoffset,
                                        const char*       compatible);

/**
 * Traverse to the next node.
 *
 * @param handle The FDT handle to the current blob.
 * @param offset The offset in the blob to start looking for the next node.
 * @param depth Pointer to return the depth the node is.
 * @return int If less than 0 an error else the node offset.
 */
int rtems_fdt_next_node (rtems_fdt_handle* handle, int offset, int* depth);

/**
 * Return an error string given an error value.
 *
 * @param errval The error value.
 * @return const char* The error string.
 */
const char* rtems_fdt_strerror (int errval);

/**
 * Return a property given a path.
 */
int rtems_fdt_prop_value(const char* const path,
                         const char* const propname,
                         void*             value,
                         size_t*           size);

/**
 * Create a map given a path the property name and the names of the subnodes of
 * the path.
 */
int rtems_fdt_prop_map (const char* const path,
                        const char* const propname,
                        const char* const names[],
                        uint32_t*         values,
                        size_t            count);

/*
 * Get a value given a path and a property.
 */
int rtems_fdt_get_value (const char* const path,
                         const char* const property,
                         size_t            size,
                         uint32_t*         value);

/**
 * Get the number of entries in an FDT handle.
 */
int rtems_fdt_num_entries(rtems_fdt_handle* handle);

/**
 * Get the numbered entry name. Note that the id isn't the same as
 * the offset - it's numbered 0, 1, 2 ... num_entries-1
 */
const char *rtems_fdt_entry_name(rtems_fdt_handle* handle, int id);

/**
 * Get the numbered entry offset. Note that the id isn't the same as
 * the offset - it's numbered 0, 1, 2 ... num_entries-1
 */
int rtems_fdt_entry_offset(rtems_fdt_handle* handle, int id);

/*
 * Helper function to convert the void* property result to a 32bit unsigned int.
 */
uint32_t rtems_fdt_get_uint32 (const void* prop);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
