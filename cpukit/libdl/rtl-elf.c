/*
 *  COPYRIGHT (c) 2012-2018 Chris Johns <chrisj@rtems.org>
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

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <rtems/rtl/rtl.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include <rtems/rtl/rtl-trace.h>
#include "rtl-unwind.h"
#include <rtems/rtl/rtl-unresolved.h>

/**
 * The offsets in the unresolved array.
 */
#define REL_R_OFFSET (0)
#define REL_R_INFO   (1)
#define REL_R_ADDEND (2)

/**
 * The ELF format signature.
 */
static rtems_rtl_loader_format elf_sig =
{
  .label = "ELF",
  .flags = RTEMS_RTL_FMT_ELF
};

static bool
rtems_rtl_elf_machine_check (Elf_Ehdr* ehdr)
{
  /*
   * This code is determined by the NetBSD machine headers.
   */
  switch (ehdr->e_machine)
  {
    ELFDEFNNAME (MACHDEP_ID_CASES)
    default:
      return false;
  }
  return true;
}

static bool
rtems_rtl_elf_find_symbol (rtems_rtl_obj*      obj,
                           const Elf_Sym*      sym,
                           const char*         symname,
                           rtems_rtl_obj_sym** symbol,
                           Elf_Word*           value)
{
  rtems_rtl_obj_sect* sect;

  /*
   * If the symbol type is STT_NOTYPE the symbol references a global
   * symbol. The gobal symbol table is searched to find it and that value
   * returned. If the symbol is local to the object module the section for the
   * symbol is located and it's base added to the symbol's value giving an
   * absolute location.
   */
  if (ELF_ST_TYPE(sym->st_info) == STT_NOTYPE || sym->st_shndx == SHN_COMMON)
  {
    /*
     * Search the object file then the global table for the symbol.
     */
    *symbol = rtems_rtl_symbol_obj_find (obj, symname);
    if (!*symbol)
    {
      rtems_rtl_set_error (EINVAL, "global symbol not found: %s", symname);
      return false;
    }

    *value = (Elf_Addr) (*symbol)->value;
    return true;
  }

  *symbol = NULL;

  sect = rtems_rtl_obj_find_section_by_index (obj, sym->st_shndx);
  if (!sect)
  {
    rtems_rtl_set_error (EINVAL, "reloc symbol's section not found");
    return false;
  }

  *value = sym->st_value + (Elf_Addr) sect->base;
  return true;
}

/**
 * Relocation worker routine.
 */
typedef bool (*rtems_rtl_elf_reloc_handler)(rtems_rtl_obj*      obj,
                                            bool                is_rela,
                                            void*               relbuf,
                                            rtems_rtl_obj_sect* targetsect,
                                            rtems_rtl_obj_sym*  symbol,
                                            Elf_Sym*            sym,
                                            const char*         symname,
                                            Elf_Word            symvalue,
                                            bool                resolved,
                                            void*               data);

/**
 * Relocation parser data.
 */
typedef struct
{
  size_t dependents; /**< The number of dependent object files. */
  size_t unresolved; /**< The number of unresolved symbols. */
} rtems_rtl_elf_reloc_data;

static bool
rtems_rtl_elf_reloc_parser (rtems_rtl_obj*      obj,
                            bool                is_rela,
                            void*               relbuf,
                            rtems_rtl_obj_sect* targetsect,
                            rtems_rtl_obj_sym*  symbol,
                            Elf_Sym*            sym,
                            const char*         symname,
                            Elf_Word            symvalue,
                            bool                resolved,
                            void*               data)
{
  rtems_rtl_elf_reloc_data* rd = (rtems_rtl_elf_reloc_data*) data;
  /*
   * If the symbol has been resolved and there is a symbol name it is a global
   * symbol and from another object file so add it as a dependency.
   */
  if (!resolved)
  {
    ++rd->unresolved;
  }
  else if (resolved && symname != NULL)
  {
    /*
     * Find the symbol's object file. It cannot be NULL so ignore that result
     * if returned, it means something is corrupted. We are in an iterator.
     */
    rtems_rtl_obj*  sobj = rtems_rtl_find_obj_with_symbol (symbol);
    if (sobj != NULL)
    {
      /*
       * A dependency is not the base kernel image or itself. Tag the object as
       * having been visited so we count it only once.
       */
      if (sobj != rtems_rtl_baseimage () && obj != sobj &&
          (sobj->flags & RTEMS_RTL_OBJ_RELOC_TAG) == 0)
      {
        sobj->flags |= RTEMS_RTL_OBJ_RELOC_TAG;
        ++rd->dependents;
      }
    }
  }
  return true;
}

