/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Optimized access routines for SPARC.
 *
 *  Note the difference between byteorder.h (inlined functions) and access.S
 *  where the functions will be declared in the library archive librtemscpu.a.
 *  Function names starting with _ are in library and can be referenced by
 *  function pointers.
 *
 *  _ldN, _stN          standard machine endianess access (SPARC: big-endian)
 *  _ld_beN, _st_beN    forced big-endian
 *  _ld_leN, _st_leN    forced little-endian (defined in access_le.C)
 *
 *  This file is written in assembly because the big-endian functions maps to
 *  machine dependant access methods, i.e. same function has two names.
 *
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler.
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

#include <rtems/asm.h>

	.align 4
	.section ".text"
	PUBLIC(_ld8)
	PUBLIC(_ld16)
	PUBLIC(_ld32)
	PUBLIC(_ld64)
	PUBLIC(_st8)
	PUBLIC(_st16)
	PUBLIC(_st32)
	PUBLIC(_st64)
	PUBLIC(_ld_be16)
	PUBLIC(_ld_be32)
	PUBLIC(_ld_be64)
	PUBLIC(_st_be16)
	PUBLIC(_st_be32)
	PUBLIC(_st_be64)

SYM(_ld8):
	retl
	 ldub [%o0], %o0

SYM(_ld_be16):
SYM(_ld16):
	retl
	 lduh [%o0], %o0

SYM(_ld_be32):
SYM(_ld32):
	retl
	 ld [%o0], %o0

SYM(_ld_be64):
SYM(_ld64):
	retl
	 ldd [%o0], %o0

#if defined(__FIX_LEON3FT_B2BST)

SYM(_st8):
	stub %o1, [%o0]
	retl
	 nop

SYM(_st_be16):
SYM(_st16):
	stuh %o1, [%o0]
	retl
	 nop

SYM(_st_be32):
SYM(_st32):
	st %o1, [%o0]
	retl
	 nop

SYM(_st_be64):
SYM(_st64):
	mov %o2, %o3
	mov %o1, %o2
	std %o2, [%o0]
	retl
	 nop

#else

SYM(_st8):
	retl
	 stb %o1, [%o0]

SYM(_st_be16):
SYM(_st16):
	retl
	 sth %o1, [%o0]

SYM(_st_be32):
SYM(_st32):
	retl
	 st %o1, [%o0]

SYM(_st_be64):
SYM(_st64):
	mov %o2, %o3
	mov %o1, %o2
	retl
	 std %o2, [%o0]
#endif
