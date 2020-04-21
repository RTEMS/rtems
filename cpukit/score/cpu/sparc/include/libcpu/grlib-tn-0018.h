/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 Cobham Gailer AB
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* NOTE: the lda should be on offset 0x18 */
#if defined(__FIX_LEON3FT_TN0018)

/* LEON3 Cache controller register accessed via ASI 2 */
#define ASI_CTRL 0x02
#define CCTRL_IP_BIT 15
#define CCTRL_ICS 0x3

/*
 * l3: (out) original cctrl
 * l4: (out) original cctrl with ics=0
 * NOTE: This macro modifies psr.icc.
 */
.macro TN0018_WAIT_IFLUSH out1 out2
1:
        ! wait for pending iflush to complete
        lda     [%g0] ASI_CTRL, \out1
        srl     \out1, CCTRL_IP_BIT, \out2
        andcc   \out2, 1, %g0
        bne     1b
         andn   \out1, CCTRL_ICS, \out2
.endm


.macro TN0018_WRITE_PSR src
        wr      \src, %psr
.endm

/* Prevent following jmp;rett sequence from "re-executing" due to cached RETT or source
 * registers (l1 and l2) containing bit faults triggering ECC.
 *
 * l3: (in) original cctrl
 * l4: (in) original cctrl with ics=0
 * NOTE: This macro MUST be immediately followed by the "jmp;rett" pair.
 */
.macro TN0018_FIX in1 in2
        .align  0x20                    ! align the sta for performance
        sta     \in2, [%g0] ASI_CTRL    ! disable icache
        nop                             ! delay for sta to have effect on rett
        or      %l1, %l1, %l1           ! delay + catch rf parity error on l1
        or      %l2, %l2, %l2           ! delay + catch rf parity error on l2
        sta     \in1, [%g0] ASI_CTRL     ! re-enable icache after rett
        nop                             ! delay ensures insn after gets cached
.endm

#else

.macro TN0018_WAIT_IFLUSH out1 out2
.endm

.macro TN0018_WRITE_PSR src
.endm

.macro TN0018_FIX in1 in2
.endm

#endif

