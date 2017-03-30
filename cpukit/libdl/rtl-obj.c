/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtl
 *
 * @brief RTEMS Run-Time Linker Error
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include <rtems/libio_.h>

#include <rtems/rtl/rtl.h>
#include "rtl-chain-iterator.h"
#include "rtl-obj.h"
#include "rtl-error.h"
#include "rtl-find-file.h"
#include "rtl-string.h"
#include "rtl-trace.h"

#if RTEMS_RTL_RAP_LOADER
#include "rtl-rap.h"
#define RTEMS_RTL_RAP_LOADER_COUNT 1
#else
#define RTEMS_RTL_RAP_LOADER_COUNT 0
#endif

#if RTEMS_RTL_ELF_LOADER
#include "rtl-elf.h"
#define RTEMS_RTL_ELF_LOADER_COUNT 1
#else
#define RTEMS_RTL_ELF_LOADER_COUNT 0
#endif

/**
 * The table of supported loader formats.
 */
#define RTEMS_RTL_LOADERS (RTEMS_RTL_ELF_LOADER_COUNT + RTEMS_RTL_RAP_LOADER_COUNT)
static const rtems_rtl_loader_table_t loaders[RTEMS_RTL_LOADERS] =
{
#if RTEMS_RTL_RAP_LOADER
  { .check     = rtems_rtl_rap_file_check,
    .load      = rtems_rtl_rap_file_load,
    .unload    = rtems_rtl_rap_file_unload,
    .unload    = rtems_rtl_rap_file_unload,
    .signature = rtems_rtl_rap_file_sig },
#endif
#if RTEMS_RTL_ELF_LOADER
  { .check     = rtems_rtl_elf_file_check,
    .load      = rtems_rtl_elf_file_load,
    .unload    = rtems_rtl_elf_file_unload,
    .signature = rtems_rtl_elf_file_sig },
#endif
};

rtems_rtl_obj_t*
rtems_rtl_obj_alloc (void)
{
  rtems_rtl_obj_t* obj = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT,
                                              sizeof (rtems_rtl_obj_t),
                                              true);
  if (obj)
  {
    /*
     * Initialise the chains.
     */
    rtems_chain_initialize_empty (&obj->sections);
    /*
     * No valid format.
     */
    obj->format = -1;
  }
  return obj;
}

static void
rtems_rtl_obj_free_names (rtems_rtl_obj_t* obj)
{
  if (rtems_rtl_obj_oname_valid (obj))
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, (void*) obj->oname);
  if (rtems_rtl_obj_aname_valid (obj))
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, (void*) obj->aname);
  if (rtems_rtl_obj_fname_valid (obj))
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, (void*) obj->fname);
}

bool
rtems_rtl_obj_free (rtems_rtl_obj_t* obj)
{
  if (obj->users || ((obj->flags & RTEMS_RTL_OBJ_LOCKED) != 0))
  {
    rtems_rtl_set_error (EINVAL, "cannot free obj still in use");
    return false;
  }
  if (!rtems_chain_is_node_off_chain (&obj->link))
    rtems_chain_extract (&obj->link);
  rtems_rtl_alloc_module_del (&obj->text_base, &obj->const_base, &obj->eh_base,
                              &obj->data_base, &obj->bss_base);
  rtems_rtl_symbol_obj_erase (obj);
  rtems_rtl_obj_free_names (obj);
  if (obj->sec_num)
    free (obj->sec_num);
  if (obj->linkmap)
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, (void*) obj->linkmap);
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, obj);
  return true;
}

bool
rtems_rtl_obj_unresolved (rtems_rtl_obj_t* obj)
{
  return (obj->flags & RTEMS_RTL_OBJ_UNRESOLVED) != 0 ? true : false;
}

bool
rtems_rtl_parse_name (const char*  name,
                      const char** aname,
                      const char** oname,
                      off_t*       ooffset)
{
  const char* laname = NULL;
  const char* loname = NULL;
  const char* colon;
  const char* end;

  /*
   * Parse the name to determine if the object file is part of an archive or it
   * is an object file. If an archive check the name for a '@' to see if the
   * archive contains an offset.
   */
  end = name + strlen (name);
  colon = strrchr (name, ':');
  if (colon == NULL || colon < strrchr(name, '/'))
    colon = end;

  loname = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT, colon - name + 1, true);
  if (!oname)
  {
    rtems_rtl_set_error (ENOMEM, "no memory for object file name");
    return false;
  }

  memcpy ((void*) loname, name, colon - name);

  /*
   * If the pointers match there is no ':' delimiter.
   */
  if (colon != end)
  {
    const char* at;

    /*
     * The file name is an archive and the object file name is next after the
     * delimiter. Move the pointer to the archive name.
     */
    laname = loname;
    ++colon;

    /*
     * See if there is a '@' to delimit an archive offset for the object in the
     * archive.
     */
    at = strchr (colon, '@');

    if (at == NULL)
      at = end;


    loname = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT, at - colon + 1, true);
    if (!loname)
    {
      rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, (void*) laname);
      rtems_rtl_set_error (ENOMEM, "no memory for object file name");
      return false;
    }

    memcpy ((void*) loname, colon, at - colon);

    if (at != end)
    {
      /*
       * The object name has an archive offset. If the number
       * does not parse 0 will be returned and the archive will be
       * searched.
       */
      *ooffset = strtoul (at + 1, 0, 0);
    }
  }

  *oname = loname;
  *aname = laname;
  return true;
}

