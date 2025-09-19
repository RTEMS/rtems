/*
 * Taken from NetBSD and stripped of the relocations not needed on RTEMS.
 */

/*  $NetBSD: mdreloc.c,v 1.33 2010/01/14 12:12:07 skrll Exp $  */

#include <sys/cdefs.h>

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <rtems/rtl/rtl.h>
#include "rtl-elf.h"
#include "rtl-error.h"
#include <rtems/rtl/rtl-trace.h>
#include "rtl-unwind-arm.h"

/*
 * Set to 1 to allow untested relocations. If you tested one and it
 * works or you fixed the relocation please remove the guard.
 */
#define ALLOW_UNTESTED_RELOCS 1

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

static void*
set_arm_veneer(void* trampoline, Elf_Addr target)
{
  /*
   *  ARM mode:
   *    ldr pc, [pc, #-4]
   */
  uint32_t* tramp = (uint32_t*) trampoline;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  *tramp++ = 0xe51ff004;
#else
  *tramp++ = 0xe51ff004; /* not tested */
#endif
  *tramp++ = (uint32_t) target;
  return tramp;
}

static void*
set_veneer(void* trampoline, Elf_Addr target)
{
  /*
   * http://shell-storm.org/online/Online-Assembler-and-Disassembler/
   *
   *  Thumb mode:
   *    ldr.w pc, [pc]
   *
   */
#if defined(__thumb__)
  uint32_t* tramp = (uint32_t*) trampoline;
 #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  *tramp++ = 0xf000f8df;
 #else
  *tramp++ = 0xf8dff000; /* not tested */
 #endif
  *tramp++ = (uint32_t) target;
  return tramp;
#else
  return set_arm_veneer(trampoline, target);
#endif
}

static void*
set_thumb_bx_veneer(void* trampoline, Elf_Addr target)
{
  /*
   * http://shell-storm.org/online/Online-Assembler-and-Disassembler/
   *
   *  Thumb mode:
   *    ldr.w r12, [pc, 4]
   *    bx r12
   *    .word target
   *
   */
  uint16_t* tramp = (uint16_t*) trampoline;
  /* ldr.w r12, [pc, 4]*/
  *tramp++ = 0xf8df;
  *tramp++ = 0xc002;
  /* bx r12 */
  *tramp++ = 0x4760;
  *tramp++ = (uint16_t) target & 0xffff;
  *tramp++ = (uint16_t) ((target >> 16) & 0xffff);
  return tramp;
}

static size_t
get_veneer_size(int type)
{
  (void) type;
  /* BX veneers need 10 bytes */
  return 10;
}

uint32_t rtems_rtl_obj_tramp_alignment (const rtems_rtl_obj* obj)
{
  (void) obj;
  return sizeof(uint32_t);
}

