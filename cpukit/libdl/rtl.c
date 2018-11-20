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
#include "rtl-error.h"
#include "rtl-string.h"
#include <rtems/rtl/rtl-trace.h>

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
       * Initialise the objects list and create any required services.
       */
      rtems_chain_initialize_empty (&rtl->objects);

      if (!rtems_rtl_symbol_table_open (&rtl->globals,
                                        RTEMS_RTL_SYMS_GLOBAL_BUCKETS))
      {
        rtems_recursive_mutex_destroy (&rtl->lock);
        free (rtl);
        rtems_libio_unlock ();
        return false;
      }

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

void
rtems_rtl_obj_update_flags (uint32_t clear, uint32_t set)
{
  rtems_chain_node* node  = rtems_chain_first (&rtl->objects);
  while (!rtems_chain_is_tail (&rtl->objects, node))
  {
    rtems_rtl_obj* obj = (rtems_rtl_obj*) node;
    if (clear != 0)
      obj->flags &= ~clear;
    if (set != 0)
      obj->flags |= set;
    node = rtems_chain_next (node);
  }
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

  if (!aname)
    rtems_rtl_alloc_del(RTEMS_RTL_ALLOC_OBJECT, (void*) aname);

  if (!oname)
    rtems_rtl_alloc_del(RTEMS_RTL_ALLOC_OBJECT, (void*) oname);

  return found;
}

rtems_rtl_obj*
rtems_rtl_find_obj_with_symbol (const rtems_rtl_obj_sym* sym)
{
  rtems_chain_node* node = rtems_chain_first (&rtl->objects);
  while (!rtems_chain_is_tail (&rtl->objects, node))
  {
    rtems_rtl_obj* obj = (rtems_rtl_obj*) node;
    if (sym >= obj->global_table &&
        sym < (obj->global_table + obj->global_syms))
      return obj;
    node = rtems_chain_next (node);
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

    rtems_chain_append (&rtl->objects, &obj->link);

    if (!rtems_rtl_obj_load (obj))
    {
      rtems_rtl_obj_free (obj);
      rtems_rtl_obj_caches_flush ();
      return NULL;
    }

    rtems_rtl_obj_caches_flush ();

    rtems_rtl_unresolved_resolve ();
  }

  /*
   * Increase the number of users.
   */
  ++obj->users;

  /*
   * FIXME: Resolving existing unresolved symbols could add more constructors
   *        lists that need to be called. Make a list in the obj load layer and
   *        invoke the list here.
   */

  /*
   * Run any local constructors if this is the first user because the object
   * file will have just been loaded. Unlock the linker to avoid any dead locks
   * if the object file needs to load files or update the symbol table. We also
   * do not want a constructor to unload this object file.
   */
  if (obj->users == 1)
  {
    obj->flags |= RTEMS_RTL_OBJ_LOCKED;
    rtems_rtl_unlock ();
    rtems_rtl_obj_run_ctors (obj);
    rtems_rtl_lock ();
    obj->flags &= ~RTEMS_RTL_OBJ_LOCKED;
  }

  return obj;
}

bool
rtems_rtl_unload_object (rtems_rtl_obj* obj)
{
  bool ok = true;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNLOAD))
    printf ("rtl: unloading '%s'\n", rtems_rtl_obj_fname (obj));

  /*
   * If the object is locked it cannot be unloaded and the unload fails.
   */
  if ((obj->flags & RTEMS_RTL_OBJ_LOCKED) == RTEMS_RTL_OBJ_LOCKED)
  {
    rtems_rtl_set_error (EINVAL, "cannot unload when locked");
    return false;
  }

  /*
   * Check the number of users in a safe manner. If this is the last user unload the
   * object file from memory.
   */
  if (obj->users > 0)
    --obj->users;

  if (obj->users == 0)
  {
    if (obj->refs != 0)
    {
      rtems_rtl_set_error (EBUSY, "object file referenced");
      return false;
    }
    obj->flags |= RTEMS_RTL_OBJ_LOCKED;
    rtems_rtl_unlock ();
    rtems_rtl_obj_run_dtors (obj);
    rtems_rtl_lock ();
    obj->flags &= ~RTEMS_RTL_OBJ_LOCKED;

    ok = rtems_rtl_obj_unload (obj);

    rtems_rtl_obj_free (obj);
    rtems_rtl_obj_caches_flush ();
  }

  return ok;
}

void
rtems_rtl_run_ctors (rtems_rtl_obj* obj)
{
  rtems_rtl_obj_run_ctors (obj);
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
