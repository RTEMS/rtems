/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2013-2017 Chris Johns <chrisj@rtems.org>
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

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <libfdt.h>
#include <zlib.h>

#include <rtems/malloc.h>
#include <rtems/rtems-fdt.h>
#include <rtems/thread.h>

/**
 * An index for quick access to the FDT by name or offset.
 */

typedef struct
{
  const char *             name;         /**< The full path of the FDT item. */
  int                      offset;       /**< The offset of the item in the FDT blob. */
} rtems_fdt_index_entry;

typedef struct
{
  int                    num_entries;    /**< The number of entries in this index. */
  rtems_fdt_index_entry* entries;        /**< An ordered set of entries which we
                                          *  can binary search. */
  char*                  names;          /**< Storage allocated for all the path names. */
} rtems_fdt_index;


/**
 * A blob descriptor.
 */
struct rtems_fdt_blob
{
  rtems_chain_node node;        /**< The node's link in the chain. */
  const void*      blob;        /**< The FDT blob. */
  const char*      name;        /**< The name of the blob. */
  int              refs;        /**< The number of active references of the blob. */
  rtems_fdt_index  index;       /**< The index used for quick access to items in the blob. */
};

/**
 * The global FDT data. This structure is allocated on the heap when the first
 * call to load a FDT blob is made and released when all blobs have been
 * unloaded..
 */
typedef struct
{
  rtems_mutex         lock;     /**< The FDT lock id */
  rtems_chain_control blobs;    /**< List if loaded blobs. */
  const char*         paths;    /**< Search paths for blobs. */
} rtems_fdt_data;

static void
rtems_fdt_unlock (rtems_fdt_data *fdt)
{
  rtems_mutex_unlock (&fdt->lock);
}

static rtems_fdt_data*
rtems_fdt_lock (void)
{
  static rtems_fdt_data fdt_instance = {
    .lock = RTEMS_MUTEX_INITIALIZER ("FDT"),
    .blobs = RTEMS_CHAIN_INITIALIZER_EMPTY (fdt_instance.blobs)
  };
  rtems_fdt_data *fdt = &fdt_instance;

  rtems_mutex_lock (&fdt->lock);
  return fdt;
}

/**
 * Create an index based on the contents of an FDT blob.
 */
static int
rtems_fdt_init_index (rtems_fdt_handle* fdt, rtems_fdt_blob* blob)
{
  rtems_fdt_index_entry* entries = NULL;
  int                    num_entries = 0;
  int                    entry = 0;
  size_t                 total_name_memory = 0;
  char                   node_path[256];
  int                    depth_path[32];
  int                    start_offset = 0;
  int                    start_depth = 0;
  int                    offset = 0;
  int                    depth = 0;
  char*                  names = NULL;
  char*                  names_pos = NULL;

  /*
   * Count the number of entries in the blob first.
   */
  memset(&node_path, 0, sizeof(node_path));
  strcpy(node_path, "/");
  depth_path[0] = strlen(node_path);

  start_offset = fdt_path_offset(fdt->blob->blob, node_path);
  if (start_offset < 0)
  {
    return start_offset;
  }

  start_offset = fdt_next_node(fdt->blob->blob, start_offset, &start_depth);
  if (start_offset < 0)
  {
    return start_offset;
  }

  offset = start_offset;
  depth = start_depth;

  while (depth > 0)
  {
    /*
     * Construct the node path name.
     */
    int         namelen = 0;
    const char* name = fdt_get_name(blob->blob, offset, &namelen);
    strncpy(&node_path[depth_path[depth-1]],
            name,
            sizeof(node_path) - depth_path[depth-1] - 1);

    total_name_memory += strlen(node_path) + 1;
    num_entries++;

    if (depth_path[depth-1] + namelen + 2 <= (int)sizeof(node_path))
    {
      strcpy(&node_path[depth_path[depth-1] + namelen], "/");
    }

    depth_path[depth] = depth_path[depth-1] + namelen + 1;

    /*
     * Get the next node.
     */
    offset = fdt_next_node(fdt->blob->blob, offset, &depth);
    if (offset < 0)
    {
      return offset;
    }
  }

  /*
   * Create the index.
   */
  entries = rtems_calloc(num_entries, sizeof(rtems_fdt_index_entry));
  if (!entries)
  {
    return -RTEMS_FDT_ERR_NO_MEMORY;
  }

  names = rtems_calloc(1, total_name_memory);
  if (!names)
  {
    free(entries);
    return -RTEMS_FDT_ERR_NO_MEMORY;
  }

  /*
   * Populate the index.
   */
  offset = start_offset;
  depth = start_depth;
  entry = 0;
  names_pos = names;

  while (depth > 0)
  {
    /*
     * Record this node in an entry.
     */
    int         namelen = 0;
    const char* name = fdt_get_name(blob->blob, offset, &namelen);
    strncpy(&node_path[depth_path[depth-1]],
            name,
            sizeof(node_path) - depth_path[depth-1] - 1);
    strcpy(names_pos, node_path);

    entries[entry].name = names_pos;
    entries[entry].offset = offset;

    names_pos += strlen(node_path) + 1;
    entry++;

    if (depth_path[depth-1] + namelen + 2 <= (int)sizeof(node_path))
    {
      strcpy(&node_path[depth_path[depth-1] + namelen], "/");
    }

    depth_path[depth] = depth_path[depth-1] + namelen + 1;

    /*
     * Get the next node.
     */
    offset = fdt_next_node(fdt->blob->blob, offset, &depth);
    if (offset < 0)
    {
      free(entries);
      free(names);
      return offset;
    }
  }

  fdt->blob->index.entries = entries;
  fdt->blob->index.num_entries = num_entries;
  fdt->blob->index.names = names;

  return 0;
}

