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
 *  http://www.rtems.org/license/LICENSE.
 */
#ifndef _LIBCPU_VECTORS_H
#define _LIBCPU_VECTORS_H

#include <bsp/vectors.h>

/*
 * Size of hardware vector table.
 */
#define NUM_EXCEPTIONS		0x20

#ifndef ASM

/*
 * default raw exception handlers
 */

extern	void default_exception_vector_code_prolog(void);
extern	int  default_exception_vector_code_prolog_size;
extern  void initialize_exceptions(void);

typedef void rtems_exception_handler_t (CPU_Exception_frame* excPtr);

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

#endif /* ASM */

#endif /* _LIBCPU_VECTORS_H */
