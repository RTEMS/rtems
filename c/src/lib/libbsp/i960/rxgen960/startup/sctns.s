	.file	"sctns.c"
gcc2_compiled.:
___gnu_compiled_c:
.stabs "/usr/src/rtems-970904/c/src/lib/libbsp/i960/pmc901/startup/",100,0,0,Ltext0
.stabs "sctns.c",100,0,0,Ltext0
.text
Ltext0:
.stabs "int:t(0,1)=r(0,1);-2147483648;2147483647;",128,0,0,0
.stabs "char:t(0,2)=r(0,2);0;255;",128,0,0,0
.stabs "long int:t(0,3)=r(0,3);-2147483648;2147483647;",128,0,0,0
.stabs "unsigned int:t(0,4)=r(0,4);0;-1;",128,0,0,0
.stabs "long unsigned int:t(0,5)=r(0,5);0;-1;",128,0,0,0
.stabs "long long int:t(0,6)=r(0,1);01000000000000000000000;0777777777777777777777;",128,0,0,0
.stabs "long long unsigned int:t(0,7)=r(0,1);0000000000000;01777777777777777777777;",128,0,0,0
.stabs "short int:t(0,8)=r(0,8);-32768;32767;",128,0,0,0
.stabs "short unsigned int:t(0,9)=r(0,9);0;65535;",128,0,0,0
.stabs "signed char:t(0,10)=r(0,10);-128;127;",128,0,0,0
.stabs "unsigned char:t(0,11)=r(0,11);0;255;",128,0,0,0
.stabs "float:t(0,12)=r(0,1);4;0;",128,0,0,0
.stabs "double:t(0,13)=r(0,1);8;0;",128,0,0,0
.stabs "long double:t(0,14)=r(0,1);8;0;",128,0,0,0
.stabs "complex int:t(0,15)=s8real:(0,1),0,32;imag:(0,1),32,32;;",128,0,0,0
.stabs "complex float:t(0,16)=r(0,16);4;0;",128,0,0,0
.stabs "complex double:t(0,17)=r(0,17);8;0;",128,0,0,0
.stabs "complex long double:t(0,18)=r(0,18);8;0;",128,0,0,0
.stabs "void:t(0,19)=(0,19)",128,0,0,0
.stabs "sctns.h",130,0,0,0
.stabn 162,0,0,0
.stabs "pmc901_memmap.h",130,0,0,0
.stabn 162,0,0,0
	.align 4
.stabs "ledcyc:F(0,19)",36,0,13,_ledcyc
	.globl _ledcyc
	#  Function 'ledcyc'
	#  Registers used: g0 g1 g4 g5 g6 g7 cc 
	#		   
_ledcyc:
.stabn 68,0,13,LM1-_ledcyc
LM1:
	addo	16,sp,sp
	#Prologue stats:
	#  Total Frame Size: 16 bytes
	#  Local Variable Size: 16 bytes
	#End Prologue#
.stabn 68,0,14,LM2-_ledcyc
LM2:
LBB2:
.stabn 68,0,18,LM3-_ledcyc
LM3:
	ld	_led_array,g1
.stabn 68,0,19,LM4-_ledcyc
LM4:
	mov	1,g6
L9:
.stabn 68,0,21,LM5-_ledcyc
LM5:
	cmpibge	9,g6,L10
.stabn 68,0,22,LM6-_ledcyc
LM6:
	mov	1,g6
L10:
.stabn 68,0,23,LM7-_ledcyc
LM7:
	ldob	(g1)[g6*1],g4
.stabn 68,0,24,LM8-_ledcyc
LM8:
	mov	1,g5
	ldconst	0xffff,g0
	addo	31,2,g7	# ldconst 33,g7
.stabn 68,0,23,LM9-_ledcyc
LM9:
	stob	g4,-1073741312
L14:
.stabn 68,0,25,LM10-_ledcyc
LM10:
	ld	-12(sp),g4
.stabn 68,0,24,LM11-_ledcyc
LM11:
	addo	g5,1,g5
.stabn 68,0,25,LM12-_ledcyc
LM12:
	addo	g4,g7,g4
	st	g4,-16(sp)
.stabn 68,0,24,LM13-_ledcyc
LM13:
	cmpibge	g0,g5,L14
.stabn 68,0,26,LM14-_ledcyc
LM14:
	addo	g6,1,g6