/**
 * Release the contents of the index, freeing memory.
 */
static void
rtems_fdt_release_index (rtems_fdt_index* index)
{
  if (index->entries)
  {
    free(index->entries);
    free(index->names);

    index->num_entries = 0;
    index->entries = NULL;
    index->names = NULL;
  }
}

/**
 * For a given FDT path, find the corresponding offset.
 * Returns -1 if not found;
 */
static int
rtems_fdt_index_find_by_name(rtems_fdt_index* index,
                             const char*      name)
{
  int         min = 0;
  int         max = index->num_entries;
  /*
   * Handle trailing slash case.
   */
  size_t namelen = strlen(name);
  if (namelen > 0 && name[namelen-1] == '/')
  {
    namelen--;
  }

  /* Binary search for the name. */
  while (min < max)
  {
    int middle = (min + max) / 2;
    int cmp = strncmp(name, index->entries[middle].name, namelen);
    if (cmp == 0)
    {
      /* 'namelen' characters are equal but 'index->entries[middle].name' */
      /* could have additional characters. */
      if (index->entries[middle].name[namelen] == '\0')
      {
        /* Found it. */
        return index->entries[middle].offset;
      }
      else
      {
         /* 'index->entries[middle].name' is longer than 'name'. */
         cmp = -1;
      }
    }
    if (cmp < 0)
    {
      /* Look lower than here. */
      max = middle;
    }
    else
    {
      /* Look higher than here. */
      min = middle + 1;
    }
 }

  /* Didn't find it. */
  return -FDT_ERR_NOTFOUND;
}

/**
 * For a given FDT offset, find the corresponding path name.
 */
static const char *
rtems_fdt_index_find_name_by_offset(rtems_fdt_index* index,
                                    int              offset)
{
  int min = 0;
  int max = index->num_entries;

  /*
   * Binary search for the offset.
   */
  while (min < max)
  {
    int middle = (min + max) / 2;
    if (offset < index->entries[middle].offset)
    {
      /* Look lower than here. */
      max = middle;
    }
    else if (offset > index->entries[middle].offset)
    {
      /* Look higher than here. */
      min = middle + 1;
    }
    else
    {
      /* Found it. */
      return index->entries[middle].name;
    }
  }

  /* Didn't find it. */
  return NULL;
}

void
rtems_fdt_init_handle (rtems_fdt_handle* handle)
{
  if (handle)
    handle->blob = NULL;
}

void
rtems_fdt_dup_handle (rtems_fdt_handle* from, rtems_fdt_handle* to)
{
  if (from && to)
  {
    rtems_fdt_data* fdt;

    fdt = rtems_fdt_lock ();
    to->blob = from->blob;
    ++to->blob->refs;
    rtems_fdt_unlock (fdt);
  }
}

