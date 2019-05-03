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
 * @ingroup rtems_rtld
 *
 * @brief RTEMS Run-Time Link Editor
 *
 * This is the RTL implementation.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <rtems/libio_.h>

#include <rtems/rtl/rtl.h>
#include <rtems/rtl/rtl-allocator.h>
#include <rtems/rtl/rtl-trace.h>
#include "rtl-chain-iterator.h"
#include "rtl-error.h"
#include "rtl-string.h"

/**
 * Symbol table cache size. They can be big so the cache needs space to work.
 */
#define RTEMS_RTL_ELF_SYMBOL_CACHE (2048)

/**
 * String table cache size.
 */
#define RTEMS_RTL_ELF_STRING_CACHE (2048)

/**
 * Relocations table cache size.
 */
#define RTEMS_RTL_ELF_RELOC_CACHE (2048)

/**
 * Decompression output buffer.
 */
#define RTEMS_RTL_COMP_OUTPUT (2048)

/**
 * Static RTL data is returned to the user when the linker is locked.
 */
static rtems_rtl_data* rtl;
static bool            rtl_data_init;

/**
 * Define a default base global symbol loader function that is weak
 * so a real table can be linked in when the user wants one.
 */
void rtems_rtl_base_global_syms_init (void) __attribute__ ((weak));
void
rtems_rtl_base_global_syms_init (void)
{
  /*
   * Do nothing.
   */
}

static bool
rtems_rtl_data_init (void)
{
  /*
   * Lock the RTL. We only create a lock if a call is made. First we test if a
   * lock is present. If one is present we lock it. If not the libio lock is
   * locked and we then test the lock again. If not present we create the lock
   * then release libio lock.
   */
  if (!rtl)
  {
    rtems_libio_lock ();

    if (!rtl)
    {
      /*
       * We cannot set an error in this code because there is no RTL data to
       * hold it.
       */

      if (rtl_data_init)
      {
        rtems_libio_unlock ();
        return false;
      }

      rtl_data_init = true;

      /*
       * Always in the heap.
       */
      rtl = malloc (sizeof (rtems_rtl_data));
      if (!rtl)
      {
        rtems_libio_unlock ();
        errno = ENOMEM;
        return false;
      }

      *rtl = (rtems_rtl_data) { 0 };

      /*
       * The initialise the allocator data.
       */
      rtems_rtl_alloc_initialise (&rtl->allocator);

      /*
       * Create the RTL lock.
       */
      rtems_recursive_mutex_init (&rtl->lock, "Run-Time Linker");
      rtems_recursive_mutex_lock (&rtl->lock);

      /*
       * Initialise the objects and pending list.
       */
      rtems_chain_initialize_empty (&rtl->objects);
      rtems_chain_initialize_empty (&rtl->pending);

      /*
       * Open the global symbol table.
       */
      if (!rtems_rtl_symbol_table_open (&rtl->globals,
                                        RTEMS_RTL_SYMS_GLOBAL_BUCKETS))
      {
        rtems_recursive_mutex_destroy (&rtl->lock);
        free (rtl);
        rtems_libio_unlock ();
        return false;
      }

      /*
       * Open the archives.
       */
      rtems_rtl_archives_open (&rtl->archives, "/etc/libdl.conf");

      /*
       * Open the unresolved table.
       */
      if (!rtems_rtl_unresolved_table_open (&rtl->unresolved,
                                            RTEMS_RTL_UNRESOLVED_BLOCK_SIZE))
      {
        rtems_rtl_symbol_table_close (&rtl->globals);
        rtems_recursive_mutex_destroy (&rtl->lock);
        free (rtl);
        rtems_libio_unlock ();
        return false;
      }

      if (!rtems_rtl_obj_cache_open (&rtl->symbols,
                                     RTEMS_RTL_ELF_SYMBOL_CACHE))
      {
        rtems_rtl_symbol_table_close (&rtl->globals);
        rtems_rtl_unresolved_table_close (&rtl->unresolved);
        rtems_recursive_mutex_destroy (&rtl->lock);
        free (rtl);
        rtems_libio_unlock ();
        return false;
      }

      if (!rtems_rtl_obj_cache_open (&rtl->strings,
                                     RTEMS_RTL_ELF_STRING_CACHE))
      {
        rtems_rtl_obj_cache_close (&rtl->symbols);
        rtems_rtl_unresolved_table_close (&rtl->unresolved);
        rtems_rtl_symbol_table_close (&rtl->globals);
        rtems_recursive_mutex_destroy (&rtl->lock);
        free (rtl);
        rtems_libio_unlock ();
        return false;
      }

      if (!rtems_rtl_obj_cache_open (&rtl->relocs,
                                     RTEMS_RTL_ELF_RELOC_CACHE))
      {
        rtems_rtl_obj_cache_close (&rtl->strings);
        rtems_rtl_obj_cache_close (&rtl->symbols);
        rtems_rtl_unresolved_table_close (&rtl->unresolved);
        rtems_rtl_symbol_table_close (&rtl->globals);
        rtems_recursive_mutex_destroy (&rtl->lock);
        free (rtl);
        rtems_libio_unlock ();
        return false;
      }

      if (!rtems_rtl_obj_comp_open (&rtl->decomp,
                                    RTEMS_RTL_COMP_OUTPUT))
      {
        rtems_rtl_obj_cache_close (&rtl->relocs);
        rtems_rtl_obj_cache_close (&rtl->strings);
        rtems_rtl_obj_cache_close (&rtl->symbols);
        rtems_rtl_unresolved_table_close (&rtl->unresolved);
        rtems_rtl_symbol_table_close (&rtl->globals);
        rtems_recursive_mutex_destroy (&rtl->lock);
        free (rtl);
        rtems_libio_unlock ();
        return false;
      }

      rtl->base = rtems_rtl_obj_alloc ();
      if (!rtl->base)
      {
        rtems_rtl_obj_comp_close (&rtl->decomp);
        rtems_rtl_obj_cache_close (&rtl->relocs);
        rtems_rtl_obj_cache_close (&rtl->strings);
        rtems_rtl_obj_cache_close (&rtl->symbols);
        rtems_rtl_unresolved_table_close (&rtl->unresolved);
        rtems_rtl_symbol_table_close (&rtl->globals);
        rtems_recursive_mutex_destroy (&rtl->lock);
        free (rtl);
        rtems_libio_unlock ();
        return false;
      }

      /*
       * Need to malloc the memory so the free does not complain.
       */
      rtl->base->oname = rtems_rtl_strdup ("rtems-kernel");

      /*
       * Lock the base image and flag it as the base image.
       */
      rtl->base->flags |= RTEMS_RTL_OBJ_LOCKED | RTEMS_RTL_OBJ_BASE;

      rtems_chain_append (&rtl->objects, &rtl->base->link);
    }

    rtems_libio_unlock ();

    rtems_rtl_path_append (".");

    rtems_rtl_base_global_syms_init ();

    rtems_rtl_unlock ();
  }
  return true;
}

