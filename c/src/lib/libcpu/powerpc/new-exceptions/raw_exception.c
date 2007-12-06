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

/* DO NOT INTRODUCE #ifdef <cpu_flavor> in this file */

/* enum ppc_raw_exception_category should fit into this type;
 * we are setting up arrays of these for all known CPUs
 * hence the attempt to save a few bytes.
 */
typedef uint8_t cat_ini_t;

static rtems_raw_except_connect_data* 		raw_except_table;
static rtems_raw_except_connect_data  		default_raw_except_entry;
static rtems_raw_except_global_settings* 	local_settings;

void * codemove(void *, const void *, unsigned int, unsigned long);

boolean bsp_exceptions_in_RAM = TRUE;

/* DEPRECATED VARIABLE; we need this to support
 * libbsp/powerpc/shared/vectors/vectors.S;
 * new BSPs should NOT use this.
 */
uint32_t bsp_raw_vector_is_405_critical = 0;

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

  case ASM_BOOKE_FIT_VECTOR:
#ifndef ASM_BOOKE_FIT_VECTOR_OFFSET
#define ASM_BOOKE_FIT_VECTOR_OFFSET 0x1010
#endif
  	if ( PPC_405 == current_ppc_cpu )
    	vaddr = ASM_BOOKE_FIT_VECTOR_OFFSET;
    break;
  case ASM_BOOKE_WDOG_VECTOR:
#ifndef ASM_BOOKE_WDOG_VECTOR_OFFSET
#define ASM_BOOKE_WDOG_VECTOR_OFFSET 0x1020
#endif
  	if ( PPC_405 == current_ppc_cpu )
    	vaddr = ASM_BOOKE_WDOG_VECTOR_OFFSET;
    break;
  default:
    break;
  }
  if ( bsp_exceptions_in_RAM )
    return ((void*)  vaddr);

  return ((void*)  (vaddr + 0xfff00000));
}

static cat_ini_t mpc_860_vector_categories[LAST_VALID_EXC + 1] = {
  [ ASM_RESET_VECTOR           ] = PPC_EXC_CLASSIC,
  [ ASM_MACH_VECTOR            ] = PPC_EXC_CLASSIC,
  [ ASM_PROT_VECTOR            ] = PPC_EXC_CLASSIC,
  [ ASM_ISI_VECTOR             ] = PPC_EXC_CLASSIC,
  [ ASM_EXT_VECTOR             ] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ ASM_ALIGN_VECTOR           ] = PPC_EXC_CLASSIC,
  [ ASM_PROG_VECTOR            ] = PPC_EXC_CLASSIC,
  [ ASM_FLOAT_VECTOR           ] = PPC_EXC_CLASSIC,
  [ ASM_DEC_VECTOR             ] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
    
  [ ASM_SYS_VECTOR             ] = PPC_EXC_CLASSIC,
  [ ASM_TRACE_VECTOR           ] = PPC_EXC_CLASSIC,
  [ ASM_8XX_FLOATASSIST_VECTOR ] = PPC_EXC_CLASSIC,

  [ ASM_8XX_SOFTEMUL_VECTOR    ] = PPC_EXC_CLASSIC,
  [ ASM_8XX_ITLBMISS_VECTOR    ] = PPC_EXC_CLASSIC,
  [ ASM_8XX_DTLBMISS_VECTOR    ] = PPC_EXC_CLASSIC,
  [ ASM_8XX_ITLBERROR_VECTOR   ] = PPC_EXC_CLASSIC,
  [ ASM_8XX_DTLBERROR_VECTOR   ] = PPC_EXC_CLASSIC,

  [ ASM_8XX_DBREAK_VECTOR      ] = PPC_EXC_CLASSIC,
  [ ASM_8XX_IBREAK_VECTOR      ] = PPC_EXC_CLASSIC,
  [ ASM_8XX_PERIFBREAK_VECTOR  ] = PPC_EXC_CLASSIC,
  [ ASM_8XX_DEVPORT_VECTOR     ] = PPC_EXC_CLASSIC,
};


