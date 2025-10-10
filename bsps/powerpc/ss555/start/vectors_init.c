/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 * vectors_init.c Exception hanlding initialisation (and generic handler).
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to handle exceptions.
 */

/*
 *  MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from libbsp/powerpc/mbx8xx/vectors/vectors_init.c:
 *
 *  CopyRight (C) 1999 eric.valette@free.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
#include <inttypes.h>
#include <rtems/bspIo.h>
#include <libcpu/vectors.h>
#include <libcpu/raw_exception.h>
#include <bsp/irq.h>

extern rtems_exception_handler_t default_exception_handler;

static rtems_raw_except_global_settings exception_config;
static rtems_raw_except_connect_data    exception_table[NUM_EXCEPTIONS];
rtems_exception_handler_t* exception_handler_table[NUM_EXCEPTIONS];

void C_default_exception_handler(CPU_Exception_frame* excPtr)
{
  int recoverable = 0;

  printk("exception handler called for exception %" PRIu32 "\n",
         excPtr->_EXC_number);
  printk("\t Next PC or Address of fault = %" PRIxPTR "\n", excPtr->EXC_SRR0);
  printk("\t Saved MSR = %" PRIxPTR "\n", excPtr->EXC_SRR1);
  printk("\t R0 = %" PRIxPTR "\n", excPtr->GPR0);
  printk("\t R1 = %" PRIxPTR "\n", excPtr->GPR1);
  printk("\t R2 = %" PRIxPTR "\n", excPtr->GPR2);
  printk("\t R3 = %" PRIxPTR "\n", excPtr->GPR3);
  printk("\t R4 = %" PRIxPTR "\n", excPtr->GPR4);
  printk("\t R5 = %" PRIxPTR "\n", excPtr->GPR5);
  printk("\t R6 = %" PRIxPTR "\n", excPtr->GPR6);
  printk("\t R7 = %" PRIxPTR "\n", excPtr->GPR7);
  printk("\t R8 = %" PRIxPTR "\n", excPtr->GPR8);
  printk("\t R9 = %" PRIxPTR "\n", excPtr->GPR9);
  printk("\t R10 = %" PRIxPTR "\n", excPtr->GPR10);
  printk("\t R11 = %" PRIxPTR "\n", excPtr->GPR11);
  printk("\t R12 = %" PRIxPTR "\n", excPtr->GPR12);
  printk("\t R13 = %" PRIxPTR "\n", excPtr->GPR13);
  printk("\t R14 = %" PRIxPTR "\n", excPtr->GPR14);
  printk("\t R15 = %" PRIxPTR "\n", excPtr->GPR15);
  printk("\t R16 = %" PRIxPTR "\n", excPtr->GPR16);
  printk("\t R17 = %" PRIxPTR "\n", excPtr->GPR17);
  printk("\t R18 = %" PRIxPTR "\n", excPtr->GPR18);
  printk("\t R19 = %" PRIxPTR "\n", excPtr->GPR19);
  printk("\t R20 = %" PRIxPTR "\n", excPtr->GPR20);
  printk("\t R21 = %" PRIxPTR "\n", excPtr->GPR21);
  printk("\t R22 = %" PRIxPTR "\n", excPtr->GPR22);
  printk("\t R23 = %" PRIxPTR "\n", excPtr->GPR23);
  printk("\t R24 = %" PRIxPTR "\n", excPtr->GPR24);
  printk("\t R25 = %" PRIxPTR "\n", excPtr->GPR25);
  printk("\t R26 = %" PRIxPTR "\n", excPtr->GPR26);
  printk("\t R27 = %" PRIxPTR "\n", excPtr->GPR27);
  printk("\t R28 = %" PRIxPTR "\n", excPtr->GPR28);
  printk("\t R29 = %" PRIxPTR "\n", excPtr->GPR29);
  printk("\t R30 = %" PRIxPTR "\n", excPtr->GPR30);
  printk("\t R31 = %" PRIxPTR "\n", excPtr->GPR31);
  printk("\t CR = %" PRIx32 "\n", excPtr->EXC_CR);
  printk("\t CTR = %" PRIxPTR "\n", excPtr->EXC_CTR);
  printk("\t XER = %" PRIx32 "\n", excPtr->EXC_XER);
  printk("\t LR = %" PRIxPTR "\n", excPtr->EXC_LR);
  if (excPtr->_EXC_number == ASM_DEC_VECTOR)
       recoverable = 1;
  if (excPtr->_EXC_number == ASM_SYS_VECTOR)
#ifdef TEST_RAW_EXCEPTION_CODE
    recoverable = 1;
#else
    recoverable = 0;
#endif
    if (!recoverable) {
      printk("unrecoverable exception!!! Push reset button\n");
      while(1);
    }
}

static void nop_except_enable(const rtems_raw_except_connect_data* ptr)
{
  (void) ptr;
}

static int except_always_enabled(const rtems_raw_except_connect_data* ptr)
{
  (void) ptr;

  return 1;
}

void initialize_exceptions(void)
{
  int i;

  /*
   * Initialize all entries of the exception table with a description of the
   * default exception handler.
   */
  exception_config.exceptSize 			= NUM_EXCEPTIONS;
  exception_config.rawExceptHdlTbl 		= &exception_table[0];
  exception_config.defaultRawEntry.exceptIndex	= 0;
  exception_config.defaultRawEntry.hdl.vector	= 0;
  exception_config.defaultRawEntry.hdl.raw_hdl	= default_exception_handler;

  for (i = 0; i < exception_config.exceptSize; i++) {
    printk("installing exception number %d\n", i);
    exception_table[i].exceptIndex	= i;
    exception_table[i].hdl		= exception_config.defaultRawEntry.hdl;
    exception_table[i].hdl.vector	= i;
    exception_table[i].on		= nop_except_enable;
    exception_table[i].off		= nop_except_enable;
    exception_table[i].isOn		= except_always_enabled;
  }

  /*
   * Now pass the initialized exception table to the exceptions module which
   * will install the handler pointers in the exception handler table.
   */
  if (!mpc5xx_init_exceptions(&exception_config)) {
    /*
     * At this stage we may not call CPU_Panic because it uses exceptions!!!
     */
    printk("Exception handling initialization failed\n");
    printk("System locked\n"); while(1);
  }
  else {
    printk("Exception handling initialization done\n");
  }
}
