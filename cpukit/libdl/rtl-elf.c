/*
 *  COPYRIGHT (c) 2012-2019 Chris Johns <chrisj@rtems.org>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <rtems/rtl/rtl.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include <rtems/rtl/rtl-trace.h>
#include "rtl-trampoline.h"
#include "rtl-unwind.h"
#include <rtems/rtl/rtl-unresolved.h>

/**
 * The offsets in the reloc words.
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

static const char*
rtems_rtl_elf_sym_type_label (Elf_Byte st_info)
{
  const char* label;
  switch (ELF_ST_TYPE (st_info))
  {
    case STT_NOTYPE:
      label = "STT_NOTYPE";
      break;
    case STT_OBJECT:
      label = "STT_OBJECT";
      break;
    case STT_FUNC:
      label = "STT_FUNC";
      break;
    case STT_SECTION:
      label = "STT_SECTION";
      break;
    case STT_FILE:
      label = "STT_FILE";
      break;
    case STT_COMMON:
      label = "STT_COMMON";
      break;
    case STT_TLS:
      label = "STT_TLS";
      break;
    default:
      label = "unknown";
      break;
  }
  return label;
}

static const char*
rtems_rtl_elf_sym_bind_label (Elf_Byte st_info)
{
  const char* label;
  switch (ELF_ST_BIND (st_info))
  {
    case STB_LOCAL:
      label = "STB_LOCAL";
      break;
    case STB_GLOBAL:
      label = "STB_GLOBAL";
      break;
    case STB_WEAK:
      label = "STB_WEAK";
      break;
    default:
      label = "unknown";
      break;
  }
  return label;
}

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

static const char*
rtems_rtl_elf_separated_section (const char* name)
{
  struct {
    const char* label;
    size_t      len;
  } prefix[] = {
    #define SEPARATED_PREFIX(_p) { _p, sizeof (_p) - 1 }
    SEPARATED_PREFIX (".text."),
    SEPARATED_PREFIX (".rel.text."),
    SEPARATED_PREFIX (".data."),
    SEPARATED_PREFIX (".rel.data."),
    SEPARATED_PREFIX (".rodata."),
    SEPARATED_PREFIX (".rel.rodata.")
  };
  const size_t prefixes = sizeof (prefix) / sizeof (prefix[0]);
  size_t       p;
  for (p = 0; p < prefixes; ++p)
  {
    if (strncmp (name, prefix[p].label, prefix[p].len) == 0)
      return name + prefix[p].len;
  }
  return NULL;
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
      return false;

    *value = (Elf_Addr) (*symbol)->value;
    return true;
  }

  *symbol = NULL;

  sect = rtems_rtl_obj_find_section_by_index (obj, sym->st_shndx);
  if (!sect)
    return false;

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
  rtems_rtl_word            rel_words[3];
  rtems_rtl_elf_rel_status  rs;

  /*
   * Check the reloc record to see if a trampoline is needed.
   */
  if (is_rela)
  {
    const Elf_Rela* rela = (const Elf_Rela*) relbuf;
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
      printf ("rtl: rela tramp: sym: %c:%s(%d)=%08jx type:%d off:%08jx addend:%d\n",
              ELF_ST_BIND (sym->st_info) == STB_GLOBAL ||
              ELF_ST_BIND (sym->st_info) == STB_WEAK ? 'G' : 'L',
              symname, (int) ELF_R_SYM (rela->r_info),
              (uintmax_t) symvalue, (int) ELF_R_TYPE (rela->r_info),
              (uintmax_t) rela->r_offset, (int) rela->r_addend);
    rs = rtems_rtl_elf_relocate_rela_tramp (obj, rela, targetsect,
                                            symname, sym->st_info, symvalue);
    rel_words[REL_R_OFFSET] = rela->r_offset;
    rel_words[REL_R_INFO] = rela->r_info;
    rel_words[REL_R_ADDEND] = rela->r_addend;
  }
  else
  {
    const Elf_Rel* rel = (const Elf_Rel*) relbuf;
    if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
      printf ("rtl: rel tramp: sym: %c:%s(%d)=%08jx type:%d off:%08jx\n",
              ELF_ST_BIND (sym->st_info) == STB_GLOBAL ||
              ELF_ST_BIND (sym->st_info) == STB_WEAK ? 'G' : 'L',
              symname, (int) ELF_R_SYM (rel->r_info),
              (uintmax_t) symvalue, (int) ELF_R_TYPE (rel->r_info),
              (uintmax_t) rel->r_offset);
    rs = rtems_rtl_elf_relocate_rel_tramp (obj, rel, targetsect,
                                           symname, sym->st_info, symvalue);
    rel_words[REL_R_OFFSET] = rel->r_offset;
    rel_words[REL_R_INFO] = rel->r_info;
    rel_words[REL_R_ADDEND] = 0;
  }

  if (rs == rtems_rtl_elf_rel_failure)
    return false;

  if (rs == rtems_rtl_elf_rel_tramp_cache || rs == rtems_rtl_elf_rel_tramp_add)
  {
    uint32_t flags = (is_rela ? 1 : 0) | (resolved ? 0 : 1 << 1) | (sym->st_info << 8);
    if (!rtems_rtl_trampoline_add (obj, flags,
                                   targetsect->section, symvalue, rel_words))
      return false;
  }

  /*
   * Handle any dependencies if there is a valid symbol.
   */
  if (symname != NULL)
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
    rtems_rtl_obj*           sobj;
    rtems_rtl_elf_rel_status rs;

    if (is_rela)
    {
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: rela: sym:%s(%d)=%08jx type:%d off:%08jx addend:%d\n",
                symname, (int) ELF_R_SYM (rela->r_info),
                (uintmax_t) symvalue, (int) ELF_R_TYPE (rela->r_info),
                (uintmax_t) rela->r_offset, (int) rela->r_addend);
      rs = rtems_rtl_elf_relocate_rela (obj, rela, targetsect,
                                        symname, sym->st_info, symvalue);
      if (rs != rtems_rtl_elf_rel_no_error)
        return false;
    }
    else
    {
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: rel: sym:%s(%d)=%08jx type:%d off:%08jx\n",
                symname, (int) ELF_R_SYM (rel->r_info),
                (uintmax_t) symvalue, (int) ELF_R_TYPE (rel->r_info),
                (uintmax_t) rel->r_offset);
      rs = rtems_rtl_elf_relocate_rel (obj, rel, targetsect,
                                       symname, sym->st_info, symvalue);
      if (rs != rtems_rtl_elf_rel_no_error)
        return false;
    }

    sobj = rtems_rtl_find_obj_with_symbol (symbol);

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_DEPENDENCY))
      printf ("rtl: depend: %s -> %s:%s\n",
              obj->oname,
              sobj == NULL ? "not-found" : sobj->oname,
              symname);

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

  /*
   * The section muct has been loaded. It could be a separate section in an
   * archive and not loaded.
   */
  if ((targetsect->flags & RTEMS_RTL_OBJ_SECT_LOAD) == 0)
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
    if (ELF_ST_TYPE (sym.st_info) == STT_OBJECT ||
        ELF_ST_TYPE (sym.st_info) == STT_COMMON ||
        ELF_ST_TYPE (sym.st_info) == STT_FUNC ||
        ELF_ST_TYPE (sym.st_info) == STT_NOTYPE ||
        ELF_ST_TYPE (sym.st_info) == STT_TLS ||
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
  rtems_rtl_obj_sect*      sect;
  bool                     is_rela;
  Elf_Word                 symvalue;
  rtems_rtl_obj*           sobj;
  rtems_rtl_elf_rel_status rs;

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
    rs = rtems_rtl_elf_relocate_rela (reloc->obj, &rela, sect,
                                      sym->name, sym->data, symvalue);
    if (rs != rtems_rtl_elf_rel_no_error)
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
    rs = rtems_rtl_elf_relocate_rel (reloc->obj, &rel, sect,
                                     sym->name, sym->data, symvalue);
    if (rs != rtems_rtl_elf_rel_no_error)
      return false;
  }

  if (reloc->obj->unresolved > 0)
  {
    --reloc->obj->unresolved;
    if (reloc->obj->unresolved == 0)
      reloc->obj->flags &= ~RTEMS_RTL_OBJ_UNRESOLVED;
  }

  sobj = rtems_rtl_find_obj_with_symbol (sym);

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_DEPENDENCY))
    printf ("rtl: depend: %s -> %s:%s\n",
            reloc->obj->oname,
            sobj == NULL ? "not-found" : sobj->oname,
            sym->name);

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
        printf ("rtl: com:elf:%-2d bind:%-2d type:%-2d size:%d value:%d name:%d\n",
                sym, (int) ELF_ST_BIND (symbol.st_info),
                (int) ELF_ST_TYPE (symbol.st_info),
                (int) symbol.st_size, (int) symbol.st_value,
                (int) symbol.st_name);
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