void
rtems_fdt_release_handle (rtems_fdt_handle* handle)
{
  if (handle && handle->blob)
  {
    rtems_fdt_data*   fdt;
    rtems_chain_node* node;

    fdt = rtems_fdt_lock ();

    node = rtems_chain_first (&fdt->blobs);

    while (!rtems_chain_is_tail (&fdt->blobs, node))
    {
      rtems_fdt_blob* blob = (rtems_fdt_blob*) node;
      if (handle->blob == blob)
      {
        if (blob->refs)
          --blob->refs;
        break;
      }
      node = rtems_chain_next (node);
    }

    rtems_fdt_unlock (fdt);

    handle->blob = NULL;
  }
}

bool
rtems_fdt_valid_handle (const rtems_fdt_handle* handle)
{
  if (handle && handle->blob)
  {
    rtems_fdt_data*   fdt;
    rtems_chain_node* node;

    fdt = rtems_fdt_lock ();

    node = rtems_chain_first (&fdt->blobs);

    while (!rtems_chain_is_tail (&fdt->blobs, node))
    {
      rtems_fdt_blob* blob = (rtems_fdt_blob*) node;
      if (handle->blob == blob)
      {
        rtems_fdt_unlock (fdt);
        return true;
      }
      node = rtems_chain_next (node);
    }

    rtems_fdt_unlock (fdt);
  }

  return false;
}

int
rtems_fdt_find_path_offset (rtems_fdt_handle* handle, const char* path)
{
  rtems_fdt_data*   fdt;
  rtems_chain_node* node;

  rtems_fdt_release_handle (handle);

  fdt = rtems_fdt_lock ();

  node = rtems_chain_first (&fdt->blobs);

  while (!rtems_chain_is_tail (&fdt->blobs, node))
  {
    rtems_fdt_handle temp_handle;
    int              offset;

    temp_handle.blob = (rtems_fdt_blob*) node;

    offset = rtems_fdt_path_offset (&temp_handle, path);

    if (offset >= 0)
    {
      ++temp_handle.blob->refs;
      handle->blob = temp_handle.blob;
      rtems_fdt_unlock (fdt);
      return offset;
    }

    node = rtems_chain_next (node);
  }

  rtems_fdt_unlock (fdt);

  return -FDT_ERR_NOTFOUND;
}

