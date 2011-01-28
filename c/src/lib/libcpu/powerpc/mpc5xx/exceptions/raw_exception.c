/*
 * raw_exception.c  - This file contains implementation of C functions to
 *          	      Instantiate mpc5xx primary exception entries.
 *	 	      More detailled information can be found on the Motorola
 *	    	      site and more precisely in the following book:
 *
 *		     MPC555/MPC556 User's Manual
 *		     Motorola REF : MPC555UM/D Rev. 3, 2000 October 15
 *
 *
 * MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 * Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 * Derived from libcpu/powerpc/mpc8xx/exceptions/raw_exception.c:
 *
 * Copyright (C) 1999  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <rtems.h>
#include <rtems/score/powerpc.h>
#include <libcpu/raw_exception.h>
#include <libcpu/cpuIdent.h>
#include <rtems/bspIo.h>	/* for printk */
#include <string.h>

static rtems_raw_except_connect_data* 		raw_except_table;
static rtems_raw_except_connect_data  		default_raw_except_entry;
static rtems_raw_except_global_settings* 	local_settings;

int mpc5xx_vector_is_valid(rtems_vector vector)
{
  switch (current_ppc_cpu) {
    case PPC_5XX:
      switch(vector) {
        case ASM_RESET_VECTOR:
        case ASM_MACH_VECTOR:

        case ASM_EXT_VECTOR:
        case ASM_ALIGN_VECTOR:
        case ASM_PROG_VECTOR:
        case ASM_FLOAT_VECTOR:
        case ASM_DEC_VECTOR:

        case ASM_SYS_VECTOR:
        case ASM_TRACE_VECTOR:
        case ASM_FLOATASSIST_VECTOR:

        case ASM_SOFTEMUL_VECTOR:

        case ASM_IPROT_VECTOR:
        case ASM_DPROT_VECTOR:

        case ASM_DBREAK_VECTOR:
        case ASM_IBREAK_VECTOR:
        case ASM_MEBREAK_VECTOR:
        case ASM_NMEBREAK_VECTOR:
          return 1;
        default:
          return 0;
      }
    default:
      printk("Please complete libcpu/powerpc/mpc5xx/exceptions/raw_exception.c\n");
      printk("current_ppc_cpu = %x\n", current_ppc_cpu);
      return 0;
  }
}

int mpc5xx_set_exception  (const rtems_raw_except_connect_data* except)
{
  rtems_interrupt_level level;

  if (!mpc5xx_vector_is_valid(except->exceptIndex)) {
    return 0;
  }
  /*
   * Check if default handler is actually connected. If not issue an error.
   * You must first get the current handler via mpc5xx_get_current_exception
   * and then disconnect it using mpc5xx_delete_exception.
   * RATIONALE : to always have the same transition by forcing the user
   * to get the previous handler before accepting to disconnect.
   */
  if (exception_handler_table[except->exceptIndex] !=
      default_raw_except_entry.hdl.raw_hdl) {
    return 0;
  }

  rtems_interrupt_disable(level);

  raw_except_table[except->exceptIndex] = *except;

  exception_handler_table[except->exceptIndex] = except->hdl.raw_hdl;
  if (except->on)
  	except->on(except);

  rtems_interrupt_enable(level);
  return 1;
}

int mpc5xx_get_current_exception (rtems_raw_except_connect_data* except)
{
  if (!mpc5xx_vector_is_valid(except->exceptIndex)){
    return 0;
  }

  *except = raw_except_table[except->exceptIndex];

  return 1;
}

int mpc5xx_delete_exception (const rtems_raw_except_connect_data* except)
{
  rtems_interrupt_level level;

  if (!mpc5xx_vector_is_valid(except->exceptIndex)){
    return 0;
  }
  /*
   * Check if handler passed is actually connected. If not issue an error.
   * You must first get the current handler via mpc5xx_get_current_exception
   * and then disconnect it using mpc5xx_delete_exception.
   * RATIONALE : to always have the same transition by forcing the user
   * to get the previous handler before accepting to disconnect.
   */
  if (exception_handler_table[except->exceptIndex] != except->hdl.raw_hdl) {
    return 0;
  }

  rtems_interrupt_disable(level);

  if (except->off)
  	except->off(except);
  exception_handler_table[except->exceptIndex] =
    default_raw_except_entry.hdl.raw_hdl;

  raw_except_table[except->exceptIndex] = default_raw_except_entry;
  raw_except_table[except->exceptIndex].exceptIndex = except->exceptIndex;

  rtems_interrupt_enable(level);

  return 1;
}

/*
 * Exception global init.
 *
 * Install exception handler pointers from the raw exception table into the
 * exception handler table.
 */
int mpc5xx_init_exceptions (rtems_raw_except_global_settings* config)
{
  unsigned 		i;
  rtems_interrupt_level level;

  /*
   * store various accelerators
   */
  raw_except_table 		= config->rawExceptHdlTbl;
  local_settings 		= config;
  default_raw_except_entry 	= config->defaultRawEntry;

  rtems_interrupt_disable(level);

  for (i = 0; i < NUM_EXCEPTIONS; i++) {
    exception_handler_table[i] = raw_except_table[i].hdl.raw_hdl;

    if (raw_except_table[i].hdl.raw_hdl != default_raw_except_entry.hdl.raw_hdl) {
      if (raw_except_table[i].on)
      	raw_except_table[i].on(&raw_except_table[i]);
    }
    else {
      if (raw_except_table[i].off)
      	raw_except_table[i].off(&raw_except_table[i]);
    }
  }
  rtems_interrupt_enable(level);

  return 1;
}

int mpc5xx_get_exception_config (rtems_raw_except_global_settings** config)
{
  *config = local_settings;
  return 1;
}
