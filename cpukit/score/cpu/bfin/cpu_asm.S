/*  cpu_asm.S
 *
 *  This file contains the basic algorithms for all assembly code used
 *  in the Blackfin port of RTEMS.  These algorithms must be implemented
 *  in assembly language
 *
 *  Copyright (c) 2006 by Atos Automacao Industrial Ltda.
 *             written by Alain Schaefer <alain.schaefer@easc.ch>
 *                    and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
 
 
#include <rtems/asm.h>
#include <rtems/score/cpu_asm.h>
#include <rtems/score/bfin.h>
#include <rtems/bfin/bfin.h>

#define LO(con32) ((con32) & 0xFFFF)
#define HI(con32) (((con32) >> 16) & 0xFFFF)

/*  _CPU_Context_switch
 *
 *  This routine performs a normal non-FP context switch.
 *
 *  bfin Specific Information:
 *
 *  For now we simply save all registers.
 *  
 */

.globl __CPU_Context_switch
__CPU_Context_switch:
    /* Start saving context R0 = current, R1=heir */
    /*save P0 first*/
    [FP+0x8] = P0;
    P0 = R0;
    [ P0 + R0_OFFSET ] = R0;
    [ P0 + R1_OFFSET] = R1;
    [ P0 + R2_OFFSET] = R2;
    [ P0 + R4_OFFSET] = R4;
    [ P0 + R3_OFFSET] = R3;
    [ P0 + R5_OFFSET] = R5;
    [ P0 + R6_OFFSET] = R6;
    [ P0 + R7_OFFSET] = R7;
    [ P0 + P1_OFFSET] = P1;
    /* save the original value of P0 */
    P1 = [FP+0x8];
    [ P0 + P0_OFFSET] = P1;
    [ P0 + P2_OFFSET] = P2;
    [ P0 + P3_OFFSET] = P3;
    [ P0 + P4_OFFSET] = P4;
    [ P0 + P5_OFFSET] = P5;
    [ P0 + FP_OFFSET] = FP; 
    [ P0 + SP_OFFSET] = SP;
    
    /* save ASTAT */
    R0 = ASTAT;
    [P0 + ASTAT_OFFSET] = R0;

    /* save Loop Counters */
    R0 = LC0;
    [P0 + LC0_OFFSET] = R0;
    R0 = LC1;
    [P0 + LC1_OFFSET] = R0;

    /* save Accumulators */
    R0 = A0.W;
    [P0 + A0W_OFFSET] = R0;
    R0 = A0.X;
    [P0 + A0X_OFFSET] = R0;
    R0 = A1.W;
    [P0 + A1W_OFFSET] = R0;
    R0 = A1.X;
    [P0 + A1X_OFFSET] = R0;
    
    /* save Index Registers */
    R0 = I0;
    [P0 + I0_OFFSET] = R0;
    R0 = I1;
    [P0 + I1_OFFSET] = R0;
    R0 = I2;
    [P0 + I2_OFFSET] = R0;
    R0 = I3;
    [P0 + I3_OFFSET] = R0;

    /* save Modifier Registers */
    R0 = M0;
    [P0 + M0_OFFSET] = R0;
    R0 = M1;
    [P0 + M1_OFFSET] = R0;
    R0 = M2;
    [P0 + M2_OFFSET] = R0;
    R0 = M3;
    [P0 + M3_OFFSET] = R0;

    /* save Length Registers */
    R0 = L0;
    [P0 + L0_OFFSET] = R0;
    R0 = L1;
    [P0 + L1_OFFSET] = R0;
    R0 = L2;
    [P0 + L2_OFFSET] = R0;
    R0 = L3;
    [P0 + L3_OFFSET] = R0;

    /* Base Registers */
    R0 = B0;
    [P0 + B0_OFFSET] = R0;
    R0 = B1;
    [P0 + B1_OFFSET] = R0;
    R0 = B2;
    [P0 + B2_OFFSET] = R0;
    R0 = B3;
    [P0 + B3_OFFSET] = R0;
    
    /* save RETS */
    R0 = RETS;
    [ P0 + RETS_OFFSET] = R0; 

