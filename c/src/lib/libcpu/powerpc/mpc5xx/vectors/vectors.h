/*
 * vectors.h Exception frame related contant and API.
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to handle exceptions.
 *
 *
 *  MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from libbsp/powerpc/mbx8xx/vectors/vectors.h:
 *
 *  CopyRight (C) 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef _LIBCPU_VECTORS_H
#define _LIBCPU_VECTORS_H


/*
 * Size of hardware vector table.
 */
#define NUM_EXCEPTIONS		0x20

/*
 * The callee (high level exception code written in C)
 * will store the Link Registers (return address) at entry r1 + 4 !!!.
 * So let room for it!!!.
 */
#define LINK_REGISTER_CALLEE_UPDATE_ROOM 4
#define SRR0_FRAME_OFFSET 8
#define SRR1_FRAME_OFFSET 12
#define EXCEPTION_NUMBER_OFFSET 16
#define EXC_CR_OFFSET 20
#define EXC_CTR_OFFSET 24
#define EXC_XER_OFFSET 28
#define EXC_LR_OFFSET 32
#define GPR0_OFFSET 36
#define GPR1_OFFSET 40
#define GPR2_OFFSET 44
#define GPR3_OFFSET 48
#define GPR4_OFFSET 52
#define GPR5_OFFSET 56
#define GPR6_OFFSET 60
#define GPR7_OFFSET 64
#define GPR8_OFFSET 68
#define GPR9_OFFSET 72
#define GPR10_OFFSET 76
#define GPR11_OFFSET 80
#define GPR12_OFFSET 84
#define GPR13_OFFSET 88
#define GPR14_OFFSET 92
#define GPR15_OFFSET 96
#define GPR16_OFFSET 100
#define GPR17_OFFSET 104
#define GPR18_OFFSET 108
#define GPR19_OFFSET 112
#define GPR20_OFFSET 116
#define GPR21_OFFSET 120
#define GPR22_OFFSET 124
#define GPR23_OFFSET 128
#define GPR24_OFFSET 132
#define GPR25_OFFSET 136
#define GPR26_OFFSET 140
#define GPR27_OFFSET 144
#define GPR28_OFFSET 148
#define GPR29_OFFSET 152
#define GPR30_OFFSET 156
#define GPR31_OFFSET 160
/*
 * maintain the EABI requested 8 bytes aligment
 * As SVR4 ABI requires 16, make it 16 (as some
 * exception may need more registers to be processed...)
 */
#define    EXCEPTION_FRAME_END 176

#ifndef ASM

#include <rtems.h>

/*
 * default raw exception handlers
 */

extern	void default_exception_vector_code_prolog();
extern	int  default_exception_vector_code_prolog_size;
extern  void initialize_exceptions();

typedef void rtems_exception_handler_t (CPU_Exception_frame* excPtr);
/*DEBUG typedef rtems_exception_handler_t cpuExcHandlerType; */

/*
 * Exception handler table.
 *
 * This table contains pointers to assembly-language exception handlers.
 * The common exception prologue in vectors.S looks up an entry in this
 * table and jumps to it.  No return address is saved, so the handlers in
 * this table must return directly to the interrupted code.
 *
 * On entry to an exception handler, R1 points to a new exception stack
 * frame in which R3, R4, and LR have been saved.  R4 holds the exception
 * number.
 */
extern rtems_exception_handler_t* exception_handler_table[NUM_EXCEPTIONS];

/* for compatability -- XXX remove */
typedef rtems_exception_handler_t *cpuExcHandlerType;
extern cpuExcHandlerType *globalExceptHdl;

#endif /* ASM */

#endif /* _LIBCPU_VECTORS_H */