static bool
rtems_rtl_obj_parse_name (rtems_rtl_obj_t* obj, const char* name)
{
  return rtems_rtl_parse_name (name, &(obj->aname), &(obj->oname), &(obj->ooffset));
}

static bool
rtems_rtl_seek_read (int fd, off_t off, size_t len, uint8_t* buffer)
{
  if (lseek (fd, off, SEEK_SET) < 0)
    return false;
  if (read (fd, buffer, len) != len)
    return false;
  return true;
}

/**
 * Scan the decimal number returning the value found.
 */
static uint64_t
rtems_rtl_scan_decimal (const uint8_t* string, size_t len)
{
  uint64_t value = 0;

  while (len && (*string != ' '))
  {
    value *= 10;
    value += *string - '0';
    ++string;
    --len;
  }

  return value;
}

/**
 * Align the size to the next alignment point. Assume the alignment is a
 * positive integral power of 2 if not 0 or 1. If 0 or 1 then there is no
 * alignment.
 */
static size_t
rtems_rtl_sect_align (size_t offset, uint32_t alignment)
{
  if ((alignment > 1) && ((offset & (alignment - 1)) != 0))
    offset = (offset + alignment) & ~(alignment - 1);
  return offset;
}

/**
 * Section size summer iterator data.
 */
typedef struct
{
  uint32_t mask; /**< The selection mask to sum. */
  size_t   size; /**< The size of all section fragments. */
} rtems_rtl_obj_sect_summer_t;

static bool
rtems_rtl_obj_sect_summer (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj_sect_t*        sect = (rtems_rtl_obj_sect_t*) node;
  rtems_rtl_obj_sect_summer_t* summer = data;
  if ((sect->flags & summer->mask) == summer->mask)
    summer->size =
      rtems_rtl_sect_align (summer->size, sect->alignment) + sect->size;
  return true;
}

static size_t
rtems_rtl_obj_section_size (const rtems_rtl_obj_t* obj, uint32_t mask)
{
  rtems_rtl_obj_sect_summer_t summer;
  summer.mask = mask;
  summer.size = 0;
  rtems_rtl_chain_iterate ((rtems_chain_control*) &obj->sections,
                           rtems_rtl_obj_sect_summer,
                           &summer);
  return summer.size;
}

/**
 * Section alignment iterator data. The first section's alignment sets the
 * alignment for that type of section.
 */
typedef struct
{
  uint32_t mask;      /**< The selection mask to look for alignment. */
  uint32_t alignment; /**< The alignment of the section type. */
} rtems_rtl_obj_sect_aligner_t;

/**
 * The section aligner iterator.
 */
static bool
rtems_rtl_obj_sect_aligner (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj_sect_t*         sect = (rtems_rtl_obj_sect_t*) node;
  rtems_rtl_obj_sect_aligner_t* aligner = data;
  if ((sect->flags & aligner->mask) == aligner->mask)
  {
    aligner->alignment = sect->alignment;
    return false;
  }
  return true;
}

static size_t
rtems_rtl_obj_section_alignment (const rtems_rtl_obj_t* obj, uint32_t mask)
{
  rtems_rtl_obj_sect_aligner_t aligner;
  aligner.mask = mask;
  aligner.alignment = 0;
  rtems_rtl_chain_iterate ((rtems_chain_control*) &obj->sections,
                           rtems_rtl_obj_sect_aligner,
                           &aligner);
  return aligner.alignment;
}

static bool
rtems_rtl_obj_section_handler (uint32_t                     mask,
                               rtems_rtl_obj_t*             obj,
                               int                          fd,
                               rtems_rtl_obj_sect_handler_t handler,
                               void*                        data)
{
  rtems_chain_node* node = rtems_chain_first (&obj->sections);
  while (!rtems_chain_is_tail (&obj->sections, node))
  {
    rtems_rtl_obj_sect_t* sect = (rtems_rtl_obj_sect_t*) node;
    if ((sect->flags & mask) != 0)
    {
      if (!handler (obj, fd, sect, data))
        return false;
    }
    node = rtems_chain_next (node);
  }
  return true;
}

bool
rtems_rtl_match_name (rtems_rtl_obj_t* obj, const char* name)
{
  const char* n1 = obj->oname;
  while ((*n1 != '\0') && (*n1 != '\n') && (*n1 != '/') &&
         (*name != '\0') && (*name != '/') && (*n1 == *name))
  {
    ++n1;
    ++name;
  }
  if (((*n1 == '\0') || (*n1 == '\n') || (*n1 == '/')) &&
      ((*name == '\0') || (*name == '/')))
    return true;
  return false;
}

