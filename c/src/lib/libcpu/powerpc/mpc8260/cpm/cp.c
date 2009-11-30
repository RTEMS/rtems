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
#include <mpc8260.h>
#include <mpc8260/cpm.h>

/*
 * Send a command to the CPM RISC processer
 */

void m8xx_cp_execute_cmd( uint32_t   command )
{
  uint16_t   lvl;

  rtems_interrupt_disable(lvl);
  while (m8260.cpcr & M8260_CR_FLG) {
    continue;
  }

  m8260.cpcr = command | M8260_CR_FLG;
  rtems_interrupt_enable (lvl);
}
