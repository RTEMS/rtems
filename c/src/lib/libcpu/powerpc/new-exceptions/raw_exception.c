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
#include <rtems.h>
#include <rtems/system.h>
#include <rtems/score/powerpc.h>
#include <rtems/score/isr.h>
#include <rtems/bspIo.h>
#include <libcpu/raw_exception.h>
#include <libcpu/cpuIdent.h>

#include <string.h>

#ifdef __ppc_generic
#define PPC_HAS_60X_VECTORS
#endif

static rtems_raw_except_connect_data* 		raw_except_table;
static rtems_raw_except_connect_data  		default_raw_except_entry;
static rtems_raw_except_global_settings* 	local_settings;

void * codemove(void *, const void *, unsigned int, unsigned long);

boolean bsp_exceptions_in_RAM = TRUE;

void* ppc_get_vector_addr(rtems_vector vector)
{
  unsigned vaddr;

  vaddr = ((unsigned)vector) << 8;

  switch(vector) {
    /*
     * some vectors are located at odd addresses and only available
     * on some CPU derivates. this construct will handle them
     * if available
     */
  /* Special case; altivec unavailable doesn't fit :-( */
  case ASM_60X_VEC_VECTOR:
#ifndef ASM_60X_VEC_VECTOR_OFFSET
#define ASM_60X_VEC_VECTOR_OFFSET 0xf20
#endif
  	if ( ppc_cpu_has_altivec() )
   		vaddr = ASM_60X_VEC_VECTOR_OFFSET;
    break;

#if defined(ASM_BOOKE_FIT_VECTOR)
  case ASM_BOOKE_FIT_VECTOR:
#ifndef ASM_BOOKE_FIT_VECTOR_OFFSET
#define ASM_BOOKE_FIT_VECTOR_OFFSET 0x1010
#endif
  	if ( PPC_405 == current_ppc_cpu )
    	vaddr = ASM_BOOKE_FIT_VECTOR_OFFSET;
    break;
#endif
#if defined(ASM_BOOKE_WDOG_VECTOR)
  case ASM_BOOKE_WDOG_VECTOR:
#ifndef ASM_BOOKE_WDOG_VECTOR_OFFSET
#define ASM_BOOKE_WDOG_VECTOR_OFFSET 0x1020
#endif
  	if ( PPC_405 == current_ppc_cpu )
    	vaddr = ASM_BOOKE_WDOG_VECTOR_OFFSET;
    break;
#endif
  default:
    break;
  }
  if ( bsp_exceptions_in_RAM )
    return ((void*)  vaddr);

  return ((void*)  (vaddr + 0xfff00000));
}


#if ( defined(mpc860) || defined(mpc821) || defined(__ppc_generic) )

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
  case ASM_8XX_FLOATASSIST_VECTOR:

  case ASM_8XX_SOFTEMUL_VECTOR:
  case ASM_8XX_ITLBMISS_VECTOR:
  case ASM_8XX_DTLBMISS_VECTOR:
  case ASM_8XX_ITLBERROR_VECTOR:
  case ASM_8XX_DTLBERROR_VECTOR:

  case ASM_8XX_DBREAK_VECTOR:
  case ASM_8XX_IBREAK_VECTOR:
  case ASM_8XX_PERIFBREAK_VECTOR:
  case ASM_8XX_DEVPORT_VECTOR:
    return 1;
  default: return 0;
  }
}
#endif

#if (defined(mpc555) || defined(mpc505) || defined(__ppc_generic))

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
        case ASM_5XX_FLOATASSIST_VECTOR:

        case ASM_5XX_SOFTEMUL_VECTOR:

        case ASM_5XX_IPROT_VECTOR:
        case ASM_5XX_DPROT_VECTOR:

        case ASM_5XX_DBREAK_VECTOR:
        case ASM_5XX_IBREAK_VECTOR:
        case ASM_5XX_MEBREAK_VECTOR:
        case ASM_5XX_NMEBREAK_VECTOR:
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

