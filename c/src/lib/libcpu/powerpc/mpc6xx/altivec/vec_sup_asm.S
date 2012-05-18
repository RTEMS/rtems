#ifdef __ALTIVEC__

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


#include <rtems/powerpc/powerpc.h>

#ifndef PPC_CACHE_ALIGNMENT
#error "Missing header; PPC_CACHE_ALIGNMENT is not defined"
#endif

#define ALTIVEC_TESTING

#if PPC_CACHE_ALIGNMENT != 32
#error "Altivec support assumes cache-line size is 32 bytes!"
#else
#undef  LD_PPC_CACHE_ALIGNMENT
#define LD_PPC_CACHE_ALIGNMENT 5
#endif

	.set   v0,   0
	.set   v8,   8
	.set   v16, 16
	.set   v20, 20
	.set   v24, 24
	.set   v28, 28

	.set   r0,   0
	.set   r3,   3
	.set   r4,   4
	.set   r5,   5
	.set   r6,   6
	.set   r7,   7

	.set   r10, 10
	.set   r11, 11
	.set   r12, 12

	.set   cr5,  5

	.set   VECSIZE,    16

	.set   VRSAVE_INIT_VAL, 0
	.set   VSCR_INIT_VAL,   0

	.set   VRSAVE_OFF, 16
	.set   VSCR_OFF,   16+12

	.set   ds0,  0

	/* Block size for dst -- in units of 16-bytes */
	.set   BSIZE,   2       /* = 32 bytes */
	.set   BCNT,    12/2+1  /* 12 non-volatile registers + area for vscr/vrsave */
	.set   BSTRIDE, 32      /*      bytes */

	.data

	.global _CPU_altivec_vrsave_initval
_CPU_altivec_vrsave_initval:
	.long   0

	.global _CPU_altivec_vscr_initval
_CPU_altivec_vscr_initval:
	.long   0

	.text

	.extern _CPU_altivec_psim_cpu
	.extern _CPU_altivec_ctxt_off

	.macro  CMPOFF _B0
	lis	\_B0, _CPU_altivec_ctxt_off@ha
	lwz \_B0, _CPU_altivec_ctxt_off@l(\_B0)
	.endm

	/* Conditionally load or store a vector _VR to
     *  EA(_R1|0 + _R2)
	 * If bit _VR (corresponding to _VR) is set in CRC
	 * then the load/store is performed but otherwise
	 * it is skipped.
	 * If compiled with IGNORE_VRSAVE defined then
	 * the load/store is done unconditionally.
	 *
 	 * _OPCODE: intended to be lvx, lvxl, stvx or stvxl
	 * _VR    : target vector register
	 * _R1    : base register (NOTE: _R1=r0 uses a
	 *          implicit ZERO constant, not the contents
	 *          of r0) for address computation.
	 * _R2    : 'offset' register for address computation.
	 *
	 * MODIFIES:      _VR on output if a load operation is performed.
	 * IMPLICIT USE:  CRC (unless compiled with IGNORE_VRSAVE
	 *                defined.
	 */
	.macro LDST _OPCODE, _VR, _R1, _R2
#ifndef IGNORE_VRSAVE
	bc       4, \_VR, 111f
#endif
	\_OPCODE \_VR, \_R1, \_R2
