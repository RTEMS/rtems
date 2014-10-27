/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
#include "rtl-allocator.h"
#include "rtl-error.h"
#include "rtl-string.h"
#include "rtl-trace.h"

/**
 * Semaphore configuration to create a mutex.
 */
#define RTEMS_MUTEX_ATTRIBS \
  (RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE | \
   RTEMS_INHERIT_PRIORITY | RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL)

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
static rtems_rtl_data_t* rtl;

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
      rtems_status_code sc;
      rtems_id          lock;

      /*
       * Always in the heap.
       */
      rtl = malloc (sizeof (rtems_rtl_data_t));
      if (!rtl)
      {
        errno = ENOMEM;
        return false;
      }

      *rtl = (rtems_rtl_data_t) { 0 };

      /*
       * The initialise the allocator data.
       */
      rtems_rtl_alloc_initialise (&rtl->allocator);

      /*
       * Create the RTL lock.
       */
      sc = rtems_semaphore_create (rtems_build_name ('R', 'T', 'L', 'D'),
                                   1, RTEMS_MUTEX_ATTRIBS,
                                   RTEMS_NO_PRIORITY, &lock);
      if (sc != RTEMS_SUCCESSFUL)
      {
        free (rtl);
        return false;
      }

      sc = rtems_semaphore_obtain (lock, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      if (sc != RTEMS_SUCCESSFUL)
      {
        rtems_semaphore_delete (lock);
        free (rtl);
        return false;
      }

      rtl->lock = lock;

      /*
       * Initialise the objects list and create any required services.
       */
      rtems_chain_initialize_empty (&rtl->objects);

      if (!rtems_rtl_symbol_table_open (&rtl->globals,
                                        RTEMS_RTL_SYMS_GLOBAL_BUCKETS))
      {
        rtems_semaphore_delete (lock);
        free (rtl);
        return false;
      }

      if (!rtems_rtl_unresolved_table_open (&rtl->unresolved,
                                            RTEMS_RTL_UNRESOLVED_BLOCK_SIZE))
      {
        rtems_rtl_symbol_table_close (&rtl->globals);
        rtems_semaphore_delete (lock);
        free (rtl);
        return false;
      }

      if (!rtems_rtl_obj_cache_open (&rtl->symbols,
                                     RTEMS_RTL_ELF_SYMBOL_CACHE))
      {
        rtems_rtl_symbol_table_close (&rtl->globals);
        rtems_rtl_unresolved_table_close (&rtl->unresolved);
        rtems_semaphore_delete (lock);
        free (rtl);
        return false;
      }

      if (!rtems_rtl_obj_cache_open (&rtl->strings,
                                     RTEMS_RTL_ELF_STRING_CACHE))
      {
        rtems_rtl_obj_cache_close (&rtl->symbols);
        rtems_rtl_unresolved_table_close (&rtl->unresolved);
        rtems_rtl_symbol_table_close (&rtl->globals);
        rtems_semaphore_delete (lock);
        free (rtl);
        return false;
      }

      if (!rtems_rtl_obj_cache_open (&rtl->relocs,
                                     RTEMS_RTL_ELF_RELOC_CACHE))
      {
        rtems_rtl_obj_cache_close (&rtl->strings);
        rtems_rtl_obj_cache_close (&rtl->symbols);
        rtems_rtl_unresolved_table_close (&rtl->unresolved);
        rtems_rtl_symbol_table_close (&rtl->globals);
        rtems_semaphore_delete (lock);
        free (rtl);
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
        rtems_semaphore_delete (lock);
        free (rtl);
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
        rtems_semaphore_delete (lock);
        free (rtl);
        return false;
      }

      /*
       * Need to malloc the memory so the free does not complain.
       */
      rtl->base->oname = rtems_rtl_strdup ("rtems-kernel");

      rtems_chain_append (&rtl->objects, &rtl->base->link);
    }

    rtems_libio_unlock ();

    rtems_rtl_path_append (".");

    rtems_rtl_base_global_syms_init ();

    rtems_rtl_unlock ();
  }
  return true;
}