/**
 * Struct to handle trampoline reloc recs in the unresolved table.
 */
typedef struct rtems_rtl_tramp_data
{
  bool           failure;
  rtems_rtl_obj* obj;
  size_t         count;
  size_t         total;
} rtems_rtl_tramp_data;

static bool
rtems_rtl_elf_tramp_resolve_reloc (rtems_rtl_unresolv_rec* rec,
                                   void*                   data)
{
  rtems_rtl_tramp_data* td = (rtems_rtl_tramp_data*) data;
  if (rec->type == rtems_rtl_trampoline_reloc)
  {
    const rtems_rtl_tramp_reloc* tramp = &rec->rec.tramp;

    ++td->total;

    if (tramp->obj == td->obj)
    {
      const rtems_rtl_obj_sect* targetsect;
      Elf_Byte                  st_info;
      Elf_Word                  symvalue;
      rtems_rtl_elf_rel_status  rs;
      bool*                     failure = (bool*) data;
      const bool                is_rela = (tramp->flags & 1) == 1;
      const bool                unresolved = (tramp->flags & (1 << 1)) != 0;

      ++td->count;

      targetsect = rtems_rtl_obj_find_section_by_index (tramp->obj, tramp->sect);
      st_info = tramp->flags >> 8;
      symvalue = tramp->symvalue;

      if (is_rela)
      {
        Elf_Rela rela = {
          .r_offset = tramp->rel[REL_R_OFFSET],
          .r_info   = tramp->rel[REL_R_INFO],
          .r_addend = tramp->rel[REL_R_ADDEND]
        };
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
          printf ("rtl: rela tramp: check: %c(%d)=%08jx type:%d off:%08jx addend:%d\n",
                  ELF_ST_BIND (st_info) == STB_GLOBAL ||
                  ELF_ST_BIND (st_info) == STB_WEAK ? 'G' : 'L',
                  (int) ELF_R_SYM (rela.r_info),
                  (uintmax_t) symvalue, (int) ELF_R_TYPE (rela.r_info),
                  (uintmax_t) rela.r_offset, (int) rela.r_addend);
        rs = rtems_rtl_elf_relocate_rela_tramp (tramp->obj, &rela, targetsect,
                                                NULL, st_info, symvalue);
      }
      else
      {
        Elf_Rel rel = {
          .r_offset = tramp->rel[REL_R_OFFSET],
          .r_info   = tramp->rel[REL_R_INFO],
        };
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
          printf ("rtl: rel tramp: check: %c(%d)=%08jx type:%d off:%08jx\n",
                  ELF_ST_BIND (st_info) == STB_GLOBAL ||
                  ELF_ST_BIND (st_info) == STB_WEAK ? 'G' : 'L',
                  (int) ELF_R_SYM (rel.r_info),
                  (uintmax_t) symvalue, (int) ELF_R_TYPE (rel.r_info),
                  (uintmax_t) rel.r_offset);
        rs = rtems_rtl_elf_relocate_rel_tramp (tramp->obj, &rel, targetsect,
                                               NULL, st_info, symvalue);
      }

      if (unresolved || rs == rtems_rtl_elf_rel_tramp_add)
        tramp->obj->tramps_size += tramp->obj->tramp_size;
      if (rs == rtems_rtl_elf_rel_failure)
      {
        *failure = true;
        return true;
      }
    }
  }

  return false;
}