111:
	.endm

	/*
	 * Load or store four 'adjacent' vector registers.
	 *
	 * _OPCODE: intended to be lvx, lvxl, stvx or stvxl
	 * _VR    : target vector register
	 * _R1    : base register (NOTE: _R1=r0 uses a
	 *          implicit ZERO constant, not the contents
	 *          of r0) for address computation.
	 * _B0    : base register 0
	 * _B1    : base register 1
	 * _B2    : base register 2
	 * _B3    : base register 3
	 * _RO    : offset register
	 *
 	 * memory addresses for _VR, _VR+1, _VR+2, _VR+3
	 * are _B0+_RO, _B1+_RO, _B2+_RO, _B3+_RO, respectively.
	 *
	 * MODIFIES:      _VR, _VR+1, _VR+2, _VR+3 if a load
	 *                operation is performed.
	 * IMPLICIT USE:  see LDST
	 */
	.macro LDST4 _OPCODE, _VR, _B0, _B1, _B2, _B3, _RO
	LDST _OPCODE=\_OPCODE _VR=\_VR+0 _R1=\_B0 _R2=\_RO
	LDST _OPCODE=\_OPCODE _VR=\_VR+1 _R1=\_B1 _R2=\_RO
	LDST _OPCODE=\_OPCODE _VR=\_VR+2 _R1=\_B2 _R2=\_RO
	LDST _OPCODE=\_OPCODE _VR=\_VR+3 _R1=\_B3 _R2=\_RO
	.endm

	/*
	 * Preload/zero two cache lines and save 4 vector registers
	 * to memory.
 	 * Note that the cache operation targets memory *past* the
	 * current storage area which should hopefully hit when
	 * This same code is executed on the next two cache lines...
	 *
	 * This code effectively does
	 *   dcbz (_B0 + 64)
	 *   dcbz (_B0 + 64 + 32)
	 *   stvx _VF+0, (_B0+ 0)
	 *   stvx _VF+1, (_B0+16)
	 *   stvx _VF+2, (_B0+32)
	 *   stvx _VF+3, (_B0+48)
	 *
	 * _LRU:  may be 'l' or empty. The former variant should be
	 *        used when it is conceivable that the memory area is
	 *        unlikely to be used in the near future thus making
	 *        it a candidate for early eviction from the caches.
	 *
	 *        If it is likely that the memory area is reused soon
	 *        (e.g., save/restore across ISR execution) then the
	 *        'stvx' opcode (w/o 'l' suffix) should be used.
	 *
 	 * _VR:   first of four target vector registers; _VR+0,
	 *        _VR+1, _VR+2, _VR+3 are saved.
	 *
	 * _BO:   base address of memory area.
	 * _B1:   should contain _B0+16 on entry
	 * _B2:   should contain _B0+32 on entry
	 * _B3:   should contain _B0+48 on entry
	 *
	 * _O1:   contains the offset where the four vectors are
	 *        stored. 
	 *          _VR  -> (_B0 + _O1) = (_B0 + _O1 +  0 )
	 *          _VR+1-> (_B1 + _O1) = (_B0 + _O1 + 16 )
	 *          _VR+2-> (_B2 + _O1) = (_B0 + _O1 + 32 )
	 *          _VR+3-> (_B3 + _O1) = (_B0 + _O1 + 48 )
	 * _O2:   is set to _O1 + 64 by this macro. Hence _O2 is
     *        used to address the two cache-lines past the
	 *        current memory area.
	 *
 	 * MODIFIES: _O2; contains _O1 + 64 after execution of this
	 *        code.
	 *
	 * NOTES: a different set of four vectors can be addressed
	 *        simply by changing the one offset register _O1.
	 *
	 *        Saving more than 4 registers can simply be
	 *        achieved by expanding this macro multiple 
	 *        times with _O1 and _O2 swapped (new _O1 
	 *        becomes _O2 = old _O1 + 64) thus stepping
	 *        through the memory area.
	 *
	 */
	.macro S4VEC_P _LRU, _VR, _B0, _B1, _B2, _B3, _O1, _O2
	addi  \_O2, \_O1, 2*PPC_CACHE_ALIGNMENT
	dcbz  \_B0, \_O2
	dcbz  \_B2, \_O2
	LDST4 _OPCODE=stvx\_LRU _VR=\_VR _B0=\_B0 _B1=\_B1 _B2=\_B2 _B3=\_B3 _RO=\_O1
	.endm

	/*
	 * Save eight vector registers by expanding S4VEC_P twice.
	 * See notes for S4VEC_P above.
	 *
	 * INPUTS:   _B0, _B1, _B2, _B3, _B3, _O1 must be preloaded (see above)
	 *
	 * MODIFIES: After execution, 
	 *           _O2 contains original _O1 +  64,
	 *           _O1 contains original _O1 + 128
	 *
	 * NOTES:    Expanding this macro multiple times lets you save
	 *           multiple blocks of 8 registers (no reload of _Bx / _Ox is needed).
	 */
	.macro S8VEC_P _LRU, _VR, _B0, _B1, _B2, _B3, _O1, _O2
	S4VEC_P \_LRU _VR=\_VR+0 _B0=\_B0 _B1=\_B1 _B2=\_B2 _B3=\_B3 _O1=\_O1 _O2=\_O2
	/* Note that the roles of _O1 and _O2 are swapped here */
	S4VEC_P \_LRU _VR=\_VR+4 _B0=\_B0 _B1=\_B1 _B2=\_B2 _B3=\_B3 _O1=\_O2 _O2=\_O1
	.endm

	/*
	 * Save volatile vector registers v0..v19 to memory area starting at (_B0 + _O1)
	 *
	 * See notes above (for S4VEC_P).
	 *
	 * INPUTS:   _B0, _B1, _B2, _B3, _B3, _O1 must be preloaded (see above)
	 * MODIFIES: _O1 contains original _O1 + 256
	 *           _O2 contains original _O1 + 256 - 64
	 */
	.macro S_V0TOV19 _LRU, _B0, _B1, _B2, _B3, _O1, _O2
	S8VEC_P   \_LRU _VR=v0  _B0=\_B0 _B1=\_B1 _B2=\_B2 _B3=\_B3 _O1=\_O1 _O2=\_O2
	S8VEC_P   \_LRU _VR=v8  _B0=\_B0 _B1=\_B1 _B2=\_B2 _B3=\_B3 _O1=\_O1 _O2=\_O2
	LDST4 stvx\_LRU _VR=v16 _B0=\_B0 _B1=\_B1 _B2=\_B2 _B3=\_B3 _RO=\_O1
	.endm

	/*
	 * Save non-volatile vector registers v20..v31 to memory area starting at (_B0 + _O1)
	 *
	 * See notes above (for S4VEC_P, S_V0TOV19).
	 *
	 * INPUTS:   _B0, _B1, _B2, _B3, _B3, _O1 must be preloaded (see above)
	 * MODIFIES: _O1 contains original _O1 + 128
	 *           _O2 contains original _O1 + 128 - 64
	 */
	.macro S_V20TOV31 _LRU, _B0, _B1, _B2, _B3, _O1, _O2
	S8VEC_P   \_LRU _VR=v20 _B0=\_B0 _B1=\_B1 _B2=\_B2 _B3=\_B3 _O1=\_O1 _O2=\_O2
	LDST4 stvx\_LRU v28 \_B0 \_B1 \_B2 \_B3 \_O1
	.endm

	/*
	 * Save all registers to memory area
	 *
	 * INPUTS:   _B0, _B1, _B2, _B3, _B3, _O1 must be preloaded (see above)
	 * MODIFIES: _O1 contains original _O1 + 512
	 *           _O2 contains original _O1 + 512 - 64
	 */
	.macro S_V0TOV31 _B0, _B1, _B2, _B3, _O1, _O2
	S8VEC_P   l  v0  \_B0 \_B1 \_B2 \_B3 \_O1 \_O2
	S8VEC_P   l  v8  \_B0 \_B1 \_B2 \_B3 \_O1 \_O2
	S8VEC_P   l v16  \_B0 \_B1 \_B2 \_B3 \_O1 \_O2
	S4VEC_P   l v24  \_B0 \_B1 \_B2 \_B3 \_O1 \_O2
	LDST4 stvxl v28  \_B0 \_B1 \_B2 \_B3 \_O2
	.endm


	/*
	 * Macros that expand to 'dcbt _RA, _RB' or nothing, respectively.
	 * We can pass either of them as arguments to another macro which
	 * allows us to decide if the main macro uses dcbt or not when
	 * we expand it...
	 */
	.macro DO_DCBT _RA, _RB
	dcbt \_RA, \_RB
	.endm

	.macro NO_DCBT _RA, _RB
	.endm

	/*
	 * NOTE REGARDING dcbt VS dst
	 *
	 * Preloading the cache with memory areas that we soon need
	 * can be done either using 'dcbt' or 'dst' instructions
	 * "ahead of time".
	 * When experimenting (on a mpc7457) I found that the 'dst'
	 * stream instruction was very efficient if there is enough
	 * time to read ahead. It works well when we do a context
	 * switch:
	 *
	 *   1) start DST on new context to be loaded
	 *   2) save old context to memory
	 *   3) load new context from memory
	 *
	 * Because of the interleaved step 2) dst works nicely and
	 * 3) finds what it needs in the cache.
	 *
	 * However, in a situation when there is not much time
	 * to start the DST, e.g., because we want to restore
	 * a context out of the blue (e.g., after returning
	 * from and ISR):
	 *
	 *   1) save volatile registers to memory/stack
	 *   2) execute ISR
	 *   3) might do a task context switch
	 *   4) when returned to old task context then
	 *      reload volatile registers from memory/stack.
	 *
	 * In this situation, preloading the target memory before
	 * or after step 1) makes obviously no sense because after
	 * 1) the registers area is most likely in the cache already.
	 *
	 * Starting preload after 2) doesn't make much sense either.
	 * If ISR doesn't lead to a context switch then it is quite
	 * likely that the register area is still in the cache.
	 * OTOTH, if a context switch happens then the preload after 2)
	 * might be useless.
	 * 
	 * This leaves us at step 4) where we want to load immediately.
	 * In this case, I found that 'dcbt' works more efficiently
	 * so that's what we use when restoring volatile registers.
	 *
	 * When restoring the non-volatile VRs during a 'normal'
	 * context switch then we shall use DST (and no dcbt).
	 */

	/*
	 * Symmetric to S4VEC_P above but addresses loading four
	 * vector registers from memory.
	 *
	 * Touches two cache lines past the current memory area
	 * and loads four vectors from the current area.
	 *
	 * Optionally, the DCBT operation may be omitted
	 * (when expanding with _DCBT=NO_DCBT).
	 * This is useful if the cache was already preloaded
	 * by another means (dst instruction).
	 *
	 * NOTE: We always use the 'LRU' form of lvx: lvxl, 
	 *       because we deem it unlikely that the context
	 *       that was just loaded has to be saved again
	 *       to memory in the immediate future.
	 *
	 * INPUTS:   _B0, _B1, _B2, _B3, _O1 must be loaded
	 *           as explained above.
	 *
	 * MODIFIES: _O2 contains original _O1 + 64.
	 *           _VR.._VR+3 loaded from memory.
	 */
	.macro L4VEC_A _DCBT, _VR, _B0, _B1, _B2, _B3, _O1, _O2 
	addi        \_O2, \_O1, 2*PPC_CACHE_ALIGNMENT
	/* preload/touch 2 lines at offset 64 from _B0 */
	\_DCBT   \_B0, \_O2
	\_DCBT   \_B2, \_O2
	/* load four vectors at off set 0 from _B0     */
	LDST4 lvxl, \_VR, \_B0, \_B1, \_B2, \_B3, \_O1
	.endm

	/*
	 * Symmetric to S8VEC_P; loads 8 vector registers
	 * from memory -- see comments above...
	 *
	 * INPUTS:   _B0, _B1, _B2, _B3, _O1 must be loaded
	 *           as explained above.
	 *
	 * MODIFIES: _O1 contains original _O1 + 128.
	 *           _O2 contains original _O1 +  64.
	 *           _VR.._VR+7 loaded from memory.
	 */
	.macro L8VEC_A _DCBT, _VR, _B0, _B1, _B2, _B3, _O1, _O2 
	L4VEC_A \_DCBT, \_VR+0, \_B0, \_B1, \_B2, \_B3, \_O1, \_O2
	L4VEC_A \_DCBT, \_VR+4, \_B0, \_B1, \_B2, \_B3, \_O2, \_O1
	.endm
	
	/*
	 * Load volatile vector registers v0..v19 employing
	 * the DCBT to preload the cache. The rationale for
	 * using DCBT here but not when restoring non-volatile
	 * registers is explained above, see
	 *
	 *    "NOTE REGARDING dcbt VS dst"
	 * 
	 * INPUTS:   _B0, _B1, _B2, _B3, _O1 must be loaded
	 *           as explained above.
	 *
	 * MODIFIES: _O1 contains original _O1 + 256.
	 *           _O2 contains original _O1 + 256 - 64.
	 *           VR0..VR19 loaded from memory.
	 */ 
	.macro L_V0TOV19 _B0, _B1, _B2, _B3, _O1, _O2
	L8VEC_A  DO_DCBT,  v0, \_B0, \_B1, \_B2, \_B3, \_O1, \_O2
	L8VEC_A  DO_DCBT,  v8, \_B0, \_B1, \_B2, \_B3, \_O1, \_O2
	LDST4    lvxl,    v16, \_B0, \_B1, \_B2, \_B3, \_O1
	.endm

	/*
	 * Load non-volatile vector registers v20..v31.
	 * Note that no DCBT is performed since we use
	 * DST for preloading the cache during a context
	 * switch, see
	 *
	 *    "NOTE REGARDING dcbt VS dst"
	 *
	 * INPUTS:   _B0, _B1, _B2, _B3, _O1 must be loaded
	 *           as explained above.
	 *
	 * MODIFIES: _O1 contains original _O1 + 128.
	 *           _O2 contains original _O1 + 128 - 64.
	 *           VR20..VR31 loaded from memory.
	 */
	.macro L_V20TOV31 _B0, _B1, _B2, _B3, _O1, _O2
	L8VEC_A  NO_DCBT, v20, \_B0, \_B1, \_B2, \_B3, \_O1, \_O2
	LDST4    lvxl,    v28, \_B0, \_B1, \_B2, \_B3, \_O1
	.endm

	/*
	 * Load all registers from memory area.
	 */
	.macro L_V0TOV31 _B0, _B1, _B2, _B3, _O1, _O2
	L8VEC_A  DO_DCBT,  v0, \_B0, \_B1, \_B2, \_B3, \_O1, \_O2
	L8VEC_A  DO_DCBT,  v8, \_B0, \_B1, \_B2, \_B3, \_O1, \_O2
	L8VEC_A  DO_DCBT, v16, \_B0, \_B1, \_B2, \_B3, \_O1, \_O2
	L4VEC_A  DO_DCBT, v24, \_B0, \_B1, \_B2, \_B3, \_O1, \_O2
	LDST4    lvxl,    v28, \_B0, \_B1, \_B2, \_B3, \_O2
	.endm

	/*
	 * Compute
	 *     _B1 = _B0 + 16
	 *     _B2 = _B0 + 32
	 *     _B3 = _B0 + 48
	 * and load
	 *     _RO = 0
	 *
	 * convenience macro to be expanded before
	 * any of the load/store macros that use
	 * four base addresses etc.
	 *
	 * INPUT: _B0 = cache-aligned start of memory area
	 *
	 * MODIFIES: _B1, _B2, _B3, _RO as described above.
	 */
	.macro CMP_BASES _B0, _B1, _B2, _B3, _RO
	addi       \_B1, \_B0, 1*VECSIZE
	addi       \_B2, \_B0, 2*VECSIZE
	addi       \_B3, \_B0, 3*VECSIZE
	li         \_RO, 0
	.endm

	/*
	 * Prepare for saving general vector registers.
	 *
	 * If not built with #define IGNORE_VRSAVE then
	 *
	 *  1) copy vrsave to CRC
	 *
	 * endif
	 *
	 *  2) copy vrsave to _VRSAVE_REG
	 *  3) preload/zero cache line where vrsave and vscr are stored.
	 *  4) compute base adresses from _B0
	 *  5) preload/zero first two cache lines (remember that the
	 *     first S8VEC_P starts preloading/zeroing at offset 64).
	 *
	 * INPUT:    'vrsave' register, _B0 (base address of memory area)
	 * MODIFIES: _VRSAVE_REG (holds contents of 'vrsave')
	 *           _B0 = original _BO + 32
	 *           _B1 = original _B0 + 32 + 16,
	 *           _B2 = original _B0 + 32 + 32,
	 *           _B3 = original _B0 + 32 + 48,
	 *           CRC = 'vrsave' (ONLY IF COMPILED with IGNORE_VRSAVE undefined)
	 */
	.macro PREP_FOR_SAVE _VRSAVE_REG, _B0, _B1, _B2, _B3, _RO
	mfvrsave   \_VRSAVE_REG
