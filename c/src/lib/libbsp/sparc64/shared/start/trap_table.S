/*
 * trap_table.S
 *
 * trap code for Sparc64 RTEMS.
 *
 *  COPYRIGHT (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 * 
 *  This code defines the space for the trap table used by sun4u.
 *
 *  This file is currently unused.
 *
 * $Id$
 */


#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include <traptable.h>

#define BAD_TRAP \
    ta  0; \
  nop; \
  nop; \
  nop; \
  nop; \
  nop; \
  nop; \
  nop;

.section .ttable
  .align (TABLE_SIZE)
PUBLIC(trap_table)
  SYM(trap_table):
  .space TABLE_SIZE

/* These are just bogus traps that aren't used ever

.org trap_table + 8*32
    BAD_TRAP;                                     ! 8 IAE
.org trap_table + 9*32
    BAD_TRAP;                                     ! 9
.org trap_table + 10*32
    BAD_TRAP;                                     ! A

.org trap_table + 11*32
    BAD_TRAP;                                     ! B
.org trap_table + 12*32
    BAD_TRAP;                                     ! C

.org trap_table + 16*32
    BAD_TRAP;                                     ! 10 illegal instruction
.org trap_table + 17*32
    BAD_TRAP;                                     ! 11 privileged op_code

.org trap_table + 18*32
    BAD_TRAP;                                     ! 12 unimplemented LDD
.org trap_table + 19*32
    BAD_TRAP;                                     ! 13 unimplemented std
.org trap_table + 20*32
    BAD_TRAP;                                     ! 14 dae invalid asi
.org trap_table + 21*32
    BAD_TRAP;                                     ! 15 dae priv violation
.org trap_table + 22*32
    BAD_TRAP;                                     ! 16 dae nc page
.org trap_table + 23*32
    BAD_TRAP;                                     ! 17 dae nfo page

.org trap_table + 32*32
    BAD_TRAP;                                     ! 20 fp_disabled
.org trap_table + 33*32
    BAD_TRAP;                                     ! 21 fp_exception_ieee_754
.org trap_table + 34*32
    BAD_TRAP;                                     ! 22 fp_exception_other

.org trap_table + 35*32
    BAD_TRAP;                                     ! 23 tag_overflow

.org trap_table + 36*32
    BAD_TRAP;                         ! 24-27 clean_window


.org trap_table + 40*32
    BAD_TRAP;                                     ! 28 div by zero
    
.org trap_table + 48*32
    BAD_TRAP;                                     ! 30 data access exception

.org trap_table + 49*32
    BAD_TRAP;                                     ! 31 data mmu miss
.org trap_table + 50*32
    BAD_TRAP;                                     ! 32 data access error

.org trap_table + 52*32
    BAD_TRAP;                                     ! 34 mem address not aligned 
.org trap_table + 53*32
    BAD_TRAP;                                     ! 35 LDDF mem address not aligned 
.org trap_table + 54*32
    BAD_TRAP;                                     ! 36 STDF mem addr not aligned
.org trap_table + 55*32
    BAD_TRAP;                                     ! 37 privileged action
.org trap_table + 56*32
    BAD_TRAP;                                     ! 38 LDQF mem addr not aligned
.org trap_table + 57*32
    BAD_TRAP;                                     ! 39 STQF mem addr not aligned


.org trap_table + 65*32
    BAD_TRAP;                                     ! 41-4F
                                                  ! interrupt_level_n (1-15) 
.org trap_table + 66*32
    BAD_TRAP;                                     ! 
.org trap_table + 67*32
    BAD_TRAP;                                     ! 
.org trap_table + 68*32
    BAD_TRAP;                                     ! 
.org trap_table + 69*32
    BAD_TRAP;                                     ! 
.org trap_table + 70*32
    BAD_TRAP;                                     ! 
.org trap_table + 71*32
    BAD_TRAP;                                     ! 
.org trap_table + 72*32
    BAD_TRAP;                                     ! 
.org trap_table + 73*32
    BAD_TRAP;                                     ! 
.org trap_table + 74*32
    BAD_TRAP;                                     ! 
.org trap_table + 75*32
    BAD_TRAP;                                     ! 
.org trap_table + 76*32
    BAD_TRAP;                                     ! 
.org trap_table + 77*32
    BAD_TRAP;                                     ! 
.org trap_table + 78*32
    BAD_TRAP;                                     ! 
.org trap_table + 79*32
    BAD_TRAP;                                     ! 

.org trap_table + 124*32
    BAD_TRAP;                                     ! 7C cpu mondo

.org trap_table + 125*32
    BAD_TRAP;                                     ! 7D dev mondo

.org trap_table + 128*32
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! 80-83 spill_0_normal
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! 84-87 spill_1_normal
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! 88-8B spill_2_normal
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! 8C-8F spill_3_normal
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! 90-93 spill_4_normal
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! 94-97 spill_5_normal
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! 98-9B spill_6_normal
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! 9C-9F spill_7_normal

    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! A0-A3 spill_0_other
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! A4-A7 spill_1_other
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! A8-AB spill_2_other
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! AC-AF spill_3_other
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! B0-B3 spill_4_other
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! B4-B7 spill_5_other
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! B8-BB spill_6_other
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! BC-BF spill_7_other

    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! C0-C3 fill_0_normal
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! C4-C7 fill_1_normal
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! C8-CB fill_2_normal
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! CC-CF fill_3_normal
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! D0-D3 fill_4_normal
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! D4-D7 fill_5_normal
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! D8-DB fill_6_normal
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! DC-DF fill_7_normal

    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! E0-E3 fill_0_other
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! E4-E7 fill_1_other
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! E8-EB fill_2_other
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! EC-EF fill_3_other
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! F0-F3 fill_4_other
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! F4-F7 fill_5_other
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! F8-FB fill_6_other
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;          ! FC-FF fill_7_other

    BAD_TRAP;BAD_TRAP;BAD_TRAP;                   ! 100-17F trap instruction
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;                                    
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
    BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;BAD_TRAP;
*/

