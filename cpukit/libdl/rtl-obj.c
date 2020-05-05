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
 * @ingroup rtl
 *
 * @brief RTEMS Run-Time Linker Error
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <rtems/libio_.h>

#include <rtems/rtl/rtl.h>
#include "rtl-chain-iterator.h"
#include <rtems/rtl/rtl-obj.h>
#include "rtl-error.h"
#include "rtl-find-file.h"
#include "rtl-string.h"
#include <rtems/rtl/rtl-trace.h>

#define RTEMS_RTL_ELF_LOADER 1
#define RTEMS_RTL_RAP_LOADER 1

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
static const rtems_rtl_loader_table loaders[RTEMS_RTL_LOADERS] =
{
#if RTEMS_RTL_RAP_LOADER
  { .check     = rtems_rtl_rap_file_check,
    .load      = rtems_rtl_rap_file_load,
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

rtems_rtl_obj*
rtems_rtl_obj_alloc (void)
{
  rtems_rtl_obj* obj = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT,
                                            sizeof (rtems_rtl_obj),
                                            true);
  if (obj)
  {
    /*
     * Initialise the chains.
     */
    rtems_chain_initialize_empty (&obj->sections);
    rtems_chain_initialize_empty (&obj->dependents);
    /*
     * No valid format.
     */
    obj->format = -1;
  }
  return obj;
}

static void
rtems_rtl_obj_free_names (rtems_rtl_obj* obj)
{
  if (rtems_rtl_obj_oname_valid (obj))
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, (void*) obj->oname);
  if (rtems_rtl_obj_aname_valid (obj))
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, (void*) obj->aname);
  if (rtems_rtl_obj_fname_valid (obj))
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, (void*) obj->fname);
}

bool
rtems_rtl_obj_free (rtems_rtl_obj* obj)
{
  if (obj->users > 0 || ((obj->flags & RTEMS_RTL_OBJ_LOCKED) != 0))
  {
    rtems_rtl_set_error (EINVAL, "cannot free obj still in use");
    return false;
  }
  if (!rtems_chain_is_node_off_chain (&obj->link))
    rtems_chain_extract (&obj->link);
  rtems_rtl_alloc_module_del (&obj->text_base, &obj->const_base, &obj->eh_base,
                              &obj->data_base, &obj->bss_base);
  rtems_rtl_obj_erase_sections (obj);
  rtems_rtl_obj_erase_dependents (obj);
  rtems_rtl_symbol_obj_erase (obj);
  rtems_rtl_obj_erase_trampoline (obj);
  rtems_rtl_obj_free_names (obj);
  if (obj->sec_num != NULL)
    free (obj->sec_num);
  if (obj->linkmap != NULL)
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, (void*) obj->linkmap);
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, obj);
  return true;
}

typedef struct rtems_rtl_obj_unresolved_data
{
  bool has_unresolved;
} rtems_rtl_obj_unresolved_data;

static bool
rtems_rtl_obj_unresolved_dependent (rtems_rtl_obj* obj,
                                    rtems_rtl_obj* dependent,
                                    void*          data)
{
  rtems_rtl_obj_unresolved_data* ud;
  ud = (rtems_rtl_obj_unresolved_data*) data;
  if ((dependent->flags & RTEMS_RTL_OBJ_DEP_VISITED) == 0)
  {
    dependent->flags |= RTEMS_RTL_OBJ_DEP_VISITED;
    if ((dependent->flags & RTEMS_RTL_OBJ_UNRESOLVED) != 0)
      ud->has_unresolved = true;
    else
    {
      rtems_rtl_obj_iterate_dependents (dependent,
                                        rtems_rtl_obj_unresolved_dependent,
                                        ud);
    }
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
      printf ("rtl: obj: unresolved: dep: %s is %s\n",
              dependent->oname, ud->has_unresolved ? "unresolved" : "resolved");
  }
  return ud->has_unresolved;
}

static bool
rtems_rtl_obj_unresolved_object (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj*                 obj = (rtems_rtl_obj*) node;
  rtems_rtl_obj_unresolved_data* ud;
  ud = (rtems_rtl_obj_unresolved_data*) data;
  ud->has_unresolved = (obj->flags & RTEMS_RTL_OBJ_UNRESOLVED) != 0;
  return !ud->has_unresolved;
}

