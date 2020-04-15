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
 * @brief RTEMS Run-Time Linker Object File Unresolved Relocations Table.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <rtems/rtl/rtl.h>
#include "rtl-error.h"
#include <rtems/rtl/rtl-unresolved.h>
#include <rtems/rtl/rtl-trace.h>
#include "rtl-trampoline.h"

static rtems_rtl_unresolv_block*
rtems_rtl_unresolved_block_alloc (rtems_rtl_unresolved* unresolved)
{
  /*
   * The block header contains a record.
   */
  size_t size =
    (sizeof(rtems_rtl_unresolv_block) +
     (sizeof(rtems_rtl_unresolv_rec) * unresolved->block_recs));
  rtems_rtl_unresolv_block* block =
    rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_EXTERNAL, size, true);
  if (block)
  {
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
      printf ("rtl: unresolv: block-alloc %p (%p)\n", block, block + size);
    rtems_chain_append (&unresolved->blocks, &block->link);
  }
  else
    rtems_rtl_set_error (ENOMEM, "no memory for unresolved block");
  return block;
}

static size_t
rtems_rtl_unresolved_symbol_rec_count (size_t length)
{
  const size_t rec_size = sizeof(rtems_rtl_unresolv_rec);
  const size_t rec_name_header = offsetof(rtems_rtl_unresolv_rec, rec.name.name);
  /*
   * Add on the nul and rmeove 1 to be inside a record.
   */
  return ((length + rec_name_header - 1) / rec_size) + 1;
}

static size_t
rtems_rtl_unresolved_symbol_recs (const char* name)
{
  return rtems_rtl_unresolved_symbol_rec_count (strlen (name) + 1);
}

static int
rtems_rtl_unresolved_rec_index (rtems_rtl_unresolv_block* block,
                                rtems_rtl_unresolv_rec*   rec)
{
  return rec - &block->rec[0];
}

static rtems_rtl_unresolv_rec*
rtems_rtl_unresolved_rec_first (rtems_rtl_unresolv_block* block)
{
  return &block->rec[0];
}

static rtems_rtl_unresolv_rec*
rtems_rtl_unresolved_rec_next (rtems_rtl_unresolv_rec* rec)
{
  switch (rec->type)
  {
    case rtems_rtl_unresolved_empty:
    default:
      /*
       * Empty returns NULL. The end of the records in the block.
       */
      rec = NULL;
      break;

    case rtems_rtl_unresolved_symbol:
      /*
       * Determine how many records the name occupies. Round up.
       */
      rec += rtems_rtl_unresolved_symbol_rec_count (rec->rec.name.length);
      break;

    case rtems_rtl_unresolved_reloc:
    case rtems_rtl_trampoline_reloc:
      ++rec;
      break;
  }

  return rec;
}

static bool
rtems_rtl_unresolved_rec_is_last (rtems_rtl_unresolv_block* block,
                                  rtems_rtl_unresolv_rec*   rec)
{
  int index = rtems_rtl_unresolved_rec_index (block, rec);
  return (rec == NULL ||
          (index < 0) ||
          (index >= block->recs) ||
          (rec->type == rtems_rtl_unresolved_empty));
}

static rtems_rtl_unresolv_rec*
rtems_rtl_unresolved_rec_first_free (rtems_rtl_unresolv_block* block)
{
  return &block->rec[0] + block->recs;
}

/**
 * Name management iterator data.
 */
typedef struct
{
  const char*             name;   /**< The name being searched for. */
  size_t                  length; /**< The length of the name. */
  rtems_rtl_unresolv_rec* rec;    /**< The record being searched for. */
  int                     index;  /**< The name's index. */
  int                     offset; /**< The offset to move the index. */
} rtl_unresolved_name_data;

static bool
rtems_rtl_unresolved_find_name_iterator (rtems_rtl_unresolv_rec* rec,
                                         void*                   data)
{
  rtl_unresolved_name_data* nd = (rtl_unresolved_name_data*) data;
  if (rec->type == rtems_rtl_unresolved_symbol)
  {
    if ((rec->rec.name.length == nd->length)
        && (strcmp (rec->rec.name.name, nd->name) == 0))
    {
      ++rec->rec.name.refs;
      return true;
    }
    ++nd->index;
  }
  return false;
}

