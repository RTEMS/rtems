/*
 * raw_exception.c  - This file contains implementation of C function to
 *          	      Instantiate 60x ppc primary exception entries.
 *	 	      More detailed information can be found on motorola
 *	    	      site and more precisely in the following book :
 *
 *		      MPC750
 *		      Risc Microporcessor User's Manual
 *		      Motorola REF : MPC750UM/AD 8/97
 *
 * Copyright (C) 1999  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 * Enhanced by Jay Kulpinski <jskulpin@eng01.gdds.com>
 * to support 603, 603e, 604, 604e exceptions
 *
 * moved to "libcpu/powerpc/new-exceptions and consolidated
 * by Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>
 * to be common for all PPCs with new excpetions
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */
#include <rtems/system.h>
#include <rtems/score/powerpc.h>
#include <rtems/score/isr.h>
#include <rtems/bspIo.h>
#include <libcpu/raw_exception.h>
#include <libcpu/cpuIdent.h>

#include <string.h>

static rtems_raw_except_connect_data* 		raw_except_table;
static rtems_raw_except_connect_data  		default_raw_except_entry;
static rtems_raw_except_global_settings* 	local_settings;

void * codemove(void *, const void *, unsigned int, unsigned long);


static void* ppc_get_vector_addr(rtems_vector vector)
{
  unsigned vaddr;
  extern rtems_cpu_table Cpu_table;

  switch(vector) {
    /*
     * some vectors are located at odd addresses and only available
     * on some CPU derivates. this construct will handle them
     * if available
     */
#if defined(PPC_HAS_60X_VECTORS)
  /* Special case; altivec unavailable doesn't fit :-( */
  case ASM_VEC_VECTOR:
    vaddr = ASM_VEC_VECTOR_OFFSET;
    break;
#endif
#if defined(ASM_PIT_VECTOR)
  case ASM_PIT_VECTOR:
    vaddr = ASM_PIT_VECTOR_OFFSET;
    break;
#endif
#if defined(ASM_FIT_VECTOR)
  case ASM_FIT_VECTOR:
    vaddr = ASM_FIT_VECTOR_OFFSET;
    break;
#endif
#if defined(ASM_WDOG_VECTOR)
  case ASM_WDOG_VECTOR:
    vaddr = ASM_WDOG_VECTOR_OFFSET;
    break;
#endif
  default:
    vaddr = ((unsigned)vector) << 8;
    break;
  }
  if ( Cpu_table.exceptions_in_RAM )
    return ((void*)  vaddr);

  return ((void*)  (vaddr + 0xfff00000));
}


#if ( defined(mpc860) || defined(mpc821) )

int mpc860_vector_is_valid(rtems_vector vector)
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
  case ASM_FLOATASSIST_VECTOR:

  case ASM_SOFTEMUL_VECTOR:
  case ASM_ITLBMISS_VECTOR:
  case ASM_DTLBMISS_VECTOR:
  case ASM_ITLBERROR_VECTOR:
  case ASM_DTLBERROR_VECTOR:

  case ASM_DBREAK_VECTOR:
  case ASM_IBREAK_VECTOR:
  case ASM_PERIFBREAK_VECTOR:
  case ASM_DEVPORT_VECTOR:
    return 1;
  default: return 0;
  }
}
#endif

#if (defined(mpc555) || defined(mpc505))

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
      printk("Please complete libcpu/powerpc/shared/new-exceptions/raw_exception.c\n");
      printk("current_ppc_cpu = %x\n", current_ppc_cpu);
      return 0;
  }
}
#endif

#if defined(ppc405)
int ppc405_vector_is_valid(rtems_vector vector)

{
  switch(vector) {
  case ASM_RESET_VECTOR: /* fall through */
  case ASM_MACH_VECTOR:
  case ASM_PROT_VECTOR:
  case ASM_ISI_VECTOR:
  case ASM_EXT_VECTOR:
  case ASM_ALIGN_VECTOR:
  case ASM_PROG_VECTOR:
  case ASM_SYS_VECTOR:
  case ASM_PIT_VECTOR:
  case ASM_ITLBMISS_VECTOR:
  case ASM_DTLBMISS_VECTOR:
    return 1;
  default: return 0;
  }
}
#endif /* defined(ppc405) */