static cat_ini_t mpc_5xx_vector_categories[LAST_VALID_EXC + 1] = {
  [ ASM_RESET_VECTOR           ] = PPC_EXC_CLASSIC,
  [ ASM_MACH_VECTOR            ] = PPC_EXC_CLASSIC,

  [ ASM_EXT_VECTOR             ] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ ASM_ALIGN_VECTOR           ] = PPC_EXC_CLASSIC,
  [ ASM_PROG_VECTOR            ] = PPC_EXC_CLASSIC,
  [ ASM_FLOAT_VECTOR           ] = PPC_EXC_CLASSIC,
  [ ASM_DEC_VECTOR             ] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,

  [ ASM_SYS_VECTOR             ] = PPC_EXC_CLASSIC,
  [ ASM_TRACE_VECTOR           ] = PPC_EXC_CLASSIC,
  [ ASM_5XX_FLOATASSIST_VECTOR ] = PPC_EXC_CLASSIC,

  [ ASM_5XX_SOFTEMUL_VECTOR    ] = PPC_EXC_CLASSIC,

  [ ASM_5XX_IPROT_VECTOR       ] = PPC_EXC_CLASSIC,
  [ ASM_5XX_DPROT_VECTOR       ] = PPC_EXC_CLASSIC,

  [ ASM_5XX_DBREAK_VECTOR      ] = PPC_EXC_CLASSIC,
  [ ASM_5XX_IBREAK_VECTOR      ] = PPC_EXC_CLASSIC,
  [ ASM_5XX_MEBREAK_VECTOR     ] = PPC_EXC_CLASSIC,
  [ ASM_5XX_NMEBREAK_VECTOR    ] = PPC_EXC_CLASSIC,
};

static cat_ini_t ppc_405_vector_categories[LAST_VALID_EXC + 1] = {
  [ ASM_EXT_VECTOR             ] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ ASM_BOOKE_PIT_VECTOR       ] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,

  [ ASM_PROT_VECTOR            ] = PPC_EXC_CLASSIC,
  [ ASM_ISI_VECTOR             ] = PPC_EXC_CLASSIC,
  [ ASM_ALIGN_VECTOR           ] = PPC_EXC_CLASSIC,
  [ ASM_PROG_VECTOR            ] = PPC_EXC_CLASSIC,
  [ ASM_SYS_VECTOR             ] = PPC_EXC_CLASSIC,
  [ ASM_BOOKE_ITLBMISS_VECTOR  ] = PPC_EXC_CLASSIC,
  [ ASM_BOOKE_DTLBMISS_VECTOR  ] = PPC_EXC_CLASSIC,

  [ ASM_BOOKE_CRIT_VECTOR      ] = PPC_EXC_405_CRITICAL | PPC_EXC_ASYNC,
  [ ASM_MACH_VECTOR            ] = PPC_EXC_405_CRITICAL,
};


#define PPC_BASIC_VECS \
  [ ASM_RESET_VECTOR      ] = PPC_EXC_CLASSIC, \
  [ ASM_MACH_VECTOR       ] = PPC_EXC_CLASSIC, \
  [ ASM_PROT_VECTOR       ] = PPC_EXC_CLASSIC, \
  [ ASM_ISI_VECTOR        ] = PPC_EXC_CLASSIC, \
  [ ASM_EXT_VECTOR        ] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC, \
  [ ASM_ALIGN_VECTOR      ] = PPC_EXC_CLASSIC, \
  [ ASM_PROG_VECTOR       ] = PPC_EXC_CLASSIC, \
  [ ASM_FLOAT_VECTOR      ] = PPC_EXC_CLASSIC, \
  [ ASM_DEC_VECTOR        ] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC, \
  [ ASM_SYS_VECTOR        ] = PPC_EXC_CLASSIC, \
  [ ASM_TRACE_VECTOR      ] = PPC_EXC_CLASSIC

static ppc_raw_exception_category altivec_vector_is_valid(rtems_vector vector)
{
	if ( ppc_cpu_has_altivec() ) {
		switch(vector) {
			case ASM_60X_VEC_VECTOR:
			case ASM_60X_VEC_ASSIST_VECTOR:
				return PPC_EXC_CLASSIC;
			default:
				break;
		}
	}
  return PPC_EXC_INVALID;
}