int
rtems_fdt_load (const char* filename, rtems_fdt_handle* handle)
{
  rtems_fdt_data* fdt;
  rtems_fdt_blob* blob;
  size_t          bsize;
  int             bf;
  ssize_t         r;
  size_t          name_len;
  int             fe;
  struct stat     sb;
  uint8_t         gzip_id[2];
  uint8_t*        cdata;
  size_t          size;

  rtems_fdt_release_handle (handle);

  if (stat (filename, &sb) < 0)
  {
    return -RTEMS_FDT_ERR_NOT_FOUND;
  }

  bf = open(filename, O_RDONLY);
  if (bf < 0)
  {
    return -RTEMS_FDT_ERR_READ_FAIL;
  }

  r = read(bf, &gzip_id, sizeof(gzip_id));
  if (r < 0)
  {
    close(bf);
    return -RTEMS_FDT_ERR_READ_FAIL;
  }

  if ((gzip_id[0] == 0x1f) && (gzip_id[1] == 0x8b))
  {
    size_t offset;

    cdata = rtems_malloc(sb.st_size);
    if (!cdata)
    {
      close (bf);
      return -RTEMS_FDT_ERR_NO_MEMORY;
    }

    if (lseek(bf, 0, SEEK_SET) < 0)
    {
      free(cdata);
      close(bf);
      return -RTEMS_FDT_ERR_READ_FAIL;
    }

    size = sb.st_size;
    offset = 0;
    while (size)
    {
      r = read(bf, cdata + offset, size);
      if (r < 0)
      {
        free(cdata);
        close(bf);
        return -RTEMS_FDT_ERR_READ_FAIL;
      }
      size -= r;
      offset += r;
    }

    offset = sb.st_size - 4;
    bsize = ((cdata[offset + 3] << 24) | (cdata[offset + 2] << 16) |
             (cdata[offset + 1] << 8) | cdata[offset + 0]);
  }
  else
  {
    cdata = NULL;
    bsize = sb.st_size;
  }

  name_len = strlen (filename) + 1;

  blob = rtems_malloc(sizeof (rtems_fdt_blob) + name_len + bsize);
  if (!blob)
  {
    free(cdata);
    close (bf);
    return -RTEMS_FDT_ERR_NO_MEMORY;
  }

  blob->name = (const char*) (blob + 1);
  blob->blob = blob->name + name_len + 1;

  strcpy ((char*) blob->name, filename);

  if ((gzip_id[0] == 0x1f) && (gzip_id[1] == 0x8b))
  {
    z_stream stream;
    int      err;
    stream.next_in = (Bytef*) cdata;
    stream.avail_in = (uInt) sb.st_size;
    stream.next_out = (void*) (blob->name + name_len + 1);
    stream.avail_out = (uInt) bsize;
    stream.zalloc = (alloc_func) 0;
    stream.zfree = (free_func) 0;
    err = inflateInit(&stream);
    if (err == Z_OK)
      err = inflateReset2(&stream, 31);
    if (err == Z_OK)
      err = inflate(&stream, Z_FINISH);
    if ((err == Z_OK) || (err == Z_STREAM_END))
      err = inflateEnd(&stream);
    if ((err != Z_OK) || (bsize != stream.total_out))
    {
      free (blob);
      free(cdata);
      close (bf);
      return -RTEMS_FDT_ERR_READ_FAIL;
    }
    free(cdata);
    cdata = NULL;
  }
  else
  {
    char* buf = (char*) blob->name + name_len + 1;
    size = bsize;
    while (size)
    {
      r = read (bf, buf, size);
      if (r < 0)
      {
        free (blob);
        close (bf);
        return -RTEMS_FDT_ERR_READ_FAIL;
      }
      size -= r;
      buf += r;
    }
  }

  fe = fdt_check_header(blob->blob);
  if (fe < 0)
  {
    free (blob);
    close (bf);
    return fe;
  }

  fdt = rtems_fdt_lock ();

  rtems_chain_append_unprotected (&fdt->blobs, &blob->node);

  blob->refs = 1;

  rtems_fdt_unlock (fdt);

  handle->blob = blob;

  fe = rtems_fdt_init_index(handle, blob);
  if (fe < 0)
  {
    free (blob);
    close (bf);
    return fe;
  }

  close (bf);
  return 0;
}

int
rtems_fdt_register (const void* dtb, rtems_fdt_handle* handle)
{
  rtems_fdt_data* fdt;
  rtems_fdt_blob* blob;
  int             fe;

  rtems_fdt_release_handle (handle);

  fe = fdt_check_header(dtb);
  if (fe < 0)
  {
    return fe;
  }

  blob = rtems_malloc(sizeof (rtems_fdt_blob));
  if (!blob)
  {
    return -RTEMS_FDT_ERR_NO_MEMORY;
  }

  blob->blob = dtb;
  blob->name = NULL;
  rtems_chain_initialize_node(&blob->node);

  fdt = rtems_fdt_lock ();

  rtems_chain_append_unprotected (&fdt->blobs, &blob->node);

  blob->refs = 1;

  rtems_fdt_unlock (fdt);

  handle->blob = blob;

  fe = rtems_fdt_init_index(handle, blob);
  if (fe < 0)
  {
    free(blob);
    return -RTEMS_FDT_ERR_NO_MEMORY;
  }

  return 0;
}

int
rtems_fdt_unload (rtems_fdt_handle* handle)
{
  rtems_fdt_data* fdt;

  fdt = rtems_fdt_lock ();

  if (!rtems_fdt_valid_handle (handle))
  {
    rtems_fdt_unlock (fdt);
    return -RTEMS_FDT_ERR_INVALID_HANDLE;
  }

  if (handle->blob->refs > 1)
  {
    rtems_fdt_unlock (fdt);
    return -RTEMS_FDT_ERR_REFERENCED;
  }

  rtems_chain_extract_unprotected (&handle->blob->node);

  rtems_fdt_release_index(&handle->blob->index);

  free (handle->blob);

  handle->blob = NULL;

  rtems_fdt_unlock (fdt);

  return 0;
}