#ifndef IGNORE_VRSAVE
	mtcr       \_VRSAVE_REG
#endif
	dcbz       0, \_B0
	addi       \_B0, \_B0, PPC_CACHE_ALIGNMENT
	dcbz       0, \_B0
	CMP_BASES \_B0, \_B1, \_B2, \_B3, \_RO
	dcbz       0, \_B2
	.endm

	/*
	 * Store _VRSAVE_REG and _VSCR_VREG to memory. These registers
	 * must have been loaded from 'vrsave' and 'vscr', respectively,
	 * prior to expanding this macro.
	 *
	 * INPUTS:   _VRSAVE_REG GPR holding 'vrsave' contents
	 *           _VSCR_VREG  VR  holding 'vscr'   contents
	 *           _B0 cache-aligned (base) address of memory area.
	 * MODIFIES: _SCRATCH_REG
	 */
	.macro S_VSCR_VRSAVE _VRSAVE_REG, _VSCR_VREG, _B0, _SCRATCH_REG
	stw       \_VRSAVE_REG,   - PPC_CACHE_ALIGNMENT + VRSAVE_OFF(\_B0)
	li        \_SCRATCH_REG,  - PPC_CACHE_ALIGNMENT + VSCR_OFF
	stvewx    \_VSCR_VREG,    \_B0, \_SCRATCH_REG
	.endm

	/*
	 * Load 'vrsave' and 'vscr' from memory.
	 *
	 * INPUTS:   _B0 cache-aligned (base) address of memory area.
	 * MODIFIES: _SCRATCH_REG (gpr), _SCRATCH_VREG (vr)
	 *           'vscr', 'vrsave'.
	 *           CRC (holds contents of 'vrsave') (ONLY IF COMPILED
	 *           with IGNORE_VRSAVE undefined).
	 */
	.macro L_VSCR_VRSAVE _B0, _SCRATCH_REG, _SCRATCH_VREG
	lwz       \_SCRATCH_REG,  - PPC_CACHE_ALIGNMENT + VRSAVE_OFF(\_B0)
	mtvrsave  \_SCRATCH_REG