size_t
rtems_rtl_elf_relocate_tramp_max_size (void)
{
  return 8;
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

static rtems_rtl_elf_rel_status
rtems_rtl_elf_reloc_rel (rtems_rtl_obj*            obj,
                         const Elf_Rel*            rel,
                         const rtems_rtl_obj_sect* sect,
                         const char*               symname,
                         const Elf_Byte            syminfo,
                         const Elf_Word            symvalue,
                         const bool                parsing)
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
      if (parsing)
      {
        addend = 0;
      }
      else
      {
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
              return rtems_rtl_elf_rel_failure;
            }
          }
        }
      }

      if (parsing && sect->base == 0) {
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
          printf ("rtl: JUMP24/PC24/CALL tramp cache\n");
        return rtems_rtl_elf_rel_tramp_cache;
      }

      tmp = symvalue + (addend << 2) - (Elf_Addr)where;
      tmp = (Elf_Sword)tmp >> 2;

      if (((Elf_Sword)tmp > 0x7fffff) || ((Elf_Sword)tmp < -0x800000)) {
        Elf_Word tramp_addr;
        size_t   tramp_size = get_veneer_size(ELF_R_TYPE(rel->r_info));

        if (parsing) {
          if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
            printf ("rtl: JUMP24/PC24/CALL tramp add\n");
          return rtems_rtl_elf_rel_tramp_add;
        }

        if (!rtems_rtl_obj_has_tramp_space (obj, tramp_size)) {
          rtems_rtl_set_error (EINVAL,
                               "%s: CALL/JUMP24: overflow: no tramp memory",
                               sect->name);
          return rtems_rtl_elf_rel_failure;
        }

        /*
         * Align tramp_brk as necessary.
         */
        obj->tramp_brk = (void *)RTEMS_ALIGN_UP((uintptr_t)obj->tramp_brk, 4);
        tramp_addr = ((Elf_Addr) obj->tramp_brk) | (symvalue & 1);
        obj->tramp_brk = set_veneer(obj->tramp_brk, symvalue);

        tmp = tramp_addr + (addend << 2) - (Elf_Addr)where;
        tmp = (Elf_Sword)tmp >> 2;
      }

      if (!parsing) {
        *where = (*where & 0xff000000) | (tmp & 0xffffff);

        if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
          printf ("rtl: JUMP24/PC24/CALL %p @ %p in %s\n",
                  (void *)*where, where, rtems_rtl_obj_oname (obj));
      }
      break;

    case R_TYPE(V4BX):
      /* Miscellaneous, ignore */
      if (!parsing && rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)) {
        printf ("rtl: V4BX @ %p in %s\n",
                where, rtems_rtl_obj_oname (obj));
      }
      break;

    case R_TYPE(MOVT_ABS):
    case R_TYPE(MOVW_ABS_NC):
      if (!parsing) {
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
            return rtems_rtl_elf_rel_failure;
          }
        }

        *where = (insn & 0xfff0f000) | ((tmp & 0xf000) << 4) | (tmp & 0xfff);

        if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
          printf ("rtl: MOVT_ABS/MOVW_ABS_NC %p @ %p in %s\n",
                  (void *)*where, where, rtems_rtl_obj_oname (obj));
      }
      break;

    case R_TYPE(REL32):     /* word32 (S + A) | T - P */
    case R_TYPE(ABS32):     /* word32 (S + A) | T */
    case R_TYPE(GLOB_DAT):  /* word32 (S + A) | T */
    case R_TYPE(PREL31):    /* word32 (S + A) | T - P */
    case R_TYPE(TARGET1):   /* Equivalent to ABS32 */
    case R_TYPE(TARGET2):   /* Equivalent to REL32 */
      if (!parsing) {
        if (__predict_true(RELOC_ALIGNED_P(where))) {
          tmp = *where + symvalue;
          if (isThumb(symvalue))
            tmp |= 1;
          if (ELF_R_TYPE(rel->r_info) == R_TYPE(REL32) ||
              ELF_R_TYPE(rel->r_info) == R_TYPE(TARGET2))
            tmp -= (Elf_Addr)where;
          else if (ELF_R_TYPE(rel->r_info) == R_TYPE(PREL31))
            tmp = sign_extend31(tmp - (Elf_Addr)where);
          if (!parsing) {
            *where = tmp;
          }
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
      }
      break;

    case R_TYPE(THM_MOVT_ABS):
    case R_TYPE(THM_MOVW_ABS_NC):
      if (!parsing) {
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
      }
      break;

    case R_TYPE(THM_JUMP24):
      /* same as THM_PC22; insn b.w */
    case R_TYPE(THM_PC22):
      if (parsing)
      {
        addend = 0;
        upper_insn = 0;
        lower_insn = 0;
      }
      else
      {
        upper_insn = *(uint16_t *)where;
        lower_insn = *((uint16_t *)where + 1);
        sign = (upper_insn & (1 << 10)) >> 10;
        i1 = ((lower_insn >> 13) & 1) ^ sign ? 0 : 1;
        i2 = ((lower_insn >> 11) & 1) ^ sign ? 0 : 1;
        tmp = (i1 << 23) | (i2 << 22) | ((upper_insn & 0x3ff) << 12) | ((lower_insn & 0x7ff) << 1);
        addend = (tmp | ((sign ? 0 : 1) << 24)) - (1 << 24);
      }

      if (isThumb(symvalue) == false
        && ELF_R_TYPE(rel->r_info) != R_TYPE(THM_JUMP24)) {
        /* THM_CALL bl-->blx */
        lower_insn &= ~(1<<12);
      }

      if (parsing && sect->base == 0) {
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
          printf ("rtl: THM_CALL/JUMP24 tramp cache\n");
        return rtems_rtl_elf_rel_tramp_cache;
      }

      tmp = symvalue + addend;
      if (isThumb(symvalue)) {
        tmp = tmp - (Elf_Addr)where;
      } else {
        /*
         * where[1:0] are set to 0 for ARM-targeted relocations because the jump
         * offset[1:0] are also 0
         */
        tmp = tmp - (Elf_Addr)((uintptr_t)where & 0xfffffffc);
      }

      if (isThumb(symvalue) == false && ELF_R_TYPE(rel->r_info) == R_TYPE(THM_JUMP24)) {
        Elf_Word tramp_addr;
        size_t   tramp_size = get_veneer_size(ELF_R_TYPE(rel->r_info));

        if (parsing) {
          if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
            printf ("rtl: THM_JUMP24 tramp add: %08x - %p = %i\n",
                    symvalue + addend, where, (Elf_Sword) tmp);
          return rtems_rtl_elf_rel_tramp_add;
        }

        if (!rtems_rtl_obj_has_tramp_space (obj, tramp_size)) {
          rtems_rtl_set_error (EINVAL,
                               "%s: THM_JUMP24: overflow: no tramp memory",
                               sect->name);
          return rtems_rtl_elf_rel_failure;
        }

        tramp_addr = (Elf_Addr) obj->tramp_brk;
        obj->tramp_brk = set_thumb_bx_veneer(obj->tramp_brk, symvalue);


        tmp = tramp_addr + addend;
        /* This targets a thumb trampoline, so no need to adjust the PC */
        tmp = tmp - (Elf_Addr)where;
      } else if (((Elf_Sword)tmp > 0x7fffff) || ((Elf_Sword)tmp < -0x800000)) {
        Elf_Word tramp_addr;
        size_t   tramp_size = get_veneer_size(ELF_R_TYPE(rel->r_info));

        if (parsing) {
          if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
            printf ("rtl: THM_CALL/JUMP24 tramp add: %08x - %p = %i\n",
                    symvalue + addend, where, (Elf_Sword) tmp);
          return rtems_rtl_elf_rel_tramp_add;
        }

        if (!rtems_rtl_obj_has_tramp_space (obj, tramp_size)) {
          rtems_rtl_set_error (EINVAL,
                               "%s: THM_CALL/JUMP24: overflow: no tramp memory",
                               sect->name);
          return rtems_rtl_elf_rel_failure;
        }

        /*
         * Align tramp_brk as necessary.
         */
        obj->tramp_brk = (void *)RTEMS_ALIGN_UP((uintptr_t)obj->tramp_brk, 4);
        tramp_addr = ((Elf_Addr) obj->tramp_brk) | (symvalue & 1);
        tmp = tramp_addr + addend;
        if (isThumb(symvalue)) {
          obj->tramp_brk = set_veneer(obj->tramp_brk, symvalue);
          tmp = tmp - (Elf_Addr)where;
        } else {
          /*
           * The B[L]X expects ARM code at the target.
           */
          tramp_addr = (Elf_Addr) obj->tramp_brk;
          obj->tramp_brk = set_arm_veneer(obj->tramp_brk, symvalue);
          /*
           * where[1:0] are set to 0 for ARM-targeted relocations because the jump
           * offset[1:0] are also 0
           */
          tmp = tmp - (Elf_Addr)((uintptr_t)where & 0xfffffffc);
        }
      }

      if (!parsing) {
        uint16_t bl_tmp;

        sign = (tmp >> 24) & 1;
        *(uint16_t *)where = (uint16_t)((upper_insn & 0xf800) | (sign << 10) |
                                        ((tmp >> 12) & 0x3ff));

        bl_tmp = (uint16_t)((lower_insn & 0xd000)|
                           ((sign ^ (~(tmp >> 23) & 1)) << 13) |
                           ((sign ^ (~(tmp >> 22) & 1)) << 11) |
                           ((tmp >> 1) & 0x7ff));
        /* Thumb jumps to ARM mode must have 0 in the LSB of the BLX */
        if ((bl_tmp & 0xf000) == 0xe000) {
          bl_tmp &= 0xfffe;
        }
        *((uint16_t *)where + 1) = bl_tmp;

        if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC)){
          printf ("rtl: THM_CALL/JUMP24 %p @ %p in %s\n",
                  (void *)*where, where, rtems_rtl_obj_oname (obj));
        }
      }
      break;

    case R_TYPE(THM_JUMP19):

      if (!isThumb(symvalue)) {
        printf("THM_JUMP19 to arm not supported\n");
        return rtems_rtl_elf_rel_failure;
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
        rtems_rtl_set_error (EINVAL, "%s: Overflow %" PRIu32 " "
                             "THM_JUMP19 relocations",
                             sect->name, (uint32_t) ELF_R_TYPE(rel->r_info));
        return rtems_rtl_elf_rel_failure;
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

    case R_TYPE(TLS_LE32):
      if (!parsing) {
        addend = *where;
        *where = symvalue + addend;
        if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
          printf ("rtl: TLS_LE32 %p @ %p in %s\n",
                  (void *)*where, where, rtems_rtl_obj_oname (obj));
      }
      break;

    case R_TYPE(TLS_GD32):
    case R_TYPE(TLS_LDM32):
    case R_TYPE(TLS_LDO32):
    case R_TYPE(TLS_IE32):
    case R_TYPE(TLS_LDO12):
    case R_TYPE(TLS_LE12):
    case R_TYPE(TLS_IE12GP):
      printf("TSL relocations not supported\n");
      RTEMS_FALL_THROUGH();

    default:
      printf ("rtl: reloc unknown: sym = %" PRIu32 ", type = %" PRIu32 ", offset = %p",
              ELF_R_SYM(rel->r_info), (uint32_t) ELF_R_TYPE(rel->r_info),
                (void *)rel->r_offset);
      if (!parsing)
        printf("contents = %p", (void *)*where);
      printf("\n");
      rtems_rtl_set_error (EINVAL,
                           "%s: Unsupported relocation type %" PRIu32 " "
                           "in non-PLT relocations",
                           sect->name, (uint32_t) ELF_R_TYPE(rel->r_info));
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
  return rtems_rtl_elf_reloc_rel (obj,
                                  rel,
                                  sect,
                                  symname,
                                  syminfo,
                                  symvalue,
                                  true);
}

rtems_rtl_elf_rel_status
rtems_rtl_elf_relocate_rel (rtems_rtl_obj*            obj,
                            const Elf_Rel*            rel,
                            const rtems_rtl_obj_sect* sect,
                            const char*               symname,
                            const Elf_Byte            syminfo,
                            const Elf_Word            symvalue)
{
  return rtems_rtl_elf_reloc_rel (obj,
                                  rel,
                                  sect,
                                  symname,
                                  syminfo,
                                  symvalue,
                                  false);
}