rtems_rtl_data*
rtems_rtl_data_unprotected (void)
{
  return rtl;
}

rtems_rtl_symbols*
rtems_rtl_global_symbols (void)
{
  if (!rtl)
  {
    rtems_rtl_set_error (ENOENT, "no rtl");
    return NULL;
  }
  return &rtl->globals;
}

const char*
rtems_rtl_last_error_unprotected (void)
{
  if (!rtl)
    return NULL;
  return rtl->last_error;
}

rtems_chain_control*
rtems_rtl_objects_unprotected (void)
{
  if (!rtl)
  {
    rtems_rtl_set_error (ENOENT, "no rtl");
    return NULL;
  }
  return &rtl->objects;
}

rtems_chain_control*
rtems_rtl_pending_unprotected (void)
{
  if (!rtl)
  {
    rtems_rtl_set_error (ENOENT, "no rtl");
    return NULL;
  }
  return &rtl->pending;
}

rtems_rtl_unresolved*
rtems_rtl_unresolved_unprotected (void)
{
  if (!rtl)
  {
    rtems_rtl_set_error (ENOENT, "no rtl");
    return NULL;
  }
  return &rtl->unresolved;
}

rtems_rtl_archives*
rtems_rtl_archives_unprotected (void)
{
  if (!rtl)
  {
    rtems_rtl_set_error (ENOENT, "no rtl");
    return NULL;
  }
  return &rtl->archives;
}

void
rtems_rtl_obj_caches (rtems_rtl_obj_cache** symbols,
                      rtems_rtl_obj_cache** strings,
                      rtems_rtl_obj_cache** relocs)
{
  if (!rtl)
  {
    if (symbols)
       *symbols = NULL;
    if (strings)
      *strings = NULL;
    if (relocs)
      *relocs = NULL;
  }
  else
  {
    if (symbols)
      *symbols = &rtl->symbols;
    if (strings)
      *strings = &rtl->strings;
    if (relocs)
      *relocs = &rtl->relocs;
  }
}

void
rtems_rtl_obj_caches_flush (void)
{
  if (rtl)
  {
    rtems_rtl_obj_cache_flush (&rtl->symbols);
    rtems_rtl_obj_cache_flush (&rtl->strings);
    rtems_rtl_obj_cache_flush (&rtl->relocs);
  }
}

