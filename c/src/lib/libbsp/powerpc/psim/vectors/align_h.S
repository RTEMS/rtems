/*  align_h.s	1.1 - 95/12/04
 *
 *  This file contains the assembly code for the PowerPC 403
 *  alignment exception handler for RTEMS.
 *
 *  Based upon IBM provided code with the following release:
 *
 *  This source code has been made available to you by IBM on an AS-IS
 *  basis.  Anyone receiving this source is licensed under IBM
 *  copyrights to use it in any way he or she deems fit, including
 *  copying it, modifying it, compiling it, and redistributing it either
 *  with or without modifications.  No license under IBM patents or
 *  patent applications is to be implied by the copyright license.
 *
 *  Any user of this software should understand that IBM cannot provide
 *  technical support for this software and will not be responsible for
 *  any consequences resulting from the use of this software.
 *
 *  Any person who transfers this source code or any derivative work
 *  must include the IBM copyright notice, this paragraph, and the
 *  preceding two paragraphs in the transferred software.
 *
 *      COPYRIGHT   I B M   CORPORATION 1995
 *      LICENSED MATERIAL  -  PROGRAM PROPERTY OF I B M
 *
 * Modifications:
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 */

#include <rtems/asm.h>
#include <bsp.h>

.set    CACHE_SIZE,16           # cache line size of 32 bytes
.set    CACHE_SIZE_L2,4         # cache line size, log 2

.set	Open_gpr0,0
.set	Open_gpr1,4
.set	Open_gpr2,8
.set	Open_gpr3,12
.set	Open_gpr4,16
.set	Open_gpr5,20
.set	Open_gpr6,24
.set	Open_gpr7,28
.set	Open_gpr8,32
.set	Open_gpr9,36
.set	Open_gpr10,40
.set	Open_gpr11,44
.set	Open_gpr12,48
.set	Open_gpr13,52
.set	Open_gpr14,56
.set	Open_gpr15,60
.set	Open_gpr16,64
.set	Open_gpr17,68
.set	Open_gpr18,72
.set	Open_gpr19,76
.set	Open_gpr20,80
.set	Open_gpr21,84
.set	Open_gpr22,88
.set	Open_gpr23,92
.set	Open_gpr24,96
.set	Open_gpr25,100
.set	Open_gpr26,104
.set	Open_gpr27,108
.set	Open_gpr28,112
.set	Open_gpr29,116
.set	Open_gpr30,120
.set	Open_gpr31,124
.set	Open_xer,128
.set	Open_lr,132
.set	Open_ctr,136
.set	Open_cr,140
.set	Open_srr2,144
.set	Open_srr3,148
.set	Open_srr0,152
.set	Open_srr1,156

/*
 *  This code makes several assumptions for processing efficiency
 *  *  General purpose registers are continuous in the image, beginning with
 *     Open_gpr0
 *  *  Hash table is highly dependent on opcodes - opcode changes *will*
 *     require rework of the instruction decode mechanism.
 */

        .text
        .globl  align_h

	.align	CACHE_SIZE_L2
align_h:
        /*-----------------------------------------------------------------------
         * Store GPRs in Open Reg save area
         * Set up r2 as base reg, r1 pointing to Open Reg save area
         *----------------------------------------------------------------------*/
        stmw    r0,ALIGN_REGS(r0)
	li	r1,ALIGN_REGS
        /*-----------------------------------------------------------------------
         * Store special purpose registers in reg save area
         *----------------------------------------------------------------------*/
        mfxer   r7
        mflr    r8
        mfcr    r9
        mfctr   r10
        stw     r7,Open_xer(r1)
        stw     r8,Open_lr(r1)
        stw     r9,Open_cr(r1)
        stw     r10,Open_ctr(r1)
#if defined(ppc403) || defined(ppc405)
        mfspr	r7, srr2		/* SRR 2 */
        mfspr	r8, srr3		/* SRR 3 */
#endif
        mfspr	r9, srr0		/* SRR 0 */
        mfspr	r10, srr1		/* SRR 1 */
#if defined(ppc403) || defined(ppc405)
        stw     r7,Open_srr2(r1)
        stw     r8,Open_srr3(r1)