bool
rtems_rtl_obj_find_file (rtems_rtl_obj_t* obj, const char* name)
{
  const char*       pname;
  rtems_rtl_data_t* rtl;

  /*
   * Parse the name. The object descriptor will have the archive name and/or
   * object name fields filled in. A find of the file will result in the file
   * name (fname) field pointing to the actual file if present on the file
   * system.
   */
  if (!rtems_rtl_obj_parse_name (obj, name))
    return false;

  /*
   * If the archive field (aname) is set we use that name else we use the
   * object field (oname). If selected name is absolute we just point the aname
   * field to the fname field to that name. If the field is relative we search
   * the paths set in the RTL for the file.
   */
  if (rtems_rtl_obj_aname_valid (obj))
    pname = rtems_rtl_obj_aname (obj);
  else
    pname = rtems_rtl_obj_oname (obj);

  rtl = rtems_rtl_lock ();

  if (!rtems_rtl_find_file (pname, rtl->paths, &obj->fname, &obj->fsize))
  {
    rtems_rtl_set_error (ENOENT, "file not found");
    rtems_rtl_unlock ();
    return false;
  }

  rtems_rtl_unlock ();

  return true;
}

bool
rtems_rtl_obj_add_section (rtems_rtl_obj_t* obj,
                           int              section,
                           const char*      name,
                           size_t           size,
                           off_t            offset,
                           uint32_t         alignment,
                           int              link,
                           int              info,
                           uint32_t         flags)
{
  if (size > 0)
  {
    rtems_rtl_obj_sect_t* sect = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT,
                                                      sizeof (rtems_rtl_obj_sect_t),
                                                      true);
    if (!sect)
    {
      rtems_rtl_set_error (ENOMEM, "adding allocated section");
      return false;
    }
    sect->section = section;
    sect->name = rtems_rtl_strdup (name);
    sect->size = size;
    sect->offset = offset;
    sect->alignment = alignment;
    sect->link = link;
    sect->info = info;
    sect->flags = flags;
    sect->base = NULL;
    rtems_chain_append (&obj->sections, &sect->node);

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_SECTION))
      printf ("rtl: sect: %-2d: %s (%zu)\n", section, name, size);
  }

  return true;
}

void
rtems_rtl_obj_erase_sections (rtems_rtl_obj_t* obj)
{
  rtems_chain_node* node = rtems_chain_first (&obj->sections);
  while (!rtems_chain_is_tail (&obj->sections, node))
  {
    rtems_rtl_obj_sect_t* sect = (rtems_rtl_obj_sect_t*) node;
    rtems_chain_node*     next_node = rtems_chain_next (node);
    rtems_chain_extract (node);
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, (void*) sect->name);
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, sect);
    node = next_node;
  }
}

/**
 * Section finder iterator data.
 */
typedef struct
{
  rtems_rtl_obj_sect_t*  sect;  /**< The matching section. */
  const char*            name;  /**< The name to match. */
  int                    index; /**< The index to match. */
} rtems_rtl_obj_sect_finder_t;

static bool
rtems_rtl_obj_sect_match_name (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj_sect_t*        sect = (rtems_rtl_obj_sect_t*) node;
  rtems_rtl_obj_sect_finder_t* match = data;
  if (strcmp (sect->name, match->name) == 0)
  {
    match->sect = sect;
    return false;
  }
  return true;
}

rtems_rtl_obj_sect_t*
rtems_rtl_obj_find_section (const rtems_rtl_obj_t* obj,
                            const char*            name)
{
  rtems_rtl_obj_sect_finder_t match;
  match.sect = NULL;
  match.name = name;
  rtems_rtl_chain_iterate ((rtems_chain_control*) &obj->sections,
                           rtems_rtl_obj_sect_match_name,
                           &match);
  return match.sect;
}

static bool
rtems_rtl_obj_sect_match_index (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj_sect_t*        sect = (rtems_rtl_obj_sect_t*) node;
  rtems_rtl_obj_sect_finder_t* match = data;
  if (sect->section == match->index)
  {
    match->sect = sect;
    return false;
  }
  return true;
}

rtems_rtl_obj_sect_t*
rtems_rtl_obj_find_section_by_index (const rtems_rtl_obj_t* obj,
                                     int                    index)
{
  rtems_rtl_obj_sect_finder_t match;
  match.sect = NULL;
  match.index = index;
  rtems_rtl_chain_iterate ((rtems_chain_control*) &obj->sections,
                           rtems_rtl_obj_sect_match_index,
                           &match);
  return match.sect;
}

size_t
rtems_rtl_obj_text_size (const rtems_rtl_obj_t* obj)
{
  return rtems_rtl_obj_section_size (obj, RTEMS_RTL_OBJ_SECT_TEXT);
}