bool
rtems_rtl_obj_unresolved (rtems_rtl_obj* obj)
{
  rtems_rtl_obj_unresolved_data ud = {
    .has_unresolved = (obj->flags & RTEMS_RTL_OBJ_UNRESOLVED) != 0
  };
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_UNRESOLVED))
    printf ("rtl: obj: unresolved: dep: %s is %s\n",
            obj->oname, ud.has_unresolved ? "unresolved" : "resolved");
  if (!ud.has_unresolved)
  {
    if ((obj->flags & RTEMS_RTL_OBJ_BASE) != 0)
    {
      rtems_rtl_data* rtl = rtems_rtl_data_unprotected ();
      rtems_rtl_chain_iterate (&rtl->objects,
                               rtems_rtl_obj_unresolved_object,
                               &ud);
    }
    else
    {
      rtems_rtl_obj_update_flags (RTEMS_RTL_OBJ_DEP_VISITED, 0);
      obj->flags |= RTEMS_RTL_OBJ_DEP_VISITED;
      rtems_rtl_obj_iterate_dependents (obj,
                                        rtems_rtl_obj_unresolved_dependent,
                                        &ud);
      rtems_rtl_obj_update_flags (RTEMS_RTL_OBJ_DEP_VISITED, 0);
    }
  }
  return ud.has_unresolved;
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
   *
   * Note, if an archive the object file oofset may be know but the
   *       object file is not. Leave the object name as a NULL.
   */
  end = name + strlen (name);
  colon = strrchr (name, ':');
  if (colon == NULL || colon < strrchr(name, '/'))
    colon = end;

  loname = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT, colon - name + 1, true);
  if (!loname)
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
rtems_rtl_obj_parse_name (rtems_rtl_obj* obj, const char* name)
{
  return rtems_rtl_parse_name (name, &(obj->aname), &(obj->oname), &(obj->ooffset));
}

/**
 * Section size summer iterator data.
 */
typedef struct
{
  uint32_t mask; /**< The selection mask to sum. */
  size_t   size; /**< The size of all section fragments. */
} rtems_rtl_obj_sect_summer_data;

static bool
rtems_rtl_obj_sect_summer (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj_sect* sect = (rtems_rtl_obj_sect*) node;
  if ((sect->flags & RTEMS_RTL_OBJ_SECT_ARCH_ALLOC) == 0)
  {
    rtems_rtl_obj_sect_summer_data* summer = data;
    if ((sect->flags & summer->mask) == summer->mask)
      summer->size =
        rtems_rtl_obj_align (summer->size, sect->alignment) + sect->size;
  }
  return true;
}

static size_t
rtems_rtl_obj_section_size (const rtems_rtl_obj* obj, uint32_t mask)
{
  rtems_rtl_obj_sect_summer_data summer;
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
} rtems_rtl_obj_sect_aligner_data;

/**
 * The section aligner iterator.
 */
static bool
rtems_rtl_obj_sect_aligner (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj_sect*              sect = (rtems_rtl_obj_sect*) node;
  rtems_rtl_obj_sect_aligner_data* aligner = data;
  if ((sect->flags & aligner->mask) == aligner->mask)
  {
    aligner->alignment = sect->alignment;
    return false;
  }
  return true;
}

static size_t
rtems_rtl_obj_section_alignment (const rtems_rtl_obj* obj, uint32_t mask)
{
  rtems_rtl_obj_sect_aligner_data aligner;
  aligner.mask = mask;
  aligner.alignment = 0;
  rtems_rtl_chain_iterate ((rtems_chain_control*) &obj->sections,
                           rtems_rtl_obj_sect_aligner,
                           &aligner);
  return aligner.alignment;
}