.stabn 68,0,27,LM15-_ledcyc
LM15:
	b	L9
.stabn 68,0,28,LM16-_ledcyc
LM16:
LBE2:
.stabs "k:(0,1)",128,0,17,-16
.stabs "m:(0,1)",128,0,17,-12
.stabn 192,0,0,LBB2-_ledcyc
.stabn 224,0,0,LBE2-_ledcyc
	.align 4
.stabs "copyCodeToRom:F(0,19)",36,0,31,_copyCodeToRom
	.globl _copyCodeToRom
	#  Function 'copyCodeToRom'
	#  Registers used: g0 g1 g2 g3 g4 g5 g6 g7 cc 
	#		   
	.globl	copyCodeToRom.lf
	.leafproc	_copyCodeToRom,copyCodeToRom.lf
_copyCodeToRom:
	lda    LR2,g14
copyCodeToRom.lf:
	mov    g14,g3
	mov    0,g14
.stabn 68,0,31,LM17-_copyCodeToRom
LM17:
.stabn 68,0,32,LM18-_copyCodeToRom
LM18:
LBB3:
.stabn 68,0,40,LM19-_copyCodeToRom
LM19:
	ldconst	_codeRamStart,g6
.stabn 68,0,41,LM20-_copyCodeToRom
LM20:
	st	g14,(g6)
.stabn 68,0,42,LM21-_copyCodeToRom
LM21:
	ld	(g6),g4
.stabn 68,0,39,LM22-_copyCodeToRom
LM22:
	mov	1,g2
.stabn 68,0,42,LM23-_copyCodeToRom
LM23:
	cmpobne	0,g4,L18
.stabn 68,0,45,LM24-_copyCodeToRom
LM24:
	subo	1,0,g5	# ldconst -1,g5
	st	g5,(g6)
.stabn 68,0,46,LM25-_copyCodeToRom
LM25:
	ld	(g6),g4
.stabn 68,0,44,LM26-_copyCodeToRom
LM26:
	mov	2,g2
.stabn 68,0,46,LM27-_copyCodeToRom
LM27:
	cmpobne	g5,g4,L18
.stabn 68,0,48,LM28-_copyCodeToRom
LM28:
	mov	3,g2
.stabn 68,0,49,LM29-_copyCodeToRom
LM29:
	mov	1,g5
.stabn 68,0,50,LM30-_copyCodeToRom
LM30:
	mov	0,g7
L23:
.stabn 68,0,51,LM31-_copyCodeToRom
LM31:
	st	g5,(g6)
.stabn 68,0,52,LM32-_copyCodeToRom
LM32:
	ld	(g6),g4
	cmpobne	g5,g4,L18
.stabn 68,0,54,LM33-_copyCodeToRom
LM33:
	shlo	1,g5,g5
.stabn 68,0,50,LM34-_copyCodeToRom
LM34:
	addo	g7,1,g7
	cmpobge	30,g7,L23
.stabn 68,0,56,LM35-_copyCodeToRom
LM35:
	ldconst	_codeRamStart,g5
.stabn 68,0,57,LM36-_copyCodeToRom
LM36:
	stob	g14,(g5)
.stabn 68,0,58,LM37-_copyCodeToRom
LM37:
	ldob	(g5),g4
	cmpobne	0,g4,L18
.stabn 68,0,60,LM38-_copyCodeToRom
LM38:
	mov	15,g4
	stob	g4,(g5)
.stabn 68,0,61,LM39-_copyCodeToRom
LM39:
	ldob	(g5),g4
	cmpobne	15,g4,L18
.stabn 68,0,64,LM40-_copyCodeToRom
LM40:
	ldconst	_codeRomStart,g7
	mov	g5,g6
	ldconst	_codeRamEnd,g5
	cmpoble	g5,g6,L29
L31:
.stabn 68,0,65,LM41-_copyCodeToRom
LM41:
	ld	(g7),g4
	st	g4,(g6)
.stabn 68,0,64,LM42-_copyCodeToRom
LM42:
	addo	g7,4,g7
	addo	g6,4,g6
	cmpobg	g5,g6,L31
L29:
.stabn 68,0,67,LM43-_copyCodeToRom
LM43:
	mov	5,g2