#if ( defined(ppc405) || defined(__ppc_generic) )
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
  case ASM_BOOKE_PIT_VECTOR:
  case ASM_BOOKE_ITLBMISS_VECTOR:
  case ASM_BOOKE_DTLBMISS_VECTOR:
    return 1;
  default: return 0;
  }
}
#endif /* defined(ppc405) */

#if defined(PPC_HAS_60X_VECTORS) /* 60x style cpu types */

int altivec_vector_is_valid(rtems_vector vector)
{
	if ( ppc_cpu_has_altivec() ) {
		switch(vector) {
			case ASM_60X_VEC_VECTOR:
			case ASM_60X_VEC_ASSIST_VECTOR:
				return 1;
			default:
				break;
		}
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
  case ASM_60X_ADDR_VECTOR:
  case ASM_60X_SYSMGMT_VECTOR:
  case ASM_60X_ITM_VECTOR:
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
  case ASM_60X_PERFMON_VECTOR:
    return 0;
  case ASM_60X_IMISS_VECTOR: /* fall through */
  case ASM_60X_DLMISS_VECTOR:
  case ASM_60X_DSMISS_VECTOR:
  case ASM_60X_ADDR_VECTOR:
  case ASM_60X_SYSMGMT_VECTOR:
    return 1;
  case ASM_60X_ITM_VECTOR:
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
  case ASM_60X_PERFMON_VECTOR:
    return 0;
  case ASM_60X_IMISS_VECTOR: /* fall through */
  case ASM_60X_DLMISS_VECTOR:
  case ASM_60X_DSMISS_VECTOR:
  case ASM_60X_ADDR_VECTOR:
  case ASM_60X_SYSMGMT_VECTOR:
    return 1;
  case ASM_60X_ITM_VECTOR:
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
  case ASM_60X_PERFMON_VECTOR:
    return 1;
  case ASM_60X_IMISS_VECTOR: /* fall through */
  case ASM_60X_DLMISS_VECTOR:
  case ASM_60X_DSMISS_VECTOR:
    return 0;
  case ASM_60X_ADDR_VECTOR: /* fall through */
  case ASM_60X_SYSMGMT_VECTOR:
    return 1;
  case ASM_60X_ITM_VECTOR:
    return 0;
  }
  return 0;
}

