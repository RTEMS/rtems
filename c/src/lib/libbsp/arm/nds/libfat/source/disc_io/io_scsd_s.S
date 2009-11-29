@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	io_scsd_s.s
@
@	Hardware Routines for reading a Secure Digital card
@	using the SC SD
@
@	Based on code supplied by Romman
@
@ Copyright (c) 2006 Michael "Chishm" Chisholm
@
@ Redistribution and use in source and binary forms, with or without modification,
@ are permitted provided that the following conditions are met:
@
@  1. Redistributions of source code must retain the above copyright notice,
@     this list of conditions and the following disclaimer.
@  2. Redistributions in binary form must reproduce the above copyright notice,
@     this list of conditions and the following disclaimer in the documentation and/or
@     other materials provided with the distribution.
@  3. The name of the author may not be used to endorse or promote products derived
@     from this software without specific prior written permission.
@
@ THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
@ WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
@ AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
@ LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
@ DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
@ LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
@ THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
@ (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
@ EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
@
@	2006-07-22 - Chishm
@		* First release of stable code
@
@	2006-08-19 - Chishm
@		* Added SuperCard Lite support
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    .align 4
	.arm

	.equ	REG_SCSD_DATAWRITE,	0x09000000
	.equ	BYTES_PER_READ,	0x200
	.equ	SCSD_STS_BUSY,	0x100
	.equ	BUSY_WAIT_TIMEOUT, 0x10000
	.equ	FALSE,	0
	.equ	TRUE,	1

@ bool _SCSD_writeData_s (u8 *data, u16* crc)

    .global _SCSD_writeData_s

_SCSD_writeData_s:
	stmfd   r13!, {r4-r5}
	mov		r5, #BYTES_PER_READ
	mov		r2, #REG_SCSD_DATAWRITE

@ Wait for a free data buffer on the SD card
	mov		r4, #BUSY_WAIT_TIMEOUT
_SCSD_writeData_busy_wait:
	@ Test for timeout
	subs	r4, r4, #1
	moveq	r0, #FALSE			@ return false on failure
	beq		_SCSD_writeData_return
	@ Check the busy bit of the status register
	ldrh	r3, [r2]
	tst		r3,	#SCSD_STS_BUSY
	beq		_SCSD_writeData_busy_wait

	ldrh	r3, [r2]   			@ extra clock

	mov		r3, #0 				@ start bit
	strh	r3,[r2]

@ Check if the data buffer is aligned on a halfword boundary
	tst		r0, #1
	beq		_SCSD_writeData_data_loop

@ Used when the source data is unaligned
_SCSD_writeData_data_loop_unaligned:
		ldrb	r3, [r0], #1
		ldrb	r4, [r0], #1
		orr		r3, r3, r4, lsl #8
		stmia   r2, {r3-r4}
	subs    r5, r5, #2
    bne     _SCSD_writeData_data_loop_unaligned
	b		_SCSD_writeData_crc

@ Write the data to the card
@ 4 halfwords are transmitted to the Supercard at once, for timing purposes
@ Only the first halfword needs to contain data for standard SuperCards
@ For the SuperCard Lite, the data is split into 4 nibbles, one per halfword
_SCSD_writeData_data_loop:
		ldrh	r3, [r0], #2

@ This bit added for SCLite. Notice that the shift is not the same as in
@ the original (buggy) code supplied by Romman
		add	r3, r3, r3, lsl #20
		mov	r4, r3, lsr #8

		stmia   r2, {r3-r4}

	subs    r5, r5, #2
    bne     _SCSD_writeData_data_loop

@ Send the data CRC
_SCSD_writeData_crc:
	cmp		r1, #0
	movne	r0, r1
	movne	r1, #0
	movne	r5, #8
	bne		_SCSD_writeData_data_loop

	mov		r3, #0xff 			@ end bit
	strh	r3, [r2]

@ Wait for the SD card to signal that it is finished recieving
	mov		r4, #BUSY_WAIT_TIMEOUT
_SCSD_writeData_finished_wait:
	@ Test for timeout
	subs	r4, r4, #1
	moveq	r0, #FALSE			@ return false on failure
	beq		_SCSD_writeData_return
	@ Check the busy bit of the status register
	ldrh	r3, [r2]
	tst		r3, #0x100
	bne		_SCSD_writeData_finished_wait

@ Send 8 more clocks, as required by the SD card
	ldmia   r2, {r3-r4}

@ return true for success
	mov 	r0, #TRUE

_SCSD_writeData_return:
	ldmfd	r13!,{r4-r5}
	bx      r14