static bool
rtems_rtl_elf_reloc_relocator (rtems_rtl_obj*      obj,
                               bool                is_rela,
                               void*               relbuf,
                               rtems_rtl_obj_sect* targetsect,
                               rtems_rtl_obj_sym*  symbol,
                               Elf_Sym*            sym,
                               const char*         symname,
                               Elf_Word            symvalue,
                               bool                resolved,
                               void*               data)
{
  const Elf_Rela* rela = (const Elf_Rela*) relbuf;
  const Elf_Rel*  rel = (const Elf_Rel*) relbuf;

  if (!resolved)
  {
    uint16_t       flags = 0;
    rtems_rtl_word rel_words[3];

    if (is_rela)
    {
      flags = 1;
      rel_words[REL_R_OFFSET] = rela->r_offset;
      rel_words[REL_R_INFO] = rela->r_info;
      rel_words[REL_R_ADDEND] = rela->r_addend;
    }
    else
    {
      rel_words[REL_R_OFFSET] = rel->r_offset;
      rel_words[REL_R_INFO] = rel->r_info;
      rel_words[REL_R_ADDEND] = 0;
    }

    if (!rtems_rtl_unresolved_add (obj,
                                   flags,
                                   symname,
                                   targetsect->section,
                                   rel_words))
      return false;

    ++obj->unresolved;
  }
  else
  {
    rtems_rtl_obj* sobj;

    if (is_rela)
    {
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: rela: sym:%s(%d)=%08jx type:%d off:%08jx addend:%d\n",
                symname, (int) ELF_R_SYM (rela->r_info),
                (uintmax_t) symvalue, (int) ELF_R_TYPE (rela->r_info),
                (uintmax_t) rela->r_offset, (int) rela->r_addend);
      if (!rtems_rtl_elf_relocate_rela (obj, rela, targetsect,
                                        symname, sym->st_info, symvalue))
        return false;
    }
    else
    {
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: rel: sym:%s(%d)=%08jx type:%d off:%08jx\n",
                symname, (int) ELF_R_SYM (rel->r_info),
                (uintmax_t) symvalue, (int) ELF_R_TYPE (rel->r_info),
                (uintmax_t) rel->r_offset);
      if (!rtems_rtl_elf_relocate_rel (obj, rel, targetsect,
                                       symname, sym->st_info, symvalue))
        return false;
    }

    sobj = rtems_rtl_find_obj_with_symbol (symbol);
    if (sobj != NULL)
    {
      if (rtems_rtl_obj_add_dependent (obj, sobj))
        rtems_rtl_obj_inc_reference (sobj);
    }
  }

  return true;
}

static bool
rtems_rtl_elf_relocate_worker (rtems_rtl_obj*              obj,
                               int                         fd,
                               rtems_rtl_obj_sect*         sect,
                               rtems_rtl_elf_reloc_handler handler,
                               void*                       data)
{
  rtems_rtl_obj_cache* symbols;
  rtems_rtl_obj_cache* strings;
  rtems_rtl_obj_cache* relocs;
  rtems_rtl_obj_sect*  targetsect;
  rtems_rtl_obj_sect*  symsect;
  rtems_rtl_obj_sect*  strtab;
  bool                 is_rela;
  size_t               reloc_size;
  int                  reloc;

  /*
   * First check if the section the relocations are for exists. If it does not
   * exist ignore these relocations. They are most probably debug sections.
   */
  targetsect = rtems_rtl_obj_find_section_by_index (obj, sect->info);
  if (!targetsect)
    return true;

  rtems_rtl_obj_caches (&symbols, &strings, &relocs);

  if (!symbols || !strings || !relocs)
    return false;

  symsect = rtems_rtl_obj_find_section (obj, ".symtab");
  if (!symsect)
  {
    rtems_rtl_set_error (EINVAL, "no .symtab section");
    return false;
  }

  strtab = rtems_rtl_obj_find_section (obj, ".strtab");
  if (!strtab)
  {
    rtems_rtl_set_error (EINVAL, "no .strtab section");
    return false;
  }

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
    printf ("rtl: relocation: %s, syms:%s\n", sect->name, symsect->name);

  /*
   * Handle the different relocation record types.
   */
  is_rela = ((sect->flags & RTEMS_RTL_OBJ_SECT_RELA) ==
             RTEMS_RTL_OBJ_SECT_RELA) ? true : false;
  reloc_size = is_rela ? sizeof (Elf_Rela) : sizeof (Elf_Rel);

  for (reloc = 0; reloc < (sect->size / reloc_size); ++reloc)
  {
    uint8_t            relbuf[reloc_size];
    const Elf_Rela*    rela = (const Elf_Rela*) relbuf;
    const Elf_Rel*     rel = (const Elf_Rel*) relbuf;
    rtems_rtl_obj_sym* symbol = NULL;
    Elf_Sym            sym;
    const char*        symname = NULL;
    off_t              off;
    Elf_Word           rel_type;
    Elf_Word           symvalue = 0;
    bool               resolved;

    off = obj->ooffset + sect->offset + (reloc * reloc_size);

    if (!rtems_rtl_obj_cache_read_byval (relocs, fd, off,
                                         &relbuf[0], reloc_size))
      return false;

    /*
     * Read the symbol details.
     */
    if (is_rela)
      off = (obj->ooffset + symsect->offset +
             (ELF_R_SYM (rela->r_info) * sizeof (sym)));
    else
      off = (obj->ooffset + symsect->offset +
             (ELF_R_SYM (rel->r_info) * sizeof (sym)));

    if (!rtems_rtl_obj_cache_read_byval (symbols, fd, off,
                                         &sym, sizeof (sym)))
      return false;

    /*
     * Only need the name of the symbol if global or a common symbol.
     */
    if (ELF_ST_TYPE (sym.st_info) == STT_NOTYPE ||
        sym.st_shndx == SHN_COMMON)
    {
      size_t len;
      off = obj->ooffset + strtab->offset + sym.st_name;
      len = RTEMS_RTL_ELF_STRING_MAX;

      if (!rtems_rtl_obj_cache_read (strings, fd, off,
                                     (void**) &symname, &len))
        return false;
    }

    /*
     * See if the record references an external symbol. If it does find the
     * symbol value. If the symbol cannot be found flag the object file as
     * having unresolved externals and store the external. The load of an
     * object after this one may provide the unresolved externals.
     */
    if (is_rela)
      rel_type = ELF_R_TYPE(rela->r_info);
    else
      rel_type = ELF_R_TYPE(rel->r_info);

    resolved = true;

    if (rtems_rtl_elf_rel_resolve_sym (rel_type))
      resolved = rtems_rtl_elf_find_symbol (obj,
                                            &sym, symname,
                                            &symbol, &symvalue);

    if (!handler (obj,
                  is_rela, relbuf, targetsect,
                  symbol, &sym, symname, symvalue, resolved,
                  data))
      return false;
  }

  /*
   * Set the unresolved externals status if there are unresolved externals.
   */
  if (obj->unresolved)
    obj->flags |= RTEMS_RTL_OBJ_UNRESOLVED;

  return true;
}