#ifndef IGNORE_VRSAVE
	mtcr      \_SCRATCH_REG
#endif
	li        \_SCRATCH_REG, - PPC_CACHE_ALIGNMENT + VSCR_OFF
	lvewx     \_SCRATCH_VREG, \_B0, \_SCRATCH_REG
	mtvscr    \_SCRATCH_VREG
	.endm

	/*
	 * _B0 &= ~ (PPC_CACHE_ALIGNMENT - 1)
	 *
	 * INPUT:    _B0
	 * MODIFIES: _B0 (as stated above)
	 */
	.macro CACHE_DOWNALGN _B0
	rlwinm    \_B0, \_B0, 0, 0, 31-LD_PPC_CACHE_ALIGNMENT
	.endm

	.text

	.global _CPU_save_altivec_volatile
_CPU_save_altivec_volatile:
	/* Align address up to next cache-line boundary */
	addi      r3, r3, PPC_CACHE_ALIGNMENT - 1
	CACHE_DOWNALGN r3

#ifndef IGNORE_VRSAVE
	/* Save CRC -- it is used implicitly by all the LOAD/STORE macros
	 * when testing if we really should do the load/store operation.
	 */
	mfcr      r12
#endif

	PREP_FOR_SAVE r0, r3, r4, r5, r6, r10
	/* r0 now contains VRSAVE, r3 still the aligned memory area
	 * and r4, r5, r6 are offset by 16, 32, and 48 bytes from r3,
	 * respectively. r10 holds zero
	 */
	S_V0TOV19     _B0=r3, _B1=r4, _B2=r5, _B3=r6, _O1=r10, _O2=r11
	mfvscr        v0
	/* Store vrsave (still in r0) and vscr (in v0) to memory area */
	S_VSCR_VRSAVE r0, v0, r3, r11

