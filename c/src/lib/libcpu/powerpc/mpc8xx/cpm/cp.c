/*
 * cp.c
 *
 * MPC8xx CPM RISC Communication Processor routines.
 * 
 * Based on code (alloc860.c in eth_comm port) by
 * Jay Monkman (jmonkman@frasca.com),
 * which, in turn, is based on code by
 * W. Eric Norum (eric@skatter.usask.ca).
 *
 * Modifications by Darlene Stewart (Darlene.Stewart@iit.nrc.ca):
 * Copyright (c) 1999, National Research Council of Canada
 */

#include <bsp.h>
#include <rtems/rtems/intr.h>
#include <rtems/error.h>

/*
 * Send a command to the CPM RISC processer
 */
void m8xx_cp_execute_cmd( unsigned16 command )
{
  rtems_unsigned16 lvl;
  
  rtems_interrupt_disable(lvl);
  while (m8xx.cpcr & M8xx_CR_FLG) {
    continue;
  }

  m8xx.cpcr = command | M8xx_CR_FLG;
  rtems_interrupt_enable (lvl);
}