uint32_t
rtems_rtl_obj_text_alignment (const rtems_rtl_obj_t* obj)
{
  return rtems_rtl_obj_section_alignment (obj, RTEMS_RTL_OBJ_SECT_TEXT);
}

size_t
rtems_rtl_obj_const_size (const rtems_rtl_obj_t* obj)
{
  return rtems_rtl_obj_section_size (obj, RTEMS_RTL_OBJ_SECT_CONST);
}

uint32_t
rtems_rtl_obj_eh_alignment (const rtems_rtl_obj_t* obj)
{
  return rtems_rtl_obj_section_alignment (obj, RTEMS_RTL_OBJ_SECT_EH);
}

size_t
rtems_rtl_obj_eh_size (const rtems_rtl_obj_t* obj)
{
  return rtems_rtl_obj_section_size (obj, RTEMS_RTL_OBJ_SECT_EH);
}

uint32_t
rtems_rtl_obj_const_alignment (const rtems_rtl_obj_t* obj)
{
  return rtems_rtl_obj_section_alignment (obj, RTEMS_RTL_OBJ_SECT_CONST);
}

size_t
rtems_rtl_obj_data_size (const rtems_rtl_obj_t* obj)
{
  return rtems_rtl_obj_section_size (obj, RTEMS_RTL_OBJ_SECT_DATA);
}

uint32_t
rtems_rtl_obj_data_alignment (const rtems_rtl_obj_t* obj)
{
  return rtems_rtl_obj_section_alignment (obj, RTEMS_RTL_OBJ_SECT_DATA);
}

size_t
rtems_rtl_obj_bss_size (const rtems_rtl_obj_t* obj)
{
  return rtems_rtl_obj_section_size (obj, RTEMS_RTL_OBJ_SECT_BSS);
}

uint32_t
rtems_rtl_obj_bss_alignment (const rtems_rtl_obj_t* obj)
{
  return rtems_rtl_obj_section_alignment (obj, RTEMS_RTL_OBJ_SECT_BSS);
}

bool
rtems_rtl_obj_relocate (rtems_rtl_obj_t*             obj,
                        int                          fd,
                        rtems_rtl_obj_sect_handler_t handler,
                        void*                        data)
{
  uint32_t mask = RTEMS_RTL_OBJ_SECT_REL | RTEMS_RTL_OBJ_SECT_RELA;
  return rtems_rtl_obj_section_handler (mask, obj, fd, handler, data);
}

/**
 * Cache synchronization after runtime object load (dlopen)
 */
typedef struct
{
  uint32_t mask;
  void     *start_va;
  void     *end_va;
  size_t   cache_line_size;
} rtems_rtl_obj_sect_sync_ctx_t;

static bool
rtems_rtl_obj_sect_sync_handler (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj_sect_t*          sect = (rtems_rtl_obj_sect_t*) node;
  rtems_rtl_obj_sect_sync_ctx_t* sync_ctx = data;
  uintptr_t                      old_end;
  uintptr_t                      new_start;

  if ((sect->flags & sync_ctx->mask) == 0 || sect->size == 0)
    return true;

  if (sync_ctx->end_va == sync_ctx->start_va)
  {
    sync_ctx->start_va = sect->base;
  }
  else
  {
    old_end = (uintptr_t) sync_ctx->end_va & ~(sync_ctx->cache_line_size - 1);
    new_start = (uintptr_t) sect->base & ~(sync_ctx->cache_line_size - 1);
    if ((sect->base <  sync_ctx->start_va) ||
        (new_start - old_end > sync_ctx->cache_line_size))
    {
      rtems_cache_instruction_sync_after_code_change(sync_ctx->start_va,
                             sync_ctx->end_va - sync_ctx->start_va + 1);
      sync_ctx->start_va = sect->base;
    }
  }

  sync_ctx->end_va = sect->base + sect->size;

  return true;
}

void
rtems_rtl_obj_synchronize_cache (rtems_rtl_obj_t* obj)
{
  rtems_rtl_obj_sect_sync_ctx_t sync_ctx;

  if (rtems_cache_get_instruction_line_size() == 0)
    return;

  sync_ctx.cache_line_size = rtems_cache_get_maximal_line_size();

  sync_ctx.mask = RTEMS_RTL_OBJ_SECT_TEXT | RTEMS_RTL_OBJ_SECT_CONST |
                  RTEMS_RTL_OBJ_SECT_DATA | RTEMS_RTL_OBJ_SECT_BSS |
                  RTEMS_RTL_OBJ_SECT_EH   | RTEMS_RTL_OBJ_SECT_EXEC;

  sync_ctx.start_va = 0;
  sync_ctx.end_va = sync_ctx.start_va;
  rtems_rtl_chain_iterate (&obj->sections,
                           rtems_rtl_obj_sect_sync_handler,
                           &sync_ctx);

  if (sync_ctx.end_va != sync_ctx.start_va) {
    rtems_cache_instruction_sync_after_code_change(sync_ctx.start_va,
                              sync_ctx.end_va - sync_ctx.start_va);
  }
}