void
rtems_rtl_obj_decompress (rtems_rtl_obj_comp** decomp,
                          rtems_rtl_obj_cache* cache,
                          int                  fd,
                          int                  compression,
                          off_t                offset)
{
  if (!rtl)
  {
    *decomp = NULL;
  }
  else
  {
    *decomp = &rtl->decomp;
    rtems_rtl_obj_comp_set (*decomp, cache, fd, compression, offset);
  }
}

typedef struct rtems_rtl_obj_flags_data
{
  uint32_t clear;   /**< Flags to clear, do not invert. */
  uint32_t set;     /**< Flags to set, applied after clearing. */
} rtems_rtl_obj_flags_data;

static bool
rtems_rtl_obj_flags_iterator (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj* obj              = (rtems_rtl_obj*) node;
  rtems_rtl_obj_flags_data* flags = (rtems_rtl_obj_flags_data*) data;
  if (flags->clear != 0)
    obj->flags &= ~flags->clear;
  if (flags->set != 0)
    obj->flags |= flags->set;
  return true;
}

void
rtems_rtl_obj_update_flags (uint32_t clear, uint32_t set)
{
  rtems_rtl_obj_flags_data flags = {
    .clear = clear,
    .set   = set
  };
  rtems_rtl_chain_iterate (&rtl->objects,
                           rtems_rtl_obj_flags_iterator,
                           &flags);
}

rtems_rtl_data*
rtems_rtl_lock (void)
{
  if (!rtems_rtl_data_init ())
    return NULL;

  rtems_recursive_mutex_lock (&rtl->lock);

  return rtl;
}

void
rtems_rtl_unlock (void)
{
  rtems_recursive_mutex_unlock (&rtl->lock);
}

rtems_rtl_obj*
rtems_rtl_check_handle (void* handle)
{
  rtems_rtl_obj*    obj;
  rtems_chain_node* node;

  obj = handle;
  node = rtems_chain_first (&rtl->objects);

  while (!rtems_chain_is_tail (&rtl->objects, node))
  {
    rtems_rtl_obj* check = (rtems_rtl_obj*) node;
    if (check == obj)
      return obj;
    node = rtems_chain_next (node);
  }

  return NULL;
}

rtems_rtl_obj*
rtems_rtl_find_obj (const char* name)
{
  rtems_chain_node* node;
  rtems_rtl_obj*    found = NULL;
  const char*       aname = NULL;
  const char*       oname = NULL;
  off_t             ooffset;

  if (!rtems_rtl_parse_name (name, &aname, &oname, &ooffset))
    return NULL;

  node = rtems_chain_first (&rtl->objects);

  while (!rtems_chain_is_tail (&rtl->objects, node))
  {
    rtems_rtl_obj* obj = (rtems_rtl_obj*) node;
    if ((aname == NULL && strcmp (obj->oname, oname) == 0) ||
        (aname != NULL &&
         strcmp (obj->aname, aname) == 0 && strcmp (obj->oname, oname) == 0))
    {
      found = obj;
      break;
    }
    node = rtems_chain_next (node);
  }

  if (aname != NULL)
    rtems_rtl_alloc_del(RTEMS_RTL_ALLOC_OBJECT, (void*) aname);

  if (oname != NULL)
    rtems_rtl_alloc_del(RTEMS_RTL_ALLOC_OBJECT, (void*) oname);

  return found;
}

rtems_rtl_obj*
rtems_rtl_find_obj_with_symbol (const rtems_rtl_obj_sym* sym)
{
  if (sym != NULL)
  {
    rtems_chain_node* node = rtems_chain_first (&rtl->objects);
    while (!rtems_chain_is_tail (&rtl->objects, node))
    {
      rtems_rtl_obj* obj = (rtems_rtl_obj*) node;
      if (rtems_rtl_obj_has_symbol (obj, sym))
        return obj;
      node = rtems_chain_next (node);
    }
    node = rtems_chain_first (&rtl->pending);
    while (!rtems_chain_is_tail (&rtl->pending, node))
    {
      rtems_rtl_obj* obj = (rtems_rtl_obj*) node;
      if (rtems_rtl_obj_has_symbol (obj, sym))
        return obj;
      node = rtems_chain_next (node);
    }
  }
  return NULL;
}

