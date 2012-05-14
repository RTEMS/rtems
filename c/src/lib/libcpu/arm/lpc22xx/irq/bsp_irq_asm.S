/*
 *  LPC22XX/LPC21xx Intererrupt handler
 *
 *  Modified by Ray <rayx.cn@gmail.com> 2006 from Jay Monkman's code
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#define __asm__

/*
 * BSP specific interrupt handler for INT or FIQ. In here
 * you do determine which interrupt happened and call its
 * handler.
 * Called from ISR_Handler, It is better to write in C function
 */
	.globl bsp_interrupt_dispatch
bsp_interrupt_dispatch :
#ifdef __thumb__
    .code 16
#endif

/*
 * Look at interrupt status register to determine source.
 * From source, determine offset into expanded vector table
 * and load handler address into r0.
 */

  ldr 	r0, =0xFFFFF030  /* Read the vector number */
    ldr	r0, [r0]
#ifdef __thumb__
    push {lr}
    ldr	r2, =IRQ_return         /* prepare the return from handler  */
    mov lr, r2
#else
  stmdb	  sp!,{lr}
  ldr     lr, =IRQ_return         /* prepare the return from handler  */
#endif


    /*C code will be called*/
    mov	pc, r0		/* EXECUTE INT HANDLER */

    /*
     * C code may come back from Thumb if --thumb-interwork flag is False
     * Add some veneer to make sure that code back to ARM
     */
IRQ_return:
#ifdef __thumb__
    pop {r1}
    bx  r1
#else
    ldmia sp!,{r1}
    mov pc, r1
#endif