static bool
rtems_rtl_obj_section_handler (uint32_t                   mask,
                               rtems_rtl_obj*             obj,
                               int                        fd,
                               rtems_rtl_obj_sect_handler handler,
                               void*                      data)
{
  rtems_chain_node* node = rtems_chain_first (&obj->sections);
  while (!rtems_chain_is_tail (&obj->sections, node))
  {
    rtems_rtl_obj_sect* sect = (rtems_rtl_obj_sect*) node;
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
rtems_rtl_obj_find_file (rtems_rtl_obj* obj, const char* name)
{
  const char*     pname;
  rtems_rtl_data* rtl;

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
rtems_rtl_obj_add_section (rtems_rtl_obj* obj,
                           int            section,
                           const char*    name,
                           size_t         size,
                           off_t          offset,
                           uint32_t       alignment,
                           int            link,
                           int            info,
                           uint32_t       flags)
{
  if (size > 0)
  {
    rtems_rtl_obj_sect* sect = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT,
                                                    sizeof (rtems_rtl_obj_sect),
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
      printf ("rtl: sect: add: %-2d: %s (%zu) 0x%08" PRIu32 "\n",
              section, name, size, flags);
  }
  return true;
}

void
rtems_rtl_obj_erase_sections (rtems_rtl_obj* obj)
{
  rtems_chain_node* node = rtems_chain_first (&obj->sections);
  while (!rtems_chain_is_tail (&obj->sections, node))
  {
    rtems_rtl_obj_sect* sect = (rtems_rtl_obj_sect*) node;
    rtems_chain_node*   next_node = rtems_chain_next (node);
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
  rtems_rtl_obj_sect*  sect;  /**< The matching section. */
  const char*          name;  /**< The name to match. */
  int                  index; /**< The index to match. */
  uint32_t             mask;  /**< The mask to match. */
  unsigned int         flags; /**< The flags to use when matching. */
} rtems_rtl_obj_sect_finder;

static bool
rtems_rtl_obj_sect_match_name (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj_sect*        sect = (rtems_rtl_obj_sect*) node;
  rtems_rtl_obj_sect_finder* match = data;
  if (strcmp (sect->name, match->name) == 0)
  {
    match->sect = sect;
    return false;
  }
  return true;
}

rtems_rtl_obj_sect*
rtems_rtl_obj_find_section (const rtems_rtl_obj* obj,
                            const char*          name)
{
  rtems_rtl_obj_sect_finder match;
  match.sect = NULL;
  match.name = name;
  match.mask = 0;
  match.flags = 0;
  rtems_rtl_chain_iterate ((rtems_chain_control*) &obj->sections,
                           rtems_rtl_obj_sect_match_name,
                           &match);
  return match.sect;
}

static bool
rtems_rtl_obj_sect_match_index (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj_sect*        sect = (rtems_rtl_obj_sect*) node;
  rtems_rtl_obj_sect_finder* match = data;
  if (sect->section == match->index)
  {
    match->sect = sect;
    return false;
  }
  return true;
}

rtems_rtl_obj_sect*
rtems_rtl_obj_find_section_by_index (const rtems_rtl_obj* obj,
                                     int                  index)
{
  rtems_rtl_obj_sect_finder match;
  match.sect = NULL;
  match.index = index;
  match.mask = 0;
  match.flags = 0;
  rtems_rtl_chain_iterate ((rtems_chain_control*) &obj->sections,
                           rtems_rtl_obj_sect_match_index,
                           &match);
  return match.sect;
}

static bool
rtems_rtl_obj_sect_match_mask (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj_sect*        sect = (rtems_rtl_obj_sect*) node;
  rtems_rtl_obj_sect_finder* match = data;
  if (match->flags == 0)
  {
    if (match->index < 0 || sect->section == match->index)
      match->flags = 1;
    if (match->index >= 0)
      return true;
  }
  if ((sect->flags & match->mask) != 0)
  {
    match->sect = sect;
    return false;
  }
  return true;
}

rtems_rtl_obj_sect*
rtems_rtl_obj_find_section_by_mask (const rtems_rtl_obj* obj,
                                    int                  index,
                                    uint32_t             mask)
{
  rtems_rtl_obj_sect_finder match;
  match.sect = NULL;
  match.index = index;
  match.mask = mask;
  match.flags = 0;
  rtems_rtl_chain_iterate ((rtems_chain_control*) &obj->sections,
                           rtems_rtl_obj_sect_match_mask,
                           &match);
  return match.sect;
}

bool
rtems_rtl_obj_alloc_trampoline (rtems_rtl_obj* obj)
{
  if (obj->tramps_size == 0)
    return true;
  obj->trampoline = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT,
                                         obj->tramps_size,
                                         true);
  if (obj->trampoline == NULL)
    rtems_rtl_set_error (ENOMEM, "no memory for the trampoline");
  obj->tramp_brk = obj->trampoline;
  return obj->trampoline != NULL;
}

void
rtems_rtl_obj_erase_trampoline (rtems_rtl_obj* obj)
{
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, obj->trampoline);
}

bool
rtems_rtl_obj_alloc_dependents (rtems_rtl_obj* obj, size_t dependents)
{
  rtems_rtl_obj_depends* depends;
  size_t                 size;

  size = sizeof (rtems_rtl_obj_depends) + sizeof (rtems_rtl_obj*) * dependents;

  depends = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT,
                                 size,
                                 true);
  if (depends == NULL)
  {
    rtems_rtl_set_error (ENOMEM, "no memory for the dependency");
  }
  else
  {
    depends->dependents = dependents;
    rtems_chain_append (&obj->dependents, &depends->node);
  }

  return depends != NULL;
}

void
rtems_rtl_obj_erase_dependents (rtems_rtl_obj* obj)
{
  rtems_chain_node* node = rtems_chain_first (&obj->dependents);
  while (!rtems_chain_is_tail (&obj->dependents, node))
  {
    rtems_rtl_obj_depends* depends = (rtems_rtl_obj_depends*) node;
    rtems_chain_node*      next_node = rtems_chain_next (node);
    rtems_chain_extract (node);
    rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, depends);
    node = next_node;
  }
}

