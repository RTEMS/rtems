/*
 **************************************************************************
 *
 * Component =   
 * 
 * Synopsis  =   rkdb/rkdb.c
 *
 * $Id$
 *
 **************************************************************************
 */

#include <assert.h>
#include <errno.h>
#include <rdbg/rdbg.h>
#include <rdbg/rdbg_f.h>
#include <rdbg/servrpc.h>

extern rtems_id serializeSemId;
extern rtems_id wakeupEventSemId;
extern rtems_id eventTaskId;
extern rtems_id debugId;
extern Exception_context *FirstCtx;
extern Exception_context *LastCtx;
extern CPU_Exception_frame SavedContext;
extern unsigned int NbExceptCtx;

/* 
 * return a pointer to the Thread Control structure of the specified
 * Id
 */

  Thread_Control *
Thread_Get_RDBG (Objects_Id Id)
{
  unsigned index;

  if (Id < _Objects_Information_table[OBJECTS_RTEMS_TASKS]->maximum_id &&
      Id > _Objects_Information_table[OBJECTS_RTEMS_TASKS]->minimum_id) {

    index = Id - _Objects_Information_table[OBJECTS_RTEMS_TASKS]->minimum_id;
    if (_Objects_Information_table[OBJECTS_RTEMS_TASKS]->local_table[1 + index] != NULL) {
      return (Thread_Control*) (_Objects_Information_table[OBJECTS_RTEMS_TASKS]->local_table[1 + index]);
    }
  }

  if (Id < _Objects_Information_table[OBJECTS_POSIX_THREADS]->maximum_id &&
      Id > _Objects_Information_table[OBJECTS_POSIX_THREADS]->minimum_id) {

    index = Id - _Objects_Information_table[OBJECTS_POSIX_THREADS]->minimum_id;
    if (_Objects_Information_table[OBJECTS_POSIX_THREADS]->
        local_table[1 + index] != NULL)
      return (Thread_Control*) (_Objects_Information_table[OBJECTS_POSIX_THREADS]->local_table[1 + index]);
  }

  return 0;

}

  int
safeMemRead (void *src, void *dest, int nbBytes)
{

  /*
   * safe because if it generates an exception,
   * it must return normally
   * TBD
   */

  memcpy (dest, src, nbBytes);
  return 0;
}

  int
safeMemWrite (void *src, void *dest, int nbBytes)
{

  /*
   * safe because if it generates an exception,
   * it must return normally
   * TBD
   */

  memcpy (dest, src, nbBytes);
  return 0;
}

  int
