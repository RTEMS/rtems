        #.section ".text"
	.section .entry_point_section,"awx",@progbits
        .global _start

_start:
        lis   %r1,_stack@h
        ori   %r1,%r1,_stack@l
        li     %r3,8192
        mtmsr   %r3

	/* Read-only small data */
	lis %r2, _SDA2_BASE_@h
	ori %r2, %r2,_SDA2_BASE_@l

	/* Read-write small data */
	lis %r13, _SDA_BASE_@h
	ori %r13, %r13,_SDA_BASE_@l

        bl      cmain
	.size _start, . - _start

	.global __eabi
__eabi:
	blr
	.size __eabi, . - __eabi

	.section ".reset","ax"
_reset:
	b _start
	.size _reset, . - _reset
