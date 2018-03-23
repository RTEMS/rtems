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

#include <rtems.h>
#include <mpc8xx.h>
#include <mpc8xx/cpm.h>

/*
 * Send a command to the CPM RISC processer
 */

void m8xx_cp_execute_cmd( uint16_t   command )
{
  rtems_interrupt_level lvl;

  rtems_interrupt_disable(lvl);
  while (m8xx.cpcr & M8xx_CR_FLG) {
    continue;
  }

  m8xx.cpcr = command | M8xx_CR_FLG;
  rtems_interrupt_enable (lvl);
}