#ifndef IGNORE_VRSAVE
	/* Restore CRC */
	mtcr      r12
#endif
	blr

	.global _CPU_load_altivec_volatile
_CPU_load_altivec_volatile:
	/* Align address up to next cache-line boundary */
	addi      r3, r3, PPC_CACHE_ALIGNMENT - 1
	CACHE_DOWNALGN r3
#ifndef IGNORE_VRSAVE
	/* Save CRC -- it is used implicitly by all the LOAD/STORE macros
	 * when testing if we really should do the load/store operation.
	 */
	mfcr      r12
#endif

	/* Try to preload 1st line (where vscr and vrsave are stored) */
	dcbt      0, r3
	/* Point to start of general vector-register area             */
	addi      r3, r3, PPC_CACHE_ALIGNMENT
	/* Start preloading 2nd line (where first two vectors are)    */
	dcbt      0, r3
	L_VSCR_VRSAVE r3, r0, v0
	CMP_BASES     r3, r4, r5, r6, r10
	/* Start preloading 3rd line (where vectors 3 and 4 are)      */
	dcbt      0, r5
	L_V0TOV19 r3, r4, r5, r6, r10, r11

#ifndef IGNORE_VRSAVE
	mtcr      r12
#endif
	blr

	.global _CPU_Context_switch_altivec