static bool
rtems_rtl_elf_relocs_parser (rtems_rtl_obj*      obj,
                             int                 fd,
                             rtems_rtl_obj_sect* sect,
                             void*               data)
{
  bool r = rtems_rtl_elf_relocate_worker (obj, fd, sect,
                                          rtems_rtl_elf_reloc_parser, data);
  rtems_rtl_obj_update_flags (RTEMS_RTL_OBJ_RELOC_TAG, 0);
  return r;
}

static bool
rtems_rtl_elf_relocs_locator (rtems_rtl_obj*      obj,
                              int                 fd,
                              rtems_rtl_obj_sect* sect,
                              void*               data)
{
  return rtems_rtl_elf_relocate_worker (obj, fd, sect,
                                        rtems_rtl_elf_reloc_relocator, data);
}

bool
rtems_rtl_obj_relocate_unresolved (rtems_rtl_unresolv_reloc* reloc,
                                   rtems_rtl_obj_sym*        sym)
{
  rtems_rtl_obj_sect* sect;
  bool                is_rela;
  Elf_Word            symvalue;
  rtems_rtl_obj*      sobj;

  is_rela = reloc->flags & 1;

  sect = rtems_rtl_obj_find_section_by_index (reloc->obj, reloc->sect);
  if (!sect)
  {
    rtems_rtl_set_error (ENOEXEC, "unresolved sect not found");
    return false;
  }

  symvalue = (Elf_Word) (intptr_t) sym->value;
  if (is_rela)
  {
    Elf_Rela rela;
    rela.r_offset = reloc->rel[REL_R_OFFSET];
    rela.r_info = reloc->rel[REL_R_INFO];
    rela.r_addend = reloc->rel[REL_R_ADDEND];
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
          printf ("rtl: rela: sym:%d type:%d off:%08jx addend:%d\n",
                  (int) ELF_R_SYM (rela.r_info), (int) ELF_R_TYPE (rela.r_info),
                  (uintmax_t) rela.r_offset, (int) rela.r_addend);
    if (!rtems_rtl_elf_relocate_rela (reloc->obj, &rela, sect,
                                      sym->name, sym->data, symvalue))
      return false;
  }
  else
  {
    Elf_Rel rel;
    rel.r_offset = reloc->rel[REL_R_OFFSET];
    rel.r_info = reloc->rel[REL_R_INFO];
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
      printf ("rtl: rel: sym:%d type:%d off:%08jx\n",
              (int) ELF_R_SYM (rel.r_info), (int) ELF_R_TYPE (rel.r_info),
              (uintmax_t) rel.r_offset);
    if (!rtems_rtl_elf_relocate_rel (reloc->obj, &rel, sect,
                                     sym->name, sym->data, symvalue))
      return false;
  }

  if (reloc->obj->unresolved > 0)
  {
    --reloc->obj->unresolved;
    if (reloc->obj->unresolved == 0)
      reloc->obj->flags &= ~RTEMS_RTL_OBJ_UNRESOLVED;
  }

  sobj = rtems_rtl_find_obj_with_symbol (sym);
  if (sobj != NULL)
  {
    if (rtems_rtl_obj_add_dependent (reloc->obj, sobj))
      rtems_rtl_obj_inc_reference (sobj);
  }

  return true;
}

/**
 * Common symbol iterator data.
 */
typedef struct
{
  size_t   size;      /**< The size of the common section */
  uint32_t alignment; /**< The alignment of the common section. */
} rtems_rtl_elf_common_data;