ptrace (int request, int pid, char *addr, int data, char *addr2)
{
  int diag;
  errno = 0;
  if (pid != 1) {
    errno = ESRCH;
    return -1;
  }
  switch (request) {

  case RPT_SINGLESTEP:{
      Exception_context *ctx;

      if (CannotRestart == 1) {
        setErrno (EIO);
        return -1;
      }

      if ((ctx = GetExceptCtx (currentTargetThread)) != NULL) {
        Single_Step (ctx->ctx);
        rtems_semaphore_release (ctx->semaphoreId);
        return 0;
      }
      break;
    }

  case RPT_PEEKTEXT:
  case RPT_PEEKDATA:{
      diag = safeMemRead (addr, &data, sizeof data);
      if (diag == 0)
        return data;
    mem_error:
      return -1;
    }

  case RPT_POKETEXT:{
      diag = safeMemWrite (&data, addr, sizeof data);

      /*
       * We must flush the INSTR and DATA cache to be sure the
       * opcode modification is taken into account, because
       * the breakpoint opcode is written via the data cache
       * while execution code is fetched via the instruction
       * cache
       */

      if (diag == 0) {
        copyback_data_cache_and_invalidate_instr_cache (addr, sizeof data);
        return 0;
      }
      goto mem_error;
    }
  case RPT_POKEDATA:{
      diag = safeMemWrite (&data, addr, sizeof data);
      if (diag == 0)
        return 0;
      goto mem_error;
    }
  case RPT_CONT:{
      Exception_context *ctx;

      if (CannotRestart == 1) {
        setErrno (EIO);
        return -1;
      }

      ctx = GetExceptCtx (currentTargetThread);
      if (ctx == NULL)
        ctx = GetExceptCtx (debugId);

      if (ctx != NULL) {

        if (!isRdbgException (ctx)) {
          CannotRestart = 1;
          setErrno (EIO);
          return -1;
        }

        assert (data == 0);
        assert (ExitForSingleStep == 0);

        rtems_semaphore_release (ctx->semaphoreId);
      }
      rtems_semaphore_release (serializeSemId);
      return 0;
    }

  case RPT_ATTACH:
    return 0;

  case RPT_DETACH:{
      Exception_context *ctx;

      if (NbExceptCtx || NbSerializedCtx) {
        ctx = FirstCtx;
        rtems_task_delete (eventTaskId);
        rtems_semaphore_delete (serializeSemId);
        rtems_semaphore_delete (wakeupEventSemId);
      }
      return 0;
    }

  case RPT_GETREGS:{
      Exception_context *ctx;

      if ((ctx = GetExceptCtx (currentTargetThread)) != NULL) {
        CtxToRegs (ctx->ctx, (xdr_regs *) addr);
        return 0;
      }
      break;
    }

  case RPT_SETREGS:{
      Exception_context *ctx;

      if ((ctx = GetExceptCtx (currentTargetThread)) != NULL) {
        RegsToCtx ((xdr_regs *) addr, ctx->ctx);
        return 0;
      }
      break;
    }

  case RPT_READTEXT:
  case RPT_READDATA:{
      diag = safeMemRead (addr, addr2, data);
      if (diag == 0)
        return 0;
      goto mem_error;
    }
  case RPT_WRITETEXT:
  case RPT_WRITEDATA:{
      diag = safeMemWrite (addr2, addr, data);
      if (diag == 0)
        return 0;
      goto mem_error;
    }

  case RPT_GETTARGETTHREAD:
    if (!NbExceptCtx) {
      errno = EBUSY;
      return -1;
    }
    return currentTargetThread;

  case RPT_SETTARGETTHREAD:
    if (!NbExceptCtx) {
      errno = EBUSY;
      return -1;
    }
    currentTargetThread = data;
    return 0;

  case RPT_GETTHREADNAME:{
      return TgtGetThreadName (NULL, (unsigned) (data), (char *) addr);
    }

  case RPT_THREADLIST:{
      int count = TgtThreadList (NULL, (unsigned *) addr, UTHREAD_MAX
                                 * sizeof (unsigned));
      if (count < 0) {
        errno = EINVAL;
        return -1;
      }
      return count;
    }

  case RPT_SETTHREADREGS:{
      Exception_context *ctx;
      CPU_Exception_frame Ectx;
      Thread_Control *thread;
      rtems_id id;

      rtems_task_ident (RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &id);
      if (data == (unsigned) id)
        break;

      if ((ctx = GetExceptCtx (data)) != NULL) {
        RegsToCtx ((xdr_regs *) addr, ctx->ctx);
        return 0;
      }
      thread = Thread_Get_RDBG ((Objects_Id) (data));
      if (thread != NULL) {
        RegsToCtx ((xdr_regs *) addr, &Ectx);
        set_ctx_thread (thread, &Ectx);
        return 0;
      }
      break;
    }

  case RPT_GETTHREADREGS:{
      Exception_context *ctx;
      CPU_Exception_frame Ectx;
      Thread_Control *thread;
      rtems_id id;

      rtems_task_ident (RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &id);
      if (data == (unsigned) id) {
        justSaveContext = 1;
        enterRdbg ();
        CtxToRegs (&(SavedContext), (xdr_regs *) addr);
        return 0;
      }

      if ((ctx = GetExceptCtx (data)) != NULL) {
        CtxToRegs (ctx->ctx, (xdr_regs *) addr);
        return 0;
      }
      thread = Thread_Get_RDBG ((Objects_Id) (data));
      if (thread != NULL) {
        get_ctx_thread (thread, &Ectx);
        CtxToRegs (&Ectx, (xdr_regs *) addr);
        return 0;
      }
      break;
    }

  case RPT_KILL:
    TotalReboot = 1;
    return 0;

  case RPT_TRACEME:
  case RPT_PEEKUSER:
  case RPT_POKEUSER:
  case RPT_GETFPREGS:
  case RPT_SETFPREGS:
  case RPT_GETFPAREGS:
  case RPT_SETFPAREGS:
  case RPT_SYSCALL:
  case RPT_DUMPCORE:
  case RPT_GETUCODE:
  case RPT_THREADSUSPEND:
  case RPT_THREADRESUME:
  case RPT_SETTHREADNAME:
  default:
    break;
  }
  errno = EINVAL;
  return -1;
}
