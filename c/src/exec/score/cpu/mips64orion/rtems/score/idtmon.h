/*

Based upon IDT provided code with the following release:

This source code has been made available to you by IDT on an AS-IS
basis. Anyone receiving this source is licensed under IDT copyrights
to use it in any way he or she deems fit, including copying it,
modifying it, compiling it, and redistributing it either with or
without modifications.  No license under IDT patents or patent
applications is to be implied by the copyright license.

Any user of this software should understand that IDT cannot provide
technical support for this software and will not be responsible for
any consequences resulting from the use of this software.

Any person who transfers this source code or any derivative work must
include the IDT copyright notice, this paragraph, and the preceeding
two paragraphs in the transferred software.

COPYRIGHT IDT CORPORATION 1996
LICENSED MATERIAL - PROGRAM PROPERTY OF IDT

  $Id$
*/ 

/*
**	idtmon.h - General header file for the IDT Prom Monitor
**
**	Copyright 1989  Integrated Device Technology, Inc.
**	All Rights Reserved.
**
**	June 1989 - D.Cahoon
*/
#ifndef __IDTMON_H__
#define __IDTMON_H__

/*
** P_STACKSIZE is the size of the Prom Stack. 
** the prom stack grows downward 
*/
#define	P_STACKSIZE	0x2000   /* sets stack size to 8k */

/*
** M_BUSWIDTH
** Memory bus width (including bank interleaving) in bytes
** used when doing memory sizing to prevent bus capacitance
** reporting ghost memory locations
*/
#if defined(CPU_R3000)
#define M_BUSWIDTH	8	/* 32bit memory bank interleaved */
#endif
#if defined(CPU_R4000)
#define M_BUSWIDTH	16	/* 64 bit memory bank interleaved */
#endif

/*
** this is the default value for the number of bytes to add in calculating
** the checksums in the checksum command
*/
#define CHK_SUM_CNT	0x20000	     /* number of bytes to calc chksum for */

/*
** Monitor modes
*/
#define	MODE_MONITOR	5	/* IDT Prom Monitor is executing */
#define	MODE_USER	0xa	/* USER is executing */

/*
** memory reference widths
*/
#define	SW_BYTE		1
#define	SW_HALFWORD	2
#define	SW_WORD		4
#define SW_TRIBYTEL	12
#define SW_TRIBYTER	20

#ifdef CPU_R4000
/*
** definitions for select_cache call
*/
#define DCACHE		0
#define ICACHE		1
#define SCACHE		2

#endif

#ifndef ASM
typedef struct {
	unsigned int	mem_size;
	unsigned int	icache_size;
	unsigned int	dcache_size;
#ifdef CPU_R4000
    unsigned int    scache_size;
#endif

	} mem_config;

#endif

/*
** general equates for diagnostics and boolean functions
*/
#define	PASS			0
#define	FAIL			1

#ifndef	TRUE
#define	TRUE			1
#endif	TRUE
#ifndef	NULL
#define	NULL			0
#endif	NULL

#ifndef	FALSE
#define	FALSE			0
#endif	FALSE


/*
**	portablility equates
*/

#ifndef	BOOL
#define BOOL	unsigned int
#endif	BOOL

#ifndef GLOBAL
#define GLOBAL	/**/
#endif  GLOBAL

#ifndef MLOCAL
#define MLOCAL	static
#endif  MLOCAL


#ifdef XDS
#define CONST const
#else
#define CONST
#endif XDS 

#define u_char	unsigned char
#define u_short unsigned short
#define u_int	unsigned int
/*
** assembly instructions for compatability between xds and mips
*/
#ifndef XDS
#define sllv sll
#define srlv srl
#endif XDS
/*
**	debugger macros for assembly language routines. Allows the 
**	programmer to set up the necessary stack frame info
**	required by debuggers to do stack traces.
*/

#ifndef XDS 
#define	FRAME(name,frm_reg,offset,ret_reg)	\
	.globl	name;				\
	.ent	name;				\
name:;						\
	.frame	frm_reg,offset,ret_reg
#define ENDFRAME(name) 				\
	.end name
#else
#define	FRAME(name,frm_reg,offset,ret_reg)	\
	.globl	_##name;\
_##name:
#define ENDFRAME(name)
#endif  XDS
#endif /* __IDTMON_H__ */
