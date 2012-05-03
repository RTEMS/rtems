/*
 *  consoleIo.h  -- console I/O package interface
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __CONSOLE_IO_H
#define __CONSOLE_IO_H

typedef enum {
  CONSOLE_LOG 		= 1,
  CONSOLE_SERIAL       	= 2,
  CONSOLE_VGA		= 3,
  CONSOLE_VACUUM	= 4
}ioType;

typedef volatile unsigned char * __io_ptr;

typedef struct {
  __io_ptr io_base;
  __io_ptr isa_mem_base;
} board_memory_map;

extern board_memory_map *ptr_mem_map;

extern int select_console(ioType t);
/* extern int printk(const char *, ...) __attribute__((format(printf, 1, 2))); */
extern void debug_putc(const unsigned char c);
extern void debug_putc_onlcr(const char c);
extern int debug_getc(void);
extern int debug_tstc(void);
int kbdreset(void);

#endif
