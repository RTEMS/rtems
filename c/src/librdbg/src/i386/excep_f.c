/*
 **************************************************************************
 *
 * Component =   
 * 
 * Synopsis  =   rdbg/i386/excep.c
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

/* -----------------------------------------------------------------
   Maping of hardware exceptions into Unix-like signal numbers.
   It is identical to the one used by the PM and the AM.
   ----------------------------------------------------------------- */

  int
ExcepToSig (Exception_context * ctx)
{
  int excep = getExcNum (ctx);

  switch (excep) {

  case I386_EXCEPTION_MATH_COPROC_UNAVAIL:
  case I386_EXCEPTION_I386_COPROC_SEG_ERR:
  case I386_EXCEPTION_FLOAT_ERROR:
  case I386_EXCEPTION_BOUND:
    return SIGFPE;

  case I386_EXCEPTION_DEBUG:
  case I386_EXCEPTION_BREAKPOINT:
  case I386_EXCEPTION_ENTER_RDBG:
    return SIGTRAP;

  case I386_EXCEPTION_OVERFLOW:
  case I386_EXCEPTION_DIVIDE_BY_ZERO:
  case I386_EXCEPTION_ILLEGAL_INSTR:
    return SIGILL;

  case I386_EXCEPTION_SEGMENT_NOT_PRESENT:
  case I386_EXCEPTION_STACK_SEGMENT_FAULT:
  case I386_EXCEPTION_GENERAL_PROT_ERR:
  case I386_EXCEPTION_PAGE_FAULT:
    return SIGSEGV;

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

  if ((justSaveContext) && (ctx->idtIndex == I386_EXCEPTION_ENTER_RDBG)) {
    PushSavedExceptCtx (_Thread_Executing->Object.id, ctx);
    justSaveContext = 0;
  } else {
    if (ctx->idtIndex != I386_EXCEPTION_DEBUG) {
      NbSerializedCtx++;
      rtems_semaphore_obtain (serializeSemId, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      NbSerializedCtx--;
    }

    currentTargetThread = _Thread_Executing->Object.id;

#ifdef DDEBUG
    printk ("----------------------------------------------------------\n");
    printk ("Exception %d caught at PC %x by thread %d\n",
            ctx->idtIndex, ctx->eip, _Thread_Executing->Object.id);
    printk ("----------------------------------------------------------\n");
    printk ("Processor execution context at time of the fault was  :\n");
    printk ("----------------------------------------------------------\n");
    printk (" EAX = %x	EBX = %x	ECX = %x	EDX = %x\n",
            ctx->eax, ctx->ebx, ctx->ecx, ctx->edx);
    printk (" ESI = %x	EDI = %x	EBP = %x	ESP = %x\n",
            ctx->esi, ctx->edi, ctx->ebp, ctx->esp0);
    printk ("----------------------------------------------------------\n");
    printk ("Error code pushed by processor itself (if not 0) = %x\n",
            ctx->faultCode);
    printk ("----------------------------------------------------------\n\n");
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

    switch (ctx->idtIndex) {
    case I386_EXCEPTION_DEBUG:
      DPRINTF ((" DEBUG EXCEPTION !!!\n"));
      ctx->eflags &= ~EFLAGS_TF;
      ExitForSingleStep--;
      rtems_semaphore_release (wakeupEventSemId);
      break;

    case I386_EXCEPTION_BREAKPOINT:
      DPRINTF ((" BREAKPOINT EXCEPTION !!!\n"));
      rtems_semaphore_release (wakeupEventSemId);
      break;

    case I386_EXCEPTION_ENTER_RDBG:
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