#if defined(PPC_HAS_60X_VECTORS) /* 60x style cpu types */

int altivec_vector_is_valid(rtems_vector vector)
{
  switch(vector) {
  case ASM_VEC_VECTOR:
  case ASM_VEC_ASSIST_VECTOR:
    return 1;
    default:
      break;
  }
  return 0;
}

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

int PSIM_vector_is_valid(rtems_vector vector)
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
    return 1;
  case ASM_SYS_VECTOR:
    return 0;
  case ASM_TRACE_VECTOR:
    return 1;
  case ASM_PERFMON_VECTOR:
    return 0;
  case ASM_IMISS_VECTOR: /* fall through */
  case ASM_DLMISS_VECTOR:
  case ASM_DSMISS_VECTOR:
  case ASM_ADDR_VECTOR:
  case ASM_SYSMGMT_VECTOR:
    return 1;
  case ASM_ITM_VECTOR:
    return 0;
  }
  return 0;
}

int mpc603_vector_is_valid(rtems_vector vector)
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
    return 1;
  case ASM_PERFMON_VECTOR:
    return 0;
  case ASM_IMISS_VECTOR: /* fall through */
  case ASM_DLMISS_VECTOR:
  case ASM_DSMISS_VECTOR:
  case ASM_ADDR_VECTOR:
  case ASM_SYSMGMT_VECTOR:
    return 1;
  case ASM_ITM_VECTOR:
    return 0;
  }
  return 0;
}

int mpc604_vector_is_valid(rtems_vector vector)
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
  case ASM_PERFMON_VECTOR:
    return 1;
  case ASM_IMISS_VECTOR: /* fall through */
  case ASM_DLMISS_VECTOR:
  case ASM_DSMISS_VECTOR:
    return 0;
  case ASM_ADDR_VECTOR: /* fall through */
  case ASM_SYSMGMT_VECTOR:
    return 1;
  case ASM_ITM_VECTOR:
    return 0;
  }
  return 0;
}

#endif /* 60x style cpu types */

int ppc_vector_is_valid(rtems_vector vector)
{
     switch (current_ppc_cpu) {
#if defined(PPC_HAS_60X_VECTORS)
	case PPC_7400:
            if ( altivec_vector_is_valid(vector) )
                return 1;
            /* else fall thru */
        case PPC_750:
            if (!mpc750_vector_is_valid(vector)) {
                return 0;
            }
            break;
        case PPC_7455:   /* Kate Feng */
        case PPC_7457: 
            if ( altivec_vector_is_valid(vector) )
                return 1;
            /* else fall thru */
        case PPC_604:
        case PPC_604e:
        case PPC_604r:
            if (!mpc604_vector_is_valid(vector)) {
                return 0;
            }
            break;
        case PPC_603:
        case PPC_603e:
        case PPC_603le:
        case PPC_603ev:
        case PPC_8260:
        /* case PPC_8240: -- same value as 8260 */
        case PPC_8245:
        case PPC_e300c1:
        case PPC_e300c2:
        case PPC_e300c3:
            if (!mpc603_vector_is_valid(vector)) {
                return 0;
            }
            break;
        case PPC_PSIM:
            if (!PSIM_vector_is_valid(vector)) {
                return 0;
            }
            break;
#endif
#if ( defined(mpc555) || defined(mpc505) )
        case PPC_5XX:
            if (!mpc5xx_vector_is_valid(vector)) {
                return 0;
            }
            break;
#endif
#if ( defined(mpc860) || defined(mpc821) )
        case PPC_860:
            if (!mpc860_vector_is_valid(vector)) {
                return 0;
            }
            break;
#endif
#if defined(ppc405)
        case PPC_405:
            if (!ppc405_vector_is_valid(vector)) {
                return 0;
            }
            break;
#endif
        default:
            printk("Please complete "
                   "libcpu/powerpc/new-exceptions/raw_exception.c\n"
                   "current_ppc_cpu = %x\n", current_ppc_cpu);
            return 0;
     }
     return 1;
}