static bool
rtems_rtl_elf_alloc_trampoline (rtems_rtl_obj* obj, size_t unresolved)
{
  rtems_rtl_tramp_data td =  { 0 };
  td.obj = obj;
  /*
   * See which relocs are out of range and need a trampoline.
   */
  rtems_rtl_unresolved_iterate (rtems_rtl_elf_tramp_resolve_reloc, &td);
  if (td.failure)
    return false;
  rtems_rtl_trampoline_remove (obj);
  obj->tramp_relocs = obj->tramp_size == 0 ? 0 : obj->tramps_size / obj->tramp_size;
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
    printf ("rtl: tramp:elf: tramps: %zu count:%zu total:%zu\n",
            obj->tramp_relocs, td.count, td.total);
  /*
   * Add on enough space to handle the unresolved externals that need to be
   * resolved at some point in time. They could all require fixups and
   * trampolines.
   */
  obj->tramps_size += obj->tramp_size * unresolved;
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
    printf ("rtl: tramp:elf: slots: %zu (%zu)\n",
            obj->tramp_size == 0 ? 0 : obj->tramps_size / obj->tramp_size,
            obj->tramps_size);
  return rtems_rtl_obj_alloc_trampoline (obj);
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
rtems_rtl_elf_symbols_load (rtems_rtl_obj*      obj,
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
      printf ("rtl: sym:elf:%-4d name:%-4d: %-20s: bind:%-2d:%-12s " \
              "type:%-2d:%-10s sect:%-5d size:%-5d value:%d\n",
              sym, (int) symbol.st_name, name,
              (int) ELF_ST_BIND (symbol.st_info),
              rtems_rtl_elf_sym_bind_label (symbol.st_info),
              (int) ELF_ST_TYPE (symbol.st_info),
              rtems_rtl_elf_sym_type_label (symbol.st_info),
              symbol.st_shndx,
              (int) symbol.st_size,
              (int) symbol.st_value);

    /*
     * If a duplicate forget it.
     */
    if (rtems_rtl_symbol_global_find (name))
      continue;

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
           * symbol is not weak raise check if the object file being loaded is
           * from an archive. If the base image is built with text sections a
           * symbol with it's section will be linked into the base image and not
           * another symbol. If not an archive rause an error.
           *
           * If the symbol is weak and present globally ignore this symbol and
           * use the global one and if it is not present take this symbol global
           * or weak. We accept the first weak symbol we find and make it
           * globally exported.
           */
          if (rtems_rtl_symbol_global_find (name) &&
              (ELF_ST_BIND (symbol.st_info) != STB_WEAK))
          {
            if (!rtems_rtl_obj_aname_valid (obj))
            {
              rtems_rtl_set_error (ENOMEM, "duplicate global symbol: %s", name);
              return false;
            }
          }
          else
          {
            if (rtems_rtl_trace (RTEMS_RTL_TRACE_SYMBOL))
              printf ("rtl: sym:elf:%-4d name:%-4d: %-20s: global\n",
                      sym, (int) symbol.st_name, name);
            ++globals;
            global_string_space += strlen (name) + 1;
          }
        }
        else if (ELF_ST_BIND (symbol.st_info) == STB_LOCAL)
        {
          if (rtems_rtl_trace (RTEMS_RTL_TRACE_SYMBOL))
            printf ("rtl: sym:elf:%-4d name:%-4d: %-20s: local\n",
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
    Elf_Sym symbol;
    off_t   off;
    size_t  len;

    off = obj->ooffset + sect->offset + (sym * sizeof (symbol));

    if (!rtems_rtl_obj_cache_read_byval (symbols, fd, off,
                                         &symbol, sizeof (symbol)))
    {
      if (obj->local_syms)
      {
        rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, obj->local_table);
        obj->local_table = NULL;
        obj->local_size = 0;
        obj->local_syms = 0;
      }
      if (obj->global_syms)
      {
        rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, obj->global_table);
        obj->global_table = NULL;
        obj->global_syms = 0;
        obj->global_size = 0;
      }
      return false;
    }

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

      /*
       * There needs to be a valid section for the symbol.
       */
      symsect = rtems_rtl_obj_find_section_by_index (obj, symbol.st_shndx);

      if (symsect != NULL)
      {
        rtems_rtl_obj_sym*  osym;
        char*               string;
        Elf_Word            value;
        const char*         name;

        off = obj->ooffset + strtab->offset + symbol.st_name;
        len = RTEMS_RTL_ELF_STRING_MAX;

        if (!rtems_rtl_obj_cache_read (strings, fd, off, (void**) &name, &len))
          return false;

        /*
         * If a duplicate forget it.
         */
        if (rtems_rtl_symbol_global_find (name))
          continue;

        if ((ELF_ST_BIND (symbol.st_info) == STB_GLOBAL) ||
            (ELF_ST_BIND (symbol.st_info) == STB_WEAK))
        {
          size_t slen = strlen (name) + 1;
          if ((gstring + slen) > (char*) obj->global_table + obj->global_size)
            string = NULL;
          else
          {
            osym = gsym;
            string = gstring;
            gstring += slen;
            ++gsym;
          }
        }
        else
        {
          size_t slen = strlen (name) + 1;
          if ((lstring + slen) > (char*) obj->local_table + obj->local_size)
            string = NULL;
          else
          {
            osym = lsym;
            string = lstring;
            lstring += slen;
            ++lsym;
          }
        }

        /*
         * See if the loading has overflowed the allocated tables.
         */
        if (string == NULL)
        {
          if (obj->local_syms)
          {
            rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, obj->local_table);
            obj->local_table = NULL;
            obj->local_size = 0;
            obj->local_syms = 0;
          }
          if (obj->global_syms)
          {
            rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_SYMBOL, obj->global_table);
            obj->global_table = NULL;
            obj->global_syms = 0;
            obj->global_size = 0;
          }
          rtems_rtl_set_error (ENOMEM, "syms overlow, parsing/loading size mismatch");
          return false;
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
        osym->value = (uint8_t*) value;
        osym->data = symbol.st_shndx;

        if (rtems_rtl_trace (RTEMS_RTL_TRACE_SYMBOL))
          printf ("rtl: sym:add:%-4d name:%-4d: %-20s: bind:%-2d " \
                  "type:%-2d val:%-8p sect:%-3d size:%d\n",
                  sym, (int) symbol.st_name, osym->name,
                  (int) ELF_ST_BIND (symbol.st_info),
                  (int) ELF_ST_TYPE (symbol.st_info),
                  osym->value, symbol.st_shndx,
                  (int) symbol.st_size);
      }
    }
  }

  return true;
}

