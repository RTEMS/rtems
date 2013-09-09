/*
 *  ARM CPU Dependent Source
 *
 *  COPYRIGHT (c) 2007 Ray Xu.
 *  mailto: Rayx.cn at gmail dot com
 *
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  Copyright (c) 2002 Advent Networks, Inc
 *      Jay Monkman <jmonkman@adventnetworks.com>
 *
 *  This is a simple implement of abort
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <rtems.h>
#include <rtems/bspIo.h>

#define INSN_MASK         0xc5

#define INSN_STM1         0x80
#define INSN_STM2         0x84
#define INSN_STR          0x40
#define INSN_STRB         0x44

#define INSN_LDM1         0x81
#define INSN_LDM23        0x85
#define INSN_LDR          0x41
#define INSN_LDRB         0x45

#define GET_RD(x)         ((x & 0x0000f000) >> 12)
#define GET_RN(x)         ((x & 0x000f0000) >> 16)

#define GET_U(x)              ((x & 0x00800000) >> 23)
#define GET_I(x)              ((x & 0x02000000) >> 25)

#define GET_REG(r, ctx)      (((uint32_t   *)ctx)[r])
#define SET_REG(r, ctx, v)   (((uint32_t   *)ctx)[r] = v)
#define GET_OFFSET(insn)     (insn & 0xfff)

char *_print_full_context_mode2txt[0x10]={
  [0x0]="user",  /* User */
  [0x1]="fiq",   /* FIQ - Fast Interrupt Request */
  [0x2]="irq",   /* IRQ - Interrupt Request */
  [0x3]="super", /* Supervisor */
  [0x7]="abort", /* Abort */
  [0xb]="undef", /* Undefined */
  [0xf]="system" /* System */
};

void _print_full_context(uint32_t spsr)
{
  char *mode;
  uint32_t prev_sp,prev_lr,cpsr,arm_switch_reg;
  int i, j;

  printk("active thread thread 0x%08x\n", rtems_task_self());

  mode=_print_full_context_mode2txt[(spsr&0x1f)-0x10];
  if(!mode) mode="unknown";

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrs %[cpsr], cpsr\n"
    "orr %[arm_switch_reg], %[spsr], #0xc0\n"
    "msr cpsr_c, %[arm_switch_reg]\n"
    "mov %[prev_sp], sp\n"
    "mov %[prev_lr], lr\n"
    "msr cpsr_c, %[cpsr]\n"
    ARM_SWITCH_BACK
    : [prev_sp] "=&r" (prev_sp), [prev_lr] "=&r" (prev_lr),
      [cpsr] "=&r" (cpsr), [arm_switch_reg] "=&r" (arm_switch_reg)
    : [spsr] "r" (spsr)
    : "cc"
  );

  printk(
    "Previous sp=0x%08x lr=0x%08x and actual cpsr=%08x\n",
     prev_sp,
     prev_lr,
     cpsr
  );

  j=0;
  for(i=0;i<48;) {
      printk(" 0x%08x",((uint32_t*)prev_sp)[i++]);
      j++;
      /*try not to use % because it introduces hundreds of byte overhead*/
      if((j-6)==0) {
          printk("\n");
        j=0;
      }
  }
}


/* This function is supposed to figure out what caused the
 * data abort, do that, then return.
 *
 * All unhandled instructions cause the system to hang.
 */

void do_data_abort(
  uint32_t         insn,
  uint32_t         spsr,
  Context_Control *ctx
)
{
  /* Clarify, which type is correct, CPU_Exception_frame or Context_Control */

  uint8_t               decode;
  uint8_t               insn_type;
  rtems_interrupt_level level;

  decode = ((insn >> 20) & 0xff);

  insn_type = decode & INSN_MASK;
  switch(insn_type) {
  case INSN_STM1:
    printk("\n\nINSN_STM1\n");
    break;
  case INSN_STM2:
    printk("\n\nINSN_STM2\n");
    break;
  case INSN_STR:
    printk("\n\nINSN_STR\n");
    break;
  case INSN_STRB:
    printk("\n\nINSN_STRB\n");
    break;
  case INSN_LDM1:
    printk("\n\nINSN_LDM1\n");
    break;
  case INSN_LDM23:
    printk("\n\nINSN_LDM23\n");
    break;
  case INSN_LDR:
    printk("\n\nINSN_LDR\n");
    break;
  case INSN_LDRB:
    printk("\n\nINSN_LDRB\n");
    break;
  default:
    printk("\n\nUnrecognized instruction\n");
    break;
  }

  printk("data_abort at address 0x%x, instruction: 0x%x,   spsr = 0x%x\n",
         ctx->register_lr - 8, insn, spsr);

  _print_full_context(spsr);

  /* disable interrupts, wait forever */
  rtems_interrupt_disable(level);
  while(1) {
    continue;
  }
  return;
}

