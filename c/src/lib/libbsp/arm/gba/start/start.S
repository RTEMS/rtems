/**
 *  @file start.S
 *
 *  RTEMS entry point.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) by Jeff Frohwein.
 *
 *  Copyright (c) 2003, Jason Wilkins.
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *  based on crt0.S v1.28 by Jeff Frohwein
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/*****************************************************************************
 * This source file is based on work by Jeff Frohwein and Jason Wilkins
 *****************************************************************************
 *****************************************************************************
 * crt0.S v1.28 by Jeff Frohwein
 * :
 * This file is released into the public domain for commercial
 * or non-commercial usage with no restrictions placed upon it.
 *****************************************************************************
 * Copyright 2003, Jason Wilkins.  This source code is free for any use except
 * that this copyright notice and the following disclaimers remain intact when
 * the source is distributed.  Object code and binary distributions may be made
 * as if the code were in the public domain.
 *
 * THIS CODE WAS NOT MADE IN ASSOCIATION WITH NINTENDO AND DOES NOT MAKE USE
 * OF ANY INTELLECTUAL PROPERTY CLAIMED BY NINTENDO.
 *
 * GAMEBOY ADVANCE IS A TRADEMARK OF NINTENDO.
 *
 * THIS CODE HAS BEEN PROVIDED "AS-IS" WITHOUT A WARRANTY OF ANY KIND, EITHER
 * EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO IMPLIED WARRANTIES OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  THE ENTIRE RISK AS TO THE
 * QUALITY OR PERFORMANCE OF THE CODE IS WITH YOU.
 *
 * IN NO EVENT, UNLESS AGREED TO IN WRITING, WILL ANY COPYRIGHT HOLDER, OR ANY
 * OTHER PARTY, BE HELD LIABLE FOR ANY DAMAGES RESULTING FROM THE USE OR
 * INABILITY TO USE THIS CODE.
 *****************************************************************************/

#define __asm__
#include <rtems/asm.h>
#include <asm_macros.h>
#include <arm_mode_bits.h>
/* @cond  INCLUDE_ASM */

#ifndef NINTENDO_LOGO
#define NINTENDO_LOGO 1
#endif

#ifndef GBA_MULTIBOOT
#define GBA_MULTIBOOT 1
#endif

#ifndef GAME_TITLE
#define GAME_TITLE "RTEMS-RTOS  "
#endif

#ifdef GBA_MULTIBOOT
  #ifndef GAME_CODE
  #define GAME_CODE "MB  "
  #endif
  #ifndef COMPLEMENT_CHECK
  #define COMPLEMENT_CHECK 0xE2
  #endif
#else
  #ifndef GAME_CODE
  #define GAME_CODE "GBA "
  #endif
  #ifndef COMPLEMENT_CHECK
  #define COMPLEMENT_CHECK 0xC7
  #endif
#endif

#ifndef MAKER_CODE
#define MAKER_CODE "00"
#endif

/**
 *  RTEMS entry point
 *  function void _start(void)
 *
 */
/*****************************************************************************\
                           ROM Header
\*****************************************************************************/

.text
.align
.arm

/*---------------------------------------------------------------------------+
|  Nintendo Header:
|   A special header is required or the GBA will refuse to run your code.
|
|  File offsets from 0x0200000 or 0x08000000.
+----------------------------------------------------------------------------*/
    PUBLIC_ARM_FUNCTION(_start)
         b       SYM(_real_start)    /* 0x00 Entry Point                     */
    SYM(_gba_rom_header):
#if NINTENDO_LOGO                    /* 0x04 Nintendo Logo Character Data    */
#include        "logo.S"
#else
        .fill   156, 1, 0            /* 0x04 Nintendo Logo Character Data    */
#endif
        .ascii  GAME_TITLE           /* 0xA0 Game Title                      */
        .ascii  GAME_CODE            /* 0xAC Game Code                       */
        .ascii  MAKER_CODE           /* 0xB0 Maker Code                      */
        .byte   0x96                 /* 0xB2 Fixed Value                     */
        .byte   0                    /* 0xB3 Main Unit Code                  */
        .byte   0                    /* 0xB4 Device Type                     */
        .byte   0, 0, 0, 0, 0, 0, 0  /* 0xB5 Reserved (7 Bytes)              */
        .byte   0                    /* 0xBC Software Version No.            */
        .byte   COMPLEMENT_CHECK     /* 0xBD Complement Check                */
        .byte   0, 0                 /* 0xBE Reserved                        */
    .Lheader_end:


#if GBA_MULTIBOOT
/*---------------------------------------------------------------------------+
|  Multiboot Header:
|  The following header is required if the code is meant for Multiboot.
|
|  If the code has been downloaded through the serial port, then the GBA BIOS
|  will set offset 0xC0 depending on the boot method:
|  1 = JoyBus, 3 = Multiboot
|  It remains 0 for cartridges.
|
|  offset 0xC4 will be set to the GBA's assigned slave number 1-3.
|  This header also defines the symbols _boot_method and _slave_number for
|  easy reference to these values.  Some libraries may depend on them whether
|  or not the code is meant for Multiboot.
|
+----------------------------------------------------------------------------*/
    SYM(_gba_multiboot_start):
        b       SYM(_real_start)               /* 0xC0 Multiboot Entry Point */
    OBJECT(_boot_method)
        .byte   0                              /* 0xC4 Boot Method           */
    OBJECT(_slave_number)
        .byte   0                              /* 0xC5 Slave Number (1-3)    */
        .align
    STATIC_OBJECT(_gba_mb_reserved)
        .word   0, 0, 0, 0, 0, 0               /* 0xC8 Reserved (6 words)    */
    SYM(_gba_joybus_start):
        b       SYM(_real_start)               /* 0xE0 JoyBus Entry Point    */
    .Lmultiboot_header_end:
#endif

/*---------------------------------------------------------------------------+
|    Restore registers and stack from GBA bios IRQ frame and call ISR_Handler
+----------------------------------------------------------------------------*/
    EXTERN(_ISR_Handler)
    .align
    PUBLIC_ARM_FUNCTION(_gba_ISR_handler)
        ldmfd  r13!,{r0-r3,r12,r14}
        b      arm_exc_interrupt
    LABEL_END(_gba_ISR_handler)


/*---------------------------------------------------------------------------+
|    Code to initialize the low-level BSP environment
+----------------------------------------------------------------------------*/
    SYM(_real_start):
    /* Initialize IRQ and USR Stack Pointers */
    SYM(_gba_init_stacks):
        mov     r0, #(Mode_IRQ | Int_Bits)       /* No interrupts            */
        msr     cpsr, r0                         /* switch to IRQ mode       */
        ldr     sp, =__sp_irq                    /* defined in linkcmds      */
        mov     r0, #(ModePriv | Int_Bits)       /* No interrupts            */
        msr     cpsr, r0                         /* switch to System Mode    */
        ldr     sp, =__sp_usr                    /* defined in linkcmds      */

    /* Switch to Thumb Mode */
    SYM(_gba_bx_thumb):
        adr     r0, .Lthumb + 1
        bx      r0
        .thumb
    .Lthumb:
    /* Reduce gameboy waitstates */
    SYM(_reduce_waitstates):
        ldr     r1, =0x4000204
        ldrh    r0, =0x4490
        strh    r0, [r1]


#if GBA_MULTIBOOT
/*---------------------------------------------------------------------------+
|  Load Multiboot Image from ROM into RAM:
|  Check to see if the image is meant for Multiboot or GamePak.  If it is for
|  Multiboot then check if it is currently running from EWRAM or from CARTROM.
|  If it is running from CARTROM, then it needs to be copied to EWRAM and
|  re-executed from the beginning.
|
|  The reason for all this is to allow a program to be used "as-is" with a
|  flash-cart, emulator, or MBV2-style Multiboot cable without rebuilding.
|
|  NOTE: Any branchs used above this code need to be relative.
+----------------------------------------------------------------------------*/
    STATIC_THUMB_FUNCTION(_gba_load_multiboot)
        ldr     r0, =_start            /* 8000000h=GamePak 2000000h=Multiboot*/
        ldr     r1, =__gba_rom_start   /* defined in linkcmds                */
        cmp     r0, r1
        beq     SYM(_no_load_multiboot)/* skip if GamePak                    */
        mov     r3, pc
        cmp     r1, r3                 /* check program counter              */
        bhi     SYM(_no_load_multiboot)/* skip if already running from EWRAM */
        sub     r3, r1, r0             /* diff between _start and CARTROM    */
        ldr     r2, =__load_stop_data  /* defined in linkcmds                */
        add     r2, r2, r3             /* adjust pointer into ROM            */
        bl      SYM(gba_move_memory)

        /* patch multiboot header */
        ldr     r0, =SYM(_boot_method)
        mov     r1, #3
        str     r1, [r0]

        /* remember that multiboot image came from GamePak */
        ldr     r0, =SYM(_gba_flash_loaded_multiboot)
        mov     r1, #0
        str     r1, [r0]

        ldr     r0, =SYM(_start)
        bx      r0                      /* restart                           */
    LABEL_END(_gba_load_multiboot)

        .align 4
    OBJECT(_gba_flash_loaded_multiboot)
        .word -1
    LABEL_END(_gba_flash_loaded_multiboot)
    SYM(_no_load_multiboot):
#endif