int e500_vector_is_valid(rtems_vector vector)
{
	switch (vector) {
	case ASM_RESET_VECTOR:
	case ASM_MACH_VECTOR:
	case ASM_PROT_VECTOR:
	case ASM_ISI_VECTOR:
	case ASM_EXT_VECTOR:
	case ASM_ALIGN_VECTOR:
	case ASM_PROG_VECTOR:
	case ASM_FLOAT_VECTOR:
	case ASM_SYS_VECTOR:
	case /* APU unavailable      */ 0x0b:
	case ASM_DEC_VECTOR:
	case ASM_60X_DLMISS_VECTOR:
	case ASM_60X_DSMISS_VECTOR:
	case ASM_TRACE_VECTOR:
	case ASM_60X_VEC_VECTOR:
	case ASM_60X_PERFMON_VECTOR:

	case 0x13 /*ASM_BOOKE_FIT_VECTOR*/:
	case 0x14 /*ASM_BOOKE_WDOG_VECTOR*/:
	case /* emb FP data          */ 0x15:
	case /* emb FP round         */ 0x16:
		return 1;
	default:
		break;
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
		case PPC_8540:
			if ( !e500_vector_is_valid(vector) ) {
				return 0;
			}
			break;
#endif
#if ( defined(mpc555) || defined(mpc505) || defined(__ppc_generic) )
        case PPC_5XX:
            if (!mpc5xx_vector_is_valid(vector)) {
                return 0;
            }
            break;
#endif
#if ( defined(mpc860) || defined(mpc821) || defined(__ppc_generic) )
        case PPC_860:
            if (!mpc860_vector_is_valid(vector)) {
                return 0;
            }
            break;
#endif
#if ( defined(ppc405) || defined(__ppc_generic) )
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
    rtems_interrupt_level k;

    if (!ppc_vector_is_valid(except->hdl.vector)) {
      printk("ppc_set_exception: vector %d is not valid\n",
              except->hdl.vector);
      return 0;
    }
    /*
     * Check if default handler is actually connected. If not issue an error.
     * You must first get the current handler via mpc60x_get_current_exception
     * and then disconnect it using mpc60x_delete_exception.
     * RATIONALE : to always have the same transition by forcing the user
     * to get the previous handler before accepting to disconnect.
     */

    if (memcmp(ppc_get_vector_addr(except->hdl.vector),
               (void*)default_raw_except_entry.hdl.raw_hdl,
               default_raw_except_entry.hdl.raw_hdl_size)) {
      printk("ppc_set_exception: raw vector not installed\n");
      return 0;
    }

    rtems_interrupt_disable(k);

    raw_except_table [except->exceptIndex] = *except;
    codemove(ppc_get_vector_addr(except->hdl.vector),
	     except->hdl.raw_hdl,
	     except->hdl.raw_hdl_size,
	     PPC_CACHE_ALIGNMENT);
	if (except->on)
    	except->on(except);

    rtems_interrupt_enable(k);
    return 1;
}

int ppc_get_current_exception (rtems_raw_except_connect_data* except)
{
	rtems_interrupt_level k;
	int i;

	if (!ppc_vector_is_valid(except->hdl.vector)){
		return 0;
	}

	for (i=0; i < local_settings->exceptSize; i++) {
		if ( raw_except_table[i].hdl.vector == except->hdl.vector ) {
			rtems_interrupt_disable(k);
				if ( raw_except_table[i].hdl.vector == except->hdl.vector ) {
					*except = raw_except_table[i];	
					rtems_interrupt_enable(k);
					return 1;
				}
			rtems_interrupt_enable(k);
		}
	}

	return 0;
}

int ppc_delete_exception (const rtems_raw_except_connect_data* except)
{
  rtems_interrupt_level k;

  if (!ppc_vector_is_valid(except->hdl.vector)){
    return 0;
  }
  /*
   * Check if handler passed is actually connected. If not issue an error.
   * You must first get the current handler via ppc_get_current_exception
   * and then disconnect it using ppc_delete_exception.
   * RATIONALE : to always have the same transition by forcing the user
   * to get the previous handler before accepting to disconnect.
   */
  if (memcmp(ppc_get_vector_addr(except->hdl.vector),
	     (void*)except->hdl.raw_hdl,
	     except->hdl.raw_hdl_size)) {
      return 0;
  }
  rtems_interrupt_disable(k);

  if (except->off)
	  except->off(except);
  codemove(ppc_get_vector_addr(except->hdl.vector),
	   default_raw_except_entry.hdl.raw_hdl,
	   default_raw_except_entry.hdl.raw_hdl_size,
	   PPC_CACHE_ALIGNMENT);


  raw_except_table[except->exceptIndex] = default_raw_except_entry;
  raw_except_table[except->exceptIndex].exceptIndex = except->exceptIndex;
  raw_except_table[except->exceptIndex].hdl.vector  = except->hdl.vector;

  rtems_interrupt_enable(k);

  return 1;
}

/*
 * Exception global init.
 */
int ppc_init_exceptions (rtems_raw_except_global_settings* config)
{
	rtems_interrupt_level k;
    int        i;

    /*
     * store various accelerators
     */
    raw_except_table 		= config->rawExceptHdlTbl;
    local_settings 		= config;
    default_raw_except_entry 	= config->defaultRawEntry;

    rtems_interrupt_disable(k);

	if ( ppc_cpu_is_bookE() ) {
		e500_setup_raw_exceptions();
	}

	for (i=0; i < config->exceptSize; i++) {
		if (!ppc_vector_is_valid(raw_except_table[i].hdl.vector)){
			continue;
		}
		codemove(ppc_get_vector_addr(raw_except_table[i].hdl.vector),
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
    rtems_interrupt_enable(k);

    return 1;
}

int ppc_get_exception_config (rtems_raw_except_global_settings** config)
{
  *config = local_settings;
  return 1;
}