int
rtems_fdt_num_mem_rsv (rtems_fdt_handle* handle)
{
  if (!handle->blob)
    return -RTEMS_FDT_ERR_INVALID_HANDLE;
  return fdt_num_mem_rsv (handle->blob->blob);
}

int
rtems_fdt_get_mem_rsv (rtems_fdt_handle* handle,
                       int               n,
                       uint64_t*         address,
                       uint64_t*         size)
{
  if (!handle->blob)
    return -RTEMS_FDT_ERR_INVALID_HANDLE;
  return fdt_get_mem_rsv (handle->blob->blob, n, address, size);
}

int
rtems_fdt_subnode_offset_namelen (rtems_fdt_handle* handle,
                                  int               parentoffset,
                                  const char*       name,
                                  int               namelen)
{
  if (!handle->blob)
    return -RTEMS_FDT_ERR_INVALID_HANDLE;
  return fdt_subnode_offset_namelen (handle->blob->blob,
                                     parentoffset,
                                     name,
                                     namelen);
}

int
rtems_fdt_subnode_offset (rtems_fdt_handle* handle,
                          int               parentoffset,
                          const char*       name)
{
  char full_name[256];
  const char *path;

  if (!handle->blob)
    return -RTEMS_FDT_ERR_INVALID_HANDLE;

  path = rtems_fdt_index_find_name_by_offset(&handle->blob->index, parentoffset);
  snprintf(full_name, sizeof(full_name), "%s/%s", path, name);

  return rtems_fdt_index_find_by_name(&handle->blob->index, full_name);
}

int
rtems_fdt_path_offset (rtems_fdt_handle* handle, const char* path)
{
  return rtems_fdt_index_find_by_name(&handle->blob->index, path);
}

const char*
rtems_fdt_get_name (rtems_fdt_handle* handle, int nodeoffset, int* length)
{
  if (!handle->blob)
    return NULL;

  const char *name = rtems_fdt_index_find_name_by_offset(&handle->blob->index, nodeoffset);
  if (name && length)
  {
    *length = strlen(name);
  }

  return name;
}

int
rtems_fdt_first_prop_offset(rtems_fdt_handle* handle, int nodeoffset)
{
  return fdt_first_property_offset(handle->blob->blob, nodeoffset);
}

int
rtems_fdt_next_prop_offset(rtems_fdt_handle* handle, int propoffset)
{
  return fdt_next_property_offset(handle->blob->blob, propoffset);
}

const void*
rtems_fdt_getprop_by_offset(rtems_fdt_handle* handle,
                            int               propoffset,
                            const char**      name,
                            int*              length)
{
  return fdt_getprop_by_offset(handle->blob->blob, propoffset, name, length);
}

const void*
rtems_fdt_getprop_namelen (rtems_fdt_handle* handle,
                           int               nodeoffset,
                           const char*       name,
                           int               namelen,
                           int*              length)
{
  if (!handle->blob)
    return NULL;
  return fdt_getprop_namelen (handle->blob->blob,
                              nodeoffset,
                              name,
                              namelen,
                              length);
}

const void*
rtems_fdt_getprop (rtems_fdt_handle* handle,
                   int               nodeoffset,
                   const char*       name,
                   int*              length)
{
  if (!handle->blob)
    return NULL;
  return fdt_getprop (handle->blob->blob,
                      nodeoffset,
                      name,
                      length);
}

uint32_t
rtems_fdt_get_phandle (rtems_fdt_handle* handle, int nodeoffset)
{
  if (!handle->blob)
    return -RTEMS_FDT_ERR_INVALID_HANDLE;
  return fdt_get_phandle (handle->blob->blob, nodeoffset);
}

const char*
rtems_fdt_get_alias_namelen (rtems_fdt_handle* handle,
                             const char*       name,
                             int               namelen)
{
  if (!handle->blob)
    return NULL;
  return fdt_get_alias_namelen (handle->blob->blob, name, namelen);
}

const char*
rtems_fdt_get_alias (rtems_fdt_handle* handle, const char* name)
{
  if (!handle->blob)
    return NULL;
  return fdt_get_alias (handle->blob->blob, name);
}

int
rtems_fdt_get_path (rtems_fdt_handle* handle,
                    int               nodeoffset,
                    char*             buf,
                    int               buflen)
{
  if (!handle->blob)
    return -RTEMS_FDT_ERR_INVALID_HANDLE;
  return fdt_get_path (handle->blob->blob, nodeoffset, buf, buflen);
}