restore:
    P0 = R1;
    R1 = [P0 + R1_OFFSET];  
    R2 = [P0 + R2_OFFSET];
    R3 = [P0 + R3_OFFSET];
    R4 = [P0 + R4_OFFSET];
    R5 = [P0 + R5_OFFSET];
    R6 = [P0 + R6_OFFSET];
    R7 = [P0 + R7_OFFSET];
        
    P2 = [P0 + P2_OFFSET];
    P3 = [P0 + P3_OFFSET];
    P4 = [P0 + P4_OFFSET];
    P5 = [P0 + P5_OFFSET];

    /* might have to be placed more to the end */
    FP = [P0 + FP_OFFSET];
    SP = [P0 + SP_OFFSET];

    /* save ASTAT */
    R0 = [P0 + ASTAT_OFFSET];
    ASTAT = R0;

    /* save Loop Counters */
    R0 = [P0 + LC0_OFFSET];
    LC0 = R0;
    R0 = [P0 + LC1_OFFSET];
    LC1 = R0;

    /* save Accumulators */
    R0 = [P0 + A0W_OFFSET];
    A0.W = R0;
    R0 = [P0 + A0X_OFFSET];
    A0.X = R0;
    R0 = [P0 + A1W_OFFSET];
    A1.W = R0;
    R0 = [P0 + A1X_OFFSET];
    A1.X = R0;

    /* save Index Registers */
    R0 = [P0 + I0_OFFSET];
    I0 = R0;
    R0 = [P0 + I1_OFFSET];
    I1 = R0;
    R0 = [P0 + I2_OFFSET];
    I2 = R0;
    R0 = [P0 + I3_OFFSET];
    I3 = R0;

    /* save Modifier Registers */
    R0 = [P0 + M0_OFFSET];
    M0 = R0;
    R0 = [P0 + M1_OFFSET];
    M1 = R0;
    R0 = [P0 + M2_OFFSET];
    M2 = R0;
    R0 = [P0 + M3_OFFSET];
    M3 = R0;

    /* save Length Registers */
    R0 = [P0 + L0_OFFSET];
    L0 = R0;
    R0 = [P0 + L1_OFFSET];
    L1 = R0;
    R0 = [P0 + L2_OFFSET];
    L2 = R0;
    R0 = [P0 + L3_OFFSET];
    L3 = R0;

    /* Base Registers */
    R0 = [P0 + B0_OFFSET];
    B0 = R0;
    R0 = [P0 + B1_OFFSET];
    B1 = R0;
    R0 = [P0 + B2_OFFSET];
    B2 = R0;
    R0 = [P0 + B3_OFFSET];
    B3 = R0;

    /* restore RETS */
    P1 = [P0 + RETS_OFFSET];
    RETS = P1;

    /* now restore the P1 + P0 */
    P1 = [P0 + R1_OFFSET];    
    P0 = [P0 + P0_OFFSET];
    
    rts;
    

/*
 *  _CPU_Context_restore
 *
 *  This routine is generally used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  NOTE: May be unnecessary to reload some registers.
 *
 *  Blackfin Specific Information:
 *
 *  none
 *
 */
.globl __CPU_Context_restore
__CPU_Context_restore:
    jump restore;



.globl __ISR_Thread_Dispatch
__ISR_Thread_Dispatch:

    .extern __Thread_Dispatch
    R0.l = __Thread_Dispatch;
    R0.h = __Thread_Dispatch;
  
    /* Puts the address of th Thread_Dispatch function on Stack
     * Where it will be restored to the RTI register
     */
    P0 = [FP];
    /* save the old reti */
    R1 = [P0+0xc];
    [P0+0xc] = R0;
    /*
     * Overwriting the RETS Register is save because Thread_Dispatch is
     * disabled when we are between call/link or unlink/rts
     */
    [P0+0x8] = R1;
  
    /* save old rets */
  
    rts;


