/**
 *  @file
 *
 *  @brief Test FPU/SSE Context Save and Restore
 */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2009,
 *     Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 *     under Contract DE-AC03-76SFO0515 with the Department of Energy.
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


/* Code for testing FPU/SSE context save/restore across exceptions
 * (including interrupts).
 *
 * There are two tasks and an IRQ/EXC handler involved. One task (LP)
 * is of lower priority than the other (HP) task.
 *
 * 1) LP task sets up a context area in memory (known contents; every
 *    register is loaded with different values)
 *
 * 2) LP task
 *       2a saves original FP/SSE context
 *       2b loads context from 1) into FPU/SSE
 *       2c raises an exception or interrupt
 *
 *   *  (2d save FPU/SSE context after irq/exception returns to
 *          separate area for verification
 *       2e reload original FP/SSE context.)
 *
 *   * All these five steps are coded in assembly to prevent
 *     gcc from manipulating the FP/SSE state. The last two steps,
 *     however, are effectively executed during 6 when control is
 *     returned to the LP task.
 *
 * 3) IRQ/EXC handler OS wrapper saves context, initializes FPU and
 *    MXCSR.
 *
 * 4) user (our) irq/exc handler clears exception condition, clobbers
 *    FPU and XMM regs and finally releases a semaphore on which HP
 *    task is waiting.
 *
 * 5) context switch to HP task. HP task clobbers FPU and XMM regs.
 *    Then it tries to re-acquire the synchronization semaphore and
 *    blocks.
 *
 * 6) task switch back to (interrupted) LP task. Original context is
 *    restored and verified against the context that was setup in 1).
 *
 *
 * Three methods for interrupting the LP task are tested
 *
 *  a) FP exception (by setting an exception status in the context from 1)
 *  b) SSE exception (by computing the square root of a vector of negative
 *     numbers.
 *  c) IRQ (software IRQ via 'INT xx' instruction)
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __rtems__
#include <rtems.h>
#include <rtems/score/cpu.h>
#include <rtems/irq.h>
#include <rtems/error.h>
#endif

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* This is currently hardcoded (int xx opcode requires immediate operand) */
#define SSE_TEST_IRQ  10

typedef uint8_t   __v8  __attribute__((vector_size(16)));
typedef uint32_t __v32  __attribute__((vector_size(16)));
typedef float     __vf  __attribute__((vector_size(16)));

#ifndef __rtems__
/* Clone of what is defined in rtems/score/cpu.h (for testing under linux) */
typedef struct Context_Control_sse {
  uint16_t  fcw;
  uint16_t  fsw;
  uint8_t   ftw;
  uint8_t   res_1;
  uint16_t  fop;
  uint32_t  fpu_ip;
  uint16_t  cs;
  uint16_t  res_2;
  uint32_t  fpu_dp;
  uint16_t  ds;
  uint16_t  res_3;
  uint32_t  mxcsr;
  uint32_t  mxcsr_mask;
  struct {
    uint8_t fpreg[10];
    uint8_t res_4[ 6];
  } fp_mmregs[8];
  uint8_t   xmmregs[8][16];
  uint8_t   res_5[224];
} Context_Control_sse
__attribute__((aligned(16)))
;
#endif

#define MXCSR_FZ          (1<<15)   /* Flush to zero */
#define MXCSR_RC(x) (((x)&3)<<13)   /* Rounding ctrl */
#define MXCSR_PM          (1<<12)   /* Precision msk */
#define MXCSR_UM          (1<<11)   /* Underflow msk */
#define MXCSR_OM          (1<<10)   /* Overflow  msk */
#define MXCSR_ZM          (1<< 9)   /* Divbyzero msk */
#define MXCSR_DM          (1<< 8)   /* Denormal  msk */
#define MXCSR_IM          (1<< 7)   /* Invalidop msk */
#define MXCSR_DAZ         (1<< 6)   /* Denorml are 0 */
#define MXCSR_PE          (1<< 5)   /* Precision flg */
#define MXCSR_UE          (1<< 4)   /* Underflow flg */
#define MXCSR_OE          (1<< 3)   /* Overflow  flg */
#define MXCSR_ZE          (1<< 2)   /* Divbyzero flg */
#define MXCSR_DE          (1<< 1)   /* Denormal  flg */
#define MXCSR_IE          (1<< 0)   /* Invalidop flg */

