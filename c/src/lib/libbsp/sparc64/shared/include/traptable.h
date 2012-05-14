/*
 * traptable.h
 *
 * Copyright 2010 Gedare Bloom
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/* This file can be included by assembly code */

#define ENTRY_SIZE (32)

#define TABLE_SIZE (1024*ENTRY_SIZE)

#ifndef ASM
extern void* real_trap_table;
extern void* trap_table[TABLE_SIZE];
#endif

