; /* @(#)register.s	1.1 96/05/23 08:57:34, TEI */
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;		 Register Definitions and Usage Conventions 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  /* $Id$ */
;

;* File information and includes.

	.file	"c_register.s"
	.ident	"@(#)register.s	1.1 96/05/23 08:57:34, TEI\n"

; Basic guidelines for register distribution and usage are derived from
; the AMD application notes. It would be best to stick with the conventions
; laid out by AMD.
; Application Note: Context Switching with 29000 Processor By Daniel Mann.

;
;*************************************************************************
;

;
; Rule 1:
;	Gr1 is used as a pointer to the register stack
;	Lr1 is used as frame pointer
;

	.reg	regsp, 		gr1		; Register Stack Pointer
	.reg 	fp, 		lr1		; frame pointer

	.equ 	Rrsp,		&regsp
	.equ 	Rfp,		&fp

	.global	Rrsp, Rfp

;
;*************************************************************************
;

;
;	Gr2-Gr63 are not implemented in silicon
;

;
;*************************************************************************
;

;
; Rule 2:
; The registers GR64-GR95 are dedicated for operating system use.
;

; The  register range GR64-GR95 i.e 32 Registers  is furthur sub-divided as
; follows...
; gr64-gr67 interrupt handlers.
; gr68-gr71 OS temporaries I
; gr72-gr79 OS temporaries II
; gr80-gr95 OS statics. Dedicated throughout the operation of a program.


;
;	32 Registers for Operating System Use.
;

;
;	Assigning Names to Interrupt Handlers Registers.
;

	.reg	OSint0,		gr64
	.reg	OSint1,		gr65
	.reg	OSint2,		gr66
	.reg	OSint3,		gr67

	.equ	ROSint0,	&OSint0
	.equ	ROSint1,	&OSint1
	.equ	ROSint2,	&OSint2
	.equ	ROSint3,	&OSint3

	.global	ROSint0, ROSint1, ROSint2, ROSint3

	.reg 	TrapReg,	gr64		; trap register
        .reg    trapreg, 	gr64		; trapreg

	.equ	RTrapReg,	&TrapReg
	.equ	Rtrapreg,	&trapreg

	.global	RTrapReg, Rtrapreg


;
;	Assigning Names to Scratch/Temporary Registers.
;

	.reg	OStmp0,		gr68
	.reg	OStmp1,		gr69
	.reg	OStmp2,		gr70
	.reg	OStmp3,		gr71

	.reg	OStmp4,		gr72
	.reg	OStmp5,		gr73
	.reg	OStmp6,		gr74
	.reg	OStmp7,		gr75

	.reg	OStmp8,		gr76
	.reg	OStmp9,		gr77
	.reg	OStmp10,	gr78
	.reg	OStmp11,	gr79

	.equ	ROStmp0,	&OStmp0
	.equ	ROStmp1,	&OStmp1
	.equ	ROStmp2,	&OStmp2
	.equ	ROStmp3,	&OStmp3

	.equ	ROStmp4,	&OStmp4
	.equ	ROStmp5,	&OStmp5
	.equ	ROStmp6,	&OStmp6
	.equ	ROStmp7,	&OStmp7

	.equ	ROStmp8,	&OStmp8
	.equ	ROStmp9,	&OStmp9
	.equ	ROStmp10,	&OStmp10
	.equ	ROStmp11,	&OStmp11

	.global	ROStmp0, ROStmp1, ROStmp2, ROStmp3
	.global	ROStmp4, ROStmp5, ROStmp6, ROStmp7
	.global	ROStmp8, ROStmp9, ROStmp10, ROStmp11

