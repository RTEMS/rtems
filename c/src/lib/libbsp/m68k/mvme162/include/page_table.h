/*
 *  $Id$
 *
 *  This file was submitted by Eric Vaitl <vaitl@viasat.com> and 
 *  supports page table initialization.
 *
 *  $Log$
 * Revision 1.1  1995/12/05  15:29:19  joel
 * added.. submitted by  Eric Vaitl <vaitl@viasat.com>
 *
 * Revision 1.2  1995/10/27  21:00:33  vaitl
 * Modified page table routines so application code can map
 * VME space.
 *
 * Revision 1.1  1995/10/25  17:16:06  vaitl
 * Working on page table. Caching partially set up, but can't currently
 * set tc register.
 *
 */

#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

extern void page_table_teardown(void);
extern void page_table_init(void);
extern int page_table_map(void *addr, unsigned long size, int cache_type);

enum { 
    CACHE_WRITE_THROUGH, 
    CACHE_COPYBACK, 
    CACHE_NONE_SERIALIZED,
    CACHE_NONE 
};
enum { 
    PTM_SUCCESS, 
    PTM_BAD_ADDR, 
    PTM_BAD_SIZE, 
    PTM_BAD_CACHE,
    PTM_NO_TABLE_SPACE
};

#endif