bool
rtems_rtl_obj_add_dependent (rtems_rtl_obj* obj, rtems_rtl_obj* dependent)
{
  rtems_rtl_obj**   free_slot;
  rtems_chain_node* node;

  if (obj == dependent || dependent == rtems_rtl_baseimage ())
    return false;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_DEPENDENCY))
    printf ("rtl: depend: add: %s -> %s\n", obj->oname, dependent->oname);

  free_slot = NULL;

  node = rtems_chain_first (&obj->dependents);
  while (!rtems_chain_is_tail (&obj->dependents, node))
  {
    rtems_rtl_obj_depends* depends = (rtems_rtl_obj_depends*) node;
    size_t                 d;
    for (d = 0; d < depends->dependents; ++d)
    {
      if (free_slot == NULL && depends->depends[d] == NULL)
        free_slot = &(depends->depends[d]);
      if (depends->depends[d] == dependent)
        return false;
    }
    node = rtems_chain_next (node);
  }

  if (free_slot == NULL)
  {
    if (rtems_rtl_obj_alloc_dependents (obj,
                                        RTEMS_RTL_DEPENDENCY_BLOCK_SIZE))
    {
      rtems_rtl_obj_depends* depends;
      node = rtems_chain_last (&obj->dependents);
      depends = (rtems_rtl_obj_depends*) node;
      free_slot = &(depends->depends[0]);
      if (*free_slot != NULL)
      {
        rtems_rtl_set_error (EINVAL, "new dependency node not empty");
        free_slot = NULL;
      }
    }
  }

  if (free_slot != NULL)
    *free_slot = dependent;

  return free_slot != NULL;
}


bool
rtems_rtl_obj_remove_dependencies (rtems_rtl_obj* obj)
{
  /*
   * If there are no references unload the object.
   */
  if (obj->refs == 0)
  {
    /*
     * Remove the refences from the object files this file depend on. The
     * unload happens once the list of objects to be unloaded has been made and
     * the destructors have been called for all those modules.
     */
    rtems_chain_node* node = rtems_chain_first (&obj->dependents);
    while (!rtems_chain_is_tail (&obj->dependents, node))
    {
      rtems_rtl_obj_depends* depends = (rtems_rtl_obj_depends*) node;
      size_t                 d;
      for (d = 0; d < depends->dependents; ++d)
      {
        if (depends->depends[d] != NULL)
        {
          rtems_rtl_obj_dec_reference (depends->depends[d]);
          depends->depends[d] = NULL;
        }
      }
      node = rtems_chain_next (node);
    }
    return true;
  }
  return false;
}

bool
rtems_rtl_obj_iterate_dependents (rtems_rtl_obj*                 obj,
                                  rtems_rtl_obj_depends_iterator iterator,
                                  void*                          data)
{
  rtems_chain_node* node = rtems_chain_first (&obj->dependents);
  while (!rtems_chain_is_tail (&obj->dependents, node))
  {
    rtems_rtl_obj_depends* depends = (rtems_rtl_obj_depends*) node;
    size_t                 d;
    for (d = 0; d < depends->dependents; ++d)
    {
      if (depends->depends[d])
      {
        if (iterator (obj, depends->depends[d], data))
          return true;
      }
    }
    node = rtems_chain_next (node);
  }
  return false;
}

