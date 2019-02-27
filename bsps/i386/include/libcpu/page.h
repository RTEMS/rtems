/*
 * @file
 *
 * Copyright (C) 1998  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _LIBCPU_i386_PAGE_H
#define _LIBCPU_i386_PAGE_H

#ifndef ASM

#include <rtems/score/cpu.h>

/* C declaration for paging management */

extern int  	_CPU_is_cache_enabled(void);
extern int  	_CPU_is_paging_enabled(void);
extern int 	init_paging(void);
extern void 	_CPU_enable_paging(void);
extern void 	_CPU_disable_paging(void);
extern void 	_CPU_disable_cache(void);
extern void 	_CPU_enable_cache(void);
extern int 	_CPU_map_phys_address
                      (void **mappedAddress, void *physAddress,
		       int size, int flag);
extern int 	_CPU_unmap_virt_address (void *mappedAddress, int size);
extern int 	_CPU_change_memory_mapping_attribute
                         (void **newAddress, void *mappedAddress,
			  unsigned int size, unsigned int flag);
extern int  	_CPU_display_memory_attribute(void);

# endif /* ASM */

#endif