static int
rtems_rtl_unresolved_find_name (const char* name)
{
  rtl_unresolved_name_data nd = {
    .name = name,
    .length = strlen (name) + 1,
    .rec = NULL,
    .index = 1,
    .offset = 0
  };
  if (rtems_rtl_unresolved_iterate (rtems_rtl_unresolved_find_name_iterator,
                                    &nd))
    return nd.index;
  return -1;
}

static bool
rtems_rtl_unresolved_find_index_iterator (rtems_rtl_unresolv_rec* rec,
                                          void*                   data)
{
  rtl_unresolved_name_data* nd = (rtl_unresolved_name_data*) data;
  if (rec == nd->rec)
    return true;
  if (rec->type == rtems_rtl_unresolved_symbol)
    ++nd->index;
  return false;
}

static int
rtems_rtl_unresolved_find_index (rtems_rtl_unresolv_rec* rec)
{
  rtl_unresolved_name_data nd = {
    .name = NULL,
    .length = 0,
    .rec = rec,
    .index = 1,
    .offset = 0
  };
  if (rtems_rtl_unresolved_iterate (rtems_rtl_unresolved_find_index_iterator,
                                    &nd))
    return nd.index;
  return -1;
}

static bool
rtems_rtl_unresolved_reindex_iterator (rtems_rtl_unresolv_rec* rec,
                                       void*                   data)
{
  rtl_unresolved_name_data* nd = (rtl_unresolved_name_data*) data;
  /*
   * Reindexing only effects the reloc records.
   */
  if (rec->type == rtems_rtl_unresolved_reloc)
  {
    if (rec->rec.reloc.name >= nd->index)
      rec->rec.reloc.name += nd->offset;
  }
  return false;
}

static void
rtems_rtl_unresolved_reindex_names (uint16_t index, int offset)
{
  rtl_unresolved_name_data nd = {
    .name = NULL,
    .length = 0,
    .rec = NULL,
    .index = index,
    .offset = offset
  };
  rtems_rtl_unresolved_iterate (rtems_rtl_unresolved_reindex_iterator,
                                &nd);
}

/**
 * Struct to pass relocation data in the iterator.
 */
typedef struct rtems_rtl_unresolved_reloc_data
{
  uint16_t                name;     /**< Name index. */
  rtems_rtl_unresolv_rec* name_rec; /**< Name record. */
  rtems_rtl_obj_sym*      sym;      /**< The symbol record. */
} rtems_rtl_unresolved_reloc_data;

static bool
rtems_rtl_unresolved_resolve_reloc (rtems_rtl_unresolv_rec* rec,
                                    void*                   data)
{
  if (rec->type == rtems_rtl_unresolved_reloc)
  {
    rtems_chain_control*             pending;
    rtems_rtl_unresolved_reloc_data* rd;

    rd = (rtems_rtl_unresolved_reloc_data*) data;

    if (rec->rec.reloc.name == rd->name && rec->rec.reloc.obj != NULL)
    {
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
        printf ("rtl: unresolv: resolve reloc: %s\n",
                rd->name_rec->rec.name.name);

      if (rtems_rtl_obj_relocate_unresolved (&rec->rec.reloc, rd->sym))
      {
        /*
         * If all unresolved externals are resolved add the obj module
         * to the pending queue. This will flush the object module's
         * data from the cache and call it's constructors.
         */
        if (rec->rec.reloc.obj->unresolved == 0)
        {
          pending = rtems_rtl_pending_unprotected ();
          rtems_chain_extract (&rec->rec.reloc.obj->link);
          rtems_chain_append (pending, &rec->rec.reloc.obj->link);
        }

        /*
         * Set the object pointer to NULL to indicate the record is
         * not used anymore. Update the reference count of the name so
         * it can garbage collected if not referenced. The sweep after
         * relocating will remove the reloc records with obj set to
         * NULL and names with a reference count of 0.
         */
        rec->rec.reloc.obj = NULL;
        if (rd->name_rec != NULL && rd->name_rec->rec.name.refs > 0)
          --rd->name_rec->rec.name.refs;
      }
    }
  }
  return false;
}