_CPU_Context_switch_altivec:

	/* fetch offset of altivec area in context                   */
	CMPOFF    r5
	/* down-align 'to' area to cache-line boundary               */
	add       r4, r4, r5
	CACHE_DOWNALGN r4

	/* Check for PSIM                                            */
	lis       r6, _CPU_altivec_psim_cpu@ha
	lwz       r6, _CPU_altivec_psim_cpu@l(r6)
	cmpli     0, r6, 0
	bne       1f
	/* Skip data-stream instructions on PSIM (not implemented)   */
	dssall
	/* Pre-load new context into cache                           */
	lis       r6, (BSIZE<<(24-16)) | (BCNT<<(16-16))
	ori       r6, r6, BSTRIDE
	dstt      r4, r6, ds0
1:

#ifndef IGNORE_VRSAVE
	/* Save CRC -- it is used implicitly by all the LOAD/STORE macros
	 * when testing if we really should do the load/store operation.
	 */
	mfcr      r12
#endif

	/* Is 'from' context == NULL ? (then we just do a 'restore') */
	cmpli     0, r3, 0
	beq       1f           /* yes: skip saving 'from' context    */

	/* SAVE NON-VOLATILE REGISTERS                               */

	/* Compute aligned destination pointer (r5 still holds offset
	 * to 'altivec' area in context)
	 */
	add       r3, r3, r5
	CACHE_DOWNALGN r3

	PREP_FOR_SAVE r0, r3, r5, r6, r7, r10
	/* The manual says reading vscr can take some time - do 
	 * read it here (into a volatile vector register) while
	 * we wait for cache blocks to be allocated
	 */
	mfvscr    v0
	S_V20TOV31 _LRU=l, _B0=r3, _B1=r5, _B2=r6, _B3=r7, _O1=r10, _O2=r11
	/* vrsave is now in r0 (PREP_FOR_SAVE), vscr in v0 */
	S_VSCR_VRSAVE r0, v0, r3, r5