#define MXCSR_ALLM (MXCSR_PM | MXCSR_UM | MXCSR_OM | MXCSR_ZM | MXCSR_DM | MXCSR_IM)
#define MXCSR_ALLE (MXCSR_PE | MXCSR_UE | MXCSR_OE | MXCSR_ZE | MXCSR_DE | MXCSR_IE)

#define FPSR_B            (1<<15)   /* FPU busy      */
#define FPSR_C3           (1<<14)   /* Cond code C3  */
#define FPSR_TOP(x) (((x)&7)<<11)   /* TOP           */
#define FPSR_C2           (1<<10)   /* Cond code C2  */
#define FPSR_C1           (1<< 9)   /* Cond code C1  */
#define FPSR_C0           (1<< 8)   /* Cond code C0  */
#define FPSR_ES           (1<< 7)   /* Error summary */
#define FPSR_SF           (1<< 6)   /* Stack fault   */
#define FPSR_PE           (1<< 5)   /* Precision flg */
#define FPSR_UE           (1<< 4)   /* Underflow flg */
#define FPSR_OE           (1<< 3)   /* Overflow  flg */
#define FPSR_ZE           (1<< 2)   /* Divbyzero flg */
#define FPSR_DE           (1<< 1)   /* Denormal  flg */
#define FPSR_IE           (1<< 0)   /* Invalidop flg */

#define FPCW_X            (1<<12)   /* Infinity ctrl */
#define FPCW_RC(x)  (((x)&3)<<10)   /* Rounding ctrl */
#define FPCW_PC(x)  (((x)&3)<< 8)   /* Precision ctl */
#define FPCW_PM           (1<< 5)   /* Precision msk */
#define FPCW_UM           (1<< 4)   /* Underflow msk */
#define FPCW_OM           (1<< 3)   /* Overflow  msk */
#define FPCW_ZM           (1<< 2)   /* Divbyzero msk */
#define FPCW_DM           (1<< 1)   /* Denormal  msk */
#define FPCW_IM           (1<< 0)   /* Invalidop msk */

#define FPCW_ALLM (FPCW_PM | FPCW_UM | FPCW_OM | FPCW_ZM | FPCW_DM | FPCW_IM)
#define FPSR_ALLE (FPSR_ES | FPSR_SF | FPSR_PE | FPSR_UE | FPSR_OE | FPSR_ZE | FPSR_DE | FPSR_IE)

/* Store 'double' into 80-bit register image */
void
fp_st1(uint8_t (*p_dst)[10], double v)
{
	asm volatile("fstpt %0":"=m"(*p_dst):"t"(v):"st");
}

/* Store 'double' into 80-bit register image #i in context */
void
fp_st(Context_Control_sse *p_ctxt, int i, double v)
{
	fp_st1(&p_ctxt->fp_mmregs[i].fpreg,v);
}

/* Load 'double' from 80-bit register image */
double
fp_ld1(uint8_t (*p_src)[10])
{
double v;

	asm volatile("fldt %1":"=t"(v):"m"((*p_src)[0]),"m"(*p_src));
	return v;
}

/* Load 'double' from 80-bit register image #i in context */
double
fp_ld(Context_Control_sse *p_ctxt, int i)
{
	return fp_ld1(&p_ctxt->fp_mmregs[i].fpreg);
}

#define FPUCLOBBER \
	"st","st(1)","st(2)","st(3)",   \
	"st(4)","st(5)","st(6)","st(7)",\
	"fpsr","fpcr"

/* There seems to be no way to say that mxcsr was clobbered */

#define SSECLOBBER \
	"xmm0","xmm1","xmm2","xmm3",    \
	"xmm4","xmm5","xmm6","xmm7"

static void
sse_clobber(uint32_t x)
{
__v32 v = { x, x, x, x };
	asm volatile (
		"	movdqa %0,     %%xmm0      \n"
		"	movdqa %%xmm0, %%xmm1      \n"
		"	movdqa %%xmm0, %%xmm2      \n"
		"	movdqa %%xmm0, %%xmm3      \n"
		"	movdqa %%xmm0, %%xmm4      \n"
		"	movdqa %%xmm0, %%xmm5      \n"
		"	movdqa %%xmm0, %%xmm6      \n"
		"	movdqa %%xmm0, %%xmm7      \n"
		:
		:"m"(v)
		:SSECLOBBER
	);
}

