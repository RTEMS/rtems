/*
 * Taken from NetBSD and stripped of the relocations not needed on RTEMS.
 */

/*  $NetBSD: ppc_reloc.c,v 1.44 2010/01/13 20:17:22 christos Exp $  */

#include <sys/cdefs.h>

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <rtems/rtl/rtl.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include "rtl-trace.h"

#define ha(x) ((((u_int32_t)(x) & 0x8000) ? \
                 ((u_int32_t)(x) + 0x10000) : (u_int32_t)(x)) >> 16)
#define l(x) ((u_int32_t)(x) & 0xffff)


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
  Elf_Addr  target = 0;
  Elf_Addr* where;
  Elf_Word tmp;
  uint32_t mask = 0;
  uint32_t bits = 0;

  where = (Elf_Addr *)(sect->base + rela->r_offset);
  switch (ELF_R_TYPE(rela->r_info)) {
    case R_TYPE(NONE):
      break;

    case R_TYPE(32):
      /*
       * value:1; Field: word32; Expression: S + A
       */
      *where = symvalue + rela->r_addend;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: ADDR32 %p @ %p in %s\n",
                (void *)*(where), where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(14):
      /*
       * value:7; Field: low14*; Expression: (S + A) >> 2
       */
    case R_TYPE(24):
      /*
       * value:2; Field: low24*; Expression: (S + A) >> 2
       */
      if (ELF_R_TYPE(rela->r_info) == R_TYPE(14)) {
        bits = 14;
        mask = 0xfffc;
      } else {
        bits = 24;
        mask = 0x3fffffc;
      }
      tmp = (symvalue + rela->r_addend) >> 2;
      if (tmp > (1<<bits -1 )) {
        printf("Overflow ADDR14/ADDR24\n");
        return false;
      }
      tmp = *where;
      tmp &= ~mask;
      tmp |= (symvalue + rela->r_addend) & mask;
      *where = tmp;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: ADDR14/ADDR24 %p @ %p in %s\n",
                (void *)*where, where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(16_HA):
      /*
       * value:6; Field:half16; Expression: #ha(S+A)
       */

      tmp = symvalue + rela->r_addend;
      *(uint16_t *)where = (((tmp >> 16) + ((tmp & 0x8000) ? 1: 0)) & 0xffff);
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: 16_HA %p @ %p in %s\n",
                (void *)*(where), where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(16_HI):
      /*
       * value:5; Field:half16; Expression: #hi(S+A)
       */
      *(uint16_t *)where = ((symvalue + rela->r_addend) >> 16) & 0xffff;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: 16_HI %p @ %p in %s\n",
                (void *)*where, where, rtems_rtl_obj_oname (obj));
      break;
    case R_TYPE(16_LO):
      /*
       * value:4; Field:half16; Expression: #lo(S+A)
       */
      *(uint16_t *)where = (symvalue + (rela->r_addend)) & 0xffff;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: 16_LO %p @ %p in %s\n",
                (void *)*where, where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(REL14):
      /*
       * value:11; Field:low14*; Expression:(S+A-P)>>2
       */
    case R_TYPE(REL24):
      /*
       * value:10; Field:low24*; Expression:(S+A-P)>>2
       */
      if (ELF_R_TYPE(rela->r_info) == R_TYPE(REL24)) {
        mask = 0x3fffffc;
        bits = 24;
      }
      else if (ELF_R_TYPE(rela->r_info) == R_TYPE(REL14)) {
        mask = 0xfffc;
        bits = 14;
      }

      tmp =((int) (symvalue + rela->r_addend - (Elf_Addr)where)) >> 2;
      if (((Elf_Sword)tmp > ((1<<(bits-1)) - 1)) ||
          ((Elf_Sword)tmp < -(1<<(bits-1)))) {
        printf("Overflow REL14/REL24\n");
        return false;
      }

      tmp = *where;
      tmp &= ~mask;
      tmp |= (symvalue + rela->r_addend - (Elf_Addr)where) & mask;
      *where = tmp;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: REL24/REL14 %p @ %p in %s\n",
                (void *)*where, where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(REL32):
      /*
       * value:26; Field:word32*; Expression:S+A-P
       */
      *where = symvalue + rela->r_addend - (Elf_Addr)where;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: REL32 %p @ %p in %s\n",
                (void *)*where, where, rtems_rtl_obj_oname (obj));
      break;

    default:
      printf ("rtl: reloc unknown: sym = %lu, type = %lu, offset = %p, "
              "contents = %p\n",
              ELF_R_SYM(rela->r_info), (uint32_t) ELF_R_TYPE(rela->r_info),
              (void *)rela->r_offset, (void *)*where);
      rtems_rtl_set_error (EINVAL,
                           "%s: Unsupported relocation type %ld "
                           "in non-PLT relocations",
                           sect->name, (uint32_t) ELF_R_TYPE(rela->r_info));
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
  printf ("rtl: rel type record not supported; please report\n");
  return false;
}
