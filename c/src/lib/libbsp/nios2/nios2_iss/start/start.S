/*  NIOS2 startup code
 *
 *  This is the entry point on reset and when loading the
 *  executive from a bootloader.
 *
 *  COPYRIGHT (c) 2005-2006 Kolja Waschk rtemsdev/ixo.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

     .section .entry
     .align    3
     movhi  et, %hiadj(start)
     addi   et, et, %lo(start)
     jmp    et

     .section .irq
     .align 3
     movhi  et, %hiadj(_exception_vector)
     addi   et, et, %lo(_exception_vector)
     jmp    et

    .section .text
    .align  3
    .globl  start
    .type   start,@function

    .extern _exception_vector

start:
    #------------------------------------------------------
    # disable interrupts
    wrctl status, zero
    wrctl ienable, zero

    #------------------------------------------------------
    # invalidate instruction cache
    mov r2, r0
    movhi r3, %hi(__nios2_icache_size)
    ori r3, r3, %lo(__nios2_icache_size)
icache_init_loop:
    initi r2
    addi r2, r2, __nios2_icache_line_size
    bltu r2, r3, icache_init_loop

    #------------------------------------------------------
    # invalidate data cache
    mov r2, r0
    movhi r3, %hi(__nios2_dcache_size)
    ori r3, r3, %lo(__nios2_dcache_size)
dcache_init_loop:
    initd 0(r2)
    addi r2, r2, __nios2_dcache_line_size
    bltu r2, r3, dcache_init_loop

    #------------------------------------------------------
    # initialize stack pointer
    movhi   sp, %hiadj(__alt_stack_pointer - 4)
    addi    sp, sp, %lo(__alt_stack_pointer - 4)

    # initialize global pointer
    movhi gp, %hiadj(_gp)
    addi gp, gp, %lo(_gp)

    # initialize exception tmp register
    movhi et, %hiadj(_end)
    addi et, et, %lo(_end)

    #------------------------------------------------------
    # TODO: copy data from flash to RAM, if not there already
    # For now its save to assume it is there already when we're
    # loading code though JTAG into RAM-only system

    # at least copy exception code to right place
    movhi r2, %hiadj(__ram_exceptions_start)
    addi r2, r2, %lo(__ram_exceptions_start)

    movhi r3, %hiadj(brto_ev)
    addi r3, r3, %lo(brto_ev)
    ldw r4,  0(r3)
    stw r4,  0(r2)
    ldw r4,  4(r3)
    stw r4,  4(r2)
    ldw r4,  8(r3)
    stw r4,  8(r2)
    ldw r4, 12(r3)
    stw r4, 12(r2)

    #------------------------------------------------------
    # clear bss
    movhi r2, %hiadj(__bss_start)
    addi r2, r2, %lo(__bss_start)

    movhi r3, %hiadj(__bss_end)
    addi r3, r3, %lo(__bss_end)

    beq r2, r3, 1f
0:
    stw zero, (r2)
    addi r2, r2, 4
    bltu r2, r3, 0b
1:
    #------------------------------------------------------
    # jump to (shared) boot_card (never comes back)
    mov r4, zero
    mov r5, zero
    mov r6, zero
    call    boot_card
    # but just in case it does come back, stick here.
_stuck_in_start:
    br _stuck_in_start

    #------------------------------------------------------
    # code to be placed at exception address
brto_ev:
     movhi et, %hiadj(_exception_vector)
     addi et, et, %lo(_exception_vector)
     jmp et