static bool
rtems_rtl_unresolved_resolve_iterator (rtems_rtl_unresolv_rec* rec,
                                       void*                   data)
{
  if (rec->type == rtems_rtl_unresolved_symbol)
  {
    rtems_rtl_unresolved_reloc_data* rd;
    rd = (rtems_rtl_unresolved_reloc_data*) data;

    ++rd->name;

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
      printf ("rtl: unresolv: lookup: %d: %s\n", rd->name, rec->rec.name.name);

    rd->sym = rtems_rtl_symbol_global_find (rec->rec.name.name);

    if (rd->sym)
    {
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
        printf ("rtl: unresolv: found: %s\n", rec->rec.name.name);

      rd->name_rec = rec;

      rtems_rtl_unresolved_iterate (rtems_rtl_unresolved_resolve_reloc, rd);

      rd->name_rec = NULL;
      rd->sym = NULL;
    }
  }

  return false;
}

/**
 * Struct to pass archive relocation data in the iterator.
 */
typedef struct rtems_rtl_unresolved_archive_reloc_data
{
  uint16_t                 name;     /**< Name index. */
  rtems_rtl_archive_search result;   /**< The result of the load. */
  rtems_rtl_archives*      archives; /**< The archives to search. */
} rtems_rtl_unresolved_archive_reloc_data;

static bool
rtems_rtl_unresolved_archive_iterator (rtems_rtl_unresolv_rec* rec,
                                       void*                   data)
{
  if (rec->type == rtems_rtl_unresolved_symbol)
  {
    rtems_rtl_unresolved_archive_reloc_data* ard;
    ard = (rtems_rtl_unresolved_archive_reloc_data*) data;

    ++ard->name;

    if ((rec->rec.name.flags & RTEMS_RTL_UNRESOLV_SYM_SEARCH_ARCHIVE) != 0)
    {
      rtems_rtl_archive_search result;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
        printf ("rtl: unresolv: archive lookup: %d: %s\n",
                ard->name, rec->rec.name.name);

      result = rtems_rtl_archive_obj_load (ard->archives,
                                           rec->rec.name.name, true);
      if (result != rtems_rtl_archive_search_not_found)
      {
        rec->rec.name.flags &= ~RTEMS_RTL_UNRESOLV_SYM_SEARCH_ARCHIVE;
        ard->result = result;
        return true;
      }
    }
  }

  return false;
}

static bool
rtems_rtl_unresolved_archive_search_iterator (rtems_rtl_unresolv_rec* rec,
                                              void*                   data)
{
  if (rec->type == rtems_rtl_unresolved_symbol)
    rec->rec.name.flags |= RTEMS_RTL_UNRESOLV_SYM_SEARCH_ARCHIVE;
  return false;
}

static rtems_rtl_unresolv_block*
rtems_rtl_unresolved_alloc_recs (rtems_rtl_unresolved* unresolved,
                                 size_t                count)
{
  rtems_chain_node* node = rtems_chain_first (&unresolved->blocks);
  while (!rtems_chain_is_tail (&unresolved->blocks, node))
  {
    rtems_rtl_unresolv_block* block = (rtems_rtl_unresolv_block*) node;

    if (block->recs + count <= unresolved->block_recs)
      return block;
    node = rtems_chain_next (node);
  }
  return NULL;
}

static void
rtems_rtl_unresolved_clean_block (rtems_rtl_unresolv_block* block,
                                  rtems_rtl_unresolv_rec*   rec,
                                  size_t                    count,
                                  size_t                    recs_per_block)
{
  size_t index = rtems_rtl_unresolved_rec_index (block, rec);
  size_t bytes =
    (block->recs - index - count) * sizeof (rtems_rtl_unresolv_rec);
  if (bytes)
    memmove (rec, rec + count, bytes);
  block->recs -= count;
  bytes = count * sizeof (rtems_rtl_unresolv_rec);
  memset (&block->rec[block->recs], 0, bytes);
}

static rtems_chain_node*
rtems_rtl_unresolved_delete_block_if_empty (rtems_chain_control*      blocks,
                                            rtems_rtl_unresolv_block* block)
{
  rtems_chain_node* node = &block->link;
  rtems_chain_node* next_node = rtems_chain_next (node);
  /*
   * Always leave a single block allocated. Eases possible heap fragmentation.
   */
  if (block->recs == 0 && !rtems_chain_has_only_one_node (blocks))
  {
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
      printf ("rtl: unresolv: block-del %p\n", block);
    rtems_chain_extract (node);
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_EXTERNAL, block);
  }
  return next_node;
}