int ppc_set_exception  (const rtems_raw_except_connect_data* except)
{
    ISR_Level       level;

    if (!ppc_vector_is_valid(except->exceptIndex)) {
      printk("ppc_set_exception: vector %d is not valid\n",
              except->exceptIndex);
      return 0;
    }
    /*
     * Check if default handler is actually connected. If not issue an error.
     * You must first get the current handler via mpc60x_get_current_exception
     * and then disconnect it using mpc60x_delete_exception.
     * RATIONALE : to always have the same transition by forcing the user
     * to get the previous handler before accepting to disconnect.
     */

    if (memcmp(ppc_get_vector_addr(except->exceptIndex),
               (void*)default_raw_except_entry.hdl.raw_hdl,
               default_raw_except_entry.hdl.raw_hdl_size)) {
      printk("ppc_set_exception: raw vector not installed\n");
      return 0;
    }

    _ISR_Disable(level);

    raw_except_table [except->exceptIndex] = *except;
    codemove((void*)ppc_get_vector_addr(except->exceptIndex),
	     except->hdl.raw_hdl,
	     except->hdl.raw_hdl_size,
	     PPC_CACHE_ALIGNMENT);
	if (except->on)
    	except->on(except);

    _ISR_Enable(level);
    return 1;
}

int ppc_get_current_exception (rtems_raw_except_connect_data* except)
{
  if (!ppc_vector_is_valid(except->exceptIndex)){
    return 0;
  }

  *except = raw_except_table [except->exceptIndex];

  return 1;
}

int ppc_delete_exception (const rtems_raw_except_connect_data* except)
{
  ISR_Level level;

  if (!ppc_vector_is_valid(except->exceptIndex)){
    return 0;
  }
  /*
   * Check if handler passed is actually connected. If not issue an error.
   * You must first get the current handler via ppc_get_current_exception
   * and then disconnect it using ppc_delete_exception.
   * RATIONALE : to always have the same transition by forcing the user
   * to get the previous handler before accepting to disconnect.
   */
  if (memcmp(ppc_get_vector_addr(except->exceptIndex),
	     (void*)except->hdl.raw_hdl,
	     except->hdl.raw_hdl_size)) {
      return 0;
  }
  _ISR_Disable(level);

  if (except->off)
  	except->off(except);
  codemove((void*)ppc_get_vector_addr(except->exceptIndex),
	   default_raw_except_entry.hdl.raw_hdl,
	   default_raw_except_entry.hdl.raw_hdl_size,
	   PPC_CACHE_ALIGNMENT);


  raw_except_table[except->exceptIndex] = default_raw_except_entry;
  raw_except_table[except->exceptIndex].exceptIndex = except->exceptIndex;

  _ISR_Enable(level);

  return 1;
}

/*
 * Exception global init.
 */
int ppc_init_exceptions (rtems_raw_except_global_settings* config)
{
    int        i;
    ISR_Level  level;

    /*
     * store various accelerators
     */
    raw_except_table 		= config->rawExceptHdlTbl;
    local_settings 		= config;
    default_raw_except_entry 	= config->defaultRawEntry;

    _ISR_Disable(level);

    for (i=0; i <= LAST_VALID_EXC; i++) {
      if (!ppc_vector_is_valid(i)){
	continue;
      }
      codemove((void*)ppc_get_vector_addr(i),
	     raw_except_table[i].hdl.raw_hdl,
	     raw_except_table[i].hdl.raw_hdl_size,
	     PPC_CACHE_ALIGNMENT);
      if (raw_except_table[i].hdl.raw_hdl != default_raw_except_entry.hdl.raw_hdl) {
	if (raw_except_table[i].on)
		raw_except_table[i].on(&raw_except_table[i]);
      }
      else {
	if (raw_except_table[i].off)
		raw_except_table[i].off(&raw_except_table[i]);
      }
    }
    _ISR_Enable(level);

    return 1;
}

int ppc_get_exception_config (rtems_raw_except_global_settings** config)
{
  *config = local_settings;
  return 1;
}