#endif
        stw     r9,Open_srr0(r1)
        stw     r10,Open_srr1(r1)

/*      Set up common registers */
#if defined(ppc403) || defined(ppc405)
        mfspr	r5, dear		/* DEAR: R5 is data exception address */
#endif
        lwz     r9,Open_srr0(r1)        /* get faulting instruction */
        addi    r7,r9,4                 /* bump instruction */
        stw     r7,Open_srr0(r1)        /* restore to image */
        lwz     r9, 0(r9)               /* retrieve actual instruction */
        rlwinm  r6,r9,18,25,29          /* r6 is RA * 4 field from instruction */
        rlwinm  r7,r9,6,26,31           /* r7 is primary opcode */
        bl      ref_point               /* establish addressibility */
ref_point:
        mflr    r11                     /* r11 is the anchor point for ref_point */
        addi    r10, r7, -31            /* r10 = r7 - 31 */
        rlwinm  r10,r10,2,2,31          /* r10 *= 4 */
        add     r10, r10, r11           /* r10 += anchor point */
        lwz     r10, primary_jt-ref_point(r10)
        mtlr    r10
        rlwinm  r8,r9,13,25,29          /* r8 is RD * 4 */
        la      r7,Open_gpr0(r1)        /* r7 is address of GPR 0 in list */
        blr
primary_jt:
        .long   xform
        .long   lwz
        .long   lwzu
        .long   0
        .long   0
        .long   stw
        .long   stwu
        .long   0
        .long   0
        .long   lhz
        .long   lhzu
        .long   lha
        .long   lhau
        .long   sth
        .long   sthu
        .long   lmw
        .long   stmw
/*
 *   handlers
 */
/*
 * xform instructions require an additional decode.  Fortunately, a relatively
 * simple hash step breaks the instructions out with no collisions
 */
xform:
        rlwinm  r7,r9,31,22,31          /* r7 is secondary opcode */
        rlwinm  r10,r7,27,5,31          /* r10 = r7 >> 5 */
        add     r10,r7,r10              /* r10 = r7 + r10 */
        rlwinm  r10,r10,2,25,29         /* r10 = (r10 & 0x1F) * 4 */
        add     r10,r10,r11             /* r10 += anchor point */
        lwz     r10, secondary_ht-ref_point(r10)
        mtlr    r10
        la      r7,Open_gpr0(r1)        /* r7 is address of GPR 0 in list */
        rlwinm  r8,r9,13,25,29          /* r8 is RD * 4 */
        blrl

secondary_ht:
        .long   lhzux                   /* b 0  0x137 */
        .long   lhax                    /* b 1  0x157 */
        .long   lhaux                   /* b 2  0x177 */
        .long   sthx                    /* b 3  0x197 */
        .long   sthux                   /* b 4  0x1b7 */
        .long   0                       /* b 5 */
        .long   lwbrx                   /* b 6  0x216 */
        .long   0                       /* b 7 */
        .long   0                       /* b 8 */
        .long   0                       /* b 9 */
        .long   stwbrx                  /* b A  0x296 */
        .long   0                       /* b B */
        .long   0                       /* b C */
        .long   0                       /* b D */
        .long   lhbrx                   /* b E   0x316 */
        .long   0                       /* b F */
        .long   0                       /* b 10 */
        .long   0                       /* b 11 */
        .long   sthbrx                  /* b 12  0x396 */
        .long   0                       /* b 13 */
        .long   lwarx                   /* b 14  0x014 */
        .long   dcbz                    /* b 15  0x3f6 */
        .long   0                       /* b 16 */
        .long   lwzx                    /* b 17  0x017 */
        .long   lwzux                   /* b 18  0x037 */
        .long   0                       /* b 19 */
        .long   stwcx                   /* b 1A  0x096 */
        .long   stwx                    /* b 1B  0x097 */
        .long   stwux                   /* b 1C  0x0B7 */
        .long   0                       /* b 1D */
        .long   0                       /* b 1E */
        .long   lhzx                    /* b 1F 0x117 */

