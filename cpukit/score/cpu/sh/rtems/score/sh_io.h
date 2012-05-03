/**
 * @file rtems/score/sh_io.h
 */

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
 */

#ifndef _RTEMS_SCORE_SH_IO_H
#define _RTEMS_SCORE_SH_IO_H

#define readb(addr) 	(*(volatile unsigned char *) (addr))
#define readw(addr) 	(*(volatile unsigned short *) (addr))
#define readl(addr) 	(*(volatile unsigned int *) (addr))
#define read8(addr)     (*(volatile uint8_t   *) (addr))
#define read16(addr)    (*(volatile uint16_t   *) (addr))
#define read32(addr)    (*(volatile uint32_t   *) (addr))

#define writeb(b,addr) 	((*(volatile unsigned char *) (addr)) = (b))
#define writew(b,addr) 	((*(volatile unsigned short *) (addr)) = (b))
#define writel(b,addr) 	((*(volatile unsigned int *) (addr)) = (b))
#define write8(b,addr) 	((*(volatile uint8_t    *) (addr)) = (b))
#define write16(b,addr) ((*(volatile uint16_t   *) (addr)) = (b))
#define write32(b,addr) ((*(volatile uint32_t   *) (addr)) = (b))

#define inb(addr)	readb(addr)
#define outb(b,addr)	writeb(b,addr)

#endif
