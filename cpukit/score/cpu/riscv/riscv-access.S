#include <rtems/asm.h>

#if __riscv_xlen != 32 && __riscv_xlen != 64
#error "Unsupported XLEN"
#endif

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error "Only little-endian RISC-V is supported"
#endif

/*
 * Byte swap the low 16 bits of \reg. The result is zero-extended to XLEN.
 */
.macro BSWAP16 reg, tmp
	slli	\tmp, \reg, 8
	srli	\reg, \reg, 8
	or	\reg, \reg, \tmp
	slli	\reg, \reg, __riscv_xlen - 16
	srli	\reg, \reg, __riscv_xlen - 16
.endm

/*
 * Byte swap the low 32 bits of \reg. On RV64 the bits above bit 31 of the
 * result are undefined and must be fixed up by the caller (sext.w or
 * slli/srli 32) if they matter. Bits above bit 31 of the input are ignored.
 */
.macro BSWAP32 reg, tmp1, tmp2, tmp3
	srli	\tmp1, \reg, 24
	andi	\tmp1, \tmp1, 0xff
	srli	\tmp2, \reg, 8
	li	\tmp3, 0xff00
	and	\tmp2, \tmp2, \tmp3
	or	\tmp1, \tmp1, \tmp2
	slli	\tmp2, \reg, 8
	li	\tmp3, 0xff0000
	and	\tmp2, \tmp2, \tmp3
	or	\tmp1, \tmp1, \tmp2
	slli	\reg, \reg, 24
	or	\reg, \reg, \tmp1
.endm

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
	PUBLIC(_ld_le16)
	PUBLIC(_ld_le32)
	PUBLIC(_st_le16)
	PUBLIC(_st_le32)

SYM(_ld8):
	lbu	a0, 0(a0)
	ret

SYM(_st8):
	sb	a1, 0(a0)
	ret

/* The CPU is little-endian, so the native and _le accessors are identical. */
SYM(_ld16):
SYM(_ld_le16):
	lhu	a0, 0(a0)
	ret

SYM(_st16):
SYM(_st_le16):
	sh	a1, 0(a0)
	ret

SYM(_ld32):
SYM(_ld_le32):
	lw	a0, 0(a0)
	ret

SYM(_st32):
SYM(_st_le32):
	sw	a1, 0(a0)
	ret

/*
 * On RV32 a 64-bit value is passed/returned in a register pair with the low
 * word in the lower-numbered register.
 */
SYM(_ld64):
#if __riscv_xlen == 64
	ld	a0, 0(a0)
#else
	lw	a1, 4(a0)
	lw	a0, 0(a0)
#endif
	ret

SYM(_st64):
#if __riscv_xlen == 64
	sd	a1, 0(a0)
#else
	sw	a1, 0(a0)
	sw	a2, 4(a0)
#endif
	ret

SYM(_ld_be16):
	lhu	a0, 0(a0)
	BSWAP16	a0, a1
	ret

SYM(_st_be16):
	BSWAP16	a1, a2
	sh	a1, 0(a0)
	ret

SYM(_ld_be32):
	lw	a0, 0(a0)
	BSWAP32	a0, a1, a2, a3
#if __riscv_xlen == 64
	sext.w	a0, a0
#endif
	ret

SYM(_st_be32):
	BSWAP32	a1, a2, a3, a4
	sw	a1, 0(a0)
	ret

SYM(_ld_be64):
#if __riscv_xlen == 64
	ld	a0, 0(a0)
	srli	a1, a0, 32
	BSWAP32	a1, a2, a3, a4
	BSWAP32	a0, a2, a3, a4
	slli	a0, a0, 32
	slli	a1, a1, 32
	srli	a1, a1, 32
	or	a0, a0, a1
#else
	lw	a1, 0(a0)
	lw	a0, 4(a0)
	BSWAP32	a0, a2, a3, a4
	BSWAP32	a1, a2, a3, a4
#endif
	ret

SYM(_st_be64):
#if __riscv_xlen == 64
	srli	a2, a1, 32
	BSWAP32	a2, a3, a4, a5
	BSWAP32	a1, a3, a4, a5
	slli	a1, a1, 32
	slli	a2, a2, 32
	srli	a2, a2, 32
	or	a1, a1, a2
	sd	a1, 0(a0)
#else
	BSWAP32	a1, a3, a4, a5
	BSWAP32	a2, a3, a4, a5
	sw	a2, 0(a0)
	sw	a1, 4(a0)
#endif
	ret
