#include <sys/cdefs.h>

#include <stdio.h>
#include <rtems/rtl/rtl.h>
#include <errno.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include <rtems/rtl/rtl-trace.h>
#include "rtl-unwind.h"
#include "rtl-unwind-dw2.h"

uint32_t
rtems_rtl_elf_section_flags (const rtems_rtl_obj_t* obj,
                             const Elf_Shdr*        shdr)
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

static inline Elf_Addr
load_ptr(void *where)
{
	Elf_Addr res;

	memcpy(&res, where, sizeof(res));

	return (res);
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
  Elf_Addr target = 0;
  Elf_Addr *where;
  Elf_Word tmp;
  Elf_Word size; //byte

  where = (Elf_Addr *)(sect->base + rela->r_offset);

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
      printf("rela relocation type is %d relocated address 0x%08x",
              ELF_R_TYPE(rela->r_info), where);
  }

  tmp = symvalue;
  switch (ELF_R_TYPE(rela->r_info)) {
    case R_TYPE(UNUSED0):
      break;

    case R_TYPE(HUIMM16):
      tmp = symvalue >> 16;
    case R_TYPE(LUIMM16):
    case R_TYPE(RIMM16):
      size = 2;
      break;

    case R_TYPE(BYTE4_DATA):
      size = 4;
      break;

    case R_TYPE(PCREL24):
    case R_TYPE(PCREL24_JU):
      where = (Elf_Addr*)((Elf_Addr)where - 2); /* Back 2 bytes */
      tmp = symvalue - (Elf_Addr)where;
      tmp >>= 1;
      if ((tmp & 0x20000000) == 0x20000000)
        tmp |= 0xc0000000;

      if ((tmp & 0xff000000) && (~tmp & 0xff800000)) {
        printf("PCREL24/PCREL24_JU Overflow\n");
        return rtems_rtl_elf_rel_failure;
      }

      tmp = (load_ptr(where) & 0x0000ff00) | ((tmp & 0x0000ffff) << 16) |
             ((tmp & 0x00ff0000) >> 16);
      size = 4;
      break;

    case R_TYPE(PCREL12_JUMP_S):
      tmp = symvalue - (Elf_Addr)where;
      tmp >>= 1;
      if ((tmp & 0x20000000) == 0x20000000)
        tmp |= 0xc0000000;

      if ((tmp & 0xfffff000) && (~tmp & 0xfffff800)) {
        printf("PCREL12_JUMP_S Overflow\n");
        return rtems_rtl_elf_rel_failure;
      }

      tmp = ((*(uint16_t *)where) & 0xf000) | (tmp & 0xfff);
      size = 2;
      break;

    default:
      printf("Unspported rela type\n");
      return rtems_rtl_elf_rel_failure;
  }

  memcpy((void*)where, &tmp, size);

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