bool
rtems_rtl_obj_load_symbols (rtems_rtl_obj_t*             obj,
                            int                          fd,
                            rtems_rtl_obj_sect_handler_t handler,
                            void*                        data)
{
  uint32_t mask = RTEMS_RTL_OBJ_SECT_SYM;
  return rtems_rtl_obj_section_handler (mask, obj, fd, handler, data);
}

static int
rtems_rtl_obj_sections_linked_to_order (rtems_rtl_obj_t* obj,
                                        int              section,
                                        uint32_t         visited_mask)
{
  rtems_chain_control* sections = &obj->sections;
  rtems_chain_node*    node = rtems_chain_first (sections);
  /*
   * Find the section being linked-to. If the linked-to link field is 0 we have
   * the end and the section's order is the position we are after.
   */
  while (!rtems_chain_is_tail (sections, node))
  {
    rtems_rtl_obj_sect_t* sect = (rtems_rtl_obj_sect_t*) node;
    if (sect->section == section)
    {
      const uint32_t mask = sect->flags & RTEMS_RTL_OBJ_SECT_TYPES;
      int            order = 0;
      if (sect->link != 0)
      {
        /*
         * Have we already visited this type of section? Avoid nesting for
         * ever.
         */
        if ((sect->flags & visited_mask) != 0)
        {
          rtems_rtl_set_error (errno, "section link loop");
          return -1;
        }
        return rtems_rtl_obj_sections_linked_to_order (obj,
                                                       sect->link,
                                                       visited_mask | mask);
      }
      node = rtems_chain_first (sections);
      while (!rtems_chain_is_tail (sections, node))
      {
        sect = (rtems_rtl_obj_sect_t*) node;
        if ((sect->flags & mask) == mask)
        {
          if (sect->section == section)
            return order;
          ++order;
        }
        node = rtems_chain_next (node);
      }
    }
    node = rtems_chain_next (node);
  }
  rtems_rtl_set_error (errno, "section link not found");
  return -1;
}

static void
rtems_rtl_obj_sections_link_order (uint32_t mask, rtems_rtl_obj_t* obj)
{
  rtems_chain_control* sections = &obj->sections;
  rtems_chain_node*    node = rtems_chain_first (sections);
  int                  order = 0;
  while (!rtems_chain_is_tail (sections, node))
  {
    rtems_rtl_obj_sect_t* sect = (rtems_rtl_obj_sect_t*) node;
    if ((sect->flags & mask) == mask)
    {
      /*
       * If the section is linked in order find the linked-to section's order
       * and move the section in the section list to
       */
      if (sect->link == 0)
        sect->load_order = order++;
      else
      {
        sect->load_order =
          rtems_rtl_obj_sections_linked_to_order (obj,
                                                  sect->link,
                                                  mask);
      }
    }
    node = rtems_chain_next (node);
  }
}

static size_t
rtems_rtl_obj_sections_loader (uint32_t                     mask,
                               rtems_rtl_obj_t*             obj,
                               int                          fd,
                               uint8_t*                     base,
                               rtems_rtl_obj_sect_handler_t handler,
                               void*                        data)
{
  rtems_chain_control* sections = &obj->sections;
  rtems_chain_node*    node = rtems_chain_first (sections);
  size_t               base_offset = 0;
  bool                 first = true;
  int                  order = 0;

  while (!rtems_chain_is_tail (sections, node))
  {
    rtems_rtl_obj_sect_t* sect = (rtems_rtl_obj_sect_t*) node;

    if ((sect->size != 0) && ((sect->flags & mask) != 0))
    {
      if (sect->load_order == order)
      {
        if (!first)
          base_offset = rtems_rtl_sect_align (base_offset, sect->alignment);

        first = false;

        sect->base = base + base_offset;

        if (rtems_rtl_trace (RTEMS_RTL_TRACE_LOAD_SECT))
          printf ("rtl: loading:%2d: %s -> %8p (s:%zi f:%04lx a:%lu l:%02d)\n",
                  order, sect->name, sect->base, sect->size,
                  sect->flags, sect->alignment, sect->link);

        if ((sect->flags & RTEMS_RTL_OBJ_SECT_LOAD) == RTEMS_RTL_OBJ_SECT_LOAD)
        {
          if (!handler (obj, fd, sect, data))
          {
            sect->base = 0;
            return false;
          }
        }
        else if ((sect->flags & RTEMS_RTL_OBJ_SECT_ZERO) == RTEMS_RTL_OBJ_SECT_ZERO)
        {
          memset (base + base_offset, 0, sect->size);
        }
        else
        {
          sect->base = 0;
          rtems_rtl_set_error (errno, "section has no load/clear op");
          return false;
        }

        base_offset += sect->size;

        ++order;

        node = rtems_chain_first (sections);
        continue;
      }
    }

    node = rtems_chain_next (node);
  }

  return true;
}

