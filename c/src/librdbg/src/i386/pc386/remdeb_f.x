/*
 **************************************************************************
 *
 * Component =   rdblib
 * 
 * Synopsis  =   remdeb_f.x
 *
 * $Id$
 *
 **************************************************************************
 */

struct xdr_regs
{
  unsigned int  tabreg[19];
};

#ifdef RPC_HDR

%/* now define register macros to apply to xdr_reg struct */
%
%#define GS     0
%#define FS     1
%#define ES     2
%#define DS     3
%#define EDI    4
%#define ESI    5
%#define EBP    6
%#define ESP    7
%#define EBX    8
%#define EDX    9
%#define ECX    10
%#define EAX    11
%#define TRAPNO 12
%#define ERR    13
%#define EIP    14
%#define CS     15
%#define EFL    16
%#define UESP   17
%#define SS     18
%
%#define REG_PC tabreg[EIP]     /* PC (eip) register offset */
%#define REG_SP tabreg[UESP]    /* SP (uesp) register offset */
%#define REG_FP tabreg[EBP]     /* FP (ebp) register offset */

%/* now define the BREAKPOINT mask technique to a long word */
%#define SET_BREAK(l)   ((l&0xFFFFFF00) | 0xCC) 
%#define IS_BREAK(l)    (((l) & 0xFF) == 0xCC)
%#define ORG_BREAK(c,p) (((c) & 0xFFFFFF00) | ((p) & 0xFF))
%#define IS_STEP(regs)  (regs.tabreg[TRAPNO] == 1) /* was step and not break */
%#define BREAK_ADJ      1       /* must subtract one from address after bp */
%#define BREAK_SIZE     1       /* Breakpoint occupies one byte */

%#define TARGET_PROC_TYPE  0

#endif



