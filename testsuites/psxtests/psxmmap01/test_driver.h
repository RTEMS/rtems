/*
 * Copyright (c) 2017 Kevin Kirspel.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/imfs.h>
#include <rtems/libcsupport.h>

extern const IMFS_node_control node_control;
extern char test_data[PAGE_SIZE];
