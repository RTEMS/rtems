/*  cpu_asm.s
 *
 *  This file contains all assembly code for the Intel i386 implementation
 *  of RTEMS.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <asm.h>

/*
 * Format of i386 Register structure
 */

.set REG_EFLAGS,  0
.set REG_ESP,     REG_EFLAGS + 4
.set REG_EBP,     REG_ESP + 4
.set REG_EBX,     REG_EBP + 4
.set REG_ESI,     REG_EBX + 4
.set REG_EDI,     REG_ESI + 4
.set SIZE_REGS,   REG_EDI + 4

        BEGIN_CODE

/*
 *  void _CPU_Context_switch( run_context, heir_context )
 *
 *  This routine performs a normal non-FP context.
 */

        .p2align  1
        PUBLIC (_CPU_Context_switch)

.set RUNCONTEXT_ARG,   4                   # save context argument
.set HEIRCONTEXT_ARG,  8                   # restore context argument

SYM (_CPU_Context_switch):
        movl      RUNCONTEXT_ARG(esp),eax  # eax = running threads context
        pushf                              # push eflags
        popl      REG_EFLAGS(eax)          # save eflags
        movl      esp,REG_ESP(eax)         # save stack pointer
        movl      ebp,REG_EBP(eax)         # save base pointer
        movl      ebx,REG_EBX(eax)         # save ebx
        movl      esi,REG_ESI(eax)         # save source register
        movl      edi,REG_EDI(eax)         # save destination register

        movl      HEIRCONTEXT_ARG(esp),eax # eax = heir threads context

restore:
        pushl     REG_EFLAGS(eax)          # push eflags
        popf                               # restore eflags
        movl      REG_ESP(eax),esp         # restore stack pointer
        movl      REG_EBP(eax),ebp         # restore base pointer
        movl      REG_EBX(eax),ebx         # restore ebx
        movl      REG_ESI(eax),esi         # restore source register
        movl      REG_EDI(eax),edi         # restore destination register
        ret

/*
 *  NOTE: May be unnecessary to reload some registers.
 */

/*
 *  void _CPU_Context_restore( new_context )
 *
 *  This routine performs a normal non-FP context.
 */

        PUBLIC (_CPU_Context_restore)

.set NEWCONTEXT_ARG,   4                   # context to restore argument

SYM (_CPU_Context_restore):

        movl      NEWCONTEXT_ARG(esp),eax  # eax = running threads context
        jmp       restore

/*PAGE
 *  void _CPU_Context_save_fp_context( &fp_context_ptr )
 *  void _CPU_Context_restore_fp_context( &fp_context_ptr )
 *
 *  This section is used to context switch an i80287, i80387,
 *  the built-in coprocessor or the i80486 or compatible.
 */

.set FPCONTEXT_ARG,   4                    # FP context argument

        .p2align  1
        PUBLIC (_CPU_Context_save_fp)
SYM (_CPU_Context_save_fp):
        movl      FPCONTEXT_ARG(esp),eax   # eax = &ptr to FP context area
        movl      (eax),eax                # eax = FP context area
        fsave     (eax)                    # save FP context
        ret

        .p2align  1
        PUBLIC (_CPU_Context_restore_fp)
SYM (_CPU_Context_restore_fp):
        movl      FPCONTEXT_ARG(esp),eax   # eax = &ptr to FP context area
        movl      (eax),eax                # eax = FP context area
        frstor    (eax)                    # restore FP context
        ret

/*PAGE
 *  void _ISR_Handler()
 *
 *  This routine provides the RTEMS interrupt management.
 *
 *  NOTE:
 *    Upon entry, the stack will contain a stack frame back to the
 *    interrupted task.  If dispatching is enabled, this is the
 *    outer most interrupt, and (a context switch is necessary or
 *    the current task has signals), then set up the stack to
 *    transfer control to the interrupt dispatcher.
 */