static bool
rtems_rtl_elf_symbols_locate (rtems_rtl_obj*      obj,
                              int                 fd,
                              rtems_rtl_obj_sect* sect,
                              void*               data)
{
  int sym;

  for (sym = 0; sym < obj->local_syms; ++sym)
  {
      rtems_rtl_obj_sym*  osym = &obj->local_table[sym];
      rtems_rtl_obj_sect* symsect;
      symsect = rtems_rtl_obj_find_section_by_index (obj, osym->data);
      if (symsect)
      {
        osym->value += (intptr_t) symsect->base;
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_SYMBOL))
          printf ("rtl: sym:locate:local :%-4d name: %-20s val:%-8p sect:%-3d (%s, %p)\n",
                  sym, osym->name, osym->value, osym->data,
                  symsect->name, symsect->base);
      }
  }

  for (sym = 0; sym < obj->global_syms; ++sym)
  {
      rtems_rtl_obj_sym*  osym = &obj->global_table[sym];
      rtems_rtl_obj_sect* symsect;
      symsect = rtems_rtl_obj_find_section_by_index (obj, osym->data);
      if (symsect)
      {
        osym->value += (intptr_t) symsect->base;
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_SYMBOL))
          printf ("rtl: sym:locate:global:%-4d name: %-20s val:%-8p sect:%-3d (%s, %p)\n",
                  sym, osym->name, osym->value, osym->data,
                  symsect->name, symsect->base);
      }
  }

  if (obj->global_size)
    rtems_rtl_symbol_obj_add (obj);

  return true;
}