size_t
rtems_rtl_obj_text_size (const rtems_rtl_obj* obj)
{
  const uint32_t flags = RTEMS_RTL_OBJ_SECT_LOAD | RTEMS_RTL_OBJ_SECT_TEXT;
  return rtems_rtl_obj_section_size (obj, flags);
}

uint32_t
rtems_rtl_obj_text_alignment (const rtems_rtl_obj* obj)
{
  const uint32_t flags = RTEMS_RTL_OBJ_SECT_LOAD | RTEMS_RTL_OBJ_SECT_TEXT;
  return rtems_rtl_obj_section_alignment (obj, flags);
}

size_t
rtems_rtl_obj_const_size (const rtems_rtl_obj* obj)
{
  const uint32_t flags = RTEMS_RTL_OBJ_SECT_LOAD | RTEMS_RTL_OBJ_SECT_CONST;
  return rtems_rtl_obj_section_size (obj, flags);
}

uint32_t
rtems_rtl_obj_const_alignment (const rtems_rtl_obj* obj)
{
  const uint32_t flags = RTEMS_RTL_OBJ_SECT_LOAD | RTEMS_RTL_OBJ_SECT_CONST;
  return rtems_rtl_obj_section_alignment (obj, flags);
}

uint32_t
rtems_rtl_obj_eh_alignment (const rtems_rtl_obj* obj)
{
  const uint32_t flags = RTEMS_RTL_OBJ_SECT_LOAD | RTEMS_RTL_OBJ_SECT_EH;
  return rtems_rtl_obj_section_alignment (obj, flags);
}

size_t
rtems_rtl_obj_eh_size (const rtems_rtl_obj* obj)
{
  const uint32_t flags = RTEMS_RTL_OBJ_SECT_LOAD | RTEMS_RTL_OBJ_SECT_EH;
  return rtems_rtl_obj_section_size (obj, flags);
}

size_t
rtems_rtl_obj_data_size (const rtems_rtl_obj* obj)
{
  const uint32_t flags = RTEMS_RTL_OBJ_SECT_LOAD | RTEMS_RTL_OBJ_SECT_DATA;
  return rtems_rtl_obj_section_size (obj, flags);
}

uint32_t
rtems_rtl_obj_data_alignment (const rtems_rtl_obj* obj)
{
  const uint32_t flags = RTEMS_RTL_OBJ_SECT_LOAD | RTEMS_RTL_OBJ_SECT_DATA;
  return rtems_rtl_obj_section_alignment (obj, flags);
}

size_t
rtems_rtl_obj_bss_size (const rtems_rtl_obj* obj)
{
  return rtems_rtl_obj_section_size (obj, RTEMS_RTL_OBJ_SECT_BSS);
}

uint32_t
rtems_rtl_obj_bss_alignment (const rtems_rtl_obj* obj)
{
  return rtems_rtl_obj_section_alignment (obj, RTEMS_RTL_OBJ_SECT_BSS);
}

bool
rtems_rtl_obj_relocate (rtems_rtl_obj*             obj,
                        int                        fd,
                        rtems_rtl_obj_sect_handler handler,
                        void*                      data)
{
  const uint32_t flags = (RTEMS_RTL_OBJ_SECT_LOAD |
                          RTEMS_RTL_OBJ_SECT_REL |
                          RTEMS_RTL_OBJ_SECT_RELA);
  bool r = rtems_rtl_obj_section_handler (flags, obj, fd, handler, data);
  rtems_rtl_obj_update_flags (RTEMS_RTL_OBJ_RELOC_TAG, 0);
  return r;
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
} rtems_rtl_obj_sect_sync_ctx;

