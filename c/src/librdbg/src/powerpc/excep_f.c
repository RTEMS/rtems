/*
 **************************************************************************
 *
 * Component =   
 * 
 * Synopsis  =   rdbg/powerpc/excep_f.c
 *
 * $Id$
 *
 **************************************************************************
 */

#include <rtems.h>
#include <rtems/error.h>
#include <assert.h>
#include <errno.h>
#include <rdbg/rdbg.h>
#include <rdbg/servrpc.h>

  int
ExcepToSig (Exception_context * ctx)
{
  int excep = getExcNum (ctx);

  switch (excep) {
  case ASM_FLOAT_VECTOR:
    return SIGFPE;

  case ASM_TRACE_VECTOR:
  case ASM_PROG_VECTOR:
  case ASM_SYS_VECTOR:
    return SIGTRAP;

  case ASM_ISI_VECTOR:
    return SIGSEGV;

  case ASM_RESET_VECTOR:
  case ASM_MACH_VECTOR:
  case ASM_EXT_VECTOR:
  case ASM_ALIGN_VECTOR:
    return SIGILL;

  default:
    break;
  }
  return SIGKILL;
}

/*----- Breakpoint Exception management -----*/

    /*
     *  Handler for Breakpoint Exceptions :
     *  software breakpoints.
     */

void
BreakPointExcHdl (CPU_Exception_frame * ctx)
{
  rtems_status_code status;
  rtems_id continueSemId;

  /*
   * we must re-enable the floating point engine
   * if the interrupted thread is FP. Otherwise,
   * the semaphore primitives may crash when they
   * try to save FP context while switching this
   * thread... NB : deferred fp context switching
   * would 1) avoid to have to save FP, make this code
   * obsolete.
   */
  if (ctx->EXC_SRR1 & MSR_FP) {
    register unsigned long msr;
    __asm__ __volatile__ ("mfmsr %0":"=r" (msr));
    __asm__ __volatile__ ("mtmsr %0"::"r" (msr | MSR_FP));
  }

  if ((justSaveContext) && (ctx->_EXC_number == ASM_SYS_VECTOR)) {
    PushSavedExceptCtx (_Thread_Executing->Object.id, ctx);
    justSaveContext = 0;
  } else {
    if (ctx->_EXC_number != ASM_TRACE_VECTOR) {
      NbSerializedCtx++;
      rtems_semaphore_obtain (serializeSemId, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      NbSerializedCtx--;
    }

    currentTargetThread = _Thread_Executing->Object.id;

#ifdef DDEBUG
    printk ("----------------------------------------------------------\n");
    printk ("Exception %d caught at PC %x by thread %d\n",
            ctx->_EXC_number, ctx->EXC_SRR0, _Thread_Executing->Object.id);
    printk ("----------------------------------------------------------\n");
    printk ("Processor execution context at time of the fault was  :\n");
    printk ("----------------------------------------------------------\n");
    printk ("\t R0 = %x\n", ctx->GPR0);
    printk ("\t R1 = %x\n", ctx->GPR1);
    printk ("\t R2 = %x\n", ctx->GPR2);
    printk ("\t R3 = %x\n", ctx->GPR3);
    printk ("\t R4 = %x\n", ctx->GPR4);
    printk ("\t R5 = %x\n", ctx->GPR5);
    printk ("\t R6 = %x\n", ctx->GPR6);
    printk ("\t R7 = %x\n", ctx->GPR7);
    printk ("\t R8 = %x\n", ctx->GPR8);
    printk ("\t R9 = %x\n", ctx->GPR9);
    printk ("\t R10 = %x\n", ctx->GPR10);
    printk ("\t R11 = %x\n", ctx->GPR11);
    printk ("\t R12 = %x\n", ctx->GPR12);
    printk ("\t R13 = %x\n", ctx->GPR13);
    printk ("\t R14 = %x\n", ctx->GPR14);
    printk ("\t R15 = %x\n", ctx->GPR15);
    printk ("\t R16 = %x\n", ctx->GPR16);
    printk ("\t R17 = %x\n", ctx->GPR17);
    printk ("\t R18 = %x\n", ctx->GPR18);
    printk ("\t R19 = %x\n", ctx->GPR19);
    printk ("\t R20 = %x\n", ctx->GPR20);
    printk ("\t R21 = %x\n", ctx->GPR21);
    printk ("\t R22 = %x\n", ctx->GPR22);
    printk ("\t R23 = %x\n", ctx->GPR23);
    printk ("\t R24 = %x\n", ctx->GPR24);
    printk ("\t R25 = %x\n", ctx->GPR25);
    printk ("\t R26 = %x\n", ctx->GPR26);
    printk ("\t R27 = %x\n", ctx->GPR27);
    printk ("\t R28 = %x\n", ctx->GPR28);
    printk ("\t R29 = %x\n", ctx->GPR29);
    printk ("\t R30 = %x\n", ctx->GPR30);
    printk ("\t R31 = %x\n", ctx->GPR31);
    printk ("\t CR = %x\n", ctx->EXC_CR);
    printk ("\t CTR = %x\n", ctx->EXC_CTR);
    printk ("\t XER = %x\n", ctx->EXC_XER);
    printk ("\t LR = %x\n", ctx->EXC_LR);
    printk ("\t MSR = %x\n", ctx->EXC_SRR1);
#endif

    status = rtems_semaphore_create (rtems_build_name ('D', 'B', 'G', 'c'),
                                     0,
                                     RTEMS_FIFO |
                                     RTEMS_COUNTING_SEMAPHORE |
                                     RTEMS_NO_INHERIT_PRIORITY |
                                     RTEMS_NO_PRIORITY_CEILING |
                                     RTEMS_LOCAL, 0, &continueSemId);
    if (status != RTEMS_SUCCESSFUL)
      rtems_panic ("Can't create continue semaphore: `%s'\n",
                   rtems_status_text (status));

    PushExceptCtx (_Thread_Executing->Object.id, continueSemId, ctx);

    switch (ctx->_EXC_number) {
    case ASM_TRACE_VECTOR:
      DPRINTF ((" TRACE EXCEPTION !!!\n"));
      ctx->EXC_SRR1 &= ~MSR_SE;
      ExitForSingleStep--;
      rtems_semaphore_release (wakeupEventSemId);
      break;

    case ASM_PROG_VECTOR:
      DPRINTF ((" BREAKPOINT EXCEPTION !!!\n"));
      rtems_semaphore_release (wakeupEventSemId);
      break;

    case ASM_SYS_VECTOR:
      DPRINTF ((" ENTER RDBG !!!\n"));
      rtems_semaphore_release (wakeupEventSemId);
      break;

    default:
      DPRINTF ((" OTHER EXCEPTION !!!\n"));
      rtems_semaphore_release (wakeupEventSemId);
      break;
    }

    rtems_semaphore_obtain (continueSemId, RTEMS_WAIT, RTEMS_NO_TIMEOUT);

    PopExceptCtx (_Thread_Executing->Object.id);
    rtems_semaphore_delete (continueSemId);
  }
}
