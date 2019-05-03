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
  return rtems_rtl_elf_rel_no_error;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rela (rtems_rtl_obj*            obj,
                             const Elf_Rela*           rela,
                             const rtems_rtl_obj_sect* sect,
                             const char*               symname,
                             const Elf_Byte            syminfo,
                             const Elf_Word            symvalue)
{
  Elf_Addr *where;
  Elf32_Word tmp;
  Elf32_Word insn;


  where = (Elf_Addr *)(sect->base + rela->r_offset);

  switch (ELF_R_TYPE(rela->r_info)) {
    case R_TYPE(NONE):
      break;

    case R_TYPE(HI16):
      insn = *where;
      /* orhi/mvhi instruction
       *  31--------26|25-21|20-16|15----0|
       * |0 1 1 1 1 0 |rY   |rX   |imm16  |
       */
      if (0x1e == (insn >> 26)) {
        insn &= 0xffff0000;
        insn |= ((symvalue + rela->r_addend) >> 16);
        *where = insn;
      }
      break;

    case R_TYPE(LO16):
      insn = *where;
      /* ori instruction
       *  31--------26|25-21|20-16|15----0|
       * |0 0 1 1 1 0 |rY   |rX   |imm16  |
       */
      if (0xe == (insn >> 26)) {
        insn &= 0xffff0000;
        insn |= ((symvalue + rela->r_addend) & 0xffff);
        *where = insn;
      }
      break;

    case R_TYPE(CALL):
      insn = *where;
      /*
       * calli instruction
       *  31-------26|25---0|
       * |1 1 1 1 1 0|imm26 |
       * Syntax: call imm26
       * Operation: ra = pc + 4; pc = pc + sign_extend(imm26<<2)
       */
      if (0x3e == (insn >> 26)) {
        Elf_Sword imm26 = symvalue +rela->r_addend - (Elf_Addr)where;
        imm26 = (imm26 >> 2) & 0x3ffffff;
        insn = 0xf8000000 + imm26;
        *where = insn;
      }
      break;

    case R_TYPE(BRANCH):
      insn = *where;
      tmp = symvalue + rela->r_addend - (Elf_Addr)where;
      tmp = (Elf32_Sword)tmp >> 2;
      if (((Elf32_Sword)tmp > 0x7fff) || ((Elf32_Sword)tmp < -0x8000)){
        printf("BRANCH Overflow\n");
        return rtems_rtl_elf_rel_failure;
      }

      *where = (*where & 0xffff0000) | (tmp & 0xffff);
      break;

    case R_TYPE(32):
      *where = symvalue + rela->r_addend;
      break;

    default:
      rtems_rtl_set_error (EINVAL, "rela type record not supported");
      printf("Unsupported reloc types\n");
      return rtems_rtl_elf_rel_failure;
  }

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
      printf("rela relocation type is %ld\n", ELF_R_TYPE(rela->r_info));
      printf("relocated address 0x%08lx\n", (Elf_Addr)where);
  }

  return rtems_rtl_elf_rel_no_error;
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
  (void) rela;
  (void) sect;
  (void) symname;
  (void) syminfo;
  (void) symvalue;
  rtems_rtl_set_error (EINVAL, "rel type record not supported");
  return rtems_rtl_elf_rel_failure;
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rel (rtems_rtl_obj*            obj,
                            const Elf_Rel*            rel,
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