static bool
rtems_rtl_obj_sect_sync_handler (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj_sect*          sect = (rtems_rtl_obj_sect*) node;
  rtems_rtl_obj_sect_sync_ctx* sync_ctx = data;
  uintptr_t                    old_end;
  uintptr_t                    new_start;

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
rtems_rtl_obj_synchronize_cache (rtems_rtl_obj* obj)
{
  rtems_rtl_obj_sect_sync_ctx sync_ctx;

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

  if (sync_ctx.end_va != sync_ctx.start_va)
  {
    size_t size = sync_ctx.end_va - sync_ctx.start_va;
    rtems_cache_instruction_sync_after_code_change(sync_ctx.start_va,
                                                   size);
  }

  if (obj->trampoline != NULL)
  {
    rtems_cache_instruction_sync_after_code_change(obj->trampoline,
                                                   obj->tramps_size);
  }
}

bool
rtems_rtl_obj_load_symbols (rtems_rtl_obj*             obj,
                            int                        fd,
                            rtems_rtl_obj_sect_handler handler,
                            void*                      data)
{
  uint32_t mask = RTEMS_RTL_OBJ_SECT_SYM;
  bool     ok;
  ok = rtems_rtl_obj_section_handler (mask, obj, fd, handler, data);
  if (ok)
    rtems_rtl_symbol_obj_sort (obj);
  return ok;
}

static int
rtems_rtl_obj_sections_linked_to_order (rtems_rtl_obj* obj,
                                        int            section,
                                        uint32_t       visited_mask)
{
  rtems_chain_control* sections = &obj->sections;
  rtems_chain_node*    node = rtems_chain_first (sections);
  /*
   * Find the section being linked-to. If the linked-to link field is 0 we have
   * the end and the section's order is the position we are after.
   */
  while (!rtems_chain_is_tail (sections, node))
  {
    rtems_rtl_obj_sect* sect = (rtems_rtl_obj_sect*) node;
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
        sect = (rtems_rtl_obj_sect*) node;
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
rtems_rtl_obj_sections_link_order (uint32_t mask, rtems_rtl_obj* obj)
{
  rtems_chain_control* sections = &obj->sections;
  rtems_chain_node*    node = rtems_chain_first (sections);
  int                  order = 0;
  while (!rtems_chain_is_tail (sections, node))
  {
    rtems_rtl_obj_sect* sect = (rtems_rtl_obj_sect*) node;
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

static void
rtems_rtl_obj_sections_locate (uint32_t            mask,
                               rtems_rtl_alloc_tag tag,
                               rtems_rtl_obj*      obj,
                               uint8_t*            base)
{
  rtems_chain_control* sections = &obj->sections;
  rtems_chain_node*    node = rtems_chain_first (sections);
  size_t               base_offset = 0;
  int                  order = 0;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_LOAD_SECT))
    printf ("rtl: locating section: mask:%08" PRIx32 " base:%p\n", mask, base);

  while (!rtems_chain_is_tail (sections, node))
  {
    rtems_rtl_obj_sect* sect = (rtems_rtl_obj_sect*) node;

    if ((sect->size != 0) && ((sect->flags & mask) == mask))
    {
      if (sect->load_order == order)
      {
        if ((sect->flags & RTEMS_RTL_OBJ_SECT_ARCH_ALLOC) == 0)
        {
          base_offset = rtems_rtl_obj_align (base_offset, sect->alignment);
          sect->base = base + base_offset;
        }

        if (rtems_rtl_trace (RTEMS_RTL_TRACE_LOAD_SECT))
          printf ("rtl: locating:%2d: %s -> %p (s:%zi f:%04" PRIx32
                  " a:%" PRIu32 " l:%02d)\n",
                  order, sect->name, sect->base, sect->size,
                  sect->flags, sect->alignment, sect->link);

        if (sect->base)
          base_offset += sect->size;

        ++order;

        node = rtems_chain_first (sections);
        continue;
      }
    }

    node = rtems_chain_next (node);
  }
}

bool
rtems_rtl_obj_alloc_sections (rtems_rtl_obj*             obj,
                              int                        fd,
                              rtems_rtl_obj_sect_handler handler,
                              void*                      data)
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
  obj->text_size  = text_size;
  obj->const_size = const_size;
  obj->data_size  = data_size;
  obj->eh_size    = eh_size;
  obj->bss_size   = bss_size;

  /*
   * Perform any specific allocations for sections.
   */
  if (handler != NULL)
  {
    if (!rtems_rtl_obj_section_handler (RTEMS_RTL_OBJ_SECT_TYPES,
                                        obj,
                                        fd,
                                        handler,
                                        data))
    {
      obj->exec_size = 0;
      return false;
    }
  }

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
  rtems_rtl_obj_sections_link_order (RTEMS_RTL_OBJ_SECT_BSS,   obj);

  /*
   * Locate all text, data and bss sections in seperate operations so each type of
   * section is grouped together.
   */
  rtems_rtl_obj_sections_locate (RTEMS_RTL_OBJ_SECT_TEXT,
                                 rtems_rtl_alloc_text_tag (),
                                 obj, obj->text_base);
  rtems_rtl_obj_sections_locate (RTEMS_RTL_OBJ_SECT_CONST,
                                 rtems_rtl_alloc_const_tag (),
                                 obj, obj->const_base);
  rtems_rtl_obj_sections_locate (RTEMS_RTL_OBJ_SECT_EH,
                                 rtems_rtl_alloc_eh_tag (),
                                 obj, obj->eh_base);
  rtems_rtl_obj_sections_locate (RTEMS_RTL_OBJ_SECT_DATA,
                                 rtems_rtl_alloc_data_tag (),
                                 obj, obj->data_base);
  rtems_rtl_obj_sections_locate (RTEMS_RTL_OBJ_SECT_BSS,
                                 rtems_rtl_alloc_bss_tag (),
                                 obj, obj->bss_base);

  return true;
}

static bool
rtems_rtl_obj_sections_loader (uint32_t                   mask,
                               rtems_rtl_alloc_tag        tag,
                               rtems_rtl_obj*             obj,
                               int                        fd,
                               uint8_t*                   base,
                               rtems_rtl_obj_sect_handler handler,
                               void*                      data)
{
  rtems_chain_control* sections = &obj->sections;
  rtems_chain_node*    node = rtems_chain_first (sections);
  int                  order = 0;

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_LOAD_SECT))
    printf ("rtl: loading section: mask:%08" PRIx32 " base:%p\n", mask, base);

  rtems_rtl_alloc_wr_enable (tag, base);

  while (!rtems_chain_is_tail (sections, node))
  {
    rtems_rtl_obj_sect* sect = (rtems_rtl_obj_sect*) node;

    if ((sect->size != 0) && ((sect->flags & mask) == mask))
    {
      if (sect->load_order == order)
      {
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_LOAD_SECT))
          printf ("rtl: loading:%2d: %s -> %p (s:%zi f:%04" PRIx32
                  " a:%" PRIu32 " l:%02d)\n",
                  order, sect->name, sect->base, sect->size,
                  sect->flags, sect->alignment, sect->link);

        if ((sect->flags & RTEMS_RTL_OBJ_SECT_LOAD) == RTEMS_RTL_OBJ_SECT_LOAD)
        {
          if (!handler (obj, fd, sect, data))
          {
            sect->base = 0;
            rtems_rtl_alloc_wr_disable (tag, base);
            return false;
          }
        }
        else if ((sect->flags & RTEMS_RTL_OBJ_SECT_ZERO) == RTEMS_RTL_OBJ_SECT_ZERO)
        {
          memset (sect->base, 0, sect->size);
        }
        else
        {
          /*
           * This section is not to be loaded, clear the base.
           */
          sect->base = 0;
        }

        ++order;

        node = rtems_chain_first (sections);
        continue;
      }
    }

    node = rtems_chain_next (node);
  }

  rtems_rtl_alloc_wr_disable (tag, base);

  return true;
}