static bool
rtems_rtl_elf_arch_alloc (rtems_rtl_obj*      obj,
                          int                 fd,
                          rtems_rtl_obj_sect* sect,
                          void*               data)
{
  if (rtems_rtl_obj_sect_is_arch_alloc (sect))
    return rtems_rtl_elf_arch_section_alloc (obj, sect);
  return true;
}

static bool
rtems_rtl_elf_arch_free (rtems_rtl_obj* obj)
{
  int index = -1;
  while (true)
  {
    rtems_rtl_obj_sect* sect;
    sect = rtems_rtl_obj_find_section_by_mask (obj,
                                               index,
                                               RTEMS_RTL_OBJ_SECT_ARCH_ALLOC);
    if (sect == NULL)
      break;
    if (!rtems_rtl_elf_arch_section_free (obj, sect))
      return false;
    index = sect->section;
  }
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
    char*    name;
    size_t   len;
    uint32_t flags;

    /*
     * Make sure section is at least 32bits to avoid 16-bit overflow errors.
     */
    off = obj->ooffset + ehdr->e_shoff + (((uint32_t) section) * ehdr->e_shentsize);

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_DETAIL))
      printf ("rtl: section header: %2d: offset=%d\n", section, (int) off);

    if (!rtems_rtl_obj_cache_read_byval (sects, fd, off, &shdr, sizeof (shdr)))
      return false;

    len = RTEMS_RTL_ELF_STRING_MAX;
    if (!rtems_rtl_obj_cache_read (strings, fd,
                                   sectstroff + shdr.sh_name,
                                   (void**) &name, &len))
      return false;

    if (rtems_rtl_trace (RTEMS_RTL_TRACE_DETAIL))
      printf ("rtl: section: %2d: name=%s type=%d flags=%08x link=%d info=%d\n",
              section, name, (int) shdr.sh_type, (unsigned int) shdr.sh_flags,
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
        flags = RTEMS_RTL_OBJ_SECT_RELA | RTEMS_RTL_OBJ_SECT_LOAD;
        break;

      case SHT_REL:
        /*
         * The sh_link holds the section index for the symbol table. The sh_info
         * holds the section index the relocations apply to.
         */
        flags = RTEMS_RTL_OBJ_SECT_REL | RTEMS_RTL_OBJ_SECT_LOAD;
        break;

      case SHT_SYMTAB:
        flags = RTEMS_RTL_OBJ_SECT_SYM;
        break;

      case SHT_STRTAB:
        flags = RTEMS_RTL_OBJ_SECT_STR;
        break;

      case SHT_INIT_ARRAY:
        /*
         * Constructors are text and need to be loaded.
         */
        flags = (RTEMS_RTL_OBJ_SECT_CTOR |
                 RTEMS_RTL_OBJ_SECT_TEXT |
                 RTEMS_RTL_OBJ_SECT_LOAD);
        break;

      case SHT_FINI_ARRAY:
        /*
         * Destructors are text and need to be loaded.
         */
        flags = (RTEMS_RTL_OBJ_SECT_DTOR |
                 RTEMS_RTL_OBJ_SECT_TEXT |
                 RTEMS_RTL_OBJ_SECT_LOAD);
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
      /*
       * If the object file is part of a library check the section's name. If it
       * starts with '.text.*' see if the last part is a global symbol. If a
       * global symbol exists we have to assume the symbol in the archive is a
       * duplicate can can be ignored.
       */
      if (rtems_rtl_obj_aname_valid (obj))
      {
        const char* symname = rtems_rtl_elf_separated_section (name);
        if (symname != NULL && rtems_rtl_symbol_global_find (symname))
          flags &= ~RTEMS_RTL_OBJ_SECT_LOAD;
      }

      /*
       * If link ordering this section must appear in the same order in memory
       * as the linked-to section relative to the sections it loads with.
       */
      if ((shdr.sh_flags & SHF_LINK_ORDER) != 0)
        flags |= RTEMS_RTL_OBJ_SECT_LINK;

      /*
       * Some architexctures have a named PROGBIT section for INIT/FINI.
       */
      if (strcmp (".ctors", name) == 0)
        flags |= RTEMS_RTL_OBJ_SECT_CTOR;
      if (strcmp (".dtors", name) == 0)
        flags |= RTEMS_RTL_OBJ_SECT_DTOR;

      if (rtems_rtl_elf_unwind_parse (obj, name, flags))
      {
        flags &= ~(RTEMS_RTL_OBJ_SECT_TEXT | RTEMS_RTL_OBJ_SECT_CONST);
        flags |= RTEMS_RTL_OBJ_SECT_EH;
      }

      /*
       * Architecture specific parsing. Modified or extends the flags.
       */
      flags = rtems_rtl_elf_arch_parse_section (obj, section, name, &shdr, flags);
      if (flags == 0)
      {
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_WARNING))
          printf ("rtl: unsupported section: %2d: type=%02d flags=%02x\n",
                  section, (int) shdr.sh_type, (int) shdr.sh_flags);
        rtems_rtl_set_error (ENOMEM, "invalid architecture section: %s", name);
        return false;
      }

      /*
       * Add the section.
       */
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
  int                  sec_num = 0;
  section_detail*      sd;
  int                  i = 0;
  size_t               m;

  /*
   * The section masks to add to the linkmap.
   */
  const uint32_t       sect_mask[] = {
    RTEMS_RTL_OBJ_SECT_TEXT | RTEMS_RTL_OBJ_SECT_LOAD,
    RTEMS_RTL_OBJ_SECT_CONST | RTEMS_RTL_OBJ_SECT_LOAD,
    RTEMS_RTL_OBJ_SECT_DATA | RTEMS_RTL_OBJ_SECT_LOAD,
    RTEMS_RTL_OBJ_SECT_BSS
  };
  const size_t sect_masks = sizeof (sect_mask) / sizeof (sect_mask[0]);

  /*
   * Caculate the size of sections' name.
   */
  for (m = 0; m < sect_masks; ++m)
  {
    sections = &obj->sections;
    node = rtems_chain_first (sections);
    while (!rtems_chain_is_tail (sections, node))
    {
      rtems_rtl_obj_sect* sect = (rtems_rtl_obj_sect*) node;
      const uint32_t      mask = sect_mask[m];
      if ((sect->size != 0) && ((sect->flags & mask) == mask))
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

  for (m = 0; m < sect_masks; ++m)
  {
    sections = &obj->sections;
    node = rtems_chain_first (sections);
    while (!rtems_chain_is_tail (sections, node))
    {
      rtems_rtl_obj_sect* sect = (rtems_rtl_obj_sect*) node;
      const uint32_t      mask = sect_mask[m];

      if ((sect->size != 0) && ((sect->flags & mask) == mask))
      {
        sd[i].name = sect->name;
        sd[i].size = sect->size;
        if ((mask & RTEMS_RTL_OBJ_SECT_TEXT) != 0)
        {
          sd[i].rap_id = rap_text;
          sd[i].offset = sect->base - obj->text_base;
        }
        if ((mask & RTEMS_RTL_OBJ_SECT_CONST) != 0)
        {
          sd[i].rap_id = rap_const;
          sd[i].offset = sect->base - obj->const_base;
        }
        if ((mask & RTEMS_RTL_OBJ_SECT_DATA) != 0)
        {
          sd[i].rap_id = rap_data;
          sd[i].offset = sect->base - obj->data_base;
        }
        if ((mask & RTEMS_RTL_OBJ_SECT_BSS) != 0)
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
   * Set the format's architecture's maximum tramp size.
   */
  obj->tramp_size = rtems_rtl_elf_relocate_tramp_max_size ();

  /*
   * Parse the section information first so we have the memory map of the object
   * file and the memory allocated. Any further allocations we make to complete
   * the load will not fragment the memory.
   */
  if (!rtems_rtl_elf_parse_sections (obj, fd, &ehdr))
    return false;

  /*
   * Set the entry point if there is one.
   */
  obj->entry = (void*)(uintptr_t) ehdr.e_entry;

  /*
   * Load the symbol table.
   *
   * 1. See if there are any common variables and if there are add a
   *    common section.
   * 2. Add up the common.
   * 3. The load the symbols.
   */
  if (!rtems_rtl_obj_load_symbols (obj, fd, rtems_rtl_elf_common, &common))
    return false;
  if (!rtems_rtl_elf_add_common (obj, common.size, common.alignment))
    return false;
  if (!rtems_rtl_obj_load_symbols (obj, fd, rtems_rtl_elf_symbols_load, &ehdr))
    return false;

  /*
   * Parse the relocation records. It lets us know how many dependents
   * and fixup trampolines there are.
   */
  if (!rtems_rtl_obj_relocate (obj, fd, rtems_rtl_elf_relocs_parser, &relocs))
    return false;

  /*
   * Lock the allocator so the section memory and the trampoline memory are as
   * clock as possible.
   */
  rtems_rtl_alloc_lock ();

  /*
   * Allocate the sections.
   */
  if (!rtems_rtl_obj_alloc_sections (obj, fd, rtems_rtl_elf_arch_alloc, &ehdr))
    return false;

  if (!rtems_rtl_obj_load_symbols (obj, fd, rtems_rtl_elf_symbols_locate, &ehdr))
    return false;

  if (!rtems_rtl_elf_dependents (obj, &relocs))
    return false;

  if (!rtems_rtl_elf_alloc_trampoline (obj, relocs.unresolved))
    return false;

  /*
   * Unlock the allocator.
   */
  rtems_rtl_alloc_unlock ();

  /*
   * Load the sections and symbols and then relocation to the base address.
   */
  if (!rtems_rtl_obj_load_sections (obj, fd, rtems_rtl_elf_loader, &ehdr))
    return false;

  /*
   * Fix up the relocations.
   */
  if (!rtems_rtl_obj_relocate (obj, fd, rtems_rtl_elf_relocs_locator, &ehdr))
    return false;

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
  rtems_rtl_elf_arch_free (obj);
  rtems_rtl_elf_unwind_deregister (obj);
  return true;
}

rtems_rtl_loader_format*
rtems_rtl_elf_file_sig (void)
{
  return &elf_sig;
}
