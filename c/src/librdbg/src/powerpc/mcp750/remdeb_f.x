/*
 **************************************************************************
 *
 * Component =   rdblib
 * Synopsis  =   remdeb_f.x
 *
 **************************************************************************
 * $Id$
 **************************************************************************
 */

struct xdr_regs
{
  unsigned int  tabreg[40];
};

#ifdef RPC_HDR

%/* now define register macros to apply to xdr_regs struct */
%
%#define R_PC    	0
%#define R_MSR   	1
%#define R_EXCEPNB    	2
%#define R_R0    	3
%#define R_R1    (R_R0 + 1) 
%#define R_R2    (R_R0 + 2)
%#define R_R3    (R_R0 + 3)
%#define R_R4    (R_R0 + 4)
%#define R_R5    (R_R0 + 5)
%#define R_R6    (R_R0 + 6)
%#define R_R7    (R_R0 + 7)
%#define R_R8    (R_R0 + 8)
%#define R_R9    (R_R0 + 9)
%#define R_R10   (R_R0 + 10)
%#define R_R11   (R_R0 + 11)
%#define R_R12   (R_R0 + 12)
%#define R_R13   (R_R0 + 13)
%#define R_R14   (R_R0 + 14)
%#define R_R15   (R_R0 + 15)
%#define R_R16   (R_R0 + 16)
%#define R_R17   (R_R0 + 17)
%#define R_R18   (R_R0 + 18)
%#define R_R19   (R_R0 + 19)
%#define R_R20   (R_R0 + 20)
%#define R_R21   (R_R0 + 21)
%#define R_R22   (R_R0 + 22)
%#define R_R23   (R_R0 + 23)
%#define R_R24   (R_R0 + 24)
%#define R_R25   (R_R0 + 25)
%#define R_R26   (R_R0 + 26)
%#define R_R27   (R_R0 + 27)
%#define R_R28   (R_R0 + 28)
%#define R_R29   (R_R0 + 29)
%#define R_R30   (R_R0 + 30)
%#define R_R31   (R_R0 + 31)
%#define R_CR    35
%#define R_CTR   36
%#define R_XER   37
%#define R_LR    38
%#define R_MQ    39
%
%#include <libcpu/raw_exception.h>
%
%#define REG_PC tabreg[R_PC]     /* PC register offset */
%#define REG_SP tabreg[R_R1]    /* SP register offset */
%#define REG_FP tabreg[R_R1]    /* SP register offset (no FP on PPC) */
%#define BREAK_SIZE     4       /* Breakpoint occupies 4 bytes */
%#define BREAK_ADJ      0       /* Nothing to subtract from address after bp */
%#define IS_BREAK(l)    ((l) == 0x7d8d6808)
%#define SET_BREAK(l)   (0x7d8d6808)
%#define ORG_BREAK(c,p) (p)
%#define IS_STEP(regs)  (regs.tabreg[R_EXCEPNB] == ASM_TRACE_VECTOR) /* Was step and not break */
%#define TARGET_PROC_TYPE  3

#endif