rtems_rtl_data_t*
rtems_rtl_data (void)
{
  return rtl;
}

rtems_rtl_symbols_t*
rtems_rtl_global_symbols (void)
{
  if (!rtl)
  {
    rtems_rtl_set_error (ENOENT, "no rtl");
    return NULL;
  }
  return &rtl->globals;
}

rtems_rtl_unresolved_t*
rtems_rtl_unresolved (void)
{
  if (!rtl)
  {
    rtems_rtl_set_error (ENOENT, "no rtl");
    return NULL;
  }
  return &rtl->unresolved;
}

void
rtems_rtl_obj_caches (rtems_rtl_obj_cache_t** symbols,
                      rtems_rtl_obj_cache_t** strings,
                      rtems_rtl_obj_cache_t** relocs)
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
rtems_rtl_obj_caches_flush ()
{
  if (rtl)
  {
    rtems_rtl_obj_cache_flush (&rtl->symbols);
    rtems_rtl_obj_cache_flush (&rtl->strings);
    rtems_rtl_obj_cache_flush (&rtl->relocs);
  }
}

void
rtems_rtl_obj_comp (rtems_rtl_obj_comp_t** decomp,
                    rtems_rtl_obj_cache_t* cache,
                    int                    fd,
                    int                    compression,
                    off_t                  offset)
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

rtems_rtl_data_t*
rtems_rtl_lock (void)
{
  rtems_status_code sc;

  if (!rtems_rtl_data_init ())
    return NULL;

  sc = rtems_semaphore_obtain (rtl->lock,
                               RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
  {
    errno = EINVAL;
    return NULL;
  }

  return rtl;
}

bool
rtems_rtl_unlock (void)
{
  /*
   * Not sure any error should be returned or an assert.
   */
  rtems_status_code sc;
  sc = rtems_semaphore_release (rtl->lock);
  if ((sc != RTEMS_SUCCESSFUL) && (errno == 0))
  {
    errno = EINVAL;
    return false;
  }
  return true;
}

rtems_rtl_obj_t*
rtems_rtl_check_handle (void* handle)
{
  rtems_rtl_obj_t*    obj;
  rtems_chain_node* node;

  obj = handle;
  node = rtems_chain_first (&rtl->objects);

  while (!rtems_chain_is_tail (&rtl->objects, node))
  {
    rtems_rtl_obj_t* check = (rtems_rtl_obj_t*) node;
    if (check == obj)
      return obj;
    node = rtems_chain_next (node);
  }

  return NULL;
}

rtems_rtl_obj_t*
rtems_rtl_find_obj (const char* name)
{
  rtems_chain_node* node;
  rtems_rtl_obj_t*  found = NULL;
  const char*       aname = NULL;
  const char*       oname = NULL;
  off_t             ooffset;

  if (!rtems_rtl_parse_name (name, &aname, &oname, &ooffset))
    return NULL;

  node = rtems_chain_first (&rtl->objects);

  while (!rtems_chain_is_tail (&rtl->objects, node))
  {
    rtems_rtl_obj_t* obj = (rtems_rtl_obj_t*) node;
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

rtems_rtl_obj_t*
rtems_rtl_load_object (const char* name, int mode)
{
  rtems_rtl_obj_t* obj;

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
      return NULL;
    }

    rtems_chain_append (&rtl->objects, &obj->link);

    if (!rtems_rtl_obj_load (obj))
    {
      rtems_rtl_obj_free (obj);
      return NULL;
    }

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
rtems_rtl_unload_object (rtems_rtl_obj_t* obj)
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
    obj->flags |= RTEMS_RTL_OBJ_LOCKED;
    rtems_rtl_unlock ();
    rtems_rtl_obj_run_dtors (obj);
    rtems_rtl_lock ();
    obj->flags &= ~RTEMS_RTL_OBJ_LOCKED;

    ok = rtems_rtl_obj_unload (obj);
  }

  return ok;
}

void
rtems_rtl_run_ctors (rtems_rtl_obj_t* obj)
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

rtems_rtl_obj_t*
rtems_rtl_baseimage (void)
{
  return NULL;
}
