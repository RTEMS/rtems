/*  page_table.h
 *
 *  This file was submitted by Eric Vaitl <vaitl@viasat.com> and
 *  supports page table initialization.
 *
 *  For now, we only use the transparent translation registers. Page tables
 *  may be set up in the future.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modifications of respective RTEMS files:
 *  Copyright (c) 1998, National Research Council of Canada
 */

#ifndef __PAGE_TABLE_H
#define __PAGE_TABLE_H

#include <rtems.h>

void page_table_teardown( void );
void page_table_init( rtems_configuration_table *config_table );

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