static bool
rtems_rtl_elf_common (rtems_rtl_obj*      obj,
                      int                 fd,
                      rtems_rtl_obj_sect* sect,
                      void*               data)
{
  rtems_rtl_elf_common_data* common = (rtems_rtl_elf_common_data*) data;
  rtems_rtl_obj_cache*       symbols;
  int                        sym;

  rtems_rtl_obj_caches (&symbols, NULL, NULL);

  if (!symbols)
    return false;

  /*
   * Find the number size of the common section by finding all symbols that
   * reference the SHN_COMMON section.
   */
  for (sym = 0; sym < (sect->size / sizeof (Elf_Sym)); ++sym)
  {
    Elf_Sym symbol;
    off_t   off;

    off = obj->ooffset + sect->offset + (sym * sizeof (symbol));

    if (!rtems_rtl_obj_cache_read_byval (symbols, fd, off,
                                         &symbol, sizeof (symbol)))
      return false;

    if ((symbol.st_shndx == SHN_COMMON) &&
        ((ELF_ST_TYPE (symbol.st_info) == STT_OBJECT) ||
         (ELF_ST_TYPE (symbol.st_info) == STT_COMMON)))
    {
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_SYMBOL))
        printf ("rtl: com:elf:%-2d bind:%-2d type:%-2d size:%d value:%d\n",
                sym, (int) ELF_ST_BIND (symbol.st_info),
                (int) ELF_ST_TYPE (symbol.st_info),
                (int) symbol.st_size, (int) symbol.st_value);
      /*
       * If the size is zero this is the first entry, it defines the common
       * section's aligment. The symbol's value is the alignment.
       */
      if (common->size == 0)
        common->alignment = symbol.st_value;
      common->size +=
        rtems_rtl_obj_align (common->size, symbol.st_value) + symbol.st_size;
    }
  }

  return true;
}

static bool
rtems_rtl_elf_dependents (rtems_rtl_obj* obj, rtems_rtl_elf_reloc_data* reloc)
{
  /*
   * If there are dependencies and no unresolved externals allocate and size
   * the dependency table to the number of dependent object files. If there are
   * unresolved externals the number of dependencies is unknown at this point
   * in time so use dynamic allocation to allocate the block size number of
   * entries when the entries are added.
   */
  if (reloc->dependents > 0 && reloc->unresolved == 0)
  {
    if (!rtems_rtl_obj_alloc_dependents (obj, reloc->dependents))
      return false;
  }
  return true;
}

