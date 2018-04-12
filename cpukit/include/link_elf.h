/*	$NetBSD: link_elf.h,v 1.8 2009/11/04 19:28:03 pooka Exp $	*/

/*
 * This only exists for GDB.
 */

#ifndef _LINK_ELF_H_
#define	_LINK_ELF_H_

#include <sys/types.h>

#include <machine/elf_machdep.h>
#include <stdint.h>
#include <rtems/rtl/rtl-obj-fwd.h>

enum sections
{
  rap_text = 0,
  rap_const = 1,
  rap_ctor = 2,
  rap_dtor = 3,
  rap_data = 4,
  rap_bss = 5,
  rap_secs = 6
};

/**
 * Object details.
 */
typedef struct
{
  const char* name;   /**< Section name. */
  uint32_t    offset; /**< The offset in the elf file. */
  uint32_t    size;   /**< The size of the section. */
  uint32_t    rap_id; /**< Which obj does this section belongs to. */
}section_detail;

/**
 * link map structure will be used for GDB support.
 */
struct link_map {
  const char*       name;                 /**< Name of the obj. */
  uint32_t          sec_num;              /**< The count of section. */
  section_detail*   sec_detail;           /**< The section details. */
  uint32_t*         sec_addr[rap_secs];   /**< The RAP section addr. */
  uint32_t          rpathlen;             /**< The length of the path. */
  char*             rpath;                /**< The path of object files. */
  struct link_map*  l_next;               /**< Linked list of mapped libs. */
  struct link_map*  l_prev;
};

/**
 * r_debug is used to manage the debug related structures.
 */
struct r_debug {
	int r_version;			      /* not used */
	struct link_map *r_map;		/* list of loaded images */
	enum {
		RT_CONSISTENT,		      /* things are stable */
		RT_ADD,			            /* adding a shared library */
		RT_DELETE		            /* removing a shared library */
	} r_state;
};

/*
 * stub function. It is empty.
 */
void _rtld_debug_state (void);

/*
 * add link map to the list.
 */
int _rtld_linkmap_add (rtems_rtl_obj* obj);

/*
 * Remove link map from the list.
 */
void _rtld_linkmap_delete (rtems_rtl_obj* obj);
#endif	/* _LINK_ELF_H_ */