1:

	/* LOAD NON-VOLATILE REGISTERS                               */

	/* Advance past vrsave/vscr area                             */
	addi      r4, r4, PPC_CACHE_ALIGNMENT
	L_VSCR_VRSAVE r4, r0, v0
	CMP_BASES r4, r5, r6, r7, r10
	L_V20TOV31 r4, r5, r6, r7, r10, r11

#ifndef IGNORE_VRSAVE
	mtcr      r12
#endif
	blr

	.global _CPU_Context_initialize_altivec
_CPU_Context_initialize_altivec:
	CMPOFF    r5
	add       r3, r3, r5
	CACHE_DOWNALGN r3
	lis       r5, _CPU_altivec_vrsave_initval@ha
	lwz       r5, _CPU_altivec_vrsave_initval@l(r5)
	stw       r5, VRSAVE_OFF(r3)
	lis       r6, _CPU_altivec_vscr_initval@ha
	lwz       r6, _CPU_altivec_vscr_initval@l(r6)
	stw       r6, VSCR_OFF(r3)
	blr

	/*
	 * Change the initial value of VRSAVE.
	 * Can be used by initialization code if
	 * it is determined that code was compiled
	 * with -mvrsave=no. In this case, VRSAVE
	 * must be set to all-ones which causes this
	 * support code to save/restore *all* registers
	 * (only has an effect if IGNORE_VRSAVE is
	 * not defined -- otherwise all registers are
	 * saved/restored anyways).
	 */
	.global _CPU_altivec_set_vrsave_initval
