#include <sys/cdefs.h>

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <rtems/rtl/rtl.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include <rtems/rtl/rtl-trace.h>
#include "rtl-unwind.h"
#include "rtl-unwind-dw2.h"

uint32_t
rtems_rtl_elf_section_flags (const rtems_rtl_obj* obj,
                             const Elf_Shdr*      shdr)
{
  return 0;
}

uint32_t
rtems_rtl_elf_arch_parse_section (const rtems_rtl_obj* obj,
                                  int                  section,
                                  const char*          name,
                                  const Elf_Shdr*      shdr,
                                  const uint32_t       flags)
{
  (void) obj;
  (void) section;
  (void) name;
  (void) shdr;
  return flags;
}

bool
rtems_rtl_elf_arch_section_alloc (const rtems_rtl_obj* obj,
                                  rtems_rtl_obj_sect*  sect)
{
  (void) obj;
  (void) sect;
  return false;
}

bool
rtems_rtl_elf_arch_section_free (const rtems_rtl_obj* obj,
                                  rtems_rtl_obj_sect*  sect)
{
  (void) obj;
  (void) sect;
  return false;
}

bool
rtems_rtl_elf_rel_resolve_sym (Elf_Word type)
{
  return true;
}

size_t
rtems_rtl_elf_relocate_tramp_max_size (void)
{
  /*
   * Disable by returning 0.
   */
  return 0;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rela_tramp (rtems_rtl_obj*            obj,
                                   const Elf_Rela*           rela,
                                   const rtems_rtl_obj_sect* sect,
                                   const char*               symname,
                                   const Elf_Byte            syminfo,
                                   const Elf_Word            symvalue)
{
  (void) obj;
  (void) rela;
  (void) sect;
  (void) symname;
  (void) syminfo;
  (void) symvalue;
  rtems_rtl_set_error (EINVAL, "rela type record not supported");
  return rtems_rtl_elf_rel_failure;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rela (rtems_rtl_obj*            obj,
                             const Elf_Rela*           rela,
                             const rtems_rtl_obj_sect* sect,
                             const char*               symname,
                             const Elf_Byte            syminfo,
                             const Elf_Word            symvalue)
{
  (void) obj;
  (void) rela;
  (void) sect;
  (void) symname;
  (void) syminfo;
  (void) symvalue;
  rtems_rtl_set_error (EINVAL, "rela type record not supported");
  return rtems_rtl_elf_rel_failure;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rel_tramp (rtems_rtl_obj*            obj,
                                  const Elf_Rel*            rel,
                                  const rtems_rtl_obj_sect* sect,
                                  const char*               symname,
                                  const Elf_Byte            syminfo,
                                  const Elf_Word            symvalue)
{
  (void) obj;
  (void) rel;
  (void) sect;
  (void) symname;
  (void) syminfo;
  (void) symvalue;
  return rtems_rtl_elf_rel_no_error;
}

/*
 * 1. _gp_disp symbol are not considered in this file.
 * 2. There is a local/external column;
 * local corresponds to (STB_LOCAL & STT_SECTION) and
 * all others are external. Because if the type of a
 * symbol is STT_SECTION, it must be STB_LOCAL. Thus
 * just consider symtype here.
 */
rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rel (rtems_rtl_obj*            obj,
                            const Elf_Rel*            rel,
                            const rtems_rtl_obj_sect* sect,
                            const char*               symname,
                            const Elf_Byte            syminfo,
                            const Elf_Word            symvalue)
{
  Elf_Addr *where;
  Elf_Word  tmp;
  Elf_Word addend = (Elf_Word)0;
  Elf_Word local = 0;
  uint32_t t;


  static Elf_Addr *where_hi16;
  static Elf_Addr ahl;

  where = (Elf_Addr *)(sect->base + rel->r_offset);
  addend = *where;

  if (syminfo == STT_SECTION)
    local = 1;

  switch (ELF_R_TYPE(rel->r_info)) {
    case R_TYPE(NONE):
      break;

    case R_TYPE(16):
      tmp = addend & 0xffff;
      if ((tmp & 0x8000) == 0x8000)
        tmp |= 0xffff0000; /* Sign extend */
      tmp = symvalue + (int)tmp;
      if ((tmp & 0xffff0000) != 0) {
        printf("R_MIPS_16 Overflow\n");
        return rtems_rtl_elf_rel_failure;
      }

      *where = (tmp & 0xffff) | (*where & 0xffff0000);

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: R_MIPS_16 %p @ %p in %s\n",
                (void *)*(where), where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(32):
      tmp = symvalue + addend;
      if (addend != tmp)
        *where = tmp;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: R_MIPS_32 %p @ %p in %s\n",
                (void *)*(where), where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(26):

        addend &= 0x03ffffff;
        addend <<= 2;

      if (local == 1) { /* STB_LOCAL and STT_SECTION */
        tmp = symvalue + (((Elf_Addr)where & 0xf0000000) | addend);
        tmp >>= 2;

      } else { /* external */

        tmp = addend;

        if ((tmp & 0x08000000) == 0x08000000)
          tmp |= 0xf0000000; /* Sign extened */
        tmp = ((int)tmp + symvalue) >> 2;

      }

      *where &= ~0x03ffffff;
      *where |= tmp & 0x03ffffff;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: R_MIPS_26 local=%lu @ %p in %s\n",
                local, (void *)*(where), rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(HI16):
      ahl = addend << 16;
      where_hi16 = where;


      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: R_MIPS_HI16 %p @ %p in %s\n",
                (void *)*(where), where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(LO16):
      //ahl += (int16_t)addend;
      t = ahl + (int16_t)addend;
      tmp = symvalue;
      if (tmp == 0)
        return rtems_rtl_elf_rel_failure;

      addend &= 0xffff0000;
      addend |= (uint16_t)(t + tmp);
      *where = addend;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf("*where %lx where %p\n", *where, where);

      addend = *where_hi16;
      addend &= 0xffff0000;
      addend |= ((t + tmp) - (int16_t)(t + tmp)) >> 16;
      *where_hi16 = addend;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf("*where_hi %lx where_hi %p\n", *where_hi16, where_hi16);

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: R_MIPS_LO16 %p @ %p in %s\n",
                (void *)*(where), where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(PC16):
      tmp = addend & 0xffff;
      if ((tmp & 0x8000) == 0x8000)
        tmp |= 0xffff0000; /* Sign extend */
      tmp = symvalue + ((int)tmp*4) - (Elf_Addr)where;
      tmp = (Elf_Sword)tmp >> 2;
      if (((Elf_Sword)tmp > 0x7fff) || ((Elf_Sword)tmp < -0x8000)) {
        printf("R_MIPS_PC16 Overflow\n");
        return rtems_rtl_elf_rel_failure;
      }

      *where = (tmp & 0xffff) | (*where & 0xffff0000);

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: R_MIPS_PC16 %p @ %p in %s\n",
                (void *)*(where), where, rtems_rtl_obj_oname (obj));

      break;

    default:
      printf ("rtl: reloc unknown: sym = %lu, type = %lu, offset = %p, "
              "contents = %p\n",
              ELF_R_SYM(rel->r_info), (uint32_t) ELF_R_TYPE(rel->r_info),
              (void *)rel->r_offset, (void *)*where);
      rtems_rtl_set_error (EINVAL,
                           "%s: Unsupported relocation type %ld "
                           "in non-PLT relocations",
                           sect->name, (uint32_t) ELF_R_TYPE(rel->r_info));
      return rtems_rtl_elf_rel_failure;
  }

  return rtems_rtl_elf_rel_no_error;
}

bool
rtems_rtl_elf_unwind_parse (const rtems_rtl_obj* obj,
                            const char*          name,
                            uint32_t             flags)
{
  return rtems_rtl_elf_unwind_dw2_parse (obj, name, flags);
}

bool
rtems_rtl_elf_unwind_register (rtems_rtl_obj* obj)
{
  return rtems_rtl_elf_unwind_dw2_register (obj);
}

bool
rtems_rtl_elf_unwind_deregister (rtems_rtl_obj* obj)
{
  return rtems_rtl_elf_unwind_dw2_deregister (obj);
}