static void
rtems_rtl_unresolved_compact (void)
{
  rtems_rtl_unresolved* unresolved = rtems_rtl_unresolved_unprotected ();
  if (unresolved)
  {
    /*
     * Iterate over the blocks removing any empty strings. If a string is
     * removed update the indexes of all strings above this level.
     */
    rtems_chain_node* node = rtems_chain_first (&unresolved->blocks);
    uint16_t          index = 0;
    while (!rtems_chain_is_tail (&unresolved->blocks, node))
    {
      rtems_rtl_unresolv_block* block = (rtems_rtl_unresolv_block*) node;
      rtems_rtl_unresolv_rec*   rec = rtems_rtl_unresolved_rec_first (block);
      while (!rtems_rtl_unresolved_rec_is_last (block, rec))
      {
        bool next_rec = true;

        if (rec->type == rtems_rtl_unresolved_symbol)
        {
          ++index;
          if (rec->rec.name.refs == 0)
          {
            size_t name_recs;
            if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
              printf ("rtl: unresolv: remove name: %s\n", rec->rec.name.name);
            rtems_rtl_unresolved_reindex_names (index, -1);
            /*
             * Compact the block removing the name record.
             */
            name_recs = rtems_rtl_unresolved_symbol_recs (rec->rec.name.name);
            rtems_rtl_unresolved_clean_block (block, rec, name_recs,
                                              unresolved->block_recs);
            --index;
            next_rec = false;
          }
        }
        else if (rec->type == rtems_rtl_unresolved_reloc)
        {
          if (rec->rec.reloc.obj == NULL)
          {
            rtems_rtl_unresolved_clean_block (block, rec, 1,
                                              unresolved->block_recs);
            next_rec = false;
          }
        }

        if (next_rec)
          rec = rtems_rtl_unresolved_rec_next (rec);
      }

      node = rtems_rtl_unresolved_delete_block_if_empty (&unresolved->blocks,
                                                         block);
    }
  }
}

bool
rtems_rtl_unresolved_table_open (rtems_rtl_unresolved* unresolved,
                                 size_t                block_recs)
{
  unresolved->marker = 0xdeadf00d;
  unresolved->block_recs = block_recs;
  rtems_chain_initialize_empty (&unresolved->blocks);
  return rtems_rtl_unresolved_block_alloc (unresolved);
}

void
rtems_rtl_unresolved_table_close (rtems_rtl_unresolved* unresolved)
{
  rtems_chain_node* node = rtems_chain_first (&unresolved->blocks);
  while (!rtems_chain_is_tail (&unresolved->blocks, node))
  {
    rtems_chain_node* next = rtems_chain_next (node);
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_EXTERNAL, node);
    node = next;
  }
}

bool
rtems_rtl_unresolved_iterate (rtems_rtl_unresolved_iterator iterator,
                              void*                         data)
{
  rtems_rtl_unresolved* unresolved = rtems_rtl_unresolved_unprotected ();
  if (unresolved)
  {
    rtems_chain_node* node = rtems_chain_first (&unresolved->blocks);
    while (!rtems_chain_is_tail (&unresolved->blocks, node))
    {
      rtems_rtl_unresolv_block* block = (rtems_rtl_unresolv_block*) node;
      rtems_rtl_unresolv_rec*   rec = rtems_rtl_unresolved_rec_first (block);

      while (!rtems_rtl_unresolved_rec_is_last (block, rec))
      {
        if (iterator (rec, data))
          return true;
        rec = rtems_rtl_unresolved_rec_next (rec);
      }

      node = rtems_chain_next (node);
    }
  }
  return false;
}

