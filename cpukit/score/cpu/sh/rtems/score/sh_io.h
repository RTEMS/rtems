/*
 * These are some macros to access memory mapped devices
 * on the SH7000-architecture.
 *
 * Inspired from the linux kernel's include/asm/io.h
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1996-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 *
 *  COPYRIGHT (c) 1998-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _asm_io_h
#define _asm_io_h
 
#define readb(addr) 	(*(volatile unsigned char *) (addr))
#define readw(addr) 	(*(volatile unsigned short *) (addr))
#define readl(addr) 	(*(volatile unsigned int *) (addr))
#define read8(addr)     (*(volatile unsigned8 *) (addr))
#define read16(addr)    (*(volatile unsigned16 *) (addr))
#define read32(addr)    (*(volatile unsigned32 *) (addr))

#define writeb(b,addr) 	((*(volatile unsigned char *) (addr)) = (b))
#define writew(b,addr) 	((*(volatile unsigned short *) (addr)) = (b))
#define writel(b,addr) 	((*(volatile unsigned int *) (addr)) = (b))
#define write8(b,addr) 	((*(volatile unsigned8  *) (addr)) = (b))
#define write16(b,addr) ((*(volatile unsigned16 *) (addr)) = (b))
#define write32(b,addr) ((*(volatile unsigned32 *) (addr)) = (b))

#define inb(addr)	readb(addr)
#define outb(b,addr)	writeb(b,addr)

#endif
