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

#include <rtems/rtl/rtl.h>
#include "rtl-error.h"
#include "rtl-unresolved.h"
#include "rtl-trace.h"

static rtems_rtl_unresolv_block_t*
rtems_rtl_unresolved_block_alloc (rtems_rtl_unresolved_t* unresolved)
{
  /*
   * The block header contains a record.
   */
  size_t size =
    (sizeof(rtems_rtl_unresolv_block_t) +
     (sizeof(rtems_rtl_unresolv_rec_t) * (unresolved->block_recs - 1)));
  rtems_rtl_unresolv_block_t* block =
    rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_EXTERNAL, size, true);
  if (block)
    rtems_chain_append (&unresolved->blocks, &block->link);
  else
    rtems_rtl_set_error (ENOMEM, "no memory for unresolved block");
  return block;
}

static size_t
rtems_rtl_unresolved_name_recs (const char* name)
{
  size_t length = strlen (name);
  return ((length + sizeof(rtems_rtl_unresolv_name_t) - 1) /
          sizeof(rtems_rtl_unresolv_name_t));
}

static int
rtems_rtl_unresolved_rec_index (rtems_rtl_unresolv_block_t* block,
                                rtems_rtl_unresolv_rec_t* rec)
{
  return (rec - &block->rec) / sizeof (rtems_rtl_unresolv_rec_t);
}

static rtems_rtl_unresolv_rec_t*
rtems_rtl_unresolved_rec_first (rtems_rtl_unresolv_block_t* block)
{
  return &block->rec;
}

static rtems_rtl_unresolv_rec_t*
rtems_rtl_unresolved_rec_next (rtems_rtl_unresolv_rec_t* rec)
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
      rec += ((rec->rec.name.length + sizeof(rtems_rtl_unresolv_name_t) - 1) /
              sizeof(rtems_rtl_unresolv_name_t));
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
rtems_rtl_unresolved_rec_is_last (rtems_rtl_unresolv_block_t* block,
                                  rtems_rtl_unresolv_rec_t*   rec)
{
  int index = (rec - &block->rec) / sizeof (rec);
  return !rec || (index >= block->recs) || (rec->type == rtems_rtl_unresolved_empty);
}

static rtems_rtl_unresolv_rec_t*
rtems_rtl_unresolved_rec_first_free (rtems_rtl_unresolv_block_t* block)
{
  return &block->rec + block->recs;
}