/* Initialize Standard Sections */
    STATIC_THUMB_FUNCTION(_gba_init_std_sections)
        /* Copy internal work ram (iwram section ROM to RAM)*/
        ldr     r0,=__iwram_start
        ldr     r1,=__load_start_iwram
        ldr     r2,=__load_stop_iwram
        bl      SYM(gba_move_memory)

        /* Copy external work ram (ewram section ROM to RAM) */
        ldr     r0,=__ewram_start
        ldr     r1,=__load_start_ewram
        ldr     r2,=__load_stop_ewram
        bl      SYM(gba_move_memory)

        /* load initial values of variables like 'int foo = 42' */
        ldr     r0, =__data_start      /* defined in linkcmds               */
        ldr     r1, =__load_start_data /* defined in linkcmds               */
        ldr     r2, =__load_stop_data  /* defined in linkcmds               */
        bl      SYM(gba_move_memory)

        /* zero the bss */
        ldr     r0, =__bss_start       /* defined in linkcmds               */
        ldr     r1, =__bss_end         /* defined in linkcmds               */
        bl      SYM(gba_zero_memory)
    LABEL_END(_gba_init_std_sections)

/* Initialize Interrupt Vector */
    STATIC_THUMB_FUNCTION(_gba_init_intr_vect)
        ldr     r1, =__irq_vector    /* defined in linkcmds                 */
        ldr     r0, =SYM(_gba_ISR_handler)
        str     r0, [r1]
    LABEL_END(_gba_init_intr_vect)


/* Enter the C code.  If it returns, then restart */
    STATIC_THUMB_FUNCTION(_gba_call_arm_boot_card)
        adr     r1, .Larm
        bx      r1
        .arm
    .Larm:
        ldr     r1, =boot_card
        mov     r0, #0
        bl      SYM(_gba_call_via_r1)

        ldr     r0, =SYM(_gba_reset)
    SYM(_gba_call_via_r1):
        bx      r1

/* GBA Reset  */
    PUBLIC_ARM_FUNCTION(_gba_reset)
        adr     r0, .Lthumb2 + 1
        bx      r0
        .thumb
    .Lthumb2:
        /* disable interrupts */
        ldr     r0, =0x04000208
        mov     r1, #0
        strb    r1, [r0]

        /* reset stack, default free area */
        ldr     r0, =0x03007F00
        mov     sp, r0

#if GBA_MULTIBOOT
        ldr     r0, =SYM(_gba_flash_loaded_multiboot)
        ldr     r0, [r0]
        cmp     r0, #0
        beq     SYM(_reset)
#endif

        ldr     r0, =_start            /* defined in linkcmds               */
        ldr     r1, =__gba_rom_start   /* defined in linkcmds               */
        sub     r0, r0, r1
        lsr     r0, r0, #24

   SYM(_reset):
        /* soft reset (swi 0) parameter: where is _start */
        ldr     r1, =0x03007FFA
        strb    r0, [r1]
        mov     r0, #0xFE              /* clear all but EWRAM               */
        swi     1
        swi     0
    LABEL_END(_gba_reset)


/*---------------------------------------------------------------------------+
|   Library Functions
+----------------------------------------------------------------------------*/

/* gba_zero_memory */
    PUBLIC_THUMB_FUNCTION(gba_zero_memory)
        mov     r2, #0
        nop
    LABEL_END(gba_zero_memory)

/* gba_set_memory */
    PUBLIC_THUMB_FUNCTION(gba_set_memory)
        cmp     r0, r1
        bcs     .Lset_memory_return

    .Lset_memory_loop:
        stmia   r0!, {r2}
        cmp     r0, r1
        bcc     .Lset_memory_loop

    .Lset_memory_return:
        bx      lr
    LABEL_END(gba_set_memory)

/* gba_move_memory */
    .align
    PUBLIC_THUMB_FUNCTION(gba_move_memory)
        cmp     r0, r1
        bcc     .Lforward_move  /* if dst < src then forward copy */
        bhi     .Lreverse_move  /* if dst > src then reverse copy */
        bx      lr              /* else dst == src, nothing to do */

    .Lforward_move:
        cmp     r1, r2
        bcs     .Lmove_memory_return

    .Lforward_move_loop:
        ldmia   r1!, {r3}
        stmia   r0!, {r3}
        cmp     r1, r2
        bcc     .Lforward_move_loop
        bx      lr

    .Lreverse_move:
        cmp     r2, r1
        bls     .Lmove_memory_return
        sub     r3, r2, r1
        add     r0, r0, r3

    .Lreverse_move_loop:
        sub     r2, r2, #4
        ldr     r3, [r2]
        sub     r0, r0, #4
        str     r3, [r0]
        cmp     r2, r1
        bhi     .Lreverse_move_loop

    .Lmove_memory_return:
        bx      lr
    LABEL_END(gba_move_memory)


/* @todo FIXME: Remove unused handler needed by ../score/cpu_asm.S
 *****************************************************************************/
        .arm
        .global SWI_Handler
SWI_Handler:
        mov pc, lr
/* @endcond     */