bool
rtems_rtl_unresolved_add (rtems_rtl_obj*        obj,
                          const uint16_t        flags,
                          const char*           name,
                          const uint16_t        sect,
                          const rtems_rtl_word* rel)
{
  rtems_rtl_unresolved*     unresolved;
  rtems_rtl_unresolv_block* block;
  rtems_rtl_unresolv_rec*   rec;
  int                       name_index;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
    printf ("rtl: unresolv: add: %s(s:%d) -> %s\n",
            rtems_rtl_obj_oname (obj), sect, name);

  unresolved = rtems_rtl_unresolved_unprotected ();
  if (!unresolved)
    return false;

  /*
   * Is the name present?
   */
  name_index = rtems_rtl_unresolved_find_name (name);

  /*
   * An index less than 0 means the name was not found.
   */
  if (name_index < 0)
  {
    size_t name_recs;

    name_recs = rtems_rtl_unresolved_symbol_recs (name);

    /*
     * Is there enough room to fit the name ? It not add a new block.
     */
    block = rtems_rtl_unresolved_alloc_recs (unresolved, name_recs);
    if (block == NULL)
    {
      block = rtems_rtl_unresolved_block_alloc (unresolved);
      if (!block)
        return false;
    }

    /*
     * Find the record in the block.
     */
    rec = rtems_rtl_unresolved_rec_first_free (block);

    /*
     * Enter the new record before reindexing so the iterator can see
     * it and the iterator is called.
     */
    rec->type = rtems_rtl_unresolved_symbol;
    rec->rec.name.refs = 1;
    rec->rec.name.flags = RTEMS_RTL_UNRESOLV_SYM_SEARCH_ARCHIVE;
    rec->rec.name.length = strlen (name) + 1;
    memcpy ((void*) &rec->rec.name.name[0], name, rec->rec.name.length);
    block->recs += name_recs;

    /*
     * Find the name index for the name and then reindex the names which
     * are moved up because of the insertion.
     */
    name_index = rtems_rtl_unresolved_find_index (rec);
    if (name_index < 0)
    {
      rtems_rtl_set_error (ENOMEM, "internal unresolved block error");
      return false;
    }

    rtems_rtl_unresolved_reindex_names (name_index, 1);
  }

  /*
   * Find the first block with a spare record.
   */
  block = rtems_rtl_unresolved_alloc_recs (unresolved, 1);
  if (block == NULL)
  {
    block = rtems_rtl_unresolved_block_alloc (unresolved);
    if (!block)
      return false;
  }

  rec = rtems_rtl_unresolved_rec_first_free (block);
  rec->type = rtems_rtl_unresolved_reloc;
  rec->rec.reloc.obj = obj;
  rec->rec.reloc.flags = flags;
  rec->rec.reloc.name = name_index;
  rec->rec.reloc.sect = sect;
  rec->rec.reloc.rel[0] = rel[0];
  rec->rec.reloc.rel[1] = rel[1];
  rec->rec.reloc.rel[2] = rel[2];

  ++block->recs;

  return true;
}

void
rtems_rtl_unresolved_resolve (void)
{
  bool resolving = true;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
    printf ("rtl: unresolv: global resolve\n");

  /*
   * The resolving process is two separate stages, The first stage is to
   * iterate over the unresolved symbols searching the global symbol table. If
   * a symbol is found iterate over the unresolved relocation records for the
   * symbol fixing up the relocations. The second stage is to search the
   * archives for symbols we have not searched before and if a symbol is found
   * in an archve load the object file. Loading an object file stops the
   * search of the archives for symbols and stage one is performed again. The
   * process repeats until no more symbols are resolved or there is an error.
   */
  while (resolving)
  {
    rtems_rtl_unresolved_reloc_data rd = {
      .name = 0,
      .name_rec = NULL,
      .sym = NULL
    };
    rtems_rtl_unresolved_archive_reloc_data ard = {
      .name = 0,
      .result = rtems_rtl_archive_search_not_found,
      .archives = rtems_rtl_archives_unprotected ()
    };

    rtems_rtl_unresolved_iterate (rtems_rtl_unresolved_resolve_iterator, &rd);
    rtems_rtl_unresolved_compact ();
    rtems_rtl_unresolved_iterate (rtems_rtl_unresolved_archive_iterator, &ard);

    resolving = ard.result == rtems_rtl_archive_search_loaded;
  }

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
    rtems_rtl_unresolved_dump ();
}

