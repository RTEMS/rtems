/*
 *  System call support for simulator in gdb.
 *  Copied from newlib 1.16.0.
 */

;;;; libc/machine/h8300/setarch.h
#ifdef __H8300H__
#ifdef __NORMAL_MODE__
	.h8300hn
#else
	.h8300h
#endif
#endif
#ifdef __H8300S__
#ifdef __NORMAL_MODE__
	.h8300sn
#else
	.h8300s
#endif
#endif
#ifdef __H8300SX__
#ifdef __NORMAL_MODE__
	.h8300sxn
#else
	.h8300sx
#endif
#endif

;;;; libc/sys/h8300/write.S
;ssize_t _sys_write(int fd, const void *buf, size_t count);
;Integer arguments have to be zero extended.

; #include "setarch.h"

	.section .text
	.align 2
	.global __sys_write
__sys_write:
#if defined(__H8300H__) || defined(__H8300S__) || defined (__H8300SX__)
#if __INT_MAX__ == 32767
	extu.l	er0
#endif
#endif
	jsr @@0xc7
	rts
	.end