static cat_ini_t mpc_750_vector_categories[LAST_VALID_EXC + 1] = {
	PPC_BASIC_VECS,
  [ ASM_60X_SYSMGMT_VECTOR ] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ ASM_60X_ADDR_VECTOR    ] = PPC_EXC_CLASSIC,
  [ ASM_60X_ITM_VECTOR     ] = PPC_EXC_CLASSIC,
};

static cat_ini_t psim_vector_categories[LAST_VALID_EXC + 1] = {
  [ ASM_RESET_VECTOR       ] = PPC_EXC_CLASSIC,
  [ ASM_MACH_VECTOR        ] = PPC_EXC_CLASSIC,
  [ ASM_PROT_VECTOR        ] = PPC_EXC_CLASSIC,
  [ ASM_ISI_VECTOR         ] = PPC_EXC_CLASSIC,
  [ ASM_EXT_VECTOR         ] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ ASM_ALIGN_VECTOR       ] = PPC_EXC_CLASSIC,
  [ ASM_PROG_VECTOR        ] = PPC_EXC_CLASSIC,
  [ ASM_FLOAT_VECTOR       ] = PPC_EXC_CLASSIC,
  [ ASM_DEC_VECTOR         ] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ ASM_SYS_VECTOR         ] = PPC_EXC_INVALID,
  [ ASM_TRACE_VECTOR       ] = PPC_EXC_CLASSIC,
  [ ASM_60X_PERFMON_VECTOR ] = PPC_EXC_INVALID,
  [ ASM_60X_SYSMGMT_VECTOR ] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ ASM_60X_IMISS_VECTOR   ] = PPC_EXC_CLASSIC,
  [ ASM_60X_DLMISS_VECTOR  ] = PPC_EXC_CLASSIC,
  [ ASM_60X_DSMISS_VECTOR  ] = PPC_EXC_CLASSIC,
  [ ASM_60X_ADDR_VECTOR    ] = PPC_EXC_CLASSIC,
  [ ASM_60X_ITM_VECTOR     ] = PPC_EXC_INVALID,
};

static cat_ini_t mpc_603_vector_categories[LAST_VALID_EXC + 1] = {
	PPC_BASIC_VECS,
  [ ASM_60X_PERFMON_VECTOR ] = PPC_EXC_INVALID,
  [ ASM_60X_SYSMGMT_VECTOR ] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ ASM_60X_IMISS_VECTOR   ] = PPC_EXC_CLASSIC,
  [ ASM_60X_DLMISS_VECTOR  ] = PPC_EXC_CLASSIC,
  [ ASM_60X_DSMISS_VECTOR  ] = PPC_EXC_CLASSIC,
  [ ASM_60X_ADDR_VECTOR    ] = PPC_EXC_CLASSIC,
  [ ASM_60X_ITM_VECTOR     ] = PPC_EXC_INVALID,
};

static cat_ini_t mpc_604_vector_categories[LAST_VALID_EXC + 1] = {
	PPC_BASIC_VECS,
  [ ASM_60X_PERFMON_VECTOR ] = PPC_EXC_CLASSIC,
  [ ASM_60X_IMISS_VECTOR   ] = PPC_EXC_INVALID,
  [ ASM_60X_DLMISS_VECTOR  ] = PPC_EXC_INVALID,
  [ ASM_60X_DSMISS_VECTOR  ] = PPC_EXC_INVALID,
  [ ASM_60X_SYSMGMT_VECTOR ] = PPC_EXC_CLASSIC | PPC_EXC_ASYNC,
  [ ASM_60X_ADDR_VECTOR    ] = PPC_EXC_CLASSIC,
  [ ASM_60X_ITM_VECTOR     ] = PPC_EXC_INVALID,
};