int
rtems_fdt_supernode_atdepth_offset (rtems_fdt_handle* handle,
                                    int               nodeoffset,
                                    int               supernodedepth,
                                    int*              nodedepth)
{
  if (!handle->blob)
    return -RTEMS_FDT_ERR_INVALID_HANDLE;
  return fdt_supernode_atdepth_offset(handle,
                                      nodeoffset,
                                      supernodedepth,
                                      nodedepth);
}

int
rtems_fdt_node_depth (rtems_fdt_handle* handle, int nodeoffset)
{
  if (!handle->blob)
    return -RTEMS_FDT_ERR_INVALID_HANDLE;
  return fdt_node_depth (handle->blob->blob, nodeoffset);
}

int
rtems_fdt_parent_offset (rtems_fdt_handle* handle, int nodeoffset)
{
  if (!handle->blob)
    return -RTEMS_FDT_ERR_INVALID_HANDLE;
  return fdt_parent_offset (handle->blob->blob, nodeoffset);
}

int
rtems_fdt_node_offset_by_prop_value (rtems_fdt_handle* handle,
                                     int               startoffset,
                                     const char*       propname,
                                     const void*       propval,
                                     int               proplen)
{
  if (!handle->blob)
    return -RTEMS_FDT_ERR_INVALID_HANDLE;
  return fdt_node_offset_by_prop_value (handle,
                                        startoffset,
                                        propname,
                                        propval,
                                        proplen);
}

int
rtems_fdt_node_offset_by_phandle (rtems_fdt_handle* handle, uint32_t phandle)
{
  if (!handle->blob)
    return -RTEMS_FDT_ERR_INVALID_HANDLE;
  return fdt_node_offset_by_phandle (handle->blob->blob, phandle);
}

int
rtems_fdt_node_check_compatible (rtems_fdt_handle* handle,
                                 int               nodeoffset,
                                 const char*       compatible)
{
  if (!handle->blob)
    return -RTEMS_FDT_ERR_INVALID_HANDLE;
  return fdt_node_check_compatible (handle, nodeoffset, compatible);
}

int
rtems_fdt_node_offset_by_compatible (rtems_fdt_handle* handle,
                                     int               startoffset,
                                     const char*       compatible)
{
  if (!handle->blob)
    return -RTEMS_FDT_ERR_INVALID_HANDLE;
  return fdt_node_offset_by_compatible (handle->blob->blob,
                                        startoffset,
                                        compatible);
}

int
rtems_fdt_next_node (rtems_fdt_handle* handle, int offset, int* depth)
{
  if (!handle->blob)
    return -RTEMS_FDT_ERR_INVALID_HANDLE;
  return fdt_next_node (handle->blob->blob, offset, depth);
}

const char*
rtems_fdt_strerror (int errval)
{
  const char* errors[] = {
    "invalid handle",
    "no memory",
    "file not found",
    "DTB read fail",
    "blob has references",
    "bad length"
  };
  if (errval > -RTEMS_FDT_ERR_RTEMS_MIN)
    return fdt_strerror (errval);
  if (errval < -RTEMS_FDT_ERR_MAX)
    return "invalid error code";
  return errors[(-errval) - RTEMS_FDT_ERR_RTEMS_MIN];
}

int
rtems_fdt_prop_value(const char* const path,
                     const char* const propname,
                     void*             value,
                     size_t*           size)
{
  rtems_fdt_handle fdt;
  int              node;
  const void*      prop;
  int              length;

  rtems_fdt_init_handle (&fdt);

  node = rtems_fdt_find_path_offset (&fdt, path);
  if (node < 0)
    return node;

  prop = rtems_fdt_getprop(&fdt, node, propname, &length);
  if (length < 0)
  {
    rtems_fdt_release_handle (&fdt);
    return length;
  }

  if (length > (int) *size)
  {
    rtems_fdt_release_handle (&fdt);
    return -RTEMS_FDT_ERR_BADPATH;
  }

  *size = length;

  memcpy (value, prop, length);

  return 0;
}