bool
rtems_rtl_obj_load_sections (rtems_rtl_obj_t*             obj,
                             int                          fd,
                             rtems_rtl_obj_sect_handler_t handler,
                             void*                        data)
{
  size_t text_size;
  size_t const_size;
  size_t eh_size;
  size_t data_size;
  size_t bss_size;

  text_size  = rtems_rtl_obj_text_size (obj) + rtems_rtl_obj_const_alignment (obj);
  const_size = rtems_rtl_obj_const_size (obj) + rtems_rtl_obj_eh_alignment (obj);
  eh_size    = rtems_rtl_obj_eh_size (obj) + rtems_rtl_obj_data_alignment (obj);
  data_size  = rtems_rtl_obj_data_size (obj) + rtems_rtl_obj_bss_alignment (obj);
  bss_size   = rtems_rtl_obj_bss_size (obj);

  /*
   * Set the sizes held in the object data. We need this for a fast reference.
   */
  obj->text_size = text_size;
  obj->eh_size   = eh_size;
  obj->bss_size  = bss_size;

  /*
   * Let the allocator manage the actual allocation. The user can use the
   * standard heap or provide a specific allocator with memory protection.
   */
  if (!rtems_rtl_alloc_module_new (&obj->text_base, text_size,
                                   &obj->const_base, const_size,
                                   &obj->eh_base, eh_size,
                                   &obj->data_base, data_size,
                                   &obj->bss_base, bss_size))
  {
    obj->exec_size = 0;
    rtems_rtl_set_error (ENOMEM, "no memory to load obj");
    return false;
  }

  obj->exec_size = text_size + const_size + eh_size + data_size + bss_size;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_LOAD_SECT))
  {
    printf ("rtl: load sect: text  - b:%p s:%zi a:%" PRIu32 "\n",
            obj->text_base, text_size, rtems_rtl_obj_text_alignment (obj));
    printf ("rtl: load sect: const - b:%p s:%zi a:%" PRIu32 "\n",
            obj->const_base, const_size, rtems_rtl_obj_const_alignment (obj));
    printf ("rtl: load sect: eh    - b:%p s:%zi a:%" PRIu32 "\n",
            obj->eh_base, eh_size, rtems_rtl_obj_eh_alignment (obj));
    printf ("rtl: load sect: data  - b:%p s:%zi a:%" PRIu32 "\n",
            obj->data_base, data_size, rtems_rtl_obj_data_alignment (obj));
    printf ("rtl: load sect: bss   - b:%p s:%zi a:%" PRIu32 "\n",
            obj->bss_base, bss_size, rtems_rtl_obj_bss_alignment (obj));
  }

  /*
   * Determine the load order.
   */
  rtems_rtl_obj_sections_link_order (RTEMS_RTL_OBJ_SECT_TEXT,  obj);
  rtems_rtl_obj_sections_link_order (RTEMS_RTL_OBJ_SECT_CONST, obj);
  rtems_rtl_obj_sections_link_order (RTEMS_RTL_OBJ_SECT_EH,    obj);
  rtems_rtl_obj_sections_link_order (RTEMS_RTL_OBJ_SECT_DATA,  obj);

  /*
   * Load all text then data then bss sections in seperate operations so each
   * type of section is grouped together.
   */
  if (!rtems_rtl_obj_sections_loader (RTEMS_RTL_OBJ_SECT_TEXT,
                                      obj, fd, obj->text_base, handler, data) ||
      !rtems_rtl_obj_sections_loader (RTEMS_RTL_OBJ_SECT_CONST,
                                      obj, fd, obj->const_base, handler, data) ||
      !rtems_rtl_obj_sections_loader (RTEMS_RTL_OBJ_SECT_EH,
                                      obj, fd, obj->eh_base, handler, data) ||
      !rtems_rtl_obj_sections_loader (RTEMS_RTL_OBJ_SECT_DATA,
                                      obj, fd, obj->data_base, handler, data) ||
      !rtems_rtl_obj_sections_loader (RTEMS_RTL_OBJ_SECT_BSS,
                                      obj, fd, obj->bss_base, handler, data))
  {
    rtems_rtl_alloc_module_del (&obj->text_base, &obj->const_base, &obj->eh_base,
                                &obj->data_base, &obj->bss_base);
    obj->exec_size = 0;
    return false;
  }

  return true;
}

static void
rtems_rtl_obj_run_cdtors (rtems_rtl_obj_t* obj, uint32_t mask)
{
  rtems_chain_node* node = rtems_chain_first (&obj->sections);
  while (!rtems_chain_is_tail (&obj->sections, node))
  {
    rtems_rtl_obj_sect_t* sect = (rtems_rtl_obj_sect_t*) node;
    if ((sect->flags & mask) == mask)
    {
      rtems_rtl_cdtor_t* handler;
      size_t             handlers = sect->size / sizeof (rtems_rtl_cdtor_t);
      int                c;
      for (c = 0, handler = sect->base; c < handlers; ++c)
        if (*handler)
          (*handler) ();
    }
    node = rtems_chain_next (node);
  }
}

