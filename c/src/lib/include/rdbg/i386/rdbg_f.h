/*
 **************************************************************************
 *
 * Component =   RDBG
 * Module =   rdbg_f.h
 *
 * Synopsis = Machine-dependent header file
 *
 **************************************************************************
 */

#ifndef RDBG_F_H
#define RDBG_F_H

#include <rtems.h>
#include <rdbg/remdeb.h>

#define EFLAGS_TF 0x00100

typedef struct Exception_context_struct {
  struct Exception_context_struct *next;
  struct Exception_context_struct *previous;
  Objects_Id id;
  Objects_Id semaphoreId;
  CPU_Exception_frame *ctx;
} Exception_context;

extern int 	PushExceptCtx 		(Objects_Id Id,
					 Objects_Id semId,
					 CPU_Exception_frame *ctx);
extern int	PopExceptCtx  		(Objects_Id Id);
extern Exception_context *GetExceptCtx  (Objects_Id Id);
extern int  	Single_Step		(CPU_Exception_frame* ctx);
extern int 	CheckForSingleStep 	(CPU_Exception_frame* ctx);
extern void	BreakPointExcHdl   	(CPU_Exception_frame *ctx);
extern void	CtxToRegs	   	(const CPU_Exception_frame*,xdr_regs*);
extern void	RegsToCtx	   	(const xdr_regs*,CPU_Exception_frame*);

extern void	enterRdbg		();
extern void 	get_ctx_thread		(Thread_Control *thread,
					 CPU_Exception_frame* ctx);
extern void 	set_ctx_thread		(Thread_Control *thread,
					 CPU_Exception_frame* ctx);

void copyback_data_cache_and_invalidate_instr_cache();

extern int    	ExitForSingleStep;


#endif