bool
rtems_rtl_trampoline_add (rtems_rtl_obj*        obj,
                          const uint16_t        flags,
                          const uint16_t        sect,
                          const rtems_rtl_word  symvalue,
                          const rtems_rtl_word* rel)
{
  rtems_rtl_unresolved*     unresolved;
  rtems_rtl_unresolv_block* block;
  rtems_rtl_unresolv_rec*   rec;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
    printf ("rtl: tramp: add: %s sect:%d flags:%04x\n",
            rtems_rtl_obj_oname (obj), sect, flags);

  unresolved = rtems_rtl_unresolved_unprotected ();
  if (!unresolved)
    return false;

  block = rtems_rtl_unresolved_alloc_recs (unresolved, 1);
  if (block == NULL)
  {
    block = rtems_rtl_unresolved_block_alloc (unresolved);
    if (!block)
      return false;
  }

  rec = rtems_rtl_unresolved_rec_first_free (block);
  rec->type = rtems_rtl_trampoline_reloc;
  rec->rec.tramp.obj = obj;
  rec->rec.tramp.flags = flags;
  rec->rec.tramp.sect = sect;
  rec->rec.tramp.symvalue = symvalue;
  rec->rec.tramp.rel[0] = rel[0];
  rec->rec.tramp.rel[1] = rel[1];
  rec->rec.tramp.rel[2] = rel[2];

  ++block->recs;

  return true;
}

void
rtems_rtl_trampoline_remove (rtems_rtl_obj* obj)
{
  rtems_rtl_unresolved* unresolved = rtems_rtl_unresolved_unprotected ();
  if (unresolved)
  {
    /*
     * Iterate over the blocks clearing any trampoline records.
     */
    rtems_chain_node* node = rtems_chain_first (&unresolved->blocks);
    while (!rtems_chain_is_tail (&unresolved->blocks, node))
    {
      rtems_rtl_unresolv_block* block = (rtems_rtl_unresolv_block*) node;
      rtems_rtl_unresolv_rec*   rec = rtems_rtl_unresolved_rec_first (block);

      /*
       * Search the table for a trampoline record and if found clean the
       * record moving the remaining records down the block.
       */
      while (!rtems_rtl_unresolved_rec_is_last (block, rec))
      {
        bool next_rec = true;

        if (rec->type == rtems_rtl_trampoline_reloc && rec->rec.tramp.obj == obj)
        {
            rtems_rtl_unresolved_clean_block (block, rec, 1,
                                              unresolved->block_recs);
            next_rec = false;
        }

        if (next_rec)
          rec = rtems_rtl_unresolved_rec_next (rec);
      }

      node = rtems_rtl_unresolved_delete_block_if_empty (&unresolved->blocks,
                                                         block);
    }
  }
}

/**
 * Struct to pass relocation data in the iterator.
 */
typedef struct rtems_rtl_unresolved_dump_data
{
  size_t rec;
  size_t names;
  bool   show_relocs;
} rtems_rtl_unresolved_dump_data;

static bool
rtems_rtl_unresolved_dump_iterator (rtems_rtl_unresolv_rec* rec,
                                    void*                   data)
{
  rtems_rtl_unresolved_dump_data* dd;
  dd = (rtems_rtl_unresolved_dump_data*) data;
  switch (rec->type)
  {
  case rtems_rtl_unresolved_empty:
    printf (" %03zu: 0: empty\n", dd->rec);
    break;
  case rtems_rtl_unresolved_symbol:
    ++dd->names;
    printf (" %3zu: 1:  name: %3zu refs:%4d: flags:%04x %s (%d)\n",
            dd->rec, dd->names,
            rec->rec.name.refs,
            rec->rec.name.flags,
            rec->rec.name.name,
            rec->rec.name.length);
    break;
  case rtems_rtl_unresolved_reloc:
  case rtems_rtl_trampoline_reloc:
    if (dd->show_relocs)
      printf (" %3zu: 2:reloc%c: obj:%s name:%2d: sect:%d\n",
              rec->type == rtems_rtl_unresolved_reloc ? 'R' : 'T',
              dd->rec,
              rec->rec.reloc.obj == NULL ? "resolved" : rec->rec.reloc.obj->oname,
              rec->rec.reloc.name,
              rec->rec.reloc.sect);
    break;
  default:
    printf (" %03zu: %d: unknown\n", dd->rec, rec->type);
    break;
  }
  ++dd->rec;
  return false;
}

void
rtems_rtl_unresolved_dump (void)
{
  rtems_rtl_unresolved_dump_data dd = { 0 };
  printf ("RTL Unresolved data:\n");
  rtems_rtl_unresolved_iterate (rtems_rtl_unresolved_dump_iterator, &dd);
}

void
rtems_rtl_unresolved_set_archive_search (void)
{
  rtems_rtl_unresolved_iterate (rtems_rtl_unresolved_archive_search_iterator,
                                NULL);
}
