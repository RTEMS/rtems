; h8/300 and h8/300h start up file.

#ifdef __H8300__

	.text
	.global	_start
_start:
	mov.w	#_stack_init,sp
	mov.w	#_edata,r0
	mov.w	#_clear_end,r1
	mov.w	#0,r2
.loop:	mov.w	r2,@r0
	adds	#2,r0
	cmp	r1,r0
	blo	.loop
        ;       call __main
	mov.w	#0,r0		; pass in NULL
	jsr	@_boot_card
	mov.w	#0,r0		; indicate reason to exit
	sleep
	jmp	@_start 	; restart
#else

#if defined(__H8300H__)
	.h8300h
#else /* must be __H300S__ */
	.h8300s
#endif
	.text
	.global	_start
_start:
	mov.l	#_stack_init,sp
	mov.l	#_edata,er0
	mov.l	#_clear_end,er1
	mov.w	#0,r2		; not sure about alignment requirements
.loop:	mov.w	r2,@er0		; playing it safe for now
	adds	#2,er0
	cmp.l	er1,er0
	blo	.loop
        ;       call __main
	jsr	@_boot_card
	mov.w	#0,r0		; indicate reason to exit
	sleep
	jmp	@_start 	; restart

#endif /* end of H8300 */
