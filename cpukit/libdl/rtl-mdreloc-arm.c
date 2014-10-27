/*
 * Taken from NetBSD and stripped of the relocations not needed on RTEMS.
 */

/*  $NetBSD: mdreloc.c,v 1.33 2010/01/14 12:12:07 skrll Exp $  */

#include <sys/cdefs.h>

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <rtems/rtl/rtl.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include "rtl-trace.h"

/*
 * It is possible for the compiler to emit relocations for unaligned data.
 * We handle this situation with these inlines.
 */
#define	RELOC_ALIGNED_P(x) \
	(((uintptr_t)(x) & (sizeof(void *) - 1)) == 0)

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

/*
 * The address of Thumb function symbols is it's real address plus one.
 * This is done by compiler, thus do not consider symtype here.
 */
static inline int
isThumb(Elf_Word symvalue)
{
  if ((symvalue & 0x1) == 0x1)
    return true;
  else return false;
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
  rtems_rtl_set_error (EINVAL, "rela type record not supported");
  return false;
}

bool
rtems_rtl_elf_relocate_rel (const rtems_rtl_obj_t*      obj,
                            const Elf_Rel*              rel,
                            const rtems_rtl_obj_sect_t* sect,
                            const char*                 symname,
                            const Elf_Byte              syminfo,
                            const Elf_Word              symvalue)
{
  Elf_Addr *where;
  Elf_Addr tmp;
  Elf_Word insn, addend;
  Elf_Word sign, i1, i2;
  uint16_t lower_insn, upper_insn;

  where = (Elf_Addr *)(sect->base + rel->r_offset);

  switch (ELF_R_TYPE(rel->r_info)) {
		case R_TYPE(NONE):
			break;

    case R_TYPE(CALL):    /* BL/BLX */
    case R_TYPE(JUMP24):  /* B/BL<cond> */
      insn = *where;

      if (insn & 0x00800000)
        addend = insn | 0xff000000;
      else addend = insn & 0x00ffffff;

      if (isThumb(symvalue)) {
        if ((insn & 0xfe000000) == 0xfa000000);         /* Already blx */
        else {
          if ((insn & 0xff000000) == 0xeb000000) {      /* BL <label> */
            *where = (insn & 0x00ffffff) | 0xfa000000;  /* BL-->BLX */
          } else {
            printf("JUMP24 is not suppored from arm to thumb\n");
            return false;
          }
        }
      }

      tmp = symvalue + (addend << 2) - (Elf_Addr)where;
      tmp = (Elf_Sword)tmp >> 2;

      if (((Elf_Sword)tmp > 0x7fffff) || ((Elf_Sword)tmp < -0x800000)) {
        printf("CALL/JUMP24 Overflow\n");
        return false;
      }

      *where = (*where & 0xff000000) | (tmp & 0xffffff);

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: JUMP24/PC24/CALL %p @ %p in %s\n",
                (void *)*where, where, rtems_rtl_obj_oname (obj));

      break;

    case R_TYPE(V4BX):
      /* Miscellaneous, ignore */
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
        printf ("rtl: V4BX %p @ %p in %s\n",
                (void *)*where, where, rtems_rtl_obj_oname (obj));
      }
      break;

    case R_TYPE(MOVT_ABS):
    case R_TYPE(MOVW_ABS_NC):
      insn = *where;

      addend = ((insn >> 4) & 0xf000) | (insn & 0x0fff);
      if (addend & 0x8000)
        addend |= 0xffff0000;

      tmp = symvalue + addend;

      if (ELF_R_TYPE(rel->r_info) == R_TYPE(MOVW_ABS_NC))
        tmp &= 0xffff;
      else {
        tmp = (Elf_Sword)tmp >> 16;
        if (((Elf_Sword)tmp > 0x7fff) || ((Elf_Sword)tmp < -0x8000)) {
          printf("MOVT_ABS Overflow\n");
          return false;
        }
      }

      *where = (insn & 0xfff0f000) | ((tmp & 0xf000) << 4) | (tmp & 0xfff);

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: MOVT_ABS/MOVW_ABS_NC %p @ %p in %s\n",
                (void *)*where, where, rtems_rtl_obj_oname (obj));
      break;


    case R_TYPE(REL32):     /* word32 (S + A) | T - P */
    case R_TYPE(ABS32):     /* word32 (S + A) | T */
    case R_TYPE(GLOB_DAT):  /* word32 (S + A) | T */
      if (__predict_true(RELOC_ALIGNED_P(where))) {
        tmp = *where + symvalue;
        if (isThumb(symvalue))
          tmp |= 1;
        if (ELF_R_TYPE(rel->r_info) == R_TYPE(REL32))
          tmp -= (Elf_Addr)where;
        *where = tmp;
      } else {
        tmp = load_ptr(where) + symvalue;
        if (isThumb(symvalue))
          tmp |= 1;
        if (ELF_R_TYPE(rel->r_info) == R_TYPE(REL32))
          tmp -= (Elf_Addr)where;
        store_ptr(where, tmp);
      }

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: REL32/ABS32/GLOB_DAT %p @ %p in %s",
                (void *)tmp, where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(THM_MOVT_ABS):
    case R_TYPE(THM_MOVW_ABS_NC):
      upper_insn = *(uint16_t *)where;
      lower_insn = *((uint16_t *)where + 1);

      addend = ((upper_insn & 0x000f) << 12) | ((upper_insn & 0x0400) << 1) |
               ((lower_insn & 0x7000) >> 4) | (lower_insn & 0x00ff);
      addend = (addend ^ 0x8000) - 0x8000;

      tmp = addend + symvalue;
      if (ELF32_R_TYPE(rel->r_info) == R_ARM_THM_MOVT_ABS)
        tmp >>= 16;

      *(uint16_t *)where = (uint16_t)((upper_insn & 0xfbf0) |
                                     ((tmp & 0xf000) >> 12) |
                                     ((tmp & 0x0800) >> 1));
      *((uint16_t *)where + 1) = (uint16_t)((lower_insn & 0x8f00) |
                                           ((tmp & 0x0700) << 4) |
                                           (tmp & 0x00ff));

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
        printf ("rtl: THM_MOVT_ABS/THM_MOVW_ABS_NC %p @ %p in %s\n",
                (void *)*where, where, rtems_rtl_obj_oname (obj));
      }

      break;

    case R_TYPE(THM_JUMP24):
      /* same to THM_CALL; insn b.w */
    case R_TYPE(THM_CALL):
      upper_insn = *(uint16_t *)where;
      lower_insn = *((uint16_t *)where + 1);
      sign = (upper_insn & (1 << 10)) >> 10;
      i1 = ((lower_insn >> 13) & 1) ^ sign ? 0 : 1;
      i2 = ((lower_insn >> 11) & 1) ^ sign ? 0 : 1;
      tmp = (i1 << 23) | (i2 << 22) | ((upper_insn & 0x3ff) << 12) | ((lower_insn & 0x7ff) << 1);
      addend = (tmp | ((sign ? 0 : 1) << 24)) - (1 << 24);

      if (isThumb(symvalue)) ;/*Thumb to Thumb call, nothing to care */
      else {
        if (ELF_R_TYPE(rel->r_info) == R_TYPE(THM_JUMP24)) {
          tmp = (tmp + 2) & ~3; /* aligned to 4 bytes only for JUMP24 */
          printf("THM_JUMP24 to arm not supported\n");
          return false;
        }
        else {
          /* THM_CALL bl-->blx */
          lower_insn &=~(1<<12);
        }
      }

      tmp = symvalue + addend;
      tmp = tmp - (Elf_Addr)where;

      if (((Elf_Sword)tmp > 0x7fffff) || ((Elf_Sword)tmp < -0x800000)) {
        printf("THM_CALL/JUMP24 overflow\n");
        return false;
      }

      sign = (tmp >> 24) & 1;
      *(uint16_t *)where = (uint16_t)((upper_insn & 0xf800) | (sign << 10) |
                                     ((tmp >> 12) & 0x3ff));

      *((uint16_t *)where + 1) = (uint16_t)((lower_insn & 0xd000)|
                                           ((sign ^ (~(tmp >> 23) & 1)) << 13) |
                                           ((sign ^ (~(tmp >> 22) & 1)) << 11) |
                                           ((tmp >> 1) & 0x7ff));

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)){
        printf ("rtl: THM_CALL/JUMP24 %p @ %p in %s\n",
                (void *)*where, where, rtems_rtl_obj_oname (obj));
      }

      break;

    case R_TYPE(THM_JUMP19):

      if (!isThumb(symvalue)) {
        printf("THM_JUMP19 to arm not supported\n");
        return false;
      }

      upper_insn = *(uint16_t *)where;
      lower_insn = *((uint16_t *)where + 1);
      sign = (upper_insn >> 10) & 0x1;

      if ((((upper_insn & 0x3f) >> 7) & 0x7) == 0x7) {
        printf("THM_JUMP19 failed\n");
        return false; /*if cond <3:1> == '111', see Related codings in armv7a manual */
      }

      i1 = (lower_insn >> 13) & 0x1;
      i2 = (lower_insn >> 11) & 0x1;

      tmp = ((i2 << 19) | (i1 << 18) | ((upper_insn & 0x3f) << 12) | ((lower_insn & 0x7ff) << 1));
      addend = (tmp | ((sign ? 0 : 1) << 20)) - (1 << 20);
      tmp = symvalue + addend;

      tmp = tmp - (Elf_Addr)where;

      if (((Elf_Sword)tmp > 0x7ffffe) || ((Elf_Sword)tmp < -0x800000)) {
        rtems_rtl_set_error (EINVAL, "%s: Overflow %ld "
                             "THM_JUMP19 relocations",
                             sect->name, (uint32_t) ELF_R_TYPE(rel->r_info));
        return false;
      }

      sign = (tmp >> 20) & 0x1;
      i2 = (tmp >> 19) & 0x1;
      i1 = (tmp >> 18) & 0x1;

      *(uint16_t*)where = (upper_insn & 0xfbc0) | (sign << 10) | ((tmp >> 12) & 0x3f);
      *((uint16_t*)where + 1) = (lower_insn & 0xd000) | (i1 << 13) |
                                (i2 << 11) | ((tmp >> 1) & 0x7ff);

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: THM_JUMP19 %p @ %p in %s\n",
                (void *)*where, where, rtems_rtl_obj_oname (obj));
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
			return false;
  }

	return true;
}