;
;	Assigning Names to Statics/Permanent Registers.
;

	.reg	OSsta0,		gr80		; Spill Address Register
	.reg	OSsta1,		gr81		; Fill Address Register
	.reg	OSsta2,		gr82		; Signal Address Register
	.reg	OSsta3,		gr83		; pcb Register

	.reg	OSsta4,		gr84		; 
	.reg	OSsta5,		gr85		;
	.reg	OSsta6,		gr86		; 
	.reg	OSsta7,		gr87		;

	.reg	OSsta8,		gr88		; 
	.reg	OSsta9,		gr89		; 
	.reg	OSsta10,	gr90		; 
	.reg	OSsta11,	gr91		; 

	.reg	OSsta12,	gr92		; 
	.reg	OSsta13,	gr93		;
	.reg	OSsta14,	gr94		;
	.reg	OSsta15,	gr95		;

;
;	Round 2 of Name Assignments
;

;
;	Assignment of Specific Use oriented names to statics.
;
	.reg	SpillAddrReg,	gr80
	.reg	FillAddrReg,	gr81
	.reg	SignalAddrReg,	gr82
	.reg	pcb,		gr83

	.reg	etx,		gr80
	.reg	ety,		gr81
	.reg	etz,		gr82
	.reg	etc,		gr83

;*

	.reg	TimerExt,	gr84
	.reg	TimerUtil,	gr85

;*

	.reg	LEDReg,		gr86
	.reg	ERRReg,		gr87

	.reg	eta,		gr86
	.reg	etb,		gr87

;*


;* The following registers are used by switching code

	.reg	et0,		gr88
	.reg	et1,		gr89
	.reg	et2,		gr90
	.reg	et3,		gr91

	.reg	et4,		gr92
	.reg	et5,		gr93
	.reg	et6,		gr94
	.reg	et7,		gr95


; The floating point trap handlers need a few static registers

        .reg    FPStat0, 	gr88
        .reg    FPStat1, 	gr89
        .reg    FPStat2, 	gr90
        .reg    FPStat3, 	gr91

; The following registers are used temporarily during diagnostics.

	.reg	XLINXReg,	gr92
	.reg	VMBCReg,	gr93
	.reg	UARTReg,	gr94
	.reg	ETHERReg,	gr95

;*

;;*
	.reg	heapptr,	gr90
	.reg	ArgvPtr,	gr91
;;*



;
;*	Preparing to export Register Names for the Linkers benefit.
;

	.equ	RSpillAddrReg,	&SpillAddrReg
	.equ	RFillAddrReg,	&FillAddrReg
	.equ	RSignalAddrReg,	&SignalAddrReg
	.equ	Rpcb,		&pcb

	.equ	Retx,		&etx
	.equ	Rety,		&ety
	.equ	Retz,		&etz
	.equ	Reta,		&eta

	.equ	Retb,		&etb
	.equ	Retc,		&etc
	.equ	RTimerExt,	&TimerExt
	.equ	RTimerUtil,	&TimerUtil

	.equ	RLEDReg,	&LEDReg
	.equ	RERRReg,	&ERRReg

	.equ	Ret0,		&et0
	.equ	Ret1,		&et1
	.equ	Ret2,		&et2
	.equ	Ret3,		&et3

        .equ    RFPStat0, 	&FPStat0
        .equ    RFPStat1, 	&FPStat1
        .equ    RFPStat2, 	&FPStat2
        .equ    RFPStat3, 	&FPStat3

        .equ    Rheapptr, 	&heapptr
        .equ    RHeapPtr, 	&heapptr
        .equ    RArgvPtr, 	&ArgvPtr

	.equ	Ret4,		&et4
	.equ	Ret5,		&et5
	.equ	Ret6,		&et6
	.equ	Ret7,		&et7

	.equ	RXLINXReg,	&XLINXReg
	.equ	RVMBCReg,	&VMBCReg
	.equ	RUARTReg,	&UARTReg
	.equ	RETHERReg,	&ETHERReg

	.global	RSpillAddrReg, RFillAddrReg, RSignalAddrReg
	.global	Rpcb, Retc
	.global	RTimerExt, RTimerUtil, RLEDReg, RERRReg
	.global	Ret0, Ret1, Ret2, Ret3, Ret4, Ret5, Ret6, Ret7, Reta, Retb
	.global	Retx, Rety, Retz
	.global	RFPStat0, RFPStat1, RFPStat2, RFPStat3
	.global	Rheapptr, RHeapPtr, RArgvPtr
	.global	RXLINXReg, RVMBCReg, RUARTReg, RETHERReg

