/* 
 * vectors.h Exception frame related contant and API.
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to handle exceptions.
 *
 *  CopyRight (C) 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
#ifndef LIBBSP_POWERPC_MBX8XX_VECTORS_H
#define LIBBSP_POWERPC_MBX8XX_VECTORS_H

/*
 * The callee (high level exception code written in C) 
 * will store the Link Registers (return address) at entry r1 + 4 !!!.
 * So let room for it!!!.
 */ 
#define LINK_REGISTER_CALLEE_UPDATE_ROOM 4
#define SRR0_FRAME_OFFSET 8
#define SRR1_FRAME_OFFSET 12
#define EXCEPTION_NUMBER_OFFSET 16
#define GPR0_OFFSET 20
#define GPR1_OFFSET 24
#define GPR2_OFFSET 28
#define GPR3_OFFSET 32
#define GPR4_OFFSET 36
#define GPR5_OFFSET 40
#define GPR6_OFFSET 44
#define GPR7_OFFSET 48
#define GPR8_OFFSET 52
#define GPR9_OFFSET 56
#define GPR10_OFFSET 60
#define GPR11_OFFSET 64
#define GPR12_OFFSET 68
#define GPR13_OFFSET 72
#define GPR14_OFFSET 76
#define GPR15_OFFSET 80
#define GPR16_OFFSET 84
#define GPR17_OFFSET 88
#define GPR18_OFFSET 92
#define GPR19_OFFSET 96
#define GPR20_OFFSET 100
#define GPR21_OFFSET 104
#define GPR22_OFFSET 108
#define GPR23_OFFSET 112
#define GPR24_OFFSET 116
#define GPR25_OFFSET 120
#define GPR26_OFFSET 124
#define GPR27_OFFSET 128
#define GPR28_OFFSET 132
#define GPR29_OFFSET 136
#define GPR30_OFFSET 140
#define GPR31_OFFSET 144
#define EXC_CR_OFFSET 148
#define EXC_CTR_OFFSET 152
#define EXC_XER_OFFSET 156
#define EXC_LR_OFFSET 160
#define EXC_DAR_OFFSET 164
/*
 * maintain the EABI requested 8 bytes aligment
 * As SVR4 ABI requires 16, make it 16 (as some
 * exception may need more registers to be processed...)
 */
#define    EXCEPTION_FRAME_END 176

#ifndef ASM
/*
 * default raw exception handlers
 */

extern	void default_exception_vector_code_prolog();
extern	int  default_exception_vector_code_prolog_size;

/* codemove is like memmove, but it also gets the cache line size
 * as 4th parameter to synchronize them. If this last parameter is
 * zero, it performs more or less like memmove. No copy is performed if
 * source and destination addresses are equal. However the caches
 * are synchronized. Note that the size is always rounded up to the
 * next mutiple of 4. 
 */
extern void * codemove(void *, const void *, unsigned int, unsigned long);
extern void initialize_exceptions();

typedef struct {
  unsigned 	EXC_SRR0;
  unsigned 	EXC_SRR1;
  unsigned	_EXC_number;
  unsigned	GPR0;
  unsigned	GPR1;
  unsigned	GPR2;
  unsigned	GPR3;
  unsigned	GPR4;
  unsigned	GPR5;
  unsigned	GPR6;
  unsigned	GPR7;
  unsigned	GPR8;
  unsigned	GPR9;
  unsigned	GPR10;
  unsigned	GPR11;
  unsigned	GPR12;
  unsigned	GPR13;
  unsigned	GPR14;
  unsigned	GPR15;
  unsigned	GPR16;
  unsigned	GPR17;
  unsigned	GPR18;
  unsigned	GPR19;
  unsigned	GPR20;
  unsigned	GPR21;
  unsigned	GPR22;
  unsigned	GPR23;
  unsigned	GPR24;
  unsigned	GPR25;
  unsigned	GPR26;
  unsigned	GPR27;
  unsigned	GPR28;
  unsigned	GPR29;
  unsigned	GPR30;
  unsigned	GPR31;
  unsigned 	EXC_CR;
  unsigned 	EXC_CTR;
  unsigned 	EXC_XER;
  unsigned	EXC_LR;
  unsigned 	EXC_MSR;
  unsigned	EXC_DAR;
}BSP_Exception_frame;


typedef void (*exception_handler_t) (BSP_Exception_frame* excPtr);
extern exception_handler_t globalExceptHdl;
/*
 * Compatibility with pc386
 */
typedef BSP_Exception_frame CPU_Exception_frame;
typedef exception_handler_t cpuExcHandlerType;

#endif /* ASM */

#endif /* LIBBSP_POWERPC_MCP750_VECTORS_H */