rtems_rtl_obj*
rtems_rtl_load_object (const char* name, int mode)
{
  rtems_rtl_obj* obj;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_LOAD))
    printf ("rtl: loading '%s'\n", name);

  /*
   * See if the object module has already been loaded.
   */
  obj = rtems_rtl_find_obj (name);
  if (!obj)
  {
    /*
     * Allocate a new object file descriptor and attempt to load it.
     */
    obj = rtems_rtl_obj_alloc ();
    if (obj == NULL)
    {
      rtems_rtl_set_error (ENOMEM, "no memory for object descriptor");
      return NULL;
    }

    rtems_chain_append (&rtl->pending, &obj->link);

    /*
     * Find the file in the file system using the search path. The fname field
     * will point to a valid file name if found.
     */
    if (!rtems_rtl_obj_find_file (obj, name))
    {
      rtems_rtl_obj_free (obj);
      rtems_rtl_obj_caches_flush ();
      return NULL;
    }

    if (!rtems_rtl_obj_load (obj))
    {
      rtems_chain_extract (&obj->link);
      rtems_rtl_obj_free (obj);
      rtems_rtl_obj_caches_flush ();
      return NULL;
    }

    /*
     * If there are unresolved externals remove from the pending queue and place
     * on the objects list until the symbols are resolved.
     */
    if (obj->unresolved != 0)
    {
      rtems_chain_extract (&obj->link);
      rtems_chain_append (&rtl->objects, &obj->link);
    }

    rtems_rtl_obj_caches_flush ();
  }

  /*
   * Increase the number of users.
   */
  ++obj->users;

  return obj;
}

rtems_rtl_obj*
rtems_rtl_load (const char* name, int mode)
{
  rtems_rtl_obj* obj;

  /*
   * Refesh the archives.
   */
  rtems_rtl_archives_refresh (&rtl->archives);

  /*
   * Collect the loaded object files.
   */
  rtems_chain_initialize_empty (&rtl->pending);

  obj = rtems_rtl_load_object (name, mode);
  if (obj != NULL)
  {
    rtems_chain_node* node;

    rtems_rtl_unresolved_resolve ();

    /*
     * Iterator over the pending list of object files that have been loaded.
     */
    node = rtems_chain_first (&rtl->pending);
    while (!rtems_chain_is_tail (&rtl->pending, node))
    {
      rtems_rtl_obj* pobj = (rtems_rtl_obj*) node;

      /*
       * Move to the next pending object file and place this object file on the
       * RTL's objects list.
       */
      node = rtems_chain_next (&pobj->link);
      rtems_chain_extract (&pobj->link);
      rtems_chain_append (&rtl->objects, &pobj->link);

      /*
       * Make sure the object file and cache is synchronized.
       */
      rtems_rtl_obj_synchronize_cache (pobj);

      /*
       * Run any local constructors if they have not been run. Unlock the linker
       * to avoid any dead locks if the object file needs to load files or
       * update the symbol table. We also do not want a constructor to unload
       * this object file.
       */
      if ((pobj->flags & RTEMS_RTL_OBJ_CTOR_RUN) == 0)
      {
        pobj->flags |= RTEMS_RTL_OBJ_LOCKED | RTEMS_RTL_OBJ_CTOR_RUN;
        rtems_rtl_unlock ();
        rtems_rtl_obj_run_ctors (pobj);
        rtems_rtl_lock ();
        pobj->flags &= ~RTEMS_RTL_OBJ_LOCKED;
      }
    }
  }

  return obj;
}

bool
rtems_rtl_unload_object (rtems_rtl_obj* obj)
{
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNLOAD))
    printf ("rtl: unload object '%s'\n", rtems_rtl_obj_fname (obj));

  /*
   * If the object is locked it cannot be unloaded and the unload fails.
   */
  if ((obj->flags & RTEMS_RTL_OBJ_LOCKED) == RTEMS_RTL_OBJ_LOCKED)
  {
    rtems_rtl_set_error (EINVAL, "object file is locked");
    return false;
  }

  /*
   * The object file cannot be unloaded if it is referenced.
   */
  if (rtems_rtl_obj_get_reference (obj) > 0)
  {
    rtems_rtl_set_error (EINVAL, "object file has references to it");
    return false;
  }

  /*
   * Check the number of users in a safe manner. If this is the last user unload
   * the object file from memory.
   */
  if (obj->users > 0)
    --obj->users;

  return true;
}

