/*
 **************************************************************************
 *
 * Component =   rdblib
 * 
 * Synopsis  =   remdeb_f.x
 *
 *
 *  Copyright(C) 2000 Canon Research Center France SA.
 *  
 *  Developped by : 
 *	Eric Valette, 		mail to : valette@crf.canon.fr
 *	Emmanuel Raguet,  	mail to : raguet@crf.canon.fr
 *
 * $Header$
 *
 **************************************************************************
 */
/* Basic, host-specific, and target-specific definitions for GDB.
   Copyright (C) 1986, 1989, 1991, 1992, 1993, 1994, 1995
   Free Software Foundation, Inc.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

struct xdr_regs {
        int     r_dreg[8];      /* data registers */
        int     r_areg[8];      /* address registers */
        int     r_sr;           /* status register (actually a short) */
        int     r_pc;           /* program counter */
	int	r_vec;		/* last vector */
};

#ifdef RPC_HDR

%/* now define register macros to apply to xdr_reg struct */
%

%#define r_r0   r_dreg[0]       /* r0 for portability */
%#define r_sp   r_areg[7]       /* user stack pointer */
%#define r_ps   r_sr

%#define REG_PC r_pc      /* PC register offset */
%#define REG_SP r_areg[7] /* SP register offset */
%#define REG_FP r_areg[6] /* FP register offset */

%/* now define the BREAKPOINT mask technique to a long word */
%#define SET_BREAK(l)   ((l&0x0000FFFF) | 0x4E4F0000)  /* TRAP 15 */
%#define IS_BREAK(l)    (((l) & 0xFFFF0000) == 0x4E4F0000)
%#define ORG_BREAK(c,p) (((c) & 0x0000FFFF) | ((p) & 0xFFFF0000))
%#define IS_STEP(regs)  ((regs).r_vec == 9)
%#define BREAK_ADJ      0
%#define BREAK_SIZE     2

%#define TARGET_PROC_TYPE  2

#endif