static bool
rtems_rtl_elf_symbols (rtems_rtl_obj*      obj,
                       int                 fd,
                       rtems_rtl_obj_sect* sect,
                       void*               data)
{
  rtems_rtl_obj_cache* symbols;
  rtems_rtl_obj_cache* strings;
  rtems_rtl_obj_sect*  strtab;
  int                  locals;
  int                  local_string_space;
  rtems_rtl_obj_sym*   lsym;
  char*                lstring;
  int                  globals;
  int                  global_string_space;
  rtems_rtl_obj_sym*   gsym;
  char*                gstring;
  size_t               common_offset;
  int                  sym;

  strtab = rtems_rtl_obj_find_section (obj, ".strtab");
  if (!strtab)
  {
    rtems_rtl_set_error (EINVAL, "no .strtab section");
    return false;
  }

  rtems_rtl_obj_caches (&symbols, &strings, NULL);

  if (!symbols || !strings)
    return false;

  /*
   * Find the number of globals and the amount of string space
   * needed. Also check for duplicate symbols.
   */

  globals             = 0;
  global_string_space = 0;
  locals              = 0;
  local_string_space  = 0;

  for (sym = 0; sym < (sect->size / sizeof (Elf_Sym)); ++sym)
  {
    Elf_Sym     symbol;
    off_t       off;
    const char* name;
    size_t      len;

    off = obj->ooffset + sect->offset + (sym * sizeof (symbol));

    if (!rtems_rtl_obj_cache_read_byval (symbols, fd, off,
                                         &symbol, sizeof (symbol)))
      return false;

    off = obj->ooffset + strtab->offset + symbol.st_name;
    len = RTEMS_RTL_ELF_STRING_MAX;

    if (!rtems_rtl_obj_cache_read (strings, fd, off, (void**) &name, &len))
      return false;

    /*
     * Only keep the functions and global or weak symbols so place them in a
     * separate table to local symbols. Local symbols are not needed after the
     * object file has been loaded. Undefined symbols are NOTYPE so for locals
     * we need to make sure there is a valid seciton.
     */
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_SYMBOL))
      printf ("rtl: sym:elf:%-2d name:%-2d:%-20s bind:%-2d " \
              "type:%-2d sect:%d size:%d\n",
              sym, (int) symbol.st_name, name,
              (int) ELF_ST_BIND (symbol.st_info),
              (int) ELF_ST_TYPE (symbol.st_info),
              symbol.st_shndx,
              (int) symbol.st_size);

    if ((symbol.st_shndx != 0) &&
        ((ELF_ST_TYPE (symbol.st_info) == STT_OBJECT) ||
         (ELF_ST_TYPE (symbol.st_info) == STT_COMMON) ||
         (ELF_ST_TYPE (symbol.st_info) == STT_FUNC) ||
         (ELF_ST_TYPE (symbol.st_info) == STT_NOTYPE)))
    {
      /*
       * There needs to be a valid section for the symbol.
       */
      rtems_rtl_obj_sect* symsect;

      symsect = rtems_rtl_obj_find_section_by_index (obj, symbol.st_shndx);
      if (symsect != NULL)
      {
        if ((ELF_ST_BIND (symbol.st_info) == STB_GLOBAL) ||
            (ELF_ST_BIND (symbol.st_info) == STB_WEAK))
        {
          /*
           * If there is a globally exported symbol already present and this
           * symbol is not weak raise an error. If the symbol is weak and
           * present globally ignore this symbol and use the global one and if
           * it is not present take this symbol global or weak. We accept the
           * first weak symbol we find and make it globally exported.
           */
          if (rtems_rtl_symbol_global_find (name) &&
              (ELF_ST_BIND (symbol.st_info) != STB_WEAK))
          {
            rtems_rtl_set_error (ENOMEM, "duplicate global symbol: %s", name);
            return false;
          }
          else
          {
            if (rtems_rtl_trace (RTEMS_RTL_TRACE_SYMBOL))
              printf ("rtl: sym:elf:%-2d name:%-2d:%-20s: global\n",
                      sym, (int) symbol.st_name, name);
            ++globals;
            global_string_space += strlen (name) + 1;
          }
        }
        else if (ELF_ST_BIND (symbol.st_info) == STB_LOCAL)
        {
          if (rtems_rtl_trace (RTEMS_RTL_TRACE_SYMBOL))
            printf ("rtl: sym:elf:%-2d name:%-2d:%-20s: local\n",
                    sym, (int) symbol.st_name, name);
          ++locals;
          local_string_space += strlen (name) + 1;
        }
      }
    }
  }

  if (locals)
  {
    obj->local_size = locals * sizeof (rtems_rtl_obj_sym) + local_string_space;
    obj->local_table = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_SYMBOL,
                                            obj->local_size, true);
    if (!obj->local_table)
    {
      obj->local_size = 0;
      rtems_rtl_set_error (ENOMEM, "no memory for obj local syms");
      return false;
    }

    obj->local_syms = locals;
  }

  if (globals)
  {
    obj->global_size = globals * sizeof (rtems_rtl_obj_sym) + global_string_space;
    obj->global_table = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_SYMBOL,
                                             obj->global_size, true);
    if (!obj->global_table)
    {
      if (locals)
      {
        rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, obj->local_table);
        obj->local_size = 0;
        obj->local_syms = 0;
      }
      obj->global_size = 0;
      rtems_rtl_set_error (ENOMEM, "no memory for obj global syms");
      return false;
    }

    obj->global_syms = globals;
  }

  lsym = obj->local_table;
  lstring =
    (((char*) obj->local_table) + (locals * sizeof (rtems_rtl_obj_sym)));
  gsym = obj->global_table;
  gstring =
    (((char*) obj->global_table) + (globals * sizeof (rtems_rtl_obj_sym)));

  common_offset = 0;

  for (sym = 0; sym < (sect->size / sizeof (Elf_Sym)); ++sym)
  {
    Elf_Sym     symbol;
    off_t       off;
    const char* name;
    size_t      len;

    off = obj->ooffset + sect->offset + (sym * sizeof (symbol));

    if (!rtems_rtl_obj_cache_read_byval (symbols, fd, off,
                                         &symbol, sizeof (symbol)))
    {
      if (locals)
      {
        rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, obj->local_table);
        obj->local_table = NULL;
        obj->local_size = 0;
        obj->local_syms = 0;
      }
      if (globals)
      {
        rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, obj->global_table);
        obj->global_table = NULL;
        obj->global_syms = 0;
        obj->global_size = 0;
      }
      return false;
    }

    off = obj->ooffset + strtab->offset + symbol.st_name;
    len = RTEMS_RTL_ELF_STRING_MAX;

    if (!rtems_rtl_obj_cache_read (strings, fd, off, (void**) &name, &len))
      return false;

    if ((symbol.st_shndx != 0) &&
        ((ELF_ST_TYPE (symbol.st_info) == STT_OBJECT) ||
         (ELF_ST_TYPE (symbol.st_info) == STT_COMMON) ||
         (ELF_ST_TYPE (symbol.st_info) == STT_FUNC) ||
         (ELF_ST_TYPE (symbol.st_info) == STT_NOTYPE)) &&
         ((ELF_ST_BIND (symbol.st_info) == STB_GLOBAL) ||
          (ELF_ST_BIND (symbol.st_info) == STB_WEAK) ||
          (ELF_ST_BIND (symbol.st_info) == STB_LOCAL)))
      {
        rtems_rtl_obj_sect* symsect;
        rtems_rtl_obj_sym*  osym;
        char*               string;
        Elf_Word            value;

        symsect = rtems_rtl_obj_find_section_by_index (obj, symbol.st_shndx);
        if (symsect)
        {
          if ((ELF_ST_BIND (symbol.st_info) == STB_GLOBAL) ||
              (ELF_ST_BIND (symbol.st_info) == STB_WEAK))
          {
            osym = gsym;
            string = gstring;
            gstring += strlen (name) + 1;
            ++gsym;
          }
          else
          {
            osym = lsym;
            string = lstring;
            lstring += strlen (name) + 1;
            ++lsym;
          }

          /*
           * Allocate any common symbols in the common section.
           */
          if (symbol.st_shndx == SHN_COMMON)
          {
            size_t value_off = rtems_rtl_obj_align (common_offset,
                                                    symbol.st_value);
            common_offset = value_off + symbol.st_size;
            value = value_off;
          }
          else
          {
            value = symbol.st_value;
          }

          rtems_chain_set_off_chain (&osym->node);
          memcpy (string, name, strlen (name) + 1);
          osym->name = string;
          osym->value = value + (uint8_t*) symsect->base;
          osym->data = symbol.st_info;

          if (rtems_rtl_trace (RTEMS_RTL_TRACE_SYMBOL))
            printf ("rtl: sym:add:%-2d name:%-2d:%-20s bind:%-2d " \
                    "type:%-2d val:%8p sect:%d size:%d\n",
                    sym, (int) symbol.st_name, osym->name,
                    (int) ELF_ST_BIND (symbol.st_info),
                    (int) ELF_ST_TYPE (symbol.st_info),
                    osym->value, symbol.st_shndx,
                    (int) symbol.st_size);
        }
      }
  }

  if (globals)
    rtems_rtl_symbol_obj_add (obj);

  return true;
}