bool
rtems_rtl_obj_load_sections (rtems_rtl_obj*             obj,
                             int                        fd,
                             rtems_rtl_obj_sect_handler handler,
                             void*                      data)
{
  /*
   * Load all text, data and bsssections in seperate operations so each type of
   * section is grouped together. Finish by loading any architecure specific
   * sections.
   */
  if (!rtems_rtl_obj_sections_loader (RTEMS_RTL_OBJ_SECT_TEXT,
                                      rtems_rtl_alloc_text_tag (),
                                      obj, fd, obj->text_base, handler, data) ||
      !rtems_rtl_obj_sections_loader (RTEMS_RTL_OBJ_SECT_CONST,
                                      rtems_rtl_alloc_const_tag (),
                                      obj, fd, obj->const_base, handler, data) ||
      !rtems_rtl_obj_sections_loader (RTEMS_RTL_OBJ_SECT_EH,
                                      rtems_rtl_alloc_eh_tag (),
                                      obj, fd, obj->eh_base, handler, data) ||
      !rtems_rtl_obj_sections_loader (RTEMS_RTL_OBJ_SECT_DATA,
                                      rtems_rtl_alloc_data_tag (),
                                      obj, fd, obj->data_base, handler, data) ||
      !rtems_rtl_obj_sections_loader (RTEMS_RTL_OBJ_SECT_BSS,
                                      rtems_rtl_alloc_bss_tag (),
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
rtems_rtl_obj_run_cdtors (rtems_rtl_obj* obj, uint32_t mask)
{
  rtems_chain_node* node = rtems_chain_first (&obj->sections);
  while (!rtems_chain_is_tail (&obj->sections, node))
  {
    rtems_rtl_obj_sect* sect = (rtems_rtl_obj_sect*) node;
    if ((sect->flags & mask) == mask)
    {
      rtems_rtl_cdtor* handler;
      size_t           handlers = sect->size / sizeof (rtems_rtl_cdtor);
      int              c;
      for (c = 0, handler = sect->base; c < handlers; ++c)
        if (*handler)
          (*handler) ();
    }
    node = rtems_chain_next (node);
  }
}

static bool
rtems_rtl_obj_cdtors_to_run (rtems_rtl_obj* obj, uint32_t mask)
{
  rtems_chain_node* node = rtems_chain_first (&obj->sections);
  while (!rtems_chain_is_tail (&obj->sections, node))
  {
    rtems_rtl_obj_sect* sect = (rtems_rtl_obj_sect*) node;
    if ((sect->flags & mask) == mask)
      return true;
    node = rtems_chain_next (node);
  }
  return false;
}

bool
rtems_rtl_obj_ctors_to_run (rtems_rtl_obj* obj)
{
  return rtems_rtl_obj_cdtors_to_run (obj, RTEMS_RTL_OBJ_SECT_CTOR);
}

void
rtems_rtl_obj_run_ctors (rtems_rtl_obj* obj)
{
  rtems_rtl_obj_run_cdtors (obj, RTEMS_RTL_OBJ_SECT_CTOR);
}

bool
rtems_rtl_obj_dtors_to_run (rtems_rtl_obj* obj)
{
  return rtems_rtl_obj_cdtors_to_run (obj, RTEMS_RTL_OBJ_SECT_DTOR);
}

void
rtems_rtl_obj_run_dtors (rtems_rtl_obj* obj)
{
  rtems_rtl_obj_run_cdtors (obj, RTEMS_RTL_OBJ_SECT_DTOR);
}

static bool
rtems_rtl_obj_file_load (rtems_rtl_obj* obj, int fd)
{
  int l;

  for (l = 0; l < (sizeof (loaders) / sizeof (rtems_rtl_loader_table)); ++l)
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

static void
rtems_rtl_obj_set_error (int num, const char* text)
{
  rtems_rtl_set_error (num, text);
}

size_t
rtems_rtl_obj_get_reference (rtems_rtl_obj* obj)
{
  return obj->refs;
}

void
rtems_rtl_obj_inc_reference (rtems_rtl_obj* obj)
{
  ++obj->refs;
}

void
rtems_rtl_obj_dec_reference (rtems_rtl_obj* obj)
{
  if (obj->refs)
    --obj->refs;
}

bool
rtems_rtl_obj_orphaned (rtems_rtl_obj* obj)
{
  return ((obj->flags & RTEMS_RTL_OBJ_LOCKED) == 0 &&
          obj->users == 0 &&
          rtems_rtl_obj_get_reference (obj) == 0);
}

bool
rtems_rtl_obj_load (rtems_rtl_obj* obj)
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
    rtems_rtl_set_error (errno, "opening for object file");
    return false;
  }

  /*
   * Find the object file in the archive if it is an archive that
   * has been opened.
   */
  if (rtems_rtl_obj_aname_valid (obj))
  {
    off_t enames = 0;
    if (!rtems_rtl_obj_archive_find_obj (fd,
                                         obj->fsize,
                                         &obj->oname,
                                         &obj->ooffset,
                                         &obj->fsize,
                                         &enames,
                                         rtems_rtl_obj_set_error))
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

   /*
    * For GDB
    */
  if (!_rtld_linkmap_add (obj))
  {
    close (fd);
    return false;
  }

  close (fd);

  return true;
}

bool
rtems_rtl_obj_unload (rtems_rtl_obj* obj)
{
  bool ok = false;
  if (obj->format >= 0 && obj->format < RTEMS_RTL_LOADERS)
  {
    _rtld_linkmap_delete(obj);
    ok = loaders[obj->format].unload (obj);
  }
  else
  {
    rtems_rtl_set_error (EINVAL, "invalid object loader format");
  }
  return ok;
}
