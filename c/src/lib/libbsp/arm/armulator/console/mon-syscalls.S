#ifdef __STDC__
# define _C_LABEL(x)    _ ## x
#else
# define _C_LABEL(x)    _/**/x
#endif
#define _ASM_LABEL(x)   x

#define _ENTRY(name)    \
        .text; .align 4; .globl name; name:

#define ENTRY(name)     \
        _ENTRY(_C_LABEL(name))



ENTRY(_sys_exit)
	lda	257,g13
	calls	g13

ENTRY(_sys_open)
	lda	230,g13
        calls	g13
	ret

ENTRY(_sys_read)
        lda	231,g13
	calls	g13
	ret

ENTRY(_sys_write)
	lda	232,g13
	calls	g13
	ret

ENTRY(_sys_lseek)
	lda	233,g13
	calls	g13
	ret

ENTRY(_sys_close)
	lda	234,g13
	calls	g13
	ret