static bool
rtems_rtl_elf_loader (rtems_rtl_obj*      obj,
                      int                 fd,
                      rtems_rtl_obj_sect* sect,
                      void*               data)
{
  uint8_t* base_offset;
  size_t   len;

  if (lseek (fd, obj->ooffset + sect->offset, SEEK_SET) < 0)
  {
    rtems_rtl_set_error (errno, "section load seek failed");
    return false;
  }

  base_offset = sect->base;
  len = sect->size;

  while (len)
  {
    ssize_t r = read (fd, base_offset, len);
    if (r <= 0)
    {
      rtems_rtl_set_error (errno, "section load read failed");
      return false;
    }
    base_offset += r;
    len -= r;
  }

  return true;
}

static bool
rtems_rtl_elf_parse_sections (rtems_rtl_obj* obj, int fd, Elf_Ehdr* ehdr)
{
  rtems_rtl_obj_cache* sects;
  rtems_rtl_obj_cache* strings;
  int                  section;
  off_t                sectstroff;
  off_t                off;
  Elf_Shdr             shdr;

  rtems_rtl_obj_caches (&sects, &strings, NULL);

  if (!sects || !strings)
    return false;

  /*
   * Get the offset to the section string table.
   */
  off = obj->ooffset + ehdr->e_shoff + (ehdr->e_shstrndx * ehdr->e_shentsize);

  if (!rtems_rtl_obj_cache_read_byval (sects, fd, off, &shdr, sizeof (shdr)))
    return false;

  if (shdr.sh_type != SHT_STRTAB)
  {
    rtems_rtl_set_error (EINVAL, "bad .sectstr section type");
    return false;
  }

  sectstroff = obj->ooffset + shdr.sh_offset;

  for (section = 0; section < ehdr->e_shnum; ++section)
  {
    uint32_t flags;

    /*
     * Make sure section is at least 32bits to avoid 16-bit overflow errors.
     */
    off = obj->ooffset + ehdr->e_shoff + (((uint32_t) section) * ehdr->e_shentsize);

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_DETAIL))
      printf ("rtl: section header: %2d: offset=%d\n", section, (int) off);

    if (!rtems_rtl_obj_cache_read_byval (sects, fd, off, &shdr, sizeof (shdr)))
      return false;

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_DETAIL))
      printf ("rtl: section: %2d: type=%d flags=%08x link=%d info=%d\n",
              section, (int) shdr.sh_type, (unsigned int) shdr.sh_flags,
              (int) shdr.sh_link, (int) shdr.sh_info);

    flags = 0;

    switch (shdr.sh_type)
    {
      case SHT_NULL:
        /*
         * Ignore.
         */
        break;

      case SHT_PROGBITS:
        /*
         * There are 2 program bits sections. One is the program text and the
         * other is the program data. The program text is flagged
         * alloc/executable and the program data is flagged alloc/writable.
         */
        if ((shdr.sh_flags & SHF_ALLOC) == SHF_ALLOC)
        {
          if ((shdr.sh_flags & SHF_EXECINSTR) == SHF_EXECINSTR)
            flags = RTEMS_RTL_OBJ_SECT_TEXT | RTEMS_RTL_OBJ_SECT_LOAD;
          else if ((shdr.sh_flags & SHF_WRITE) == SHF_WRITE)
            flags = RTEMS_RTL_OBJ_SECT_DATA | RTEMS_RTL_OBJ_SECT_LOAD;
          else
            flags = RTEMS_RTL_OBJ_SECT_CONST | RTEMS_RTL_OBJ_SECT_LOAD;
        }
        break;

      case SHT_NOBITS:
        /*
         * There is 1 NOBIT section which is the .bss section. There is nothing
         * but a definition as the .bss is just a clear region of memory.
         */
        if ((shdr.sh_flags & (SHF_ALLOC | SHF_WRITE)) == (SHF_ALLOC | SHF_WRITE))
          flags = RTEMS_RTL_OBJ_SECT_BSS | RTEMS_RTL_OBJ_SECT_ZERO;
        break;

      case SHT_RELA:
        flags = RTEMS_RTL_OBJ_SECT_RELA;
        break;

      case SHT_REL:
        /*
         * The sh_link holds the section index for the symbol table. The sh_info
         * holds the section index the relocations apply to.
         */
        flags = RTEMS_RTL_OBJ_SECT_REL;
        break;

      case SHT_SYMTAB:
        flags = RTEMS_RTL_OBJ_SECT_SYM;
        break;

      case SHT_STRTAB:
        flags = RTEMS_RTL_OBJ_SECT_STR;
        break;

      default:
        /*
         * See if there are architecture specific flags?
         */
        flags = rtems_rtl_elf_section_flags (obj, &shdr);
        if (flags == 0)
        {
          if (rtems_rtl_trace (RTEMS_RTL_TRACE_WARNING))
            printf ("rtl: unsupported section: %2d: type=%02d flags=%02x\n",
                    section, (int) shdr.sh_type, (int) shdr.sh_flags);
        }
        break;
    }

    if (flags != 0)
    {
      char*  name;
      size_t len;

      /*
       * If link ordering this section must appear in the same order in memory
       * as the linked-to section relative to the sections it loads with.
       */
      if ((shdr.sh_flags & SHF_LINK_ORDER) != 0)
        flags |= RTEMS_RTL_OBJ_SECT_LINK;

      len = RTEMS_RTL_ELF_STRING_MAX;
      if (!rtems_rtl_obj_cache_read (strings, fd,
                                     sectstroff + shdr.sh_name,
                                     (void**) &name, &len))
        return false;

      if (strcmp (".ctors", name) == 0)
        flags |= RTEMS_RTL_OBJ_SECT_CTOR;
      if (strcmp (".dtors", name) == 0)
        flags |= RTEMS_RTL_OBJ_SECT_DTOR;

      if (rtems_rtl_elf_unwind_parse (obj, name, flags))
      {
        flags &= ~(RTEMS_RTL_OBJ_SECT_TEXT | RTEMS_RTL_OBJ_SECT_CONST);
        flags |= RTEMS_RTL_OBJ_SECT_EH;
      }

      if (!rtems_rtl_obj_add_section (obj, section, name,
                                      shdr.sh_size, shdr.sh_offset,
                                      shdr.sh_addralign, shdr.sh_link,
                                      shdr.sh_info, flags))
        return false;
    }
  }

  return true;
}

