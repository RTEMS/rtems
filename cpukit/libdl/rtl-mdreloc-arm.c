/*
 * Taken from NetBSD and stripped of the relocations not needed on RTEMS.
 */

/*  $NetBSD: mdreloc.c,v 1.33 2010/01/14 12:12:07 skrll Exp $  */

#include <sys/cdefs.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unwind.h>
#include <unwind-arm-common.h>

#include <rtems/rtl/rtl.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include <rtems/rtl/rtl-trace.h>
#include "rtl-unwind.h"

/*
 * It is possible for the compiler to emit relocations for unaligned data.
 * We handle this situation with these inlines.
 */
#define	RELOC_ALIGNED_P(x) \
	(((uintptr_t)(x) & (sizeof(void *) - 1)) == 0)

#define SHT_ARM_EXIDX  0x70000001 /* Section holds ARM unwind info. */

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

static inline Elf_SOff
sign_extend31(Elf_Addr val)
{
  if (0x40000000 & val)
    val =  ~((Elf_Addr)0x7fffffff) | (0x7fffffff & val);
  return 0x7fffffff & val;
}

uint32_t
rtems_rtl_elf_section_flags (const rtems_rtl_obj* obj,
                             const Elf_Shdr*      shdr)
{
  uint32_t flags = 0;
  if (shdr->sh_type == SHT_ARM_EXIDX)
    flags = RTEMS_RTL_OBJ_SECT_EH | RTEMS_RTL_OBJ_SECT_LOAD;
  return flags;
}

bool
rtems_rtl_elf_rel_resolve_sym (Elf_Word type)
{
  return true;
}

bool
rtems_rtl_elf_relocate_rela (const rtems_rtl_obj*      obj,
                             const Elf_Rela*           rela,
                             const rtems_rtl_obj_sect* sect,
                             const char*               symname,
                             const Elf_Byte            syminfo,
                             const Elf_Word            symvalue)
{
  rtems_rtl_set_error (EINVAL, "rela type record not supported");
  return false;
}

bool
rtems_rtl_elf_relocate_rel (const rtems_rtl_obj*      obj,
                            const Elf_Rel*            rel,
                            const rtems_rtl_obj_sect* sect,
                            const char*               symname,
                            const Elf_Byte            syminfo,
                            const Elf_Word            symvalue)
{
  Elf_Addr *where;
  Elf_Addr tmp;
  Elf_Word insn, addend;
  Elf_Word sign, i1, i2;
  uint16_t lower_insn, upper_insn;

  where = (Elf_Addr *)(sect->base + rel->r_offset);

  switch (ELF_R_TYPE(rel->r_info)) {
    case R_TYPE(NONE):
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
        printf ("rtl: NONE %p in %s\n", where, rtems_rtl_obj_oname (obj));
      }
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
    case R_TYPE(PREL31):    /* word32 (S + A) | T - P */
    case R_TYPE(TARGET2):   /* Equivalent to REL32 */
      if (__predict_true(RELOC_ALIGNED_P(where))) {
        tmp = *where + symvalue;
        if (isThumb(symvalue))
          tmp |= 1;
        if (ELF_R_TYPE(rel->r_info) == R_TYPE(REL32) ||
            ELF_R_TYPE(rel->r_info) == R_TYPE(TARGET2))
          tmp -= (Elf_Addr)where;
        else if (ELF_R_TYPE(rel->r_info) == R_TYPE(PREL31))
          tmp = sign_extend31(tmp - (Elf_Addr)where);
        *where = tmp;
      } else {
        tmp = load_ptr(where) + symvalue;
        if (isThumb(symvalue))
          tmp |= 1;
        if (ELF_R_TYPE(rel->r_info) == R_TYPE(REL32) ||
            ELF_R_TYPE(rel->r_info) == R_TYPE(TARGET2))
          tmp -= (Elf_Addr)where;
        else if (ELF_R_TYPE(rel->r_info) == R_TYPE(PREL31))
          tmp = sign_extend31(tmp - (Elf_Addr)where);
        store_ptr(where, tmp);
      }

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: REL32/ABS32/GLOB_DAT/PREL31/TARGET2 %p @ %p in %s\n",
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

bool
rtems_rtl_elf_unwind_parse (const rtems_rtl_obj* obj,
                            const char*          name,
                            uint32_t             flags)
{
  /*
   * We location the EH sections in section flags.
   */
  return false;
}

bool
rtems_rtl_elf_unwind_register (rtems_rtl_obj* obj)
{
  return true;
}

bool
rtems_rtl_elf_unwind_deregister (rtems_rtl_obj* obj)
{
  obj->loader = NULL;
  return true;
}

/* An exception index table entry.  */
typedef struct __EIT_entry
{
  _uw fnoffset;
  _uw content;
} __EIT_entry;

/* The exception index table location in the base module */
extern __EIT_entry __exidx_start;
extern __EIT_entry __exidx_end;

/*
 * A weak reference is in libgcc, provide a real version and provide a way to
 * manage loaded modules.
 *
 * Passed in the return address and a reference to the number of records
 * found. We set the start of the exidx data and the number of records.
 */
_Unwind_Ptr __gnu_Unwind_Find_exidx (_Unwind_Ptr return_address,
                                     int*        nrec) __attribute__ ((__noinline__,
                                                                       __used__,
                                                                       __noclone__));

_Unwind_Ptr __gnu_Unwind_Find_exidx (_Unwind_Ptr return_address,
                                     int*        nrec)
{
  rtems_rtl_data*   rtl;
  rtems_chain_node* node;
  __EIT_entry*      exidx_start = &__exidx_start;
  __EIT_entry*      exidx_end = &__exidx_end;

  rtl = rtems_rtl_lock ();

  node = rtems_chain_first (&rtl->objects);
  while (!rtems_chain_is_tail (&rtl->objects, node)) {
    rtems_rtl_obj* obj = (rtems_rtl_obj*) node;
    if (rtems_rtl_obj_text_inside (obj, (void*) return_address)) {
      exidx_start = (__EIT_entry*) obj->eh_base;
      exidx_end = (__EIT_entry*) (obj->eh_base + obj->eh_size);
      break;
    }
    node = rtems_chain_next (node);
  }

  rtems_rtl_unlock ();

  *nrec = exidx_end - exidx_start;

  return (_Unwind_Ptr) exidx_start;
}