.globl __ISR_Handler
__ISR_Handler:
    /* First of all check the Stackpointer and */
    /* switch to Scratchpad if necessary        */
     
    /* save P0 and R0 in the scratchpad */
    USP = P0;
    
    /* load base adress of scratchpad */
    P0.H = HI(SCRATCH);
    P0.L = LO(SCRATCH);
 
 	[--SP] = ASTAT; /*  save cc flag*/
    /* if SP is already inside the SCRATCHPAD */ 
    CC=SP<P0 (iu)
    if !CC jump continue;
    
    /* set PO to top of scratchpad */   
    P0.h=HI(SCRATCH_TOP);
    P0.l=LO(SCRATCH_TOP);
    /*save the old SP*/ 
    [P0] = SP;
    /*P0 += -4;*/
    /*set the new Stackpointer*/
    SP = P0;
    /*restore the old PO*/
    
    /* The Stackpointer is now setup as we want */  
    continue:
    /* restore P0 and save some context */
    P0 = USP;
    /* save some state on the isr stack (scratchpad), this enables interrupt nesting */
    [--SP] = RETI;
    [--SP] = RETS;
    [--SP] = ASTAT;
    [--SP] = FP;
    FP = SP;
    [--SP] = (R7:0, P5:0) ;
    
    
    /* Context is saved, now check which Instruction we were executing
     * If we were between a call and link or between a unlink and rts
     * we have to disable Thread_Dispatch because correct restore of context after
     * Thread_Dispatch would not be possible. */
    
    P0 = RETI;
    R0 = P0;
    R0.L = 0x0000;
    R1.H = 0xffa0;
    R1.L = 0x0000;
    CC = R0 == R1;
    if CC jump disablethreaddispatch;
        
    R0 = W[P0](Z);    
    
    /* shift 16 bits to the right (select the high nibble ) */
    /*R0 >>= 16;*/
    
    R3 = 0;
    /* Check if RETI is a LINK instruction */
    R1.h = HI(0x0000);
    R1.l = LO(0xE800);
    CC=R0==R1; 
    if cc jump disablethreaddispatch;
    
    /* Check if RETI is a RTS instruction */
    R1.h = HI(0x0000);
    R1.l = LO(0x0010);
    CC=R0==R1; 
    if cc jump disablethreaddispatch;
    
    jump afterthreaddispatch;
    
    disablethreaddispatch:
    /*  _Thread_Dispatch_disable_level++   */
    .extern _Thread_Dispatch_disable_level
    P0.H = __Thread_Dispatch_disable_level;
    P0.L = __Thread_Dispatch_disable_level;
    R0 = [P0];
    R0 += 1;
    [P0] = R0;
    R3 = 1;
    
    afterthreaddispatch:
    /* Put R3 on the stack */
    [--SP] = R3;

    /* Obtain a bitlist of the pending interrupts. */
    P0.H = HI(IPEND);
    P0.L = LO(IPEND);   
    R1 = [P0];
    
    /*
     * Search through the bit list stored in R0 to find the first enabled
     * bit. The offset of this bit is the index of the interrupt that is
     * to be handled.
     */
    R0 = -1;
    intloop:
        R0 += 1;
        R1 = ROT R1 by -1;
        if !cc jump intloop;
     

    /* pass SP as parameter to the C function */
    R1 = SP

    /* pass values by register as well as by stack */
    /* to comply with the c calling conventions    */
    [--SP] = R0;
    [--SP] = R1;
    
    .extern _ISR_Handler2
    call    _ISR_Handler2

    /* inc 2 to compensate the passing of arguments */
    R3 = [SP++];
    R3 = [SP++];
    /* check if _Thread_Dispatch_disable_level has been incremented */
    R3 = [SP++]
    CC=R3==0
    if cc jump dont_decrement;
    .extern _Thread_Dispatch_disable_level
    P0.H = __Thread_Dispatch_disable_level;
    P0.L = __Thread_Dispatch_disable_level;
    R0 = [P0];
    R0 += -1;
    [P0] = R0;
    
    dont_decrement:
    
    (R7:0, P5:0) = [SP++];
    FP =    [SP++];
    ASTAT = [SP++];
    RETS =  [SP++];
    RETI =  [SP++];
    /* Interrupts are now disabled again */
    
    /*should restore the old stack !!!*/
    /*if sp now points to SCRATCH_TOP */
    
    /* load base adress of scratchpad */
    USP = P0;
    P0.H = HI(SCRATCH_TOP);
    P0.L = LO(SCRATCH_TOP);
    
    CC=SP==P0
    if !cc jump restoreP0
    /* restore the stack */
    SP=[P0];
    
    restoreP0:
    P0 = USP;
    ASTAT = [SP++]; /* restore cc flag */
        
    /*now we should be on the old "user-stack" again */ 
        
    /* return from interrupt, will jump to adress stored in RETI */
    RTI;

