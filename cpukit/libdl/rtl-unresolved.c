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

#if HAVE_CONFIG_H
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

static rtems_rtl_unresolv_block*
rtems_rtl_unresolved_block_alloc (rtems_rtl_unresolved* unresolved)
{
  /*
   * The block header contains a record.
   */
  size_t size =
    (sizeof(rtems_rtl_unresolv_block) +
     (sizeof(rtems_rtl_unresolv_rec) * (unresolved->block_recs - 1)));
  rtems_rtl_unresolv_block* block =
    rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_EXTERNAL, size, true);
  if (block)
  {
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
      printf ("rtl: unresolv: block-alloc %p\n", block);
    rtems_chain_append (&unresolved->blocks, &block->link);
  }
  else
    rtems_rtl_set_error (ENOMEM, "no memory for unresolved block");
  return block;
}

static size_t
rtems_rtl_unresolved_name_recs (const char* name)
{
  size_t length = strlen (name);
  return ((length + sizeof(rtems_rtl_unresolv_name) - 1) /
          sizeof(rtems_rtl_unresolv_name));
}

static int
rtems_rtl_unresolved_rec_index (rtems_rtl_unresolv_block* block,
                                rtems_rtl_unresolv_rec* rec)
{
  return (rec - &block->rec) / sizeof (rtems_rtl_unresolv_rec);
}

static rtems_rtl_unresolv_rec*
rtems_rtl_unresolved_rec_first (rtems_rtl_unresolv_block* block)
{
  return &block->rec;
}

static rtems_rtl_unresolv_rec*
rtems_rtl_unresolved_rec_next (rtems_rtl_unresolv_rec* rec)
{
  switch (rec->type)
  {
    case rtems_rtl_unresolved_empty:
      /*
       * Empty returns NULL. The end of the records in the block.
       */
      rec = NULL;
      break;

    case rtems_rtl_unresolved_name:
      /*
       * Determine how many records the name occupies. Round up.
       */
      rec += ((rec->rec.name.length + sizeof(rtems_rtl_unresolv_name) - 1) /
              sizeof(rtems_rtl_unresolv_name));
      break;

    case rtems_rtl_unresolved_reloc:
      ++rec;
      break;

    default:
      break;
  }

  return rec;
}

static bool
rtems_rtl_unresolved_rec_is_last (rtems_rtl_unresolv_block* block,
                                  rtems_rtl_unresolv_rec*   rec)
{
  int index = (rec - &block->rec) / sizeof (rec);
  return !rec || (index >= block->recs) || (rec->type == rtems_rtl_unresolved_empty);
}

static rtems_rtl_unresolv_rec*
rtems_rtl_unresolved_rec_first_free (rtems_rtl_unresolv_block* block)
{
  return &block->rec + block->recs;
}

static int
rtems_rtl_unresolved_find_name (rtems_rtl_unresolved* unresolved,
                                const char*           name,
                                bool                  update_refcount)
{
  size_t length = strlen (name) + 1;
  int    index = 1;

  rtems_chain_node* node = rtems_chain_first (&unresolved->blocks);
  while (!rtems_chain_is_tail (&unresolved->blocks, node))
  {
    rtems_rtl_unresolv_block* block = (rtems_rtl_unresolv_block*) node;
    rtems_rtl_unresolv_rec* rec = rtems_rtl_unresolved_rec_first (block);

    while (!rtems_rtl_unresolved_rec_is_last (block, rec))
    {
      if (rec->type == rtems_rtl_unresolved_name)
      {
        if ((rec->rec.name.length == length)
            && (strcmp (rec->rec.name.name, name) == 0))
        {
          if (update_refcount)
            ++rec->rec.name.refs;
          return index;
        }
        ++index;
      }
      rec = rtems_rtl_unresolved_rec_next (rec);
    }

    node = rtems_chain_next (node);
  }

  return 0 - index;
}