/*
 * for all handlers
 *       r4 - Addressability to interrupt context
 *       r5 - DEAR address (faulting data address)
 *       r6 - RA field * 4
 *       r7 - Address of GPR 0 in image
 *       r8 - RD field * 4
 *       r9 - Failing instruction
 */

/*       Load halfword algebraic with update */
lhau:
/*       Load halfword algebraic with update indexed */
lhaux:
        stwx    r5,r7,r6                /* update RA with effective addr */

/*       Load halfword algebraic */
lha:
/*       Load halfword algebraic indexed */
lhax:
        lswi    r10,r5,2                /* load two bytes into r10 */
        srawi   r10,r10,16              /* shift right 2 bytes, extending sign */
        stwx    r10,r7,r8               /* update reg image */
        b       align_complete          /* return */

/*       Load Half Word Byte-Reversed Indexed */
lhbrx:
        lswi    r10,r5,2                /* load two bytes from DEAR into r10 */
        rlwinm  r10,r10,0,0,15          /* mask off lower 2 bytes */
        stwbrx  r10,r7,r8               /* store reversed in reg image */
        b       align_complete          /* return */

/*       Load Half Word and Zero with Update */
lhzu:
/*       Load Half Word and Zero with Update Indexed */
lhzux:
        stwx    r5,r7,r6                /* update RA with effective addr */

/*       Load Half Word and Zero */
lhz:
/*       Load Half Word and Zero Indexed */
lhzx:
        lswi    r10,r5,2                /* load two bytes from DEAR into r10 */
        rlwinm  r10,r10,16,16,31        /* shift right 2 bytes, with zero fill */
        stwx    r10,r7,r8               /* update reg image */
        b       align_complete          /* return */

/*
 *       Load Multiple Word
 */
lmw:
        lwzx    r9,r6,r7                /* R9 contains saved value of RA */
        addi    r10,r7,32*4             /* r10 points to r31 in image  + 4 */
        rlwinm  r8,r8,30,2,31           /* r8 >>= 2  (recovers RT) */
        subfic  r8,r8,32                /* r8 is reg count to load */
        mtctr   r8                      /* load counter */
        addi    r8,r8,-1                /* r8-- */
        rlwinm  r8,r8,2,2,31            /* r8 *= 4 */
        add     r5,r5,r8                /* update DEAR to point to last reg */
lwmloop:
        lswi    r11,r5,4                /* load r11 with 4 bytes from DEAR */
        stwu    r11,-4(r10)             /* load image and decrement pointer */
        addi    r5,r5,-4                /* decrement effective address */
        bdnz    lwmloop
        stwx    r9,r6,r7                /* restore RA (in case it was trashed) */
        b       align_complete          /* return */

/*
 *       Load Word and Reserve Indexed
 */
lwarx:
        lswi    r10,r5,4                /* load four bytes from DEAR into r10 */
        stwx    r10,r7,r8               /* update reg image */
        rlwinm  r5,r5,0,0,29            /* Word align address */
        lwarx   r10,0,r5                /* Set reservation */
        b       align_complete          /* return */

/*
 *       Load Word Byte-Reversed Indexed
 */
lwbrx:
        lswi    r10,r5,4                /* load four bytes from DEAR into r10 */
        stwbrx  r10,r7,r8               /* store reversed in reg image */
        b       align_complete          /* return */

/*       Load Word and Zero with Update */
lwzu:
/*       Load Word and Zero with Update Indexed */
lwzux:
        stwx    r5,r7,r6                /* update RA with effective addr */

/*       Load Word and Zero */
lwz:
/*       Load Word and Zero Indexed */
lwzx:
        lswi    r10,r5,4                /* load four bytes from DEAR into r10 */
        stwx    r10,r7,r8               /* update reg image */
        b       align_complete          /* return */

/*    Store instructions */

/* */
/*       Store Half Word and Update */
sthu:
/*       Store Half Word and Update Indexed */
sthux:
        stwx    r5,r7,r6                /* Update RA with effective address */

/*       Store Half Word */
sth:
/*       Store Half Word Indexed */
sthx:
        lwzx    r10,r8,r7               /* retrieve source register value */
        rlwinm  r10,r10,16,0,15         /* move two bytes to high end of reg */
        stswi   r10,r5,2                /* store bytes to DEAR address */
        b       align_complete          /* return */

