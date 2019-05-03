
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
  Elf_Sword tmp;

  where = (Elf_Addr *)(sect->base + rela->r_offset);

  /* Handle the not 4byte aligned address carefully */

  switch (ELF_R_TYPE(rela->r_info)) {
    case R_TYPE(NONE):
      break;

    case R_TYPE(32):
      *(uint16_t *)where = ((symvalue + rela->r_addend) >> 16) & 0xffff;
      *((uint16_t *)where + 1) = (symvalue + rela->r_addend) & 0xffff;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
          printf("*where 0x%04x%04x\n", *((uint16_t *)where + 1), *(uint16_t *)where);
      }
      break;

    case R_TYPE(PCREL10):
      /* beq, bge, bgeu, bgt, bgtu, ble, bleu, blt, bltu, bne */
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
        printf("*where %x\n", *(uint16_t *)where);
        printf("symvalue - where %x\n", (int)(symvalue - (Elf_Word)where));
      }
      tmp = (symvalue + rela->r_addend - ((Elf_Word)where + 2)); /* pc is the next instruction */
      tmp = (Elf_Sword)tmp >> 1;
      if (((Elf32_Sword)tmp > 0x1ff) || ((Elf32_Sword)tmp < -(Elf32_Sword)0x200)){
        printf("Overflow for PCREL10: %ld exceed -0x200:0x1ff\n", tmp);
        return rtems_rtl_elf_rel_failure;
      }

      *(uint16_t *)where = (*(uint16_t *)where & 0xfc00) | (tmp & 0x3ff);

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
          printf("*where 0x%04x\n",  *(uint16_t *)where);
      }

      break;

    default:
      rtems_rtl_set_error (EINVAL, "rela type record not supported");
      printf("Unsupported reloc types\n");
      return rtems_rtl_elf_rel_failure;
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
  (void) rel;
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
  (void) rel;
  (void) sect;
  (void) symname;
  (void) syminfo;
  (void) symvalue;
  rtems_rtl_set_error (EINVAL, "rel type record not supported");
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