/**
 * Struct to pass relocation data in the interator.
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
    rtems_rtl_unresolved_reloc_data* rd;
    rd = (rtems_rtl_unresolved_reloc_data*) data;

    if (rec->rec.reloc.name == rd->name && rec->rec.reloc.obj != NULL)
    {
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
        printf ("rtl: unresolv: resolve reloc: %s\n", rd->name_rec->rec.name.name);

      rtems_rtl_obj_relocate_unresolved (&rec->rec.reloc, rd->sym);

      /*
       * Set the object pointer to NULL to indicate the record is not used
       * anymore. Update the reference count of the name. The sweep after
       * relocating will remove the reloc records with obj set to NULL and
       * names with a reference count of 0.
       */
      rec->rec.reloc.obj = NULL;
      if (rd->name_rec != NULL && rd->name_rec->rec.name.refs > 0)
        --rd->name_rec->rec.name.refs;
    }
  }
  return false;
}

static bool
rtems_rtl_unresolved_resolve_iterator (rtems_rtl_unresolv_rec* rec,
                                       void*                   data)
{
  if (rec->type == rtems_rtl_unresolved_name)
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

      rtems_rtl_unresolved_interate (rtems_rtl_unresolved_resolve_reloc, rd);

      rd->name_rec = NULL;
      rd->sym = NULL;
    }
  }

  return false;
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
  memset (&block->rec + block->recs, 0, bytes);
}

static void
rtems_rtl_unresolved_compact (void)
{
  rtems_rtl_unresolved* unresolved = rtems_rtl_unresolved_unprotected ();
  if (unresolved)
  {
    /*
     * Iterate over the blocks removing any empty strings. If a string is removed
     * update the indexes of all strings above this level.
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
        if (rec->type == rtems_rtl_unresolved_name)
        {
          ++index;
          if (rec->rec.name.refs == 0)
          {
            /*
             * Iterate over the remainnig reloc records and update the index.
             */
            rtems_chain_node*       reindex_node;
            rtems_rtl_unresolv_rec* reindex_first;
            size_t                  name_recs;
            if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
              printf ("rtl: unresolv: remove name: %s\n", rec->rec.name.name);
            reindex_node = node;
            reindex_first = rtems_rtl_unresolved_rec_next (rec);
            while (!rtems_chain_is_tail (&unresolved->blocks, reindex_node))
            {
              rtems_rtl_unresolv_rec*   reindex_rec;
              rtems_rtl_unresolv_block* reindex_block;
              reindex_block = (rtems_rtl_unresolv_block*) reindex_node;
              if (reindex_first != NULL)
              {
                reindex_rec = reindex_first;
                reindex_first = NULL;
              }
              else
              {
                reindex_rec = rtems_rtl_unresolved_rec_first (reindex_block);
              }
              while (!rtems_rtl_unresolved_rec_is_last (reindex_block,
                                                        reindex_rec))
              {
                if (reindex_rec->type == rtems_rtl_unresolved_reloc)
                {
                  if (reindex_rec->rec.reloc.name >= index)
                    --reindex_rec->rec.reloc.name;
                }
                reindex_rec = rtems_rtl_unresolved_rec_next (reindex_rec);
              }
              reindex_node = rtems_chain_next (reindex_node);
            }
            /*
             * Compact the block removing the name record.
             */
            name_recs = rtems_rtl_unresolved_name_recs (rec->rec.name.name);
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

      if (block->recs == 0)
      {
        rtems_chain_node* next_node = rtems_chain_next (node);
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
          printf ("rtl: unresolv: block-del %p\n", block);
        rtems_chain_extract (node);
        rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_EXTERNAL, block);
        node = next_node;
      }
      else
      {
        node = rtems_chain_next (node);
      }
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
  return true;
}

void
rtems_rtl_unresolved_table_close (rtems_rtl_unresolved* unresolved)
{
  rtems_chain_node* node = rtems_chain_first (&unresolved->blocks);
  while (!rtems_chain_is_tail (&unresolved->blocks, node))
  {
    rtems_chain_node* next = rtems_chain_next (node);
    free (node);
    node = next;
  }
}