bool
rtems_fdt_get_parent_prop_value(rtems_fdt_handle* handle,
                                int               nodeoffset,
                                const char*       name,
                                uint32_t*         value)
{
  const void* prop;
  int plen = 0;
  int node = rtems_fdt_parent_offset(handle, nodeoffset);
  if (node < 0)
    return false;
  prop = rtems_fdt_getprop(handle, node, name, &plen);
  if (plen < 0)
    return false;
  *value = rtems_fdt_get_uint32(prop);
  return true;
}

int
rtems_fdt_prop_map(const char* const path,
                   const char* const propname,
                   const char* const names[],
                   uintptr_t*        values,
                   size_t            count)
{
  rtems_fdt_handle fdt;
  int              node;
  size_t           item;

  rtems_fdt_init_handle (&fdt);

  node = rtems_fdt_find_path_offset (&fdt, path);
  if (node < 0)
    return node;

  for (item = 0; item < count; item++)
  {
    const void* prop;
    int         length;
    int         subnode;

    subnode = rtems_fdt_subnode_offset (&fdt, node, names[item]);
    if (subnode < 0)
    {
      rtems_fdt_release_handle (&fdt);
      return subnode;
    }

    prop = rtems_fdt_getprop(&fdt, subnode, propname, &length);
    if (length < 0)
    {
      rtems_fdt_release_handle (&fdt);
      return length;
    }

    if (length > sizeof (uintptr_t))
    {
      rtems_fdt_release_handle (&fdt);
      return -RTEMS_FDT_ERR_BADPATH;
    }

    values[item] = rtems_fdt_get_offset_len_uintptr(prop, 0, length);
  }

  return 0;
}

uintptr_t
rtems_fdt_get_offset_len_uintptr (const void* prop, int offset, int len)
{
  const uint8_t* p = prop;
  uintptr_t      value = 0;
  int            b;
  if (len <= sizeof(uintptr_t)) {
    for (b = 0; b < len; ++b) {
      value = (value << 8) | (uintptr_t) p[offset++];
    }
  }
  return value;
}


uint32_t
rtems_fdt_get_offset_uint32 (const void* prop, int offset)
{
  const uint8_t* p = prop;
  uint32_t       value;
  offset *= sizeof(uint32_t);
  value = ((((uint32_t) p[offset + 0]) << 24) |
           (((uint32_t) p[offset + 1]) << 16) |
           (((uint32_t) p[offset + 2]) << 8)  |
           (uint32_t) p[offset + 3]);
  return value;
}

uint32_t
rtems_fdt_get_uint32 (const void* prop)
{
  return rtems_fdt_get_offset_uint32(prop, 0);
}

uint64_t
rtems_fdt_get_offset_uint64 (const void* prop, int offset)
{
  uint64_t value = rtems_fdt_get_offset_uint32(prop, offset);
  value = (value << 16) << 16;
  return value | rtems_fdt_get_offset_uint32(prop, offset + 1);
}

uint64_t
rtems_fdt_get_uint64 (const void* prop)
{
  return rtems_fdt_get_offset_uint64(prop, 0);
}

uintptr_t
rtems_fdt_get_uintptr (const void* prop)
{
  return rtems_fdt_get_offset_uintptr(prop, 0);
}

uintptr_t
rtems_fdt_get_offset_uintptr (const void* prop, int offset)
{
  if (sizeof(intptr_t) == sizeof(uint32_t))
    return rtems_fdt_get_offset_uint32(prop, offset);
  return rtems_fdt_get_offset_uint64(prop, offset);
}

int
rtems_fdt_get_value (const char* path,
                     const char* property,
                     size_t      size,
                     uintptr_t*  value)
{
  rtems_fdt_handle fdt;
  const void*      prop;
  int              node;
  int              length;

  rtems_fdt_init_handle (&fdt);

  node = rtems_fdt_find_path_offset (&fdt, path);
  if (node < 0)
  {
    rtems_fdt_release_handle (&fdt);
    return node;
  }

  prop = rtems_fdt_getprop(&fdt, node, property, &length);
  if (length < 0)
  {
    rtems_fdt_release_handle (&fdt);
    return length;
  }

  if (length == sizeof (uintptr_t))
    *value = rtems_fdt_get_uintptr (prop);
  else
    *value = 0;

  rtems_fdt_release_handle (&fdt);

  return 0;
}

/**
 * Get the number of entries in an FDT handle.
 */
