/*
 * raw_exception.c  - This file contains implementation of C function to
 *          	      Instanciate 60x ppc primary exception entries.
 *	 	      More detailled information can be found on motorola
 *	    	      site and more precisely in the following book :
 *
 *		      MPC750 
 *		      Risc Microporcessor User's Manual
 *		      Motorola REF : MPC750UM/AD 8/97
 *
 * Copyright (C) 1999  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 * $Id$
 */
#include <rtems/score/targopts.h>
#include <rtems/score/ppc.h>
#include <rtems/system.h>
#include <rtems/score/cpu.h>
#include <libcpu/raw_exception.h>
#include <libcpu/cpu.h>

static rtems_raw_except_connect_data* 		raw_except_table;
static rtems_raw_except_connect_data  		default_raw_except_entry;
static rtems_raw_except_global_settings* 	local_settings;

int mpc750_vector_is_valid(rtems_vector vector)
{
  switch(vector) {
  case ASM_RESET_VECTOR: /* fall through */
  case ASM_MACH_VECTOR:
  case ASM_PROT_VECTOR:
  case ASM_ISI_VECTOR:
  case ASM_EXT_VECTOR:
  case ASM_ALIGN_VECTOR:
  case ASM_PROG_VECTOR:
  case ASM_FLOAT_VECTOR:
  case ASM_DEC_VECTOR:
  case ASM_SYS_VECTOR:
  case ASM_TRACE_VECTOR:
  case ASM_ADDR_VECTOR:
  case ASM_SYSMGMT_VECTOR:
  case ASM_ITM_VECTOR:
    return 1;
  default: return 0;
  }
}

int mpc604_vector_is_valid(rtems_vector vector)
{
  /*
   * Please fill this for MVME2307
   */
  printk("Please complete libcpu/powerpc/XXX/raw_exception.c\n");
  return 0;
}

int mpc60x_set_exception  (const rtems_raw_except_connect_data* except)
{
    unsigned int level;

    if (current_ppc_cpu == PPC_750) {
      if (!mpc750_vector_is_valid(except->exceptIndex)){
	return 0;
      }
      goto exception_ok;
    }
    if (current_ppc_cpu == PPC_604) {
      if (!mpc604_vector_is_valid(except->exceptIndex)){
	return 0;
      }
      goto exception_ok;
    }
    printk("Please complete libcpu/powerpc/XXX/raw_exception.c\n");
    return 0;
    
exception_ok:
    /*
     * Check if default handler is actually connected. If not issue an error.
     * You must first get the current handler via mpc60x_get_current_exception
     * and then disconnect it using mpc60x_delete_exception.
     * RATIONALE : to always have the same transition by forcing the user
     * to get the previous handler before accepting to disconnect.
     */
    if (memcmp(mpc60x_get_vector_addr(except->exceptIndex), (void*)default_raw_except_entry.hdl.raw_hdl,default_raw_except_entry.hdl.raw_hdl_size)) {
      return 0;
    }

    _CPU_ISR_Disable(level);
    
    raw_except_table [except->exceptIndex] = *except;
    codemove((void*)mpc60x_get_vector_addr(except->exceptIndex),
	     except->hdl.raw_hdl,
	     except->hdl.raw_hdl_size,
	     PPC_CACHE_ALIGNMENT);
    except->on(except);
    
    _CPU_ISR_Enable(level);
    return 1;
}

int mpc60x_get_current_exception (rtems_raw_except_connect_data* except)
{
  if (!mpc750_vector_is_valid(except->exceptIndex)){
    return 0;
  }
    
  *except = raw_except_table [except->exceptIndex];
    
  return 1;
}

int mpc60x_delete_exception (const rtems_raw_except_connect_data* except)
{
  unsigned int level;
  
  if (!mpc750_vector_is_valid(except->exceptIndex)){
    return 0;
  }
  /*
   * Check if handler passed is actually connected. If not issue an error.
   * You must first get the current handler via mpc60x_get_current_exception
   * and then disconnect it using mpc60x_delete_exception.
   * RATIONALE : to always have the same transition by forcing the user
   * to get the previous handler before accepting to disconnect.
   */
  if (memcmp(mpc60x_get_vector_addr(except->exceptIndex),
	     (void*)except->hdl.raw_hdl,
	     except->hdl.raw_hdl_size)) {
      return 0;
  }
  _CPU_ISR_Disable(level);

  except->off(except);
  codemove((void*)mpc60x_get_vector_addr(except->exceptIndex),
	   default_raw_except_entry.hdl.raw_hdl,
	   default_raw_except_entry.hdl.raw_hdl_size,
	   PPC_CACHE_ALIGNMENT);

    
  raw_except_table[except->exceptIndex] = default_raw_except_entry;
  raw_except_table[except->exceptIndex].exceptIndex = except->exceptIndex;

  _CPU_ISR_Enable(level);
    
  return 1;
}

/*
 * Exception global init.
 */
int mpc60x_init_exceptions (rtems_raw_except_global_settings* config)
{
    unsigned 			i;
    unsigned int level;
    
    /*
     * store various accelerators
     */
    raw_except_table 		= config->rawExceptHdlTbl;
    local_settings 		= config;
    default_raw_except_entry 	= config->defaultRawEntry;

    _CPU_ISR_Disable(level);

    for (i=0; i <= LAST_VALID_EXC; i++) {
      if (!mpc750_vector_is_valid(i)){
	continue;
      }
      codemove((void*)mpc60x_get_vector_addr(i),
	     raw_except_table[i].hdl.raw_hdl,
	     raw_except_table[i].hdl.raw_hdl_size,
	     PPC_CACHE_ALIGNMENT);
      if (raw_except_table[i].hdl.raw_hdl != default_raw_except_entry.hdl.raw_hdl) {
	raw_except_table[i].on(&raw_except_table[i]);
      }
      else {
	raw_except_table[i].off(&raw_except_table[i]);
      }
    }
    _CPU_ISR_Enable(level);

    return 1;
}

int mpc60x_get_exception_config (rtems_raw_except_global_settings** config)
{
  *config = local_settings;
  return 1;
}