;
;*************************************************************************
;


;
; Rule 3:
;	Gr96-Gr127 Compiler & Programmer use registers.
;	32 Registers for Compiler & Programmer use

;
;	16 Registers for Compiler Use.
;

;
;	Compiler Temporaries and Function Return Values
;

	.reg v0,		gr96		; First word of Return Value
	.reg v1,		gr97
     	.reg v2,		gr98
     	.reg v3,		gr99

     	.reg v4,		gr100
     	.reg v5, 		gr101
     	.reg v6,		gr102
     	.reg v7,		gr103

     	.reg v8,		gr104
     	.reg v9,		gr105
     	.reg v10,		gr106
     	.reg v11,		gr107

     	.reg v12,		gr108
     	.reg v13,		gr109
     	.reg v14,		gr110
     	.reg v15,		gr111

	.equ Rv0,		&v0
	.equ Rv1,		&v1
	.equ Rv2,		&v2
	.equ Rv3,		&v3

	.equ Rv4,		&v4
	.equ Rv5,		&v5
	.equ Rv6,		&v6
	.equ Rv7,		&v7

	.equ Rv8,		&v8
	.equ Rv9,		&v9
	.equ Rv10,		&v10
	.equ Rv11,		&v11

	.equ Rv12,		&v12 
	.equ Rv13,		&v13
	.equ Rv14,		&v14
	.equ Rv15,		&v15

	.global Rv0, Rv1, Rv2, Rv3, Rv4, Rv5, Rv6, Rv7, Rv8, Rv9
	.global Rv10, Rv11, Rv12, Rv13, Rv14, Rv15


;
;	User Process Statics Registers
;

	.reg rp0, 		gr112		; Reserved for Programmer, #0
	.reg rp1, 		gr113		; Reserved for Programmer, #1
	.reg rp2, 		gr114		; Reserved for Programmer, #2
	.reg rp3, 		gr115		; Reserved for Programmer, #3

	.equ Rrp0,		&rp0
	.equ Rrp1,		&rp1
	.equ Rrp2,		&rp2
	.equ Rrp3,		&rp3

	.global	Rrp0, Rrp1, Rrp2, Rrp3

;
;	Compiler Temporaries II
;

	.reg tv0,		gr116		; 
	.reg tv1,		gr117		;
     	.reg tv2,		gr118		;
     	.reg tv3,		gr119		;
     	.reg tv4,		gr120		;

	.equ Rtv0,		&tv0		;
	.equ Rtv1,		&tv1		;
	.equ Rtv2,		&tv2		;
	.equ Rtv3,		&tv3		;
	.equ Rtv4,		&tv4		;

	.global	Rtv0, Rtv1, Rtv2, Rtv3, Rtv4

;
;	Special pointers and registers for handlers and stack operations.
;

	.reg tav,		gr121		; Temp, Arg for Trap Handlers
	.reg tpc,		gr122		; Temp, Ret PC for Trap Handlers
	.reg lrp,		gr123		; Large Return Pointer
	.reg slp,		gr124		; Static Link Pointer

	.reg msp,		gr125		; Memory Stack Pointer
	.reg rab,		gr126		; Register Allocate Bound
	.reg rfb,		gr127		; Register Free Bound

	.equ Rtav,		&tav
	.equ Rtpc,		&tpc
	.equ Rlrp,		&lrp
	.equ Rslp,		&slp
	.equ Rmsp,		&msp
	.equ Rrab,		&rab
	.equ Rrfb,		&rfb

	.global	Rtav, Rtpc, Rlrp, Rslp, Rmsp, Rrab, Rrfb