static cat_ini_t e500_vector_categories[LAST_VALID_EXC + 1] = {
  [ ASM_MACH_VECTOR                 ] = PPC_EXC_E500_MACHCHK,

  [ ASM_BOOKE_CRIT_VECTOR           ] = PPC_EXC_BOOKE_CRITICAL | PPC_EXC_ASYNC,
  [ ASM_BOOKE_WDOG_VECTOR           ] = PPC_EXC_BOOKE_CRITICAL | PPC_EXC_ASYNC,
  [ ASM_TRACE_VECTOR                ] = PPC_EXC_BOOKE_CRITICAL,

  [ ASM_EXT_VECTOR                  ] = PPC_EXC_CLASSIC        | PPC_EXC_ASYNC,
  /* FIXME: should eventually go to the PIT vector + cleanup clock driver */
  [ ASM_DEC_VECTOR                  ] = PPC_EXC_CLASSIC        | PPC_EXC_ASYNC,
  [ ASM_BOOKE_FIT_VECTOR            ] = PPC_EXC_CLASSIC        | PPC_EXC_ASYNC,

  [ ASM_PROT_VECTOR                 ] = PPC_EXC_CLASSIC,
  [ ASM_ISI_VECTOR                  ] = PPC_EXC_CLASSIC,
  [ ASM_ALIGN_VECTOR                ] = PPC_EXC_CLASSIC,
  [ ASM_PROG_VECTOR                 ] = PPC_EXC_CLASSIC,
  [ ASM_FLOAT_VECTOR                ] = PPC_EXC_CLASSIC,
  [ ASM_SYS_VECTOR                  ] = PPC_EXC_CLASSIC,
  [ /* APU unavailable      */ 0x0b ] = PPC_EXC_CLASSIC,

  [ ASM_60X_DLMISS_VECTOR           ] = PPC_EXC_CLASSIC,
  [ ASM_60X_DSMISS_VECTOR           ] = PPC_EXC_CLASSIC,
  [ ASM_60X_VEC_VECTOR              ] = PPC_EXC_CLASSIC,
  [ ASM_60X_PERFMON_VECTOR          ] = PPC_EXC_CLASSIC,

  [ /* emb FP data          */ 0x15 ] = PPC_EXC_CLASSIC,
  [ /* emb FP round         */ 0x16 ] = PPC_EXC_CLASSIC,
};

ppc_raw_exception_category ppc_vector_is_valid(rtems_vector vector)
{
ppc_raw_exception_category rval = PPC_EXC_INVALID;

	if ( vector > LAST_VALID_EXC )
		return PPC_EXC_INVALID;

     switch (current_ppc_cpu) {
	case PPC_7400:
            if ( ( rval = altivec_vector_is_valid(vector)) )
                return rval;
            /* else fall thru */
        case PPC_750:
			rval = mpc_750_vector_categories[vector];
            break;
        case PPC_7455:   /* Kate Feng */
        case PPC_7457: 
            if ( ( rval = altivec_vector_is_valid(vector) ) )
                return rval;
            /* else fall thru */
        case PPC_604:
        case PPC_604e:
        case PPC_604r:
			rval = mpc_604_vector_categories[vector];
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
			rval = mpc_603_vector_categories[vector];
            break;
        case PPC_PSIM:
			rval = psim_vector_categories[vector];
            break;
		case PPC_8540:
			rval = e500_vector_categories[vector];
			break;
        case PPC_5XX:
			rval = mpc_5xx_vector_categories[vector];
            break;
        case PPC_860:
			rval = mpc_860_vector_categories[vector];
            break;
        case PPC_405:
			rval = ppc_405_vector_categories[vector];
            break;
        default:
            printk("Please complete "
                   "libcpu/powerpc/new-exceptions/raw_exception.c\n"
                   "current_ppc_cpu = %x\n", current_ppc_cpu);
            return PPC_EXC_INVALID;
     }
     return rval;
}

int ppc_set_exception  (const rtems_raw_except_connect_data* except)
{
    rtems_interrupt_level k;

    if ( PPC_EXC_INVALID == ppc_vector_is_valid(except->hdl.vector) ) {
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

	if ( PPC_EXC_INVALID == ppc_vector_is_valid(except->hdl.vector) ) {
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

  if ( PPC_EXC_INVALID == ppc_vector_is_valid(except->hdl.vector) ) {
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

	/* Need to support libbsp/powerpc/shared/vectors.S
	 * (hopefully this can go away some day)
	 * We also rely on LAST_VALID_EXC < 32
	 */
	for ( i=0; i <= LAST_VALID_EXC; i++ ) {
		if ( PPC_EXC_405_CRITICAL == ppc_vector_is_valid( i ) )
			bsp_raw_vector_is_405_critical |= (1<<i);
	}

	for (i=0; i < config->exceptSize; i++) {
		if ( PPC_EXC_INVALID == ppc_vector_is_valid(raw_except_table[i].hdl.vector) ) {
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