static int
rtems_rtl_unresolved_find_name (rtems_rtl_unresolved_t* unresolved,
                                const char*             name,
                                bool                    update_refcount)
{
  size_t length = strlen (name);
  int    index = 1;

  rtems_chain_node* node = rtems_chain_first (&unresolved->blocks);
  while (!rtems_chain_is_tail (&unresolved->blocks, node))
  {
    rtems_rtl_unresolv_block_t* block = (rtems_rtl_unresolv_block_t*) node;
    rtems_rtl_unresolv_rec_t* rec = rtems_rtl_unresolved_rec_first (block);

    while (!rtems_rtl_unresolved_rec_is_last (block, rec))
    {
      if (rec->type == rtems_rtl_unresolved_name)
      {
        if ((rec->rec.name.length == length)
            && (strcmp (rec->rec.name.name, name)))
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
typedef struct rtems_rtl_unresolved_reloc_data_s
{
  uint16_t                  name;     /**< Name index. */
  rtems_rtl_unresolv_rec_t* name_rec; /**< Name record. */
  rtems_rtl_obj_sym_t*      sym;      /**< The symbol record. */
} rtems_rtl_unresolved_reloc_data_t;

static bool
rtems_rtl_unresolved_resolve_reloc (rtems_rtl_unresolv_rec_t* rec,
                                    void*                     data)
{
  if (rec->type == rtems_rtl_unresolved_reloc)
  {
    rtems_rtl_unresolved_reloc_data_t* rd;
    rd = (rtems_rtl_unresolved_reloc_data_t*) data;

    if (rec->rec.reloc.name == rd->name)
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
      if (rd->name_rec && rd->name_rec->rec.name.refs)
        --rd->name_rec->rec.name.refs;
    }
  }
  return false;
}

static bool
rtems_rtl_unresolved_resolve_iterator (rtems_rtl_unresolv_rec_t* rec,
                                       void*                     data)
{
  if (rec->type == rtems_rtl_unresolved_name)
  {
    rtems_rtl_unresolved_reloc_data_t* rd;
    rd = (rtems_rtl_unresolved_reloc_data_t*) data;

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
rtems_rtl_unresolved_clean_block (rtems_rtl_unresolv_block_t* block,
                                  rtems_rtl_unresolv_rec_t* rec,
                                  size_t count,
                                  size_t recs_per_block)
{
  size_t index = rtems_rtl_unresolved_rec_index (block, rec);
  size_t bytes =
    (block->recs - index - count) * sizeof (rtems_rtl_unresolv_rec_t);
  if (bytes)
    memmove (rec, rec + count, bytes);
  --block->recs;
  bytes = count * sizeof (rtems_rtl_unresolv_rec_t);
  memset (&block->rec + block->recs, 0, bytes);
}

static void
rtems_rtl_unresolved_compact (void)
{
  rtems_rtl_unresolved_t* unresolved = rtems_rtl_unresolved ();
  if (unresolved)
  {
    /*
     * Iterate backwards over the blocks removing any used records. A block is
     * compacted moving up the block.
     */
    rtems_chain_node* node = rtems_chain_last (&unresolved->blocks);
    while (!rtems_chain_is_head (&unresolved->blocks, node))
    {
      rtems_chain_node* prev = rtems_chain_previous (node);
      rtems_rtl_unresolv_block_t* block = (rtems_rtl_unresolv_block_t*) node;
      rtems_rtl_unresolv_rec_t* rec = rtems_rtl_unresolved_rec_first (block);

      while (!rtems_rtl_unresolved_rec_is_last (block, rec))
      {
        bool next = true;

        if (rec->type == rtems_rtl_unresolved_name)
        {
          if (rec->rec.name.refs == 0)
          {
            size_t name_recs = rtems_rtl_unresolved_name_recs (rec->rec.name.name);
            rtems_rtl_unresolved_clean_block (block, rec, name_recs,
                                              unresolved->block_recs);
            next = false;
          }
        }
        else if (rec->type == rtems_rtl_unresolved_reloc)
        {
          if (!rec->rec.reloc.obj)
          {
            rtems_rtl_unresolved_clean_block (block, rec, 1,
                                              unresolved->block_recs);
            next = false;
          }
        }

        if (next)
          rec = rtems_rtl_unresolved_rec_next (rec);
      }

      if (block->recs == 0)
      {
        rtems_chain_extract (node);
        free (block);
      }

      node = prev;
    }
  }
}

bool
rtems_rtl_unresolved_table_open (rtems_rtl_unresolved_t* unresolved,
                                 size_t                  block_recs)
{
  unresolved->marker = 0xdeadf00d;
  unresolved->block_recs = block_recs;
  rtems_chain_initialize_empty (&unresolved->blocks);
  return true;
}

void
rtems_rtl_unresolved_table_close (rtems_rtl_unresolved_t* unresolved)
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
rtems_rtl_unresolved_interate (rtems_rtl_unresolved_iterator_t iterator,
                               void*                           data)
{
  rtems_rtl_unresolved_t* unresolved = rtems_rtl_unresolved ();
  if (unresolved)
  {
    rtems_chain_node* node = rtems_chain_first (&unresolved->blocks);
    while (!rtems_chain_is_tail (&unresolved->blocks, node))
    {
      rtems_rtl_unresolv_block_t* block = (rtems_rtl_unresolv_block_t*) node;
      rtems_rtl_unresolv_rec_t* rec = rtems_rtl_unresolved_rec_first (block);

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
rtems_rtl_unresolved_add (rtems_rtl_obj_t*        obj,
                          const uint16_t          flags,
                          const char*             name,
                          const uint16_t          sect,
                          const rtems_rtl_word_t* rel)
{
  rtems_rtl_unresolved_t* unresolved;
  rtems_chain_node* node;
  rtems_rtl_unresolv_block_t* block;
  rtems_rtl_unresolv_rec_t* rec;
  int name_index;
  size_t name_recs;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
    printf ("rtl: unresolv: add: %s(s:%d) -> %s\n",
            rtems_rtl_obj_oname (obj), sect, name);

  unresolved = rtems_rtl_unresolved ();
  if (!unresolved)
    return false;

  /*
   * Find the first block with a spare record.
   */
  node = rtems_chain_first (&unresolved->blocks);
  block = NULL;
  while (!rtems_chain_is_tail (&unresolved->blocks, node))
  {
    block = (rtems_rtl_unresolv_block_t*) node;
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
    rtems_rtl_unresolv_block_t* name_block = block;

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
  rtems_rtl_unresolved_reloc_data_t rd;
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
    printf ("rtl: unresolv: global resolve\n");
  rd.name = 0;
  rd.name_rec = NULL;
  rd.sym = NULL;
  rtems_rtl_unresolved_interate (rtems_rtl_unresolved_resolve_iterator, &rd);
  rtems_rtl_unresolved_compact ();
}

bool
rtems_rtl_unresolved_remove (rtems_rtl_obj_t*        obj,
                             const char*             name,
                             const uint16_t          sect,
                             const rtems_rtl_word_t* rel)
{
  rtems_rtl_unresolved_t* unresolved;
  unresolved = rtems_rtl_unresolved ();
  if (!unresolved)
    return false;
  return false;
}