_CPU_altivec_set_vrsave_initval:
	lis       r5, _CPU_altivec_vrsave_initval@ha
	stw       r3, _CPU_altivec_vrsave_initval@l(r5)
	mtvrsave  r3
	blr

#ifdef ALTIVEC_TESTING
	.global msr_VE_on
msr_VE_on:
	mfmsr r3
	oris  r3, r3, 1<<(31-6-16)
	mtmsr r3
	blr

	.global msr_VE_off
msr_VE_off:
	mfmsr r3
	lis   r4,  1<<(31-6-16)
	andc  r3, r3, r4
	mtmsr r3
	blr


	.global mfvrsave
mfvrsave:
	mfvrsave r3
	blr

	.global mtvrsave
mtvrsave:
	mtvrsave r3
	blr

	/* Load all vector registers from memory area.
	 * NOTE: This routine is not strictly ABI compliant --
	 *       it guarantees that volatile vector registers
	 *       have certain values on exit!
	 */
	.global _CPU_altivec_load_all
_CPU_altivec_load_all:
	/* Align address up to next cache-line boundary */
	addi      r3, r3, PPC_CACHE_ALIGNMENT - 1
	CACHE_DOWNALGN r3
#ifndef IGNORE_VRSAVE
	/* Save CRC -- it is used implicitly by all the LOAD/STORE macros
	 * when testing if we really should do the load/store operation.
	 */
	mfcr      r12
#endif

	/* Try to preload 1st line (where vscr and vrsave are stored) */
	dcbt      0, r3
	/* Point to start of general vector-register area             */
	addi      r3, r3, PPC_CACHE_ALIGNMENT
	/* Start preloading 2nd line (where first two vectors are)    */
	dcbt      0, r3
	L_VSCR_VRSAVE r3, r0, v0
	CMP_BASES     r3, r4, r5, r6, r10
	/* Start preloading 3rd line (where vectors 3 and 4 are)      */
	dcbt      0, r5
	L_V0TOV31 r3, r4, r5, r6, r10, r11

#ifndef IGNORE_VRSAVE
	mtcr      r12
#endif
	blr

	.global _CPU_altivec_save_all
_CPU_altivec_save_all:
	/* Align address up to next cache-line boundary */
	addi      r3, r3, PPC_CACHE_ALIGNMENT - 1
	CACHE_DOWNALGN r3

#ifndef IGNORE_VRSAVE
	/* Save CRC -- it is used implicitly by all the LOAD/STORE macros
	 * when testing if we really should do the load/store operation.
	 */
	mfcr      r12
#endif

	PREP_FOR_SAVE r0, r3, r4, r5, r6, r10
	/* r0 now contains VRSAVE, r3 still the aligned memory area
	 * and r4, r5, r6 are offset by 16, 32, and 48 bytes from r3,
	 * respectively. r10 holds zero
	 */
	S_V0TOV31     _B0=r3, _B1=r4, _B2=r5, _B3=r6, _O1=r10, _O2=r11
	mfvscr        v0
	/* Store vrsave (still in r0) and vscr (in v0) to memory area */
	S_VSCR_VRSAVE r0, v0, r3, r11

#ifndef IGNORE_VRSAVE
	/* Restore CRC */
	mtcr      r12
#endif
	blr


#if 0
	.gnu_attribute 4,1
	.gnu_attribute 8,1
#endif

#endif
#endif