.set SET_SEGMENT_REGISTERS_IN_INTERRUPT, 0

.set SAVED_REGS     , 32                   # space consumed by saved regs
.set EIP_OFFSET     , SAVED_REGS           # offset of tasks eip
.set CS_OFFSET      , EIP_OFFSET+4         # offset of tasks code segment
.set EFLAGS_OFFSET  , CS_OFFSET+4          # offset of tasks eflags

        .p2align  1
        PUBLIC (_ISR_Handler)

SYM (_ISR_Handler):
       /*
        *  Before this was point is reached the vectors unique
        *  entry point did the following:
        *
        *     1. saved all registers with a "pusha"
        *     2. put the vector number in eax.
        *
        * BEGINNING OF ESTABLISH SEGMENTS
        *
        *  WARNING: If an interrupt can occur when the segments are
        *           not correct, then this is where we should establish
        *           the segments.  In addition to establishing the
        *           segments, it may be necessary to establish a stack
        *           in the current data area on the outermost interrupt.
        *
        *  NOTE:  If the previous values of the segment registers are
        *         pushed, do not forget to adjust SAVED_REGS.
        *
        *  NOTE:  Make sure the exit code which restores these
        *         when this type of code is needed.
        */

       /***** ESTABLISH SEGMENTS CODE GOES HERE  ******/

       /*
        * END OF ESTABLISH SEGMENTS
        */

        /*
         *  Now switch stacks if necessary
         */

        movl      esp, edx                  # edx = previous stack pointer
        cmpl      $0, SYM (_ISR_Nest_level) # is this the outermost interrupt?
        jne       nested                    # No, then continue
        movl      SYM (_CPU_Interrupt_stack_high), esp

        /*
         *  We want to insure that the old stack pointer is on the
         *  stack we will be on at the end of the ISR when we restore it.
         *  By saving it on every interrupt, all we have to do is pop it
         *  near the end of every interrupt.
         */

nested:
        pushl     edx                       # save the previous stack pointer
        incl      SYM (_ISR_Nest_level)     # one nest level deeper
        incl      SYM (_Thread_Dispatch_disable_level) # disable multitasking

        # EAX is preloaded with the vector number.
        push      eax                       # push vector number
        mov       SYM (_ISR_Vector_table) (,eax,4),eax
                                            # eax = Users handler
        call      eax                       # invoke user ISR
        pop       eax                       # eax = vector number

        decl      SYM (_ISR_Nest_level)     # one less ISR nest level
                                            # If interrupts are nested,
                                            #   then dispatching is disabled

        decl      SYM (_Thread_Dispatch_disable_level)
                                            # unnest multitasking
                                            # Is dispatch disabled
        jne       exit                      # Yes, then exit

        cmpl      $0, SYM (_Context_Switch_necessary)
                                            # Is task switch necessary?
        jne       bframe                    # Yes, then build stack

        cmpl      $0, SYM (_ISR_Signals_to_thread_executing)
                                            # signals sent to Run_thread
                                            #   while in interrupt handler?
        je        exit                      # No, exit

bframe:
        cli                                 # DISABLE INTERRUPTS!!
        popl      esp                       # restore the stack pointer
        movl      $0, SYM (_ISR_Signals_to_thread_executing)
                                            # push the isf for Isr_dispatch
        push      EFLAGS_OFFSET(esp)        # push tasks eflags
        push      cs                        # cs of Isr_dispatch
        push      $ SYM (_ISR_Dispatch)     # entry point
        iret

exit:
        cli                                 # DISABLE INTERRUPTS!!
        popl      esp                       # restore the stack pointer

       /*
        * BEGINNING OF DE-ESTABLISH SEGMENTS
        *
        *  NOTE:  Make sure there is code here if code is added to
        *         load the segment registers.
        *
        */

       /******* DE-ESTABLISH SEGMENTS CODE GOES HERE ********/

       /*
        * END OF DE-ESTABLISH SEGMENTS
        */

        popa                                # restore general registers
        iret