void
all_clobber(uint32_t v1, uint32_t v2);

__asm__ (
"all_clobber:               \n"
"   finit                   \n"
"	movq  0(%esp), %xmm0    \n"
"   punpcklqdq %xmm0, %xmm0 \n"
"	movdqa %xmm0, %xmm1     \n"
"	movdqa %xmm0, %xmm2     \n"
"	movdqa %xmm0, %xmm3     \n"
"	movdqa %xmm0, %xmm4     \n"
"	movdqa %xmm0, %xmm5     \n"
"	movdqa %xmm0, %xmm6     \n"
"	movdqa %xmm0, %xmm7     \n"
"	ret                     \n"
);

/* Clear FPU and save FPU/SSE registers to context area */

void
init_ctxt(Context_Control_sse *p_ctxt);

__asm__ (
"init_ctxt:            \n"
"	finit              \n"
"   mov    4(%esp), %eax\n"
"   fxsave (%eax)      \n"
"   fwait              \n"
"   ret                \n"
);

/* Save FPU/SSE registers to context area */

static void
stor_ctxt(Context_Control_sse *p_ctxt)
{
	memset(p_ctxt, 0, sizeof(*p_ctxt));
	asm volatile(
/*		"	finit                \n" */
		"	fxsave %0            \n"
		"   fwait                \n"
		: "=m"(*p_ctxt)
		:
		: FPUCLOBBER
	);
}

#define H08 "0x%02"PRIx8
#define H16 "0x%04"PRIx16
#define H32 "0x%08"PRIx32

#define F16 "mismatch ("H16" != "H16")\n"

#define FLDCMP(fld, fmt) \
	if ( a->fld != b->fld ) { \
		rval = 1;             \
		if ( !quiet )         \
           fprintf(stderr,#fld" mismatch ("fmt" != "fmt")\n",a->fld, b->fld); \
	}

#define FLTCMP(i)                                   \
	do {                                            \
		if (   ( (a->ftw ^ b->ftw) & (1<<i))        \
			|| ( (a->ftw & b->ftw  & (1<<i)) &&     \
                 memcmp(a->fp_mmregs[i].fpreg,      \
                    b->fp_mmregs[i].fpreg,          \
                    sizeof(a->fp_mmregs[i].fpreg))  \
               )                                    \
           ) {                                      \
            rval = 1;                               \
            if ( !quiet ) {                         \
              double fa = fp_ld(a, i);              \
              double fb = fp_ld(b, i);              \
			  if ( ((a->ftw ^ b->ftw) & (1<<i)) )   \
                fprintf(stderr,"fpreg[%u] TAG mismatch (%u != %u)\n",i,(a->ftw & (1<<i)) ? 1 : 0,(b->ftw & (1<<i)) ? 1 : 0); \
			  else                                  \
                fprintf(stderr,"fpreg[%u] mismatch (%g != %g)\n",i,fa,fb); \
	        }                                       \
	    }                                           \
    } while (0)	

#define XMMCMP(i)                                   \
	do {                                            \
		if ( memcmp(&a->xmmregs[i],                 \
                    &b->xmmregs[i],                 \
                    sizeof(a->xmmregs[i]))          \
           ) {                                      \
            rval = 1;                               \
            if ( !quiet ) {                         \
              int _jj;                              \
              fprintf(stderr,"xmmreg[%u] mismatch:\n", i); \
              fprintf(stderr,"    ");               \
              for (_jj=0; _jj<16; _jj++)            \
                fprintf(stderr,"%02x ",a->xmmregs[i][_jj]); \
              fprintf(stderr,"\n !=\n");            \
              fprintf(stderr,"    ");               \
              for (_jj=0; _jj<16; _jj++)            \
                fprintf(stderr,"%02x ",b->xmmregs[i][_jj]); \
              fprintf(stderr,"\n");                 \
	        }                                       \
	    }                                           \
    } while (0)	


/* Compare two FPU/SSE context areas and flag differences;
 * RETURNS: zero if the contexts match and nonzero otherwise
 */
static int
cmp_ctxt(Context_Control_sse *a, Context_Control_sse *b, int quiet)
{
int rval = 0;
int i;
	FLDCMP(fcw,H16);
	FLDCMP(fsw,H16);
	FLDCMP(ftw,H08);
	FLDCMP(fop,H16);
	FLDCMP(fpu_ip,H32);
	FLDCMP(cs,H16);
	FLDCMP(fpu_dp,H32);
	FLDCMP(ds,H16);
	FLDCMP(mxcsr,H32);
	FLDCMP(mxcsr_mask,H32);
	for ( i=0; i<8; i++ ) {
		FLTCMP(i);
	}
	for ( i=0; i<8; i++ ) {
		XMMCMP(i);
	}
	return rval;
}

/* Possible arguments to exc_raise() */

#define FP_EXC   0
#define IRQ_EXC  1
#define SSE_EXC -1

/* Check stack alignment by raising the interrupt from a
 * non-16-byte aligned section of code. The exception/IRQ
 * handler must align the stack and SSE context area
 * properly or it will crash.
 */
#define __INTRAISE(x) "	int  $32+"#x" \n"
#define INTRAISE(x)   __INTRAISE(x)

__asm__ (
"do_raise:               \n"
"   fwait                \n"
"	test    %eax, %eax   \n"
"   je      2f           \n"
"   jl      1f           \n"
INTRAISE(SSE_TEST_IRQ)
"   jmp     2f           \n"
"1: sqrtps  %xmm0, %xmm0 \n"
"2:                      \n"
"   ret                  \n"
);

#define SSE_TEST_HP_FAILED       1
#define SSE_TEST_FSPR_FAILED     2
#define SSE_TEST_CTXTCMP_FAILED  4

static const char *fail_msgs[] = {
	"Seems that HP task was not executing",
	"FPSR 'Invalid-operation' flag should be clear",
	"Restored context does NOT match the saved one",
};

static void prstat(int st, const char *where)
{
int i,msk;
	for ( i=0, msk=1; i<sizeof(fail_msgs)/sizeof(fail_msgs[0]); i++, msk<<=1 ) {
		if ( (st & msk) ) {
			fprintf(stderr,"sse_test ERROR: %s (testing: %s)\n", fail_msgs[i], where);
		}
	}
}

int                 sse_test_debug   = 0;

static int
exc_raise(int kind)
{
Context_Control_sse nctxt;
Context_Control_sse octxt;
Context_Control_sse orig_ctxt;
int                 i,j,rval;
double              s2;
uint16_t            fsw;
__vf                f4  = { -1., -2., -3., -4. };
__vf                tmp;
__v32               sgn = { (1<<31), (1<<31), (1<<31), (1<<31) };

	stor_ctxt(&octxt);

	octxt.fsw   &= ~FPSR_ALLE;
	octxt.mxcsr &= ~MXCSR_ALLE;

	for ( i=0; i<8; i++ ) {
		fp_st(&octxt, i, (double)i+0.1);
		for (j=0; j<16; j++) {
			octxt.xmmregs[i][j]=(i<<4)+j;
		}
	}


	if ( SSE_EXC == kind ) {
		memcpy(octxt.xmmregs[0], &f4, sizeof(f4));
		octxt.mxcsr &= ~MXCSR_IM;
	}

	/* set tags to 'valid'            */
	octxt.ftw = 0xff;

	/* enable 'invalid arg' exception */
	octxt.fcw &= ~ ( FPCW_IM );
	
	if ( FP_EXC == kind ) {
		octxt.fsw |=   ( FPSR_IE | FPSR_ES );
	}

	if ( sse_test_debug )
		printk("RAISE (fsw was 0x%04x)\n", orig_ctxt.fsw);
	asm volatile(
		"	fxsave  %2           \n"
#ifdef __rtems__
		"   movl    %4, sse_test_check\n"
#endif
		"	fxrstor %3           \n"
		"   call    do_raise     \n"
#ifdef __rtems__
		"   movl    sse_test_check, %1\n"
#else
		"   movl    $0, %1       \n"
#endif
#ifdef TEST_MISMATCH
		"	pxor %%xmm0, %%xmm0  \n"
#endif
		"	fxsave  %0           \n"
		"	fxrstor %2           \n"
	: "=m"(nctxt),"=&r"(rval),"=m"(orig_ctxt)
	: "m"(octxt), "i"(SSE_TEST_HP_FAILED),"a"(kind)
	: "xmm0"
	);

	if ( ( FPSR_IE & nctxt.fsw ) ) {
		rval |= SSE_TEST_FSPR_FAILED;
	}
	if ( FP_EXC == kind )
		nctxt.fsw |= (FPSR_IE | FPSR_ES);
	else if ( SSE_EXC == kind ) {
		tmp = __builtin_ia32_sqrtps( (__vf)(~sgn & (__v32)f4) );
		/* sqrt raises PE; just clear it */
		nctxt.mxcsr &= ~MXCSR_PE;
		memcpy( octxt.xmmregs[0], &tmp, sizeof(tmp) );
	}

	if ( cmp_ctxt(&nctxt, &octxt, 0) ) {
		rval |= SSE_TEST_CTXTCMP_FAILED;
	}

	s2 = sqrt(2.0);

	asm volatile("fstsw %0":"=m"(fsw));

	if ( sse_test_debug )
		printf("sqrt(2): %f (FSTW: 0x%02"PRIx16")\n", sqrt(2.0), fsw);

	return rval;
}

#ifdef __rtems__
static void
sse_test_ehdl(CPU_Exception_frame *p_f);

rtems_id            sse_test_sync    = 0;
cpuExcHandlerType   sse_test_ohdl    = 0;

CPU_Exception_frame *sse_test_frame  = 0;
volatile int        sse_test_check   = SSE_TEST_HP_FAILED;
unsigned            sse_tests        = 0;

rtems_task
sse_test_hp_task(rtems_task_argument arg)
{
rtems_id sync = (rtems_id)arg;

uint16_t 			fp_cw;
uint32_t 			mxcsr;
rtems_status_code   sc;
const char *        msgs[] = {"FPU_EXC", "SSE_EXC", "IRQ_EXC"};
int                 i;

	/* verify that FPU control word is default value */
	asm volatile("fstcw %0":"=m"(fp_cw));
	if ( fp_cw != _CPU_Null_fp_context.fpucw ) {
		fprintf(
			stderr,
			"ERROR: FPU CW initialization mismatch: got 0x%04"PRIx16"; expected 0x%04"PRIx16"\n",
			fp_cw,
			_CPU_Null_fp_context.fpucw
		);
	}

	/* check MXCSR default value                     */
	asm volatile("stmxcsr %0":"=m"(mxcsr));
	if ( mxcsr != _CPU_Null_fp_context.mxcsr ) {
		fprintf(
			stderr,
			"ERROR: MXCSR initialization mismatch: got 0x%08"PRIx32"; expected 0x%08"PRIx32"\n",
			mxcsr,
			_CPU_Null_fp_context.mxcsr
		);
	}


	for (i=0; i<sizeof(msgs)/sizeof(msgs[0]); i++ ) {
		if ( ( sse_tests & (1<<i) ) ) {
			if ( sse_test_debug )
				printk("HP task will now block for %s\n",msgs[i]);

			/* Blocking here lets the low-priority task continue */
			sc = rtems_semaphore_obtain(sync, RTEMS_WAIT, 500);

			all_clobber(0xaffeaffe, 0xcafecafe);

			if ( RTEMS_SUCCESSFUL != sc ) {
				rtems_error(sc,"ERROR: sse_test hp task wasn't notified of exception\n");
				goto bail;
			}

			/* set flag indicating that we executed until here */
			sse_test_check = 0;
		}
	}

bail:
	rtems_task_suspend(RTEMS_SELF);
}

/* Flags to skip individual tests */
#define SSE_TEST_FPU_EXC  (1<<0)
#define SSE_TEST_SSE_EXC  (1<<1)
#define SSE_TEST_IRQ_EXC  (1<<2)

#define SSE_TEST_ALL      7

/* If this flag is given the executing task is not deleted
 * when the test finishes. This is useful if you want to
 * execute from a shell or similar.
 */
#define SSE_TEST_NO_DEL    (1<<0)

/* Task arg is bitmask of these flags */
rtems_task
sse_test_lp_task(rtems_task_argument arg)
{
rtems_id                hp_task = 0;
rtems_status_code       sc;
rtems_task_priority     pri;
uint16_t                fp_cw,fp_cw_set;
uint32_t                mxcsr, mxcsr_set;
rtems_irq_connect_data  irqd;
int                     flags = (int)arg;
int                     st;
int                     errs = 0;

	sse_tests = SSE_TEST_ALL & ~(flags>>1);

	sse_test_ohdl = 0;

	fp_cw_set = _CPU_Null_fp_context.fpucw | FPCW_RC(3) ;
	mxcsr_set = _CPU_Null_fp_context.mxcsr | MXCSR_RC(3) ;
	asm volatile("ldmxcsr %0"::"m"(mxcsr_set));
	asm volatile("fldcw   %0"::"m"(fp_cw_set));

	sc = rtems_semaphore_create(
			rtems_build_name('s','s','e','S'),
			0,
			RTEMS_SIMPLE_BINARY_SEMAPHORE,
			0,
			&sse_test_sync
		);
	if ( RTEMS_SUCCESSFUL != sc ) {
		rtems_error(sc, "sse_test ERROR: creation of 'sync' semaphore failed");
		errs++;
		goto bail;
	}

	rtems_task_set_priority( RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &pri );

	sc = rtems_task_create(
			rtems_build_name('s','s','e','H'),
			pri - 2,
			20000,
			RTEMS_DEFAULT_MODES,
			RTEMS_FLOATING_POINT,
			&hp_task
		);
	if ( RTEMS_SUCCESSFUL != sc ) {
		hp_task = 0;
		rtems_error( sc, "sse_test ERROR: creation of high-priority task failed");
		errs++;
		goto bail;
	}

	sc = rtems_task_start( hp_task, sse_test_hp_task, (rtems_task_argument)sse_test_sync );
	if ( RTEMS_SUCCESSFUL != sc ) {
		rtems_error( sc, "sse_test ERROR: start of high-priority task failed");
		goto bail;
	}

	/* Test if FP/SSE context is saved/restored across an exception */
	sse_test_ohdl      = _currentExcHandler;
	_currentExcHandler = sse_test_ehdl;

	if ( (sse_tests & SSE_TEST_FPU_EXC) ) {
		if ( (st = exc_raise(FP_EXC)) ) {
			prstat(st,"FP_EXC");
			errs++;
		}

		/* Test modified FPCW/MXCSR */
		asm volatile("fstcw   %0":"=m"(fp_cw));
		asm volatile("stmxcsr %0":"=m"(mxcsr));
		mxcsr &= ~(MXCSR_ALLE);
		if ( fp_cw != fp_cw_set ) {
			fprintf(stderr,"sse_test ERROR: FPCW mismatch (after FP_EXC): expected 0x%04"PRIx16", got 0x%04"PRIx16"\n", fp_cw_set, fp_cw);
			errs++;
		}
		if ( mxcsr != mxcsr_set ) {
			fprintf(stderr,"sse_test ERROR: MXCSR mismatch (after FP_EXC): expected 0x%08"PRIx32", got 0x%08"PRIx32"\n", mxcsr_set, mxcsr);
			errs++;
		}
	}

	if ( (sse_tests & SSE_TEST_SSE_EXC) ) {
		if ( (st = exc_raise(SSE_EXC)) ) {
			prstat(st, "SSE_EXC");
			errs++;
		}

		/* Test modified FPCW/MXCSR */
		asm volatile("fstcw   %0":"=m"(fp_cw));
		asm volatile("stmxcsr %0":"=m"(mxcsr));
		mxcsr &= ~(MXCSR_ALLE);
		if ( fp_cw != fp_cw_set ) {
			fprintf(stderr,"sse_test ERROR: FPCW mismatch (after SSE_EXC): expected 0x%04"PRIx16", got 0x%04"PRIx16"\n", fp_cw_set, fp_cw);
			errs++;
		}
		if ( mxcsr != mxcsr_set ) {
			fprintf(stderr,"sse_test ERROR: MXCSR mismatch (after SSE_EXC): expected 0x%08"PRIx32", got 0x%08"PRIx32"\n", mxcsr_set, mxcsr);
			errs++;
		}
	}


	if ( (sse_tests & SSE_TEST_IRQ_EXC) ) {
		memset( &irqd, 0, sizeof(irqd) );
		irqd.name   = SSE_TEST_IRQ;
		irqd.hdl    = (void*)sse_test_ehdl;
		irqd.handle = 0;

		if ( ! BSP_install_rtems_irq_handler( &irqd ) ) {
			fprintf(stderr, "sse_test ERROR: Unable to install ISR\n");
			errs++;
			goto bail;
		}

		/* Test if FP/SSE context is saved/restored across an interrupt */
		if ( (st = exc_raise(IRQ_EXC)) ) {
			prstat(st, "IRQ");
			errs++;
		}

		if ( ! BSP_remove_rtems_irq_handler( &irqd ) ) {
			fprintf(stderr, "sse_test ERROR: Unable to uninstall ISR\n");
		}

		/* Test modified FPCW/MXCSR */
		asm volatile("fstcw   %0":"=m"(fp_cw));
		asm volatile("stmxcsr %0":"=m"(mxcsr));
		mxcsr &= ~(MXCSR_ALLE);
		if ( fp_cw != fp_cw_set ) {
			fprintf(stderr,"sse_test ERROR: FPCW mismatch (after IRQ): expected 0x%04"PRIx16", got 0x%04"PRIx16"\n", fp_cw_set, fp_cw);
			errs++;
		}
		if ( mxcsr != mxcsr_set ) {
			fprintf(stderr,"sse_test ERROR: MXCSR mismatch (after IRQ): expected 0x%08"PRIx32", got 0x%08"PRIx32"\n", mxcsr_set, mxcsr);
			errs++;
		}
	}


bail:
	/* Wait for console to calm down... */
	rtems_task_wake_after(5);
	fprintf(stderr,"SSE/FPU Test %s (%u errors)\n", errs ? "FAILED":"PASSED", errs);
	if ( sse_test_ohdl ) {
		_currentExcHandler = sse_test_ohdl;
		sse_test_ohdl      = 0;
	}
	if ( sse_test_sync )
		rtems_semaphore_delete( sse_test_sync );
	sse_test_sync = 0;
	if ( hp_task )
		rtems_task_delete( hp_task );

	if ( ! (flags & SSE_TEST_NO_DEL) )
		rtems_task_exit();
}

static void
sse_test_ehdl(CPU_Exception_frame *p_f)
{
int i,j,start = 0;
int mismatch;
__vf    f4;

	if ( p_f ) {
		printk("Got exception #%u\n",        p_f->idtIndex);
		printk("EIP: 0x%08x, ESP: 0x%08x\n", p_f->eip, p_f->esp0);
		printk("TID: 0x%08x\n",              _Thread_Executing->Object.id);

		if ( ! p_f->fp_ctxt ) {
			printk("ERROR: NO FP/SSE CONTEXT ATTACHED ??\n");
			sse_test_ohdl(p_f);
		}
		if ( 16 == p_f->idtIndex ) {
			printk("Resetting FP status (0x%04"PRIx16")\n", p_f->fp_ctxt->fsw);
			p_f->fp_ctxt->fsw = 0;
		} else if ( 19 == p_f->idtIndex ) {
			start = 1;
			memcpy(&f4, p_f->fp_ctxt->xmmregs[0], sizeof(f4));
			f4 = -f4;
			memcpy(p_f->fp_ctxt->xmmregs[0], &f4, sizeof(f4));
			p_f->fp_ctxt->mxcsr &= ~MXCSR_ALLE;
		} else {
			printk("(skipping non-FP exception)\n");
			sse_test_ohdl(p_f);
		}

		printk("Checking XMM regs -- ");
		for ( mismatch=0, i=start; i<8; i++ ) {
			for ( j=0; j<16; j++ ) {
				if ( p_f->fp_ctxt->xmmregs[i][j] != ((i<<4) | j) )
					mismatch++;
			}
		}
		if ( mismatch ) {
			printk("%u mismatches; dump:\n", mismatch);
			for ( i=0; i<8; i++ ) {
				for ( j=0; j<16; j++ ) {
					printk("0x%02x ", p_f->fp_ctxt->xmmregs[i][j]);
				}
				printk("\n");
			}
		} else {
			printk("OK\n");
		}
	} else {
		printk("IRQ %u\n", SSE_TEST_IRQ);
	}
	printk("Clobbering FPU/SSE state\n");
	asm volatile("finit");
	sse_clobber(0xdeadbeef);
	printk("Notifying task\n");
	rtems_semaphore_release( sse_test_sync );	
}

#else

/* Code using signals for testing under linux; unfortunately, 32-bit
 * linux seems to pass no SSE context info to the sigaction...
 */

#include <signal.h>
#include <ucontext.h>

#define MKCASE(X) case FPE_##X: msg="FPE_"#X; break;

#define CLRXMM(i) __asm__ volatile("pxor %%xmm"#i", %%xmm"#i:::"xmm"#i)

static void
fpe_act(int signum, siginfo_t *p_info, void *arg3)
{
ucontext_t *p_ctxt = arg3;
const char *msg    = "FPE_UNKNOWN";
uint16_t   *p_fst;

	if ( SIGFPE != signum ) {
		fprintf(stderr,"WARNING: fpe_act handles SIGFPE\n");
		return;
	}
	switch ( p_info->si_code ) {
		default:
			fprintf(stderr,"WARNING: fpe_act got unkown code %u\n", p_info->si_code);
			return;
		MKCASE(INTDIV);
		MKCASE(INTOVF);
		MKCASE(FLTDIV);
		MKCASE(FLTOVF);
		MKCASE(FLTUND);
		MKCASE(FLTRES);
		MKCASE(FLTINV);
		MKCASE(FLTSUB);
	}
	fprintf(stderr,"Got SIGFPE (%s) @%p\n", msg, p_info->si_addr);
#ifdef __linux__
	fprintf(stderr,"Resetting FP status 0x%02lx\n", p_ctxt->uc_mcontext.fpregs->sw);
	p_ctxt->uc_mcontext.fpregs->sw = 0;
#ifdef TEST_MISMATCH
	fp_st1((void*)&p_ctxt->uc_mcontext.fpregs->_st[3],2.345);
#endif
#endif

	/* Clear FPU; if context is properly saved/restored around exception
	 * then this shouldn't disturb the register contents of the interrupted
	 * task/process.
	 */
	asm volatile("finit");
	sse_clobber(0xdeadbeef);
}

static void
test(void)
{
Context_Control_sse ctxt;

	stor_ctxt(&ctxt);
	printf("FPCW: 0x%"PRIx16"\nFPSW: 0x%"PRIx16"\n", ctxt.fcw, ctxt.fsw);
	printf("FTAG: 0x%"PRIx8"\n",ctxt.ftw);
}

int
main(int argc, char **argv)
{
struct sigaction a1, a2;
uint32_t         mxcsr;

	memset(&a1, 0, sizeof(a1));

	a1.sa_sigaction = fpe_act;
	a1.sa_flags     = SA_SIGINFO;	

	if ( sigaction(SIGFPE, &a1, &a2) ) {
		perror("sigaction");
		return 1;
	}

	asm volatile("stmxcsr %0":"=m"(mxcsr));
	printf("MXCSR: 0x%08"PRIx32"\n", mxcsr);

	test();
	exc_raise(0);
	return 0;
}
#endif

/* Helpers to access CR4 and MXCSR */

uint32_t
mfcr4()
{
uint32_t rval;
	asm volatile("mov %%cr4, %0":"=r"(rval));
	return rval;
}

void
mtcr4(uint32_t rval)
{
	asm volatile("mov %0, %%cr4"::"r"(rval));
}

uint32_t
mfmxcsr()
{
uint32_t rval;
	asm volatile("stmxcsr %0":"=m"(rval));
	return rval;
}

void
mtmxcsr(uint32_t rval)
{
	asm volatile("ldmxcsr %0"::"m"(rval));
}


float
sseraise()
{
__vf f4={-2., -2., -2. -2.};
float f;
     f4 = __builtin_ia32_sqrtps( f4 );
	memcpy(&f,&f4,sizeof(f));
	return f;
}