void
rtems_rtl_obj_run_ctors (rtems_rtl_obj_t* obj)
{
  return rtems_rtl_obj_run_cdtors (obj, RTEMS_RTL_OBJ_SECT_CTOR);
}

void
rtems_rtl_obj_run_dtors (rtems_rtl_obj_t* obj)
{
  return rtems_rtl_obj_run_cdtors (obj, RTEMS_RTL_OBJ_SECT_DTOR);
}

/**
 * Find a module in an archive returning the offset in the archive in the
 * object descriptor.
 */
static bool
rtems_rtl_obj_archive_find (rtems_rtl_obj_t* obj, int fd)
{
#define RTEMS_RTL_AR_IDENT      "!<arch>\n"
#define RTEMS_RTL_AR_IDENT_SIZE (sizeof (RTEMS_RTL_AR_IDENT) - 1)
#define RTEMS_RTL_AR_FHDR_BASE  RTEMS_RTL_AR_IDENT_SIZE
#define RTEMS_RTL_AR_FNAME      (0)
#define RTEMS_RTL_AR_FNAME_SIZE (16)
#define RTEMS_RTL_AR_SIZE       (48)
#define RTEMS_RTL_AR_SIZE_SIZE  (10)
#define RTEMS_RTL_AR_MAGIC      (58)
#define RTEMS_RTL_AR_MAGIC_SIZE (2)
#define RTEMS_RTL_AR_FHDR_SIZE  (60)

  size_t  fsize = obj->fsize;
  off_t   extended_file_names;
  uint8_t header[RTEMS_RTL_AR_FHDR_SIZE];
  bool    scanning;

  if (read (fd, &header[0], RTEMS_RTL_AR_IDENT_SIZE) !=  RTEMS_RTL_AR_IDENT_SIZE)
  {
    rtems_rtl_set_error (errno, "reading archive identifer");
    return false;
  }

  if (memcmp (header, RTEMS_RTL_AR_IDENT, RTEMS_RTL_AR_IDENT_SIZE) != 0)
  {
    rtems_rtl_set_error (EINVAL, "invalid archive identifer");
    return false;
  }

  /*
   * Seek to the current offset in the archive and if we have a valid archive
   * file header present check the file name for a match with the oname field
   * of the object descriptor. If the archive header is not valid and it is the
   * first pass reset the offset and start the search again in case the offset
   * provided is not valid any more.
   *
   * The archive can have a symbol table at the start. Ignore it. A symbol
   * table has the file name '/'.
   *
   * The archive can also have the GNU extended file name table. This
   * complicates the processing. If the object's file name starts with '/' the
   * remainder of the file name is an offset into the extended file name
   * table. To find the extended file name table we need to scan from start of
   * the archive for a file name of '//'. Once found we remeber the table's
   * start and can direct seek to file name location. In other words the scan
   * only happens once.
   *
   * If the name had the offset encoded we go straight to that location.
   */

  if (obj->ooffset != 0)
    scanning = false;
  else
  {
    scanning = true;
    obj->ooffset = RTEMS_RTL_AR_FHDR_BASE;
  }

  extended_file_names = 0;

  while (obj->ooffset < fsize)
  {
    /*
     * Clean up any existing data.
     */
    memset (header, 0, sizeof (header));

    if (!rtems_rtl_seek_read (fd, obj->ooffset, RTEMS_RTL_AR_FHDR_SIZE, &header[0]))
    {
      rtems_rtl_set_error (errno, "seek/read archive file header");
      obj->ooffset = 0;
      obj->fsize = 0;
      return false;
    }

    if ((header[RTEMS_RTL_AR_MAGIC] != 0x60) ||
        (header[RTEMS_RTL_AR_MAGIC + 1] != 0x0a))
    {
      if (scanning)
      {
        rtems_rtl_set_error (EINVAL, "invalid archive file header");
        obj->ooffset = 0;
        obj->fsize = 0;
        return false;
      }

      scanning = true;
      obj->ooffset = RTEMS_RTL_AR_FHDR_BASE;
      continue;
    }

    /*
     * The archive header is always aligned to an even address.
     */
    obj->fsize = (rtems_rtl_scan_decimal (&header[RTEMS_RTL_AR_SIZE],
                                          RTEMS_RTL_AR_SIZE_SIZE) + 1) & ~1;

    /*
     * Check for the GNU extensions.
     */
    if (header[0] == '/')
    {
      off_t extended_off;

      switch (header[1])
      {
        case ' ':
          /*
           * Symbols table. Ignore the table.
           */
          break;
        case '/':
          /*
           * Extended file names table. Remember.
           */
          extended_file_names = obj->ooffset + RTEMS_RTL_AR_FHDR_SIZE;
          break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          /*
           * Offset into the extended file name table. If we do not have the
           * offset to the extended file name table find it.
           */
          extended_off =
            rtems_rtl_scan_decimal (&header[1], RTEMS_RTL_AR_FNAME_SIZE);

          if (extended_file_names == 0)
          {
            off_t off = obj->ooffset;
            while (extended_file_names == 0)
            {
              off_t esize =
                (rtems_rtl_scan_decimal (&header[RTEMS_RTL_AR_SIZE],
                                         RTEMS_RTL_AR_SIZE_SIZE) + 1) & ~1;
              off += esize + RTEMS_RTL_AR_FHDR_SIZE;

              if (!rtems_rtl_seek_read (fd, off,
                                        RTEMS_RTL_AR_FHDR_SIZE, &header[0]))
              {
                rtems_rtl_set_error (errno,
                                     "seeking/reading archive ext file name header");
                obj->ooffset = 0;
                obj->fsize = 0;
                return false;
              }

              if ((header[RTEMS_RTL_AR_MAGIC] != 0x60) ||
                  (header[RTEMS_RTL_AR_MAGIC + 1] != 0x0a))
              {
                rtems_rtl_set_error (errno, "invalid archive file header");
                obj->ooffset = 0;
                obj->fsize = 0;
                return false;
              }

              if ((header[0] == '/') && (header[1] == '/'))
              {
                extended_file_names = off + RTEMS_RTL_AR_FHDR_SIZE;
                break;
              }
            }
          }

          if (extended_file_names)
          {
            /*
             * We know the offset in the archive to the extended file. Read the
             * name from the table and compare with the name we are after.
             */
#define RTEMS_RTL_MAX_FILE_SIZE (256)
            char name[RTEMS_RTL_MAX_FILE_SIZE];

            if (!rtems_rtl_seek_read (fd, extended_file_names + extended_off,
                                      RTEMS_RTL_MAX_FILE_SIZE, (uint8_t*) &name[0]))
            {
              rtems_rtl_set_error (errno,
                                   "invalid archive ext file seek/read");
              obj->ooffset = 0;
              obj->fsize = 0;
              return false;
            }

            if (rtems_rtl_match_name (obj, name))
            {
              obj->ooffset += RTEMS_RTL_AR_FHDR_SIZE;
              return true;
            }
          }
          break;
        default:
          /*
           * Ignore the file because we do not know what it it.
           */
          break;
      }
    }
    else
    {
      if (rtems_rtl_match_name (obj, (const char*) &header[RTEMS_RTL_AR_FNAME]))
      {
        obj->ooffset += RTEMS_RTL_AR_FHDR_SIZE;
        return true;
      }
    }

    obj->ooffset += obj->fsize + RTEMS_RTL_AR_FHDR_SIZE;
  }

  rtems_rtl_set_error (ENOENT, "object file not found");
  obj->ooffset = 0;
  obj->fsize = 0;
  return false;
}

