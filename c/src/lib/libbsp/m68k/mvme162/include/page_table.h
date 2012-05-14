/*
 *  This file was submitted by Eric Vaitl <vaitl@viasat.com> and
 *  supports page table initialization.
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
