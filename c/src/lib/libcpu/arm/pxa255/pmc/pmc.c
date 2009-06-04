/*
 *  By Yang Xi <hiyangxi@gmail.com>.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <pxa255.h>

unsigned int int_latency;

static void pmc_isr_on(const rtems_irq_connect_data *unused)
{
  unsigned int operand;
  /*clean CC*/
  operand = 0x0;
  asm volatile("mcr p14,0,%0,c1,c0,0 \n"::"r"(operand));
   /*clean the Clock counter flag and enable the interrupt of CC*/
  operand = 0x0|RESET_CCOF|ENABLE_CC_INT|RESET_CC|ENABLE_PMC_CC;
  asm volatile("mcr p14,0,%0,c0,c0,0 \n"::"r"(operand));
  /*Set to the 4kHZ*/
  operand = (unsigned int)0xffffffff-(unsigned int)100000;
  asm volatile("mcr p14,0,%0,c1,c0,0 \n"::"r"(operand));
}

static void pmc_isr_off(const rtems_irq_connect_data *unused)
{
  unsigned int operand;
  operand = 0x0|RESET_CCOF;
  asm volatile("mcr p14,0,%0,c0,c0,0 \n"::"r"(operand));
}

static int pmc_isr_is_on(const rtems_irq_connect_data *unused)
{
  unsigned int operand;
  asm volatile("mrc p14,0,%0,c0,c0,0 \n":"=r"(operand):);
  if((operand & ENABLE_PMC_CC ) && (operand & ENABLE_CC_INT))
    return 1;
  return 0;
}