bool
rtems_rtl_unload (rtems_rtl_obj* obj)
{
  bool ok = rtems_rtl_unload_object (obj);
  if (ok && obj->users == 0)
  {
    rtems_chain_control unloading;
    rtems_chain_node*   node;
    bool                orphaned_found = true;
    int                 loop = 0;

    /*
     * Remove the orphaned object files from the objects list. The unloading is
     * private so any changes in any desctructors will not effect the list as it
     * is being iterated over.
     *
     * To avoid maintaining a complex tree loop while oprhans are still be found.
     */

    rtems_chain_initialize_empty (&unloading);

    while (orphaned_found)
    {
      orphaned_found = false;
      ++loop;
      node = rtems_chain_first (&rtl->objects);
      while (!rtems_chain_is_tail (&rtl->objects, node))
      {
        rtems_chain_node* next_node = rtems_chain_next (node);
        rtems_rtl_obj* uobj = (rtems_rtl_obj*) node;
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNLOAD))
          printf ("rtl: unload object: %3i: %9s: %s\n",
                  loop,
                  rtems_rtl_obj_orphaned (uobj) ? "orphaned" : "inuse",
                  rtems_rtl_obj_oname (uobj));
        if (rtems_rtl_obj_orphaned (uobj))
        {
          orphaned_found = true;
          rtems_rtl_obj_remove_dependencies (uobj);
          rtems_chain_extract (&uobj->link);
          rtems_chain_append (&unloading, &uobj->link);
          uobj->flags |= RTEMS_RTL_OBJ_LOCKED;
        }
        node = next_node;
      }
    }

    /*
     * Call the desctructors unlocked. An RTL call will not deadlock.
     */
    rtems_rtl_unlock ();

    node = rtems_chain_first (&unloading);
    while (!rtems_chain_is_tail (&unloading, node))
    {
      rtems_rtl_obj* uobj = (rtems_rtl_obj*) node;
      if ((uobj->flags & RTEMS_RTL_OBJ_CTOR_RUN) != 0)
      {
        rtems_rtl_obj_run_dtors (uobj);
        uobj->flags &= ~RTEMS_RTL_OBJ_CTOR_RUN;
      }
      node = rtems_chain_next (node);
    }

    rtems_rtl_lock ();

    /*
     * Unload the object files.
     */
    node = rtems_chain_first (&unloading);
    while (!rtems_chain_is_tail (&unloading, node))
    {
      rtems_chain_node* next_node = rtems_chain_next (node);
      rtems_rtl_obj*    uobj = (rtems_rtl_obj*) node;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNLOAD))
        printf ("rtl: unloading '%s'\n", rtems_rtl_obj_oname (uobj));
      uobj->flags &= ~RTEMS_RTL_OBJ_LOCKED;
      if (!rtems_rtl_obj_unload (uobj))
        ok = false;
      rtems_rtl_obj_free (uobj);
      rtems_rtl_obj_caches_flush ();
      node = next_node;
    }
  }
  return ok;
}

static bool
rtems_rtl_path_update (bool prepend, const char* path)
{
  char*       paths;
  const char* src = NULL;
  char*       dst;
  int         len;

  if (!rtems_rtl_lock ())
    return false;

  len = strlen (path);

  if (rtl->paths)
    len += strlen (rtl->paths) + 1;
  else
    prepend = true;

  paths = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT, len + 1, false);

  if (!paths)
  {
    rtems_rtl_unlock ();
    return false;
  }

  dst = paths;

  if (prepend)
  {
    len = strlen (path);
    src = path;
  }
  else if (rtl->paths)
  {
    len = strlen (rtl->paths);
    src = rtl->paths;
  }

  memcpy (dst, src, len);

  dst += len;

  if (rtl->paths)
  {
    *dst = ':';
    ++dst;
  }

  if (prepend)
  {
    src = rtl->paths;
    if (src)
      len = strlen (src);
  }
  else
  {
    len = strlen (path);
    src = path;
  }

  if (src)
  {
    memcpy (dst, src, len);
    dst += len;
  }

  *dst = '\0';

  rtl->paths = paths;

  rtems_rtl_unlock ();
  return false;
}

bool
rtems_rtl_path_append (const char* path)
{
  return rtems_rtl_path_update (false, path);
}

bool
rtems_rtl_path_prepend (const char* path)
{
  return rtems_rtl_path_update (true, path);
}

void
rtems_rtl_base_sym_global_add (const unsigned char* esyms,
                               unsigned int         size)
{
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_GLOBAL_SYM))
    printf ("rtl: adding global symbols, table size %u\n", size);

  if (!rtems_rtl_lock ())
  {
    rtems_rtl_set_error (EINVAL, "global add cannot lock rtl");
    return;
  }

  rtems_rtl_symbol_global_add (rtl->base, esyms, size);

  rtems_rtl_unlock ();
}

rtems_rtl_obj*
rtems_rtl_baseimage (void)
{
  rtems_rtl_obj* base = NULL;
  if (rtems_rtl_lock ())
  {
    base = rtl->base;
    rtems_rtl_unlock ();
  }
  return base;
}
