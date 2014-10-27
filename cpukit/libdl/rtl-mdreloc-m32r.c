#include <sys/cdefs.h>

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <rtems/rtl/rtl.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include "rtl-trace.h"

static inline Elf_Addr
load_ptr(void *where)
{
  Elf_Addr res;

  memcpy(&res, where, sizeof(res));

  return (res);
}

static inline void
store_ptr(void *where, Elf_Addr val)
{
	memcpy(where, &val, sizeof(val));
}

bool
rtems_rtl_elf_rel_resolve_sym (Elf_Word type)
{
  return true;
}

bool
rtems_rtl_elf_relocate_rela (const rtems_rtl_obj_t*      obj,
                             const Elf_Rela*             rela,
                             const rtems_rtl_obj_sect_t* sect,
                             const char*                 symname,
                             const Elf_Byte              syminfo,
                             const Elf_Word              symvalue)
{

  Elf_Addr *where;
  Elf_Word  tmp;

  where = (Elf_Addr *)(sect->base + rela->r_offset);
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
      printf("relocated address 0x%08lx\n", (Elf_Addr)where);
  }

  switch (ELF_R_TYPE(rela->r_info)) {
    case R_TYPE(NONE):
      break;

    case R_TYPE(16_RELA):
      /*
       * half16: S + A
       */
      *(uint16_t *)where = (symvalue + rela->r_addend) & 0xffff;
      break;

    case R_TYPE(24_RELA):
      /*
       * imm24: (S + A) & 0xFFFFFF
       */
      tmp = symvalue + rela->r_addend;
      if (((Elf_Sword)tmp > 0x7fffff) || ((Elf_Sword)tmp < -0x800000)) {
        printf("24_RELA Overflow\n");
        return false;
      }
      *where = (*where & 0xff000000) | tmp & 0xffffff;
      break;

    case R_TYPE(32_RELA):
      /*
       * word32: S + A
       */
      *where += symvalue + rela->r_addend;
      break;

    case R_TYPE(26_PCREL_RELA):
      /*
       * disp24: ((S + A - P) >> 2) & 0xFFFFFF
       */
      tmp = symvalue + rela->r_addend - (Elf_Addr)where;
      tmp = (Elf_Sword)tmp >> 2;
      if (((Elf_Sword)tmp > 0x7fffff) || ((Elf_Sword)tmp < -0x800000)) {
        printf("26_PCREL_RELA Overflow\n");
        return false;
      }

      *where = (*where & 0xff000000) | (tmp & 0xffffff);
      break;

    case R_TYPE(18_PCREL_RELA):
      /*
       * disp16: ((S + A - P) >> 2) & 0xFFFFFF
       */
      tmp = symvalue + rela->r_addend - (Elf_Addr)where;
      tmp = (Elf_Sword)tmp >> 2;
      if (((Elf_Sword)tmp > 0x7fff) || ((Elf_Sword)tmp < -0x8000)) {
        printf("18_PCREL_RELA Overflow\n");
        return false;
      }

      *where = (*where & 0xffff0000) | (tmp & 0xffff);
      break;

    case R_TYPE(HI16_ULO_RELA):
      /*
       * imm16: ((S + A) >> 16)
       */
      tmp = *where;
      tmp += ((symvalue + rela->r_addend) >> 16) & 0xffff;
      *where = tmp;
      break;

    case R_TYPE(HI16_SLO_RELA):
      /*
       * imm16: ((S + A) >> 16) or ((S + A + 0x10000) >> 16)
       */
      tmp = symvalue + rela->r_addend;
      if (tmp & 0x8000) tmp += 0x10000;
      tmp = (tmp >> 16) & 0xffff;
      *where += tmp;
      break;

    case R_TYPE(LO16_RELA):
      /*
       * imm16: (S + A) & 0xFFFF
       */
      tmp = symvalue + rela->r_addend;
      *where = (*where & 0xffff0000) | (tmp & 0xffff);
      break;

    default:
      rtems_rtl_set_error (EINVAL, "rela type record not supported");
      printf("Unsupported rela reloc types\n");
      return false;
  }
  return true;
}

bool
rtems_rtl_elf_relocate_rel (const rtems_rtl_obj_t*      obj,
                            const Elf_Rel*              rel,
                            const rtems_rtl_obj_sect_t* sect,
                            const char*                 symname,
                            const Elf_Byte              syminfo,
                            const Elf_Word              symvalue)
{

  rtems_rtl_set_error (EINVAL, "rel type record not supported");
  return true;
}
