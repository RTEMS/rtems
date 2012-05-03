/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 5/2002,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */
/* Copyright :
 * (C) S. Kate Feng <feng1@bnl.gov> 4/2004 modified it for MVME5500
 */

#include <bsp.h>
#include <bsp/vectors.h>
#include <libcpu/spr.h>
#include <bsp/pci.h>
#include <rtems/bspIo.h>

#include <bsp/bspException.h>

#define SRR1_TEA_EXC	(1<<(31-13))
#define SRR1_MCP_EXC	(1<<(31-12))

static volatile BSP_ExceptionExtension	BSP_exceptionExtension = 0;

BSP_ExceptionExtension
BSP_exceptionHandlerInstall(BSP_ExceptionExtension e)
{
volatile BSP_ExceptionExtension	test;
	if ( RTEMS_SUCCESSFUL != rtems_task_variable_get(RTEMS_SELF, (void*)&BSP_exceptionExtension, (void**)&test) ) {
		/* not yet added */
		rtems_task_variable_add(RTEMS_SELF, (void*)&BSP_exceptionExtension, 0);
	}
	test = BSP_exceptionExtension;
	BSP_exceptionExtension = e;
	return test;
}

void
BSP_exceptionHandler(BSP_Exception_frame* excPtr)
{
uint32_t	note;
BSP_ExceptionExtension	ext=0;
rtems_id		id=0;
int			recoverable = 0;
char			*fmt="Uhuuuh, Exception %d in unknown task???\n";
int			quiet=0;

 if (!quiet) printk("In BSP_exceptionHandler()\n");
   /* If we are in interrupt context, we are in trouble - skip the user
    * hook and panic
    */
    if (rtems_interrupt_is_in_progress()) {
      fmt="Aieeh, Exception %d in interrupt handler\n";
    } else if ( !_Thread_Executing) {
      fmt="Aieeh, Exception %d in initialization code\n";
    } else {
    /* retrieve the notepad which possibly holds an extention pointer */
    if (RTEMS_SUCCESSFUL==rtems_task_ident(RTEMS_SELF,RTEMS_LOCAL,&id) &&
#if 0
/* Must not use a notepad due to unknown initial value (notepad memory is allocated from the
 * workspace)!
 */
	        RTEMS_SUCCESSFUL==rtems_task_get_note(id, BSP_EXCEPTION_NOTEPAD, &note)
#else
	        RTEMS_SUCCESSFUL==rtems_task_variable_get(id, (void*)&BSP_exceptionExtension, (void**)&note)
#endif
         ) {
	   ext = (BSP_ExceptionExtension)note;
	   if (ext)
	       quiet=ext->quiet;
	   if (!quiet) {
	      printk("Task (Id 0x%08x) got ",id);
	   }
	   fmt="exception %d\n";
	}
    }

    if (ext && ext->lowlevelHook && ext->lowlevelHook(excPtr,ext,0)) {
		/* they did all the work and want us to do nothing! */
      printk("they did all the work and want us to do nothing!\n");
		return;
    }

    if (!quiet) {
       /* message about exception */
       printk(fmt, excPtr->_EXC_number);
       /* register dump */
       printk("\t Next PC or Address of fault = %x, ", excPtr->EXC_SRR0);
       printk("Mvme5500 Saved MSR = %x\n", excPtr->EXC_SRR1);
       printk("\t R0  = %08x", excPtr->GPR0);
       printk(" R1  = %08x", excPtr->GPR1);
       printk(" R2  = %08x", excPtr->GPR2);
       printk(" R3  = %08x\n", excPtr->GPR3);
       printk("\t R4  = %08x", excPtr->GPR4);
       printk(" R5  = %08x", excPtr->GPR5);
       printk(" R6  = %08x", excPtr->GPR6);
       printk(" R7  = %08x\n", excPtr->GPR7);
       printk("\t R8  = %08x", excPtr->GPR8);
       printk(" R9  = %08x", excPtr->GPR9);
       printk(" R10 = %08x", excPtr->GPR10);
       printk(" R11 = %08x\n", excPtr->GPR11);
       printk("\t R12 = %08x", excPtr->GPR12);
       printk(" R13 = %08x", excPtr->GPR13);
       printk(" R14 = %08x", excPtr->GPR14);
       printk(" R15 = %08x\n", excPtr->GPR15);
       printk("\t R16 = %08x", excPtr->GPR16);
       printk(" R17 = %08x", excPtr->GPR17);
       printk(" R18 = %08x", excPtr->GPR18);
       printk(" R19 = %08x\n", excPtr->GPR19);
       printk("\t R20 = %08x", excPtr->GPR20);
       printk(" R21 = %08x", excPtr->GPR21);
       printk(" R22 = %08x", excPtr->GPR22);
       printk(" R23 = %08x\n", excPtr->GPR23);
       printk("\t R24 = %08x", excPtr->GPR24);
       printk(" R25 = %08x", excPtr->GPR25);
       printk(" R26 = %08x", excPtr->GPR26);
       printk(" R27 = %08x\n", excPtr->GPR27);
       printk("\t R28 = %08x", excPtr->GPR28);
       printk(" R29 = %08x", excPtr->GPR29);
       printk(" R30 = %08x", excPtr->GPR30);
       printk(" R31 = %08x\n", excPtr->GPR31);
       printk("\t CR  = %08x\n", excPtr->EXC_CR);
       printk("\t CTR = %08x\n", excPtr->EXC_CTR);
       printk("\t XER = %08x\n", excPtr->EXC_XER);
       printk("\t LR  = %08x\n", excPtr->EXC_LR);
       printk("\t DAR = %08x\n", excPtr->EXC_DAR);

       BSP_printStackTrace(excPtr);
    }

    if (ASM_MACH_VECTOR == excPtr->_EXC_number) {
       /* ollah , we got a machine check - this could either
	* be a TEA, MCP or internal; let's see and provide more info
	*/
       if (!quiet)
	   printk("Machine check; reason:");
       if ( ! (excPtr->EXC_SRR1 & (SRR1_TEA_EXC | SRR1_MCP_EXC)) ) {
	   if (!quiet)
	       printk("SRR1\n");
       } else {
	   if (excPtr->EXC_SRR1 & (SRR1_TEA_EXC)) {
	      if (!quiet)
		 printk(" TEA");
	   }
	   if (excPtr->EXC_SRR1 & (SRR1_MCP_EXC)) {
	      unsigned long gerr;

	      if (!quiet) printk(" MCP\n");

	      /* it's MCP; gather info from the host bridge */
 	      gerr=_BSP_clear_hostbridge_errors(0,0);
              if (gerr&0x80000000) printk("GT64260 Parity error\n");
              if (gerr&0x40000000) printk("GT64260 SysErr\n");
	      if ((!quiet) && (!gerr)) printk("GT64260 host bridge seems OK\n");
	    }
       }
    } else if (ASM_DEC_VECTOR == excPtr->_EXC_number) {
		recoverable = 1;
    } else if (ASM_SYS_VECTOR == excPtr->_EXC_number) {
#ifdef TEST_RAW_EXCEPTION_CODE
		recoverable = 1;
#else
		recoverable = 0;
#endif
    }

    /* call them for a second time giving a chance to intercept
     * the task_suspend
     */
    if (ext && ext->lowlevelHook && ext->lowlevelHook(excPtr, ext, 1))
		return;

    if (!recoverable) {
		if (id) {
			/* if there's a highlevel hook, install it */
			if (ext && ext->highlevelHook) {
				excPtr->EXC_SRR0 = (uint32_t)ext->highlevelHook;
				excPtr->GPR3     = (uint32_t)ext;
				return;
			}
			if (excPtr->EXC_SRR1 & MSR_FP) {
				/* thread dispatching is _not_ disabled at this point; hence
				 * we must make sure we have the FPU enabled...
				 */
				_write_MSR( _read_MSR() | MSR_FP );
				__asm__ __volatile__("isync");
			}
			printk("unrecoverable exception!!! task %08x suspended\n",id);
			rtems_task_suspend(id);
		} else {
			printk("PANIC, rebooting...\n");
			bsp_reset();
		}
    }
}
