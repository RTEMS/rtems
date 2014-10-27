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
#include "rtl-trace.h"

bool
rtems_rtl_elf_rel_resolve_sym (Elf_Word type)
{
  return true;
}


bool
rtems_rtl_elf_relocate_rela (rtems_rtl_obj_t*      obj,
                             const Elf_Rela*       rela,
                             rtems_rtl_obj_sect_t* sect,
                             Elf_Word              symvalue)
{

  return true;
}

bool
rtems_rtl_elf_relocate_rel (rtems_rtl_obj_t*      obj,
                            const Elf_Rel*        rel,
                            rtems_rtl_obj_sect_t* sect,
                            Elf_Word              symvalue)
{
  printf ("rtl: rel type record not supported; please report\n");
  return false;
}