.stabn 68,0,68,LM44-_copyCodeToRom
LM44:
	ldconst	_codeRomStart,g7
	ldconst	_codeRamStart,g6
	ldconst	_codeRamEnd,g0
	cmpoble	g0,g6,L34
L36:
.stabn 68,0,69,LM45-_copyCodeToRom
LM45:
	ld	(g6),g5
	ld	(g7),g4
	cmpobne	g4,g5,L18
.stabn 68,0,68,LM46-_copyCodeToRom
LM46:
	addo	g7,4,g7
	addo	g6,4,g6
	cmpobg	g0,g6,L36
L34:
.stabn 68,0,73,LM47-_copyCodeToRom
LM47:
	bx	(g3)
L18:
.stabn 68,0,76,LM48-_copyCodeToRom
LM48:
	ld	_led_array,g0
	ldconst	0xfffff,g1
	ldconst	0xc0000200,g7
L41:
.stabn 68,0,77,LM49-_copyCodeToRom
LM49:
	mov	1,g5
L45:
.stabn 68,0,78,LM50-_copyCodeToRom
LM50:
	stob	g2,(g7)
.stabn 68,0,77,LM51-_copyCodeToRom
LM51:
	addo	g5,1,g5
	cmpobge	g1,g5,L45
.stabn 68,0,79,LM52-_copyCodeToRom
LM52:
	mov	1,g5
	mov	g1,g6
L50:
.stabn 68,0,80,LM53-_copyCodeToRom
LM53:
	ldob	(g0),g4
.stabn 68,0,79,LM54-_copyCodeToRom
LM54:
	addo	g5,1,g5
.stabn 68,0,80,LM55-_copyCodeToRom
LM55:
	stob	g4,(g7)
.stabn 68,0,79,LM56-_copyCodeToRom
LM56:
	cmpobge	g6,g5,L50
.stabn 68,0,81,LM57-_copyCodeToRom
LM57:
	b	L41
.stabn 68,0,83,LM58-_copyCodeToRom
LM58:
LBE3:
LR2:	ret
.stabs "errval:r(0,1)",64,0,32,18
.stabs "s:r(0,20)=*(0,4)",64,0,33,23
.stabs "d:r(0,21)=*(0,4)",64,0,34,22
.stabs "t:r(0,4)",64,0,35,21
.stabs "i:r(0,4)",64,0,35,23
.stabs "z:r(0,22)=*(0,11)",64,0,36,21
.stabn 192,0,0,LBB3-_copyCodeToRom
.stabn 224,0,0,LBE3-_copyCodeToRom
	.align 4
.stabs "zeroBss:F(0,19)",36,0,85,_zeroBss
	.globl _zeroBss
	#  Function 'zeroBss'
	#  Registers used: g0 g4 g5 cc 
	#		   
	.globl	zeroBss.lf
	.leafproc	_zeroBss,zeroBss.lf
_zeroBss:
	lda    LR3,g14
zeroBss.lf:
	mov    g14,g0
	mov    0,g14
.stabn 68,0,85,LM59-_zeroBss
LM59:
.stabn 68,0,86,LM60-_zeroBss
LM60:
LBB4:
.stabn 68,0,90,LM61-_zeroBss
LM61:
	ldconst	_bssStart,g4
	ldconst	_bssEnd,g5
	cmpoble	g5,g4,L65
L67:
.stabn 68,0,91,LM62-_zeroBss
LM62:
	st	g14,(g4)
.stabn 68,0,90,LM63-_zeroBss
LM63:
	addo	g4,4,g4
	cmpobg	g5,g4,L67
L65:
.stabn 68,0,93,LM64-_zeroBss
LM64:
	ldconst	_bssStart_1,g4
	ldconst	_bssEnd_1,g5
	cmpoble	g5,g4,L70
L72:
.stabn 68,0,94,LM65-_zeroBss
LM65:
	st	g14,(g4)
.stabn 68,0,93,LM66-_zeroBss
LM66:
	addo	g4,4,g4
	cmpobg	g5,g4,L72
	bx	(g0)
L70:
.stabn 68,0,96,LM67-_zeroBss
LM67:
LBE4:
	bx	(g0)
LR3:	ret
.stabn 192,0,0,LBB4-_zeroBss
.stabn 224,0,0,LBE4-_zeroBss
	.text
	.stabs "",100,0,0,Letext
Letext:
