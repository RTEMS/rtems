/* Altivec support for RTEMS; vector register context management.  */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2009,
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

#ifdef __ALTIVEC__

#include <rtems.h>
#include <libcpu/cpuIdent.h>
#include <rtems/bspIo.h>
#include <rtems/error.h>
#include <rtems/score/cpu.h>
#include <rtems/powerpc/powerpc.h>

#define STATIC static

#define VEC_ALIGNMENT	16

#define NAM				"AltiVec Support"
#define ERRID(a,b,c,d)	(((a)<<24) | ((b)<<16) | ((c)<<8) | (d))

typedef uint32_t _vu32 __attribute__((vector_size(VEC_ALIGNMENT)));

#ifndef MSR_VE
#define MSR_VE	(1<<(31-6))
#endif

/* NOTE: These two variables are accessed by assembly code
 *       which assumes 32-bit data!
 */
uint32_t _CPU_altivec_ctxt_off = 0;
uint32_t _CPU_altivec_psim_cpu = 0;

static inline uint32_t
mfmsr(void)
{
uint32_t v;	
	_CPU_MSR_GET(v);
	return v;
}

static inline void
mtmsr(uint32_t v)
{
	_CPU_MSR_SET(v);
}

static inline void
isync(void)
{
	asm volatile("isync");
}

static inline void
dssall(void)
{
	if ( !_CPU_altivec_psim_cpu)
		asm volatile("dssall");
}

static inline uint32_t
set_MSR_VE(void)
{
uint32_t rval;
	rval=mfmsr();
	if ( ! (MSR_VE & rval ) ) {
		mtmsr(rval | MSR_VE);
		isync();
	}
	return rval;
}

static inline void
clr_MSR_VE(void)
{
	dssall();
	mtmsr(mfmsr() & ~MSR_VE);
	isync();
}

static inline void
rst_MSR_VE(uint32_t old)
{
	if ( ! ( MSR_VE & old ) ) {
		dssall();
		mtmsr(old);
		isync();
	}
}


/* Code to probe the compiler's stack alignment (PowerPC);
 * The routine determines at run-time if the compiler generated
 * 8 or 16-byte aligned code.
 *
 * Till Straumann <strauman@slac.stanford.edu>, 2005
 */

static void dummy(void) __attribute__((noinline));
/* add (empty) asm-statement to make sure this isn't optimized away */
static void dummy(void) { __asm__ volatile(""); }

static unsigned probe_r1(void) __attribute__((noinline));
static unsigned probe_r1(void)
{
unsigned r1;
	/* call something to enforce creation of a minimal stack frame;
     * (8 bytes: r1 and lr space for 'dummy' callee). If compiled
     * with -meabi -mno-altivec gcc allocates 8 bytes, if -mno-eabi
     * or -maltivec / -mabi=altivec then gcc allocates 16 bytes
     * according to the sysv / altivec ABI specs.
     */
	dummy();
	/* return stack pointer */
	asm volatile("mr %0,1":"=r"(r1));
	return r1;
}

static unsigned
probe_ppc_stack_alignment(void)
{
unsigned r1;
	asm volatile("mr %0,1":"=r"(r1));
	return (r1 - probe_r1()) & ~ 0xf;
}

STATIC int check_stack_alignment(void)
{
int rval = 0;
	if ( VEC_ALIGNMENT > PPC_STACK_ALIGNMENT ) {
		printk(NAM": CPU support has unsufficient stack alignment;\n");
		printk("modify 'cpukit/score/cpu/powerpc/rtems/score/powerpc.h'\n");
		printk("and choose PPC_ABI_SVR4. I'll enable a workaround for now.\n");
		rval |= 1;
	}
	/* Run-time check; should compile with -mabi=altivec */
	if ( probe_ppc_stack_alignment() < VEC_ALIGNMENT ) {
		printk(NAM": run-time stack alignment unsufficient; make sure you compile with -mabi=altivec\n");
		rval |= 2;
	}
	return rval;
}


static uint32_t probe_vrsave(_vu32 *p_v) __attribute__((noinline));

/* Check if this code was compiled with -mvrsave=yes or no 
 * so that we can set the default/init value accordingly.
 */
static uint32_t probe_vrsave(_vu32 *p_v)
{
_vu32     x;
uint32_t vrsave;
	/* Explicitly clobber a volatile vector reg (0) that is
	 * not used to pass return values.
	 * If -mvrsave=yes was used this should cause gcc to
	 * set bit 0 in vrsave. OTOH this bit cannot be set
	 * because v0 is volatile and not used to pass a value
	 * to the caller...
	 */
	asm volatile("vxor %0, 0, 0; mfvrsave %1":"=v"(x),"=r"(vrsave)::"v0");
	if ( p_v ) {
		*p_v = x;
	}
	return vrsave;
}

static int vrsave_yes(void) __attribute__((noinline));

static int vrsave_yes(void)
{
uint32_t vrsave_pre;
	asm volatile("mfvrsave %0":"=r"(vrsave_pre));
	if ( (vrsave_pre & 0x80000000) ) {
		printk(NAM": WARNING - unable to determine whether -mvrsave was used; assuming NO\n");
		return 0;
	}
	return probe_vrsave(0) != vrsave_pre;
}

extern void
_CPU_altivec_set_vrsave_initval(uint32_t);


void
_CPU_Initialize_altivec(void)
{
unsigned          pvr;

	/* I don't like to have to #define the offset of the altivec area
	 * for use by assembly code.
	 * Therefore, we compute it here and store it in memory...
	 */
	_CPU_altivec_ctxt_off  = offsetof(ppc_context, altivec);
	/* 
	 * Add space possibly needed for alignment
	 */
	_CPU_altivec_ctxt_off += PPC_CACHE_ALIGNMENT - 1;

	if ( ! vrsave_yes() ) {
		/* They seemed to compile with -mvrsave=no. Hence we
		 * must set VRSAVE so that all registers are saved/restored
		 * in case this support was not built with IGNORE_VRSAVE.
		 */
		_CPU_altivec_set_vrsave_initval( -1 );
	}

	if ( check_stack_alignment() & 2 )
		rtems_fatal_error_occurred(ERRID('V','E','C','1'));

	pvr                   = get_ppc_cpu_type();
	/* psim has altivec but lacks the streaming instructions :-( */
	_CPU_altivec_psim_cpu = (PPC_PSIM == pvr);

	if ( ! ppc_cpu_has_altivec() ) {
		printk(NAM": This CPU seems not to have AltiVec\n");
		rtems_panic("Unable to initialize AltiVec Support\n");
	}

	if ( ! (mfmsr() & MSR_VE) ) {
		printk(NAM": Warning: BSP should set MSR_VE early; doing it now...\n");
		set_MSR_VE();	
	}
}
#endif