bool
rtems_rtl_unresolved_interate (rtems_rtl_unresolved_iterator iterator,
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
  rtems_chain_node*         node;
  rtems_rtl_unresolv_block* block;
  rtems_rtl_unresolv_rec*   rec;
  int                       name_index;
  size_t                    name_recs;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
    printf ("rtl: unresolv: add: %s(s:%d) -> %s\n",
            rtems_rtl_obj_oname (obj), sect, name);

  unresolved = rtems_rtl_unresolved_unprotected ();
  if (!unresolved)
    return false;

  /*
   * Find the first block with a spare record.
   */
  node = rtems_chain_first (&unresolved->blocks);
  block = NULL;
  while (!rtems_chain_is_tail (&unresolved->blocks, node))
  {
    block = (rtems_rtl_unresolv_block*) node;
    if (block->recs < unresolved->block_recs)
      break;
    block = NULL;
    node = rtems_chain_next (node);
  }

  /*
   * No blocks with any spare records, allocate a new block.
   */
  if (!block)
  {
    block = rtems_rtl_unresolved_block_alloc (unresolved);
    if (!block)
      return false;
  }

  name_index = rtems_rtl_unresolved_find_name (unresolved, name, true);
  name_recs = rtems_rtl_unresolved_name_recs (name);

  /*
   * An index less than 0 means the name is present and "0 - index" is the next
   * index to use.
   */
  if (name_index < 0)
  {
    rtems_rtl_unresolv_block* name_block = block;

    /*
     * Is there enough room to fit the name ? It not add a new block.
     */
    if (name_recs > (unresolved->block_recs - block->recs))
    {
      name_block = rtems_rtl_unresolved_block_alloc (unresolved);
      if (!name_block)
        return false;
    }

    rec = rtems_rtl_unresolved_rec_first_free (name_block);
    rec->type = rtems_rtl_unresolved_name;
    rec->rec.name.refs = 1;
    rec->rec.name.length = strlen (name) + 1;
    memcpy ((void*) &rec->rec.name.name[0], name, rec->rec.name.length + 1);
    block->recs += name_recs;
    name_index = 0 - name_index;

    /*
     * If the name block is the reloc block and it is full allocate a new
     * block for the relocation record.
     */
    if ((block == name_block) && (block->recs >= unresolved->block_recs))
    {
      block = rtems_rtl_unresolved_block_alloc (unresolved);
      if (!block)
        return false;
    }
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
  rtems_rtl_unresolved_reloc_data rd;
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
    printf ("rtl: unresolv: global resolve\n");
  rd.name = 0;
  rd.name_rec = NULL;
  rd.sym = NULL;
  rtems_rtl_unresolved_interate (rtems_rtl_unresolved_resolve_iterator, &rd);
  rtems_rtl_unresolved_compact ();
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
    rtems_rtl_unresolved_dump ();
}

bool
rtems_rtl_unresolved_remove (rtems_rtl_obj*        obj,
                             const char*           name,
                             const uint16_t        sect,
                             const rtems_rtl_word* rel)
{
  rtems_rtl_unresolved* unresolved;
  unresolved = rtems_rtl_unresolved_unprotected ();
  if (!unresolved)
    return false;
  return false;
}

/**
 * Struct to pass relocation data in the iterator.
 */
typedef struct rtems_rtl_unresolved_dump_data
{
  size_t rec;
  size_t names;
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
  case rtems_rtl_unresolved_name:
    ++dd->names;
    printf (" %3zu: 1:  name: %3d refs: %2d: %2d: %s\n",
            dd->rec, dd->names,
            rec->rec.name.refs, rec->rec.name.length, rec->rec.name.name);
    break;
  case rtems_rtl_unresolved_reloc:
    printf (" %3zu: 2: reloc: obj:%s name:%2d: sect:%d\n",
            dd->rec,
            rec->rec.reloc.obj->oname,
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
  rtems_rtl_unresolved_interate (rtems_rtl_unresolved_dump_iterator, &dd);
}