static bool
rtems_rtl_elf_add_common (rtems_rtl_obj* obj, size_t size, uint32_t alignment)
{
  if (size > 0)
  {
    if (!rtems_rtl_obj_add_section (obj, SHN_COMMON, ".common.rtems.rtl",
                                    size, 0, alignment, 0, 0,
                                    RTEMS_RTL_OBJ_SECT_BSS | RTEMS_RTL_OBJ_SECT_ZERO))
      return false;
  }
  return true;
}

bool
rtems_rtl_elf_file_check (rtems_rtl_obj* obj, int fd)
{
  rtems_rtl_obj_cache* header;
  Elf_Ehdr             ehdr;

  rtems_rtl_obj_caches (&header, NULL, NULL);

  if (!rtems_rtl_obj_cache_read_byval (header, fd, obj->ooffset,
                                       &ehdr, sizeof (ehdr)))
    return false;

  /*
   * Check we have a valid ELF file.
   */
  if ((memcmp (ELFMAG, ehdr.e_ident, SELFMAG) != 0)
      || ehdr.e_ident[EI_CLASS] != ELFCLASS)
  {
    return false;
  }

  if ((ehdr.e_ident[EI_VERSION] != EV_CURRENT)
      || (ehdr.e_version != EV_CURRENT)
      || (ehdr.e_ident[EI_DATA] != ELFDEFNNAME (MACHDEP_ENDIANNESS)))
  {
    return false;
  }

  return true;
}

static bool
rtems_rtl_elf_load_linkmap (rtems_rtl_obj* obj)
{
  rtems_chain_control* sections = NULL;
  rtems_chain_node*    node = NULL;
  size_t               mask = 0;
  int                  sec_num = 0;
  section_detail*      sd;
  int                  i = 0;

  /*
   * Caculate the size of sections' name.
   */

  for (mask = RTEMS_RTL_OBJ_SECT_TEXT;
       mask <= RTEMS_RTL_OBJ_SECT_BSS;
       mask <<= 1)
  {
    sections = &obj->sections;
    node = rtems_chain_first (sections);
    while (!rtems_chain_is_tail (sections, node))
    {
      rtems_rtl_obj_sect* sect = (rtems_rtl_obj_sect*) node;
      if ((sect->size != 0) && ((sect->flags & mask) != 0))
      {
        ++sec_num;
      }
      node = rtems_chain_next (node);
    }
  }

  obj->obj_num = 1;
  obj->linkmap = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT,
                                      sizeof(struct link_map) +
                                      sec_num * sizeof (section_detail), true);
  if (!obj->linkmap)
  {
    rtems_rtl_set_error (ENOMEM, "no memory for obj linkmap");
    return false;
  }

  obj->linkmap->name = obj->oname;
  obj->linkmap->sec_num = sec_num;
  obj->linkmap->sec_detail = (section_detail*) (obj->linkmap + 1);
  obj->linkmap->rpathlen = 0;
  obj->linkmap->rpath = NULL;
  obj->linkmap->l_next = NULL;
  obj->linkmap->l_prev = NULL;
  obj->linkmap->sec_addr[rap_text] = obj->text_base;
  obj->linkmap->sec_addr[rap_const] = obj->const_base;
  obj->linkmap->sec_addr[rap_data] = obj->data_base;
  obj->linkmap->sec_addr[rap_bss] = obj->bss_base;

  sd = obj->linkmap->sec_detail;
  sections = &obj->sections;
  node = rtems_chain_first (sections);

  for (mask = RTEMS_RTL_OBJ_SECT_TEXT;
       mask <= RTEMS_RTL_OBJ_SECT_BSS;
       mask <<= 1)
  {
    sections = &obj->sections;
    node = rtems_chain_first (sections);
    while (!rtems_chain_is_tail (sections, node))
    {
      rtems_rtl_obj_sect* sect = (rtems_rtl_obj_sect*) node;

      if ((sect->size != 0) && ((sect->flags & mask) != 0))
      {
        sd[i].name = sect->name;
        sd[i].size = sect->size;
        if (mask == RTEMS_RTL_OBJ_SECT_TEXT)
        {
          sd[i].rap_id = rap_text;
          sd[i].offset = sect->base - obj->text_base;
        }
        if (mask == RTEMS_RTL_OBJ_SECT_CONST)
        {
          sd[i].rap_id = rap_const;
          sd[i].offset = sect->base - obj->const_base;
        }
        if (mask == RTEMS_RTL_OBJ_SECT_DATA)
        {
          sd[i].rap_id = rap_data;
          sd[i].offset = sect->base - obj->data_base;
        }
        if (mask == RTEMS_RTL_OBJ_SECT_BSS)
        {
          sd[i].rap_id = rap_bss;
          sd[i].offset = sect->base - obj->bss_base;
        }

        ++i;
      }
      node = rtems_chain_next (node);
    }
  }

  return true;
}

