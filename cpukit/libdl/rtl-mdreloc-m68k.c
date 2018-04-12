/*
 * Taken from NetBSD and stripped of the relocations not needed on RTEMS.
 */

/*	$NetBSD: mdreloc.c,v 1.26 2010/01/14 11:58:32 skrll Exp $	*/

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

static inline int overflow_8_check(int value)
{
  if ((value & 0xffffff00) && (~value & 0xffffff80))
    return true;
  return false;
}

static inline int overflow_16_check(int value)
{
  if ((value & 0xffff0000) && (~value & 0xffff8000))
    return true;
  return false;
}

uint32_t
rtems_rtl_elf_section_flags (const rtems_rtl_obj* obj,
                             const Elf_Shdr*      shdr)
{
  return 0;
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
                             const char*               symnane,
                             const Elf_Byte            syminfo,
                             const Elf_Word            symvalue)
{
  Elf_Addr  target = 0;
  Elf_Addr* where;
  Elf_Word  tmp;

  where = (Elf_Addr *)(sect->base + rela->r_offset);

  switch (ELF_R_TYPE(rela->r_info)) {
		case R_TYPE(NONE):
			break;

    case R_TYPE(PC8):
      tmp = symvalue + rela->r_addend - (Elf_Addr)where;
      if (overflow_8_check(tmp))
        return false;

      *(uint8_t *)where = tmp;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: reloc R_TYPE_8/PC8 in %s --> %p (%p) in %s\n",
                sect->name, (void*) (symvalue + rela->r_addend),
                (void *)*where, rtems_rtl_obj_oname (obj));
      break;

    case R_TYPE(PC16):
      tmp = symvalue + rela->r_addend - (Elf_Addr)where;
      if (overflow_16_check(tmp))
        return false;

      *(uint16_t*)where = tmp;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: reloc R_TYPE_16/PC16 in %s --> %p (%p) in %s\n",
                sect->name, (void*) (symvalue + rela->r_addend),
                (void *)*where, rtems_rtl_obj_oname (obj));
      break;
		case R_TYPE(PC32):
      target = (Elf_Addr) symvalue + rela->r_addend;
      *where += target - (Elf_Addr)where;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: reloc PC32 in %s --> %p (%p) in %s\n",
                sect->name, (void*) (symvalue + rela->r_addend),
                (void *)*where, rtems_rtl_obj_oname (obj));
      break;

		case R_TYPE(GOT32):
		case R_TYPE(32):
		case R_TYPE(GLOB_DAT):
      target = (Elf_Addr) symvalue + rela->r_addend;

			if (*where != target)
				*where = target;

      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: reloc 32/GLOB_DAT in %s --> %p in %s\n",
                sect->name, (void *)*where,
                rtems_rtl_obj_oname (obj));
			break;

		case R_TYPE(RELATIVE):
			*where += (Elf_Addr) sect->base + rela->r_addend;
      if (rtems_rtl_trace (RTEMS_RTL_TRACE_RELOC))
        printf ("rtl: reloc RELATIVE in %s --> %p\n",
                rtems_rtl_obj_oname (obj), (void *)*where);
			break;

		case R_TYPE(COPY):
			/*
			 * These are deferred until all other relocations have
			 * been done.  All we do here is make sure that the
			 * COPY relocation is not in a shared library.  They
			 * are allowed only in executable files.
			 */
      printf ("rtl: reloc COPY (please report)\n");
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
rtems_rtl_elf_relocate_rel (const rtems_rtl_obj*      obj,
                            const Elf_Rel*            rel,
                            const rtems_rtl_obj_sect* sect,
                            const char*               symname,
                            const Elf_Byte            syminfo,
                            const Elf_Word            symvalue)
{
  rtems_rtl_set_error (EINVAL, "rel type record not supported");
  return false;
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