/* */
/*       Store Half Word Byte-Reversed Indexed */
sthbrx:
        lwbrx   r10,r8,r7               /* retrieve src reg value byte reversed */
        stswi   r10,r5,2                /* move two bytes to DEAR address */
        b       align_complete          /* return */

/* */
/*       Store Multiple Word */
stmw:
        addi    r10,r7,32*4             /* r10 points to r31 in image  + 4 */
        rlwinm  r8,r8,30,2,31           /* r8 >>= 2  (recovers RT) */
        subfic  r8,r8,32                /* r8 is reg count to load */
        mtctr   r8                      /* load counter */
        addi    r8,r8,-1                /* r8-- */
        rlwinm  r8,r8,2,2,31            /* r8 *= 4 */
        add     r5,r5,r8                /* update DEAR to point to last reg */
stmloop:
        lwzu    r11,-4(r10)             /* get register value */
        stswi   r11,r5,4                /* output to DEAR address */
        addi    r5,r5,-4                /* decrement effective address */
        bdnz    stmloop
        b       align_complete          /* return */

/* */
/*       Store Word and Update */
stwu:
/*       Store Word and Update Indexed */
stwux:
        stwx    r5,r7,r6                /* Update RA with effective address */

/*       Store Word */
stw:
/*       Store Word Indexed */
stwx:
        lwzx    r10,r8,r7               /* retrieve source register value */
        stswi   r10,r5,4                /* store bytes to DEAR address */
        b       align_complete          /* return */

/* */
/*       Store Word Byte-Reversed Indexed */
stwbrx:
        lwbrx   r10,r8,r7               /* retrieve src reg value byte reversed */
        stswi   r10,r5,4                /* move two bytes to DEAR address */
        b       align_complete          /* return */

/* */
/*       Store Word Conditional Indexed */
stwcx:
        rlwinm  r10,r5,0,0,29           /* r10 = word aligned DEAR */
        lwz     r11,0(r10)              /* save original value of store */
        stwcx.  r11,r0,r10              /* attempt store to address */
        bne     stwcx_moveon            /* store failed, move on */
        stw     r11,0(r10)              /* repair damage */
        lwzx    r9,r7,r8                /* get register value */
        stswi   r10,r5,4                /* store bytes to DEAR address */
stwcx_moveon:
        mfcr    r11                     /* get condition reg */
        lwz     r9,Open_cr(r1)          /* get condition reg image */
        rlwimi  r9,r11,0,0,2            /* insert 3 CR bits into cr image */
        lwz     r11,Open_xer(r1)        /* get XER reg */
        rlwimi  r9,r11,29,2,2           /* insert XER SO bit into cr image */
        stw     r9,Open_cr(r1)          /* store cr image */
        b       align_complete          /* return */

/* */
/*       Data Cache Block Zero */
dcbz:
        rlwinm  r5,r5,0,0,31-CACHE_SIZE_L2
                                        /* get address to nearest Cache line */
        addi    r5,r5,-4                /* adjust by a word */
        addi    r10,r0,CACHE_SIZE/4     /* set counter value */
        mtctr   r10
        addi    r11,r0,0                /* r11 = 0 */
dcbz_loop:
        stwu    r11,4(r5)               /* store a word and update EA */
        bdnz    dcbz_loop
        b       align_complete          /* return */

align_complete:
        /*-----------------------------------------------------------------------
         * Restore regs and return from the interrupt
         *----------------------------------------------------------------------*/
        lmw     r24,Open_xer+ALIGN_REGS(r0)
        mtxer   r24
        mtlr    r25
        mtctr   r26
        mtcrf   0xFF, r27
#if defined(ppc403) || defined(ppc405)
        mtspr	srr2, r28		/* SRR 2 */
        mtspr	srr3, r29		/* SRR 3 */
#endif
        mtspr	srr0, r30		/* SRR 0 */
        mtspr	srr1, r31		/* SRR 1 */
        lmw     r1,Open_gpr1+ALIGN_REGS(r0)
        lwz     r0,Open_gpr0+ALIGN_REGS(r0)
        rfi