bool
rtems_rtl_elf_file_load (rtems_rtl_obj* obj, int fd)
{
  rtems_rtl_obj_cache*      header;
  Elf_Ehdr                  ehdr;
  rtems_rtl_elf_reloc_data  relocs = { 0 };
  rtems_rtl_elf_common_data common = { 0 };

  rtems_rtl_obj_caches (&header, NULL, NULL);

  if (!rtems_rtl_obj_cache_read_byval (header, fd, obj->ooffset,
                                       &ehdr, sizeof (ehdr)))
    return false;

  /*
   * Check we have a valid ELF file.
   */
  if ((memcmp (ELFMAG, ehdr.e_ident, SELFMAG) != 0)
      || ehdr.e_ident[EI_CLASS] != ELFCLASS)
  {
    rtems_rtl_set_error (EINVAL, "invalid ELF file format");
    return false;
  }

  if ((ehdr.e_ident[EI_VERSION] != EV_CURRENT)
      || (ehdr.e_version != EV_CURRENT)
      || (ehdr.e_ident[EI_DATA] != ELFDEFNNAME (MACHDEP_ENDIANNESS)))
  {
    rtems_rtl_set_error (EINVAL, "unsupported ELF file version");
    return false;
  }

  if (!rtems_rtl_elf_machine_check (&ehdr))
  {
    rtems_rtl_set_error (EINVAL, "unsupported machine type");
    return false;
  }

  if (ehdr.e_type == ET_DYN)
  {
    rtems_rtl_set_error (EINVAL, "unsupported ELF file type");
    return false;
  }

  if (ehdr.e_phentsize != 0)
  {
    rtems_rtl_set_error (EINVAL, "ELF file contains program headers");
    return false;
  }

  if (ehdr.e_shentsize != sizeof (Elf_Shdr))
  {
    rtems_rtl_set_error (EINVAL, "invalid ELF section header size");
    return false;
  }

  /*
   * Parse the section information first so we have the memory map of the object
   * file and the memory allocated. Any further allocations we make to complete
   * the load will not fragment the memory.
   */
  if (!rtems_rtl_elf_parse_sections (obj, fd, &ehdr))
    return false;

  /*
   * See if there are any common variables and if there are add a common
   * section.
   */
  if (!rtems_rtl_obj_load_symbols (obj, fd, rtems_rtl_elf_common, &common))
    return false;
  if (!rtems_rtl_elf_add_common (obj, common.size, common.alignment))
    return false;

  /*
   * Set the entry point if there is one.
   */
  obj->entry = (void*)(uintptr_t) ehdr.e_entry;

  /*
   * Load the sections and symbols and then relocation to the base address.
   */
  if (!rtems_rtl_obj_load_sections (obj, fd, rtems_rtl_elf_loader, &ehdr))
    return false;

  if (!rtems_rtl_obj_relocate (obj, fd, rtems_rtl_elf_relocs_parser, &relocs))
    return false;

  if (!rtems_rtl_elf_dependents (obj, &relocs))
    return false;

  if (!rtems_rtl_obj_load_symbols (obj, fd, rtems_rtl_elf_symbols, &ehdr))
    return false;

  if (!rtems_rtl_obj_relocate (obj, fd, rtems_rtl_elf_relocs_locator, &ehdr))
    return false;

  rtems_rtl_obj_synchronize_cache (obj);

  rtems_rtl_symbol_obj_erase_local (obj);

  if (!rtems_rtl_elf_load_linkmap (obj))
  {
    return false;
  }

  if (!rtems_rtl_elf_unwind_register (obj))
  {
    return false;
  }

  return true;
}

bool
rtems_rtl_elf_file_unload (rtems_rtl_obj* obj)
{
  rtems_rtl_elf_unwind_deregister (obj);
  return true;
}

rtems_rtl_loader_format*
rtems_rtl_elf_file_sig (void)
{
  return &elf_sig;
}