/*PAGE
 *  Distinct Interrupt Entry Points
 *
 *  The following macro and the 256 instantiations of the macro
 *  are necessary to determine which interrupt vector occurred.
 *  The following macro allows a unique entry point to be defined
 *  for each vector.
 *
 *  NOTE: There are not spaces around the vector number argument
 *        to the DISTINCT_INTERRUPT_ENTRY macro because m4 will
 *        undesirably generate the symbol "_Isr_handler_ N"
 *        instead of "_Isr_handler_N" like we want.
 */

#define DISTINCT_INTERRUPT_ENTRY(_vector) \
        .p2align 4                         ; \
        PUBLIC (_ISR_Handler_ ## _vector ) ; \
SYM (_ISR_Handler_ ## _vector ):             \
        pusha                              ; \
        xor   eax, eax                     ; \
        movb  $ ## _vector, al             ; \
        jmp   SYM (_ISR_Handler) ;

DISTINCT_INTERRUPT_ENTRY(0)
DISTINCT_INTERRUPT_ENTRY(1)
DISTINCT_INTERRUPT_ENTRY(2)
DISTINCT_INTERRUPT_ENTRY(3)
DISTINCT_INTERRUPT_ENTRY(4)
DISTINCT_INTERRUPT_ENTRY(5)
DISTINCT_INTERRUPT_ENTRY(6)
DISTINCT_INTERRUPT_ENTRY(7)
DISTINCT_INTERRUPT_ENTRY(8)
DISTINCT_INTERRUPT_ENTRY(9)
DISTINCT_INTERRUPT_ENTRY(10)
DISTINCT_INTERRUPT_ENTRY(11)
DISTINCT_INTERRUPT_ENTRY(12)
DISTINCT_INTERRUPT_ENTRY(13)
DISTINCT_INTERRUPT_ENTRY(14)
DISTINCT_INTERRUPT_ENTRY(15)
DISTINCT_INTERRUPT_ENTRY(16)
DISTINCT_INTERRUPT_ENTRY(17)
DISTINCT_INTERRUPT_ENTRY(18)
DISTINCT_INTERRUPT_ENTRY(19)
DISTINCT_INTERRUPT_ENTRY(20)
DISTINCT_INTERRUPT_ENTRY(21)
DISTINCT_INTERRUPT_ENTRY(22)
DISTINCT_INTERRUPT_ENTRY(23)
DISTINCT_INTERRUPT_ENTRY(24)
DISTINCT_INTERRUPT_ENTRY(25)
DISTINCT_INTERRUPT_ENTRY(26)
DISTINCT_INTERRUPT_ENTRY(27)
DISTINCT_INTERRUPT_ENTRY(28)
DISTINCT_INTERRUPT_ENTRY(29)
DISTINCT_INTERRUPT_ENTRY(30)
DISTINCT_INTERRUPT_ENTRY(31)
DISTINCT_INTERRUPT_ENTRY(32)
DISTINCT_INTERRUPT_ENTRY(33)
DISTINCT_INTERRUPT_ENTRY(34)
DISTINCT_INTERRUPT_ENTRY(35)
DISTINCT_INTERRUPT_ENTRY(36)
DISTINCT_INTERRUPT_ENTRY(37)
DISTINCT_INTERRUPT_ENTRY(38)
DISTINCT_INTERRUPT_ENTRY(39)
DISTINCT_INTERRUPT_ENTRY(40)
DISTINCT_INTERRUPT_ENTRY(41)
DISTINCT_INTERRUPT_ENTRY(42)
DISTINCT_INTERRUPT_ENTRY(43)
DISTINCT_INTERRUPT_ENTRY(44)
DISTINCT_INTERRUPT_ENTRY(45)
DISTINCT_INTERRUPT_ENTRY(46)
DISTINCT_INTERRUPT_ENTRY(47)
DISTINCT_INTERRUPT_ENTRY(48)
DISTINCT_INTERRUPT_ENTRY(49)
DISTINCT_INTERRUPT_ENTRY(50)
DISTINCT_INTERRUPT_ENTRY(51)
DISTINCT_INTERRUPT_ENTRY(52)
DISTINCT_INTERRUPT_ENTRY(53)
DISTINCT_INTERRUPT_ENTRY(54)
DISTINCT_INTERRUPT_ENTRY(55)
DISTINCT_INTERRUPT_ENTRY(56)
DISTINCT_INTERRUPT_ENTRY(57)
DISTINCT_INTERRUPT_ENTRY(58)
DISTINCT_INTERRUPT_ENTRY(59)
DISTINCT_INTERRUPT_ENTRY(60)
DISTINCT_INTERRUPT_ENTRY(61)
DISTINCT_INTERRUPT_ENTRY(62)
DISTINCT_INTERRUPT_ENTRY(63)
DISTINCT_INTERRUPT_ENTRY(64)
DISTINCT_INTERRUPT_ENTRY(65)
DISTINCT_INTERRUPT_ENTRY(66)
DISTINCT_INTERRUPT_ENTRY(67)
DISTINCT_INTERRUPT_ENTRY(68)
DISTINCT_INTERRUPT_ENTRY(69)
DISTINCT_INTERRUPT_ENTRY(70)
DISTINCT_INTERRUPT_ENTRY(71)
DISTINCT_INTERRUPT_ENTRY(72)
DISTINCT_INTERRUPT_ENTRY(73)
DISTINCT_INTERRUPT_ENTRY(74)
DISTINCT_INTERRUPT_ENTRY(75)
DISTINCT_INTERRUPT_ENTRY(76)
DISTINCT_INTERRUPT_ENTRY(77)
DISTINCT_INTERRUPT_ENTRY(78)
DISTINCT_INTERRUPT_ENTRY(79)
DISTINCT_INTERRUPT_ENTRY(80)
DISTINCT_INTERRUPT_ENTRY(81)
DISTINCT_INTERRUPT_ENTRY(82)
DISTINCT_INTERRUPT_ENTRY(83)
DISTINCT_INTERRUPT_ENTRY(84)
DISTINCT_INTERRUPT_ENTRY(85)
DISTINCT_INTERRUPT_ENTRY(86)
DISTINCT_INTERRUPT_ENTRY(87)
DISTINCT_INTERRUPT_ENTRY(88)
DISTINCT_INTERRUPT_ENTRY(89)
DISTINCT_INTERRUPT_ENTRY(90)
DISTINCT_INTERRUPT_ENTRY(91)
DISTINCT_INTERRUPT_ENTRY(92)
DISTINCT_INTERRUPT_ENTRY(93)
DISTINCT_INTERRUPT_ENTRY(94)
DISTINCT_INTERRUPT_ENTRY(95)
DISTINCT_INTERRUPT_ENTRY(96)
DISTINCT_INTERRUPT_ENTRY(97)
DISTINCT_INTERRUPT_ENTRY(98)
DISTINCT_INTERRUPT_ENTRY(99)
DISTINCT_INTERRUPT_ENTRY(100)
DISTINCT_INTERRUPT_ENTRY(101)
DISTINCT_INTERRUPT_ENTRY(102)
DISTINCT_INTERRUPT_ENTRY(103)
DISTINCT_INTERRUPT_ENTRY(104)
DISTINCT_INTERRUPT_ENTRY(105)
DISTINCT_INTERRUPT_ENTRY(106)
DISTINCT_INTERRUPT_ENTRY(107)
DISTINCT_INTERRUPT_ENTRY(108)
DISTINCT_INTERRUPT_ENTRY(109)
DISTINCT_INTERRUPT_ENTRY(110)
DISTINCT_INTERRUPT_ENTRY(111)
DISTINCT_INTERRUPT_ENTRY(112)
DISTINCT_INTERRUPT_ENTRY(113)
DISTINCT_INTERRUPT_ENTRY(114)
DISTINCT_INTERRUPT_ENTRY(115)
DISTINCT_INTERRUPT_ENTRY(116)
DISTINCT_INTERRUPT_ENTRY(117)
DISTINCT_INTERRUPT_ENTRY(118)
DISTINCT_INTERRUPT_ENTRY(119)
DISTINCT_INTERRUPT_ENTRY(120)
DISTINCT_INTERRUPT_ENTRY(121)
DISTINCT_INTERRUPT_ENTRY(122)
DISTINCT_INTERRUPT_ENTRY(123)
DISTINCT_INTERRUPT_ENTRY(124)
DISTINCT_INTERRUPT_ENTRY(125)
DISTINCT_INTERRUPT_ENTRY(126)
DISTINCT_INTERRUPT_ENTRY(127)
DISTINCT_INTERRUPT_ENTRY(128)
DISTINCT_INTERRUPT_ENTRY(129)
DISTINCT_INTERRUPT_ENTRY(130)
DISTINCT_INTERRUPT_ENTRY(131)
DISTINCT_INTERRUPT_ENTRY(132)
DISTINCT_INTERRUPT_ENTRY(133)
DISTINCT_INTERRUPT_ENTRY(134)
DISTINCT_INTERRUPT_ENTRY(135)
DISTINCT_INTERRUPT_ENTRY(136)
DISTINCT_INTERRUPT_ENTRY(137)
DISTINCT_INTERRUPT_ENTRY(138)
DISTINCT_INTERRUPT_ENTRY(139)
DISTINCT_INTERRUPT_ENTRY(140)
DISTINCT_INTERRUPT_ENTRY(141)
DISTINCT_INTERRUPT_ENTRY(142)
DISTINCT_INTERRUPT_ENTRY(143)
DISTINCT_INTERRUPT_ENTRY(144)
DISTINCT_INTERRUPT_ENTRY(145)
DISTINCT_INTERRUPT_ENTRY(146)
DISTINCT_INTERRUPT_ENTRY(147)
DISTINCT_INTERRUPT_ENTRY(148)
DISTINCT_INTERRUPT_ENTRY(149)
DISTINCT_INTERRUPT_ENTRY(150)
DISTINCT_INTERRUPT_ENTRY(151)
DISTINCT_INTERRUPT_ENTRY(152)
DISTINCT_INTERRUPT_ENTRY(153)
DISTINCT_INTERRUPT_ENTRY(154)
DISTINCT_INTERRUPT_ENTRY(155)
DISTINCT_INTERRUPT_ENTRY(156)
DISTINCT_INTERRUPT_ENTRY(157)
DISTINCT_INTERRUPT_ENTRY(158)
DISTINCT_INTERRUPT_ENTRY(159)
DISTINCT_INTERRUPT_ENTRY(160)
DISTINCT_INTERRUPT_ENTRY(161)
DISTINCT_INTERRUPT_ENTRY(162)
DISTINCT_INTERRUPT_ENTRY(163)
DISTINCT_INTERRUPT_ENTRY(164)
DISTINCT_INTERRUPT_ENTRY(165)
DISTINCT_INTERRUPT_ENTRY(166)
DISTINCT_INTERRUPT_ENTRY(167)
DISTINCT_INTERRUPT_ENTRY(168)
DISTINCT_INTERRUPT_ENTRY(169)
DISTINCT_INTERRUPT_ENTRY(170)
DISTINCT_INTERRUPT_ENTRY(171)
DISTINCT_INTERRUPT_ENTRY(172)
DISTINCT_INTERRUPT_ENTRY(173)
DISTINCT_INTERRUPT_ENTRY(174)
DISTINCT_INTERRUPT_ENTRY(175)
DISTINCT_INTERRUPT_ENTRY(176)
DISTINCT_INTERRUPT_ENTRY(177)
DISTINCT_INTERRUPT_ENTRY(178)
DISTINCT_INTERRUPT_ENTRY(179)
DISTINCT_INTERRUPT_ENTRY(180)
DISTINCT_INTERRUPT_ENTRY(181)
DISTINCT_INTERRUPT_ENTRY(182)
DISTINCT_INTERRUPT_ENTRY(183)
DISTINCT_INTERRUPT_ENTRY(184)
DISTINCT_INTERRUPT_ENTRY(185)
DISTINCT_INTERRUPT_ENTRY(186)
DISTINCT_INTERRUPT_ENTRY(187)
DISTINCT_INTERRUPT_ENTRY(188)
DISTINCT_INTERRUPT_ENTRY(189)
DISTINCT_INTERRUPT_ENTRY(190)
DISTINCT_INTERRUPT_ENTRY(191)
DISTINCT_INTERRUPT_ENTRY(192)
DISTINCT_INTERRUPT_ENTRY(193)
DISTINCT_INTERRUPT_ENTRY(194)
DISTINCT_INTERRUPT_ENTRY(195)
DISTINCT_INTERRUPT_ENTRY(196)
DISTINCT_INTERRUPT_ENTRY(197)
DISTINCT_INTERRUPT_ENTRY(198)
DISTINCT_INTERRUPT_ENTRY(199)
DISTINCT_INTERRUPT_ENTRY(200)
DISTINCT_INTERRUPT_ENTRY(201)
DISTINCT_INTERRUPT_ENTRY(202)
DISTINCT_INTERRUPT_ENTRY(203)
DISTINCT_INTERRUPT_ENTRY(204)
DISTINCT_INTERRUPT_ENTRY(205)
DISTINCT_INTERRUPT_ENTRY(206)
DISTINCT_INTERRUPT_ENTRY(207)
DISTINCT_INTERRUPT_ENTRY(208)
DISTINCT_INTERRUPT_ENTRY(209)
DISTINCT_INTERRUPT_ENTRY(210)
DISTINCT_INTERRUPT_ENTRY(211)
DISTINCT_INTERRUPT_ENTRY(212)
DISTINCT_INTERRUPT_ENTRY(213)
DISTINCT_INTERRUPT_ENTRY(214)
DISTINCT_INTERRUPT_ENTRY(215)
DISTINCT_INTERRUPT_ENTRY(216)
DISTINCT_INTERRUPT_ENTRY(217)
DISTINCT_INTERRUPT_ENTRY(218)
DISTINCT_INTERRUPT_ENTRY(219)
DISTINCT_INTERRUPT_ENTRY(220)
DISTINCT_INTERRUPT_ENTRY(221)
DISTINCT_INTERRUPT_ENTRY(222)
DISTINCT_INTERRUPT_ENTRY(223)
DISTINCT_INTERRUPT_ENTRY(224)
DISTINCT_INTERRUPT_ENTRY(225)
DISTINCT_INTERRUPT_ENTRY(226)
DISTINCT_INTERRUPT_ENTRY(227)
DISTINCT_INTERRUPT_ENTRY(228)
DISTINCT_INTERRUPT_ENTRY(229)
DISTINCT_INTERRUPT_ENTRY(230)
DISTINCT_INTERRUPT_ENTRY(231)
DISTINCT_INTERRUPT_ENTRY(232)
DISTINCT_INTERRUPT_ENTRY(233)
DISTINCT_INTERRUPT_ENTRY(234)
DISTINCT_INTERRUPT_ENTRY(235)
DISTINCT_INTERRUPT_ENTRY(236)
DISTINCT_INTERRUPT_ENTRY(237)
DISTINCT_INTERRUPT_ENTRY(238)
DISTINCT_INTERRUPT_ENTRY(239)
DISTINCT_INTERRUPT_ENTRY(240)
DISTINCT_INTERRUPT_ENTRY(241)
DISTINCT_INTERRUPT_ENTRY(242)
DISTINCT_INTERRUPT_ENTRY(243)
DISTINCT_INTERRUPT_ENTRY(244)
DISTINCT_INTERRUPT_ENTRY(245)
DISTINCT_INTERRUPT_ENTRY(246)
DISTINCT_INTERRUPT_ENTRY(247)
DISTINCT_INTERRUPT_ENTRY(248)
DISTINCT_INTERRUPT_ENTRY(249)
DISTINCT_INTERRUPT_ENTRY(250)
DISTINCT_INTERRUPT_ENTRY(251)
DISTINCT_INTERRUPT_ENTRY(252)
DISTINCT_INTERRUPT_ENTRY(253)
DISTINCT_INTERRUPT_ENTRY(254)
DISTINCT_INTERRUPT_ENTRY(255)

/*PAGE
 *  void _ISR_Dispatch()
 *
 *  Entry point from the outermost interrupt service routine exit.
 *  The current stack is the supervisor mode stack.
 */

        PUBLIC (_ISR_Dispatch)
SYM (_ISR_Dispatch):

        call      SYM (_Thread_Dispatch)   # invoke Dispatcher

       /*
        * BEGINNING OF DE-ESTABLISH SEGMENTS
        *
        *  NOTE:  Make sure there is code here if code is added to
        *         load the segment registers.
        *
        */

       /***** DE-ESTABLISH SEGMENTS CODE GOES HERE ****/

       /*
        * END OF DE-ESTABLISH SEGMENTS
        */

        popa                                # restore general registers
        iret                                # return to interrupted thread

/*
 *  GO32 does not require these segment related routines.
 */

#ifndef __GO32__

/*
 *  void *i386_Logical_to_physical(
 *     rtems_unsigned16  segment,
 *     void             *address
 *  );
 *
 *  Returns thirty-two bit physical address for segment:address.
 */

.set SEGMENT_ARG, 4
.set ADDRESS_ARG, 8

             PUBLIC (i386_Logical_to_physical)

SYM (i386_Logical_to_physical):

        xorl    eax,eax                # clear eax
        movzwl  SEGMENT_ARG(esp),ecx   # ecx = segment value
        movl    $ SYM (_Global_descriptor_table),edx
                                       # edx = address of our GDT
        addl    ecx,edx                # edx = address of desired entry
        movb    7(edx),ah              # ah = base 31:24
        movb    4(edx),al              # al = base 23:16
        shll    $16,eax                # move ax into correct bits
        movw    2(edx),ax              # ax = base 0:15
        movl    ADDRESS_ARG(esp),ecx   # ecx = address to convert
        addl    eax,ecx                # ecx = physical address equivalent
        movl    ecx,eax                # eax = ecx
        ret

/*
 *  void *i386_Physical_to_logical(
 *     rtems_unsigned16  segment,
 *     void             *address
 *  );
 *
 *  Returns thirty-two bit physical address for segment:address.
 */

/*
 *.set SEGMENT_ARG, 4
 *.set ADDRESS_ARG, 8   -- use sets from above
 */

       PUBLIC (i386_Physical_to_logical)

SYM (i386_Physical_to_logical):
        xorl    eax,eax                # clear eax
        movzwl  SEGMENT_ARG(esp),ecx   # ecx = segment value
        movl    $ SYM (_Global_descriptor_table),edx
                                       # edx = address of our GDT
        addl    ecx,edx                # edx = address of desired entry
        movb    7(edx),ah              # ah = base 31:24
        movb    4(edx),al              # al = base 23:16
        shll    $16,eax                # move ax into correct bits
        movw    2(edx),ax              # ax = base 0:15
        movl    ADDRESS_ARG(esp),ecx   # ecx = address to convert
        subl    eax,ecx                # ecx = logical address equivalent
        movl    ecx,eax                # eax = ecx
        ret
#endif /* __GO32__ */

END_CODE

END