int
rtems_fdt_num_entries(rtems_fdt_handle* handle)
{
  return handle->blob->index.num_entries;
}

/**
 * Get the numbered entry name. Note that the id isn't the same as
 * the offset - it's numbered 0, 1, 2 ... num_entries-1
 */
const char *
rtems_fdt_entry_name(rtems_fdt_handle* handle, int id)
{
  return handle->blob->index.entries[id].name;
}

/**
 * Get the numbered entry offset. Note that the id isn't the same as
 * the offset - it's numbered 0, 1, 2 ... num_entries-1
 */
int
rtems_fdt_entry_offset(rtems_fdt_handle* handle, int id)
{
  return handle->blob->index.entries[id].offset;
}

int
rtems_fdt_getprop_address_cells(rtems_fdt_handle* handle, int nodeoffset)
{
  uint32_t value = 0;
  if (!rtems_fdt_get_parent_prop_value(handle, nodeoffset, "#address-cells", &value))
    return -1;
  return value;
}

int
rtems_fdt_getprop_size_cells(rtems_fdt_handle* handle, int nodeoffset)
{
  uint32_t value = 0;
  if (!rtems_fdt_get_parent_prop_value(handle, nodeoffset, "#size-cells", &value))
    return -1;
  return value;
}

int rtems_fdt_getprop_address_map(rtems_fdt_handle*      handle,
                                  const char*            path,
                                  const char*            name,
                                  rtems_fdt_address_map* addr_map)
{
  const void* prop;
  int plen = 0;
  int poff = 0;
  int len;

  memset(addr_map, 0, sizeof(*addr_map));

  addr_map->node = rtems_fdt_path_offset(handle, path);
  if (addr_map->node < 0)
    return -RTEMS_FDT_ERR_NOTFOUND;

  addr_map->address_cells = rtems_fdt_getprop_address_cells(handle, addr_map->node);
  addr_map->size_cells = rtems_fdt_getprop_size_cells(handle, addr_map->node);

  prop = rtems_fdt_getprop(handle, addr_map->node, name, &plen);
  if (plen < 0)
    return -RTEMS_FDT_ERR_NOTFOUND;

  if (addr_map->address_cells == 0)
    return -RTEMS_FDT_ERR_BADOFFSET;

  if (addr_map->address_cells < 0)
  {
    if (addr_map->size_cells > 0)
      return -RTEMS_FDT_ERR_BADOFFSET;
    addr_map->address_cells = 1;
  }

  if (addr_map->size_cells < 0)
  {
    addr_map->size = sizeof(uint32_t);
    addr_map->size_cells = 0;
  }

  len = (addr_map->address_cells + addr_map->size_cells) * sizeof(uint32_t);

  if (len != plen)
    return -RTEMS_FDT_ERR_BADLENGTH;

  switch (addr_map->address_cells)
  {
    case 1:
      if (plen < sizeof(uint32_t))
        return -RTEMS_FDT_ERR_BADLENGTH;
      addr_map->address = rtems_fdt_get_offset_uint32(prop, poff);
      poff += 1;
      plen -= sizeof(uint32_t);
      break;
    case 2:
      if (plen < sizeof(uint64_t))
        return -RTEMS_FDT_ERR_BADLENGTH;
      addr_map->address = rtems_fdt_get_offset_uint64(prop, poff);
      poff += 2;
      plen -= sizeof(uint64_t);
      break;
    default:
      return -RTEMS_FDT_ERR_BADLENGTH;
  }

  switch (addr_map->size_cells)
  {
    case 0:
      addr_map->size = sizeof(uint32_t);
      break;
    case 1:
      if (plen < sizeof(uint32_t))
        return -RTEMS_FDT_ERR_BADLENGTH;
      addr_map->size = rtems_fdt_get_offset_uint32(prop, poff);
      poff += 1;
      plen -= sizeof(uint32_t);
      break;
    case 2:
      if (plen < sizeof(uint64_t))
        return -RTEMS_FDT_ERR_BADLENGTH;
      addr_map->size = rtems_fdt_get_offset_uint64(prop, poff);
      poff += 2;
      plen -= sizeof(uint64_t);
      break;
    default:
      return -RTEMS_FDT_ERR_BADLENGTH;
  }

  return 0;
}