static bool
rtems_rtl_obj_file_load (rtems_rtl_obj_t* obj, int fd)
{
  int l;

  for (l = 0; l < (sizeof (loaders) / sizeof (rtems_rtl_loader_table_t)); ++l)
  {
    if (loaders[l].check (obj, fd))
    {
      obj->format = l;
      return loaders[l].load (obj, fd);
    }
  }

  rtems_rtl_set_error (ENOENT, "no format loader found");
  return false;
}

static bool
rtems_rtl_obj_file_unload (rtems_rtl_obj_t* obj)
{
  if (obj->format >= 0 && obj->format < RTEMS_RTL_LOADERS)
      return loaders[obj->format].unload (obj);
  return false;
}

bool
rtems_rtl_obj_load (rtems_rtl_obj_t* obj)
{
  int fd;

  if (!rtems_rtl_obj_fname_valid (obj))
  {
    rtems_rtl_set_error (ENOMEM, "invalid object file name path");
    return false;
  }

  fd = open (rtems_rtl_obj_fname (obj), O_RDONLY);
  if (fd < 0)
  {
    rtems_rtl_set_error (ENOMEM, "opening for object file");
    return false;
  }

  /*
   * Find the object file in the archive if it is an archive that
   * has been opened.
   */
  if (rtems_rtl_obj_aname_valid (obj))
  {
    if (!rtems_rtl_obj_archive_find (obj, fd))
    {
      close (fd);
      return false;
    }
  }

  /*
   * Call the format specific loader.
   */
  if (!rtems_rtl_obj_file_load (obj, fd))
  {
    close (fd);
    return false;
  }

  if (!_rtld_linkmap_add (obj)) /* For GDB */
  {
    close (fd);
    return false;
  }

  close (fd);

  return true;
}

bool
rtems_rtl_obj_unload (rtems_rtl_obj_t* obj)
{
  _rtld_linkmap_delete(obj);
  rtems_rtl_obj_file_unload (obj);
  return true;
}
