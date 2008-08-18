#include <libcpu/cpuIdent.h>
#include <libcpu/raw_exception.h>

#define MTIVPR(prefix) asm volatile("mtivpr %0"::"r"(prefix));
#define MTIVOR(x, vec) asm volatile("mtivor"#x" %0"::"r"(vec));

/* Use during early init for initializing the e500 IVOR/IVPR registers */
void
e500_setup_raw_exceptions(void)
{
	unsigned c;
	if ( ! (c = ppc_cpu_is_bookE()) || PPC_BOOKE_405 == c )
		return;

	/* Set interupt vector prefix register */
	MTIVPR( ppc_exc_vector_base);

	/* setup vectors to be compatible with classic PPC */
	MTIVOR(0,  ppc_get_vector_addr(ASM_BOOKE_CRIT_VECTOR)); /* Critical input not (yet) supported; use reset vector */
	MTIVOR(1,  ppc_get_vector_addr(ASM_MACH_VECTOR));
	MTIVOR(2,  ppc_get_vector_addr(ASM_PROT_VECTOR));
	MTIVOR(3,  ppc_get_vector_addr(ASM_ISI_VECTOR));
	MTIVOR(4,  ppc_get_vector_addr(ASM_EXT_VECTOR));
	MTIVOR(5,  ppc_get_vector_addr(ASM_ALIGN_VECTOR));
	MTIVOR(6,  ppc_get_vector_addr(ASM_PROG_VECTOR));
	MTIVOR(7,  ppc_get_vector_addr(ASM_FLOAT_VECTOR));
	MTIVOR(8,  ppc_get_vector_addr(ASM_SYS_VECTOR));
	MTIVOR(9,  ppc_get_vector_addr(0x0b));
	MTIVOR(10, ppc_get_vector_addr(ASM_BOOKE_DEC_VECTOR));
	MTIVOR(11, ppc_get_vector_addr(ASM_BOOKE_FIT_VECTOR));
	MTIVOR(12, ppc_get_vector_addr(ASM_BOOKE_WDOG_VECTOR));
	MTIVOR(13, ppc_get_vector_addr(ASM_60X_DSMISS_VECTOR));
	MTIVOR(14, ppc_get_vector_addr(ASM_60X_DLMISS_VECTOR));
	MTIVOR(15, ppc_get_vector_addr(ASM_TRACE_VECTOR));
	MTIVOR(32, ppc_get_vector_addr(ASM_60X_VEC_VECTOR));
	MTIVOR(33, ppc_get_vector_addr(0x16));
	MTIVOR(34, ppc_get_vector_addr(0x15));
	MTIVOR(35, ppc_get_vector_addr(ASM_60X_PERFMON_VECTOR));
}

void e200_setup_raw_exceptions()
{
	if (current_ppc_cpu != PPC_e200z6) {
		return;
	}

	/* Interupt vector prefix register */
	MTIVPR( ppc_exc_vector_base);

	/* Interupt vector offset register */
	MTIVOR( 0,  0); /* Critical input */
	MTIVOR( 1,  ppc_get_vector_addr( ASM_MACH_VECTOR));
	MTIVOR( 2,  ppc_get_vector_addr( ASM_PROT_VECTOR));
	MTIVOR( 3,  ppc_get_vector_addr( ASM_ISI_VECTOR));
	MTIVOR( 4,  ppc_get_vector_addr( ASM_EXT_VECTOR));
	MTIVOR( 5,  ppc_get_vector_addr( ASM_ALIGN_VECTOR));
	MTIVOR( 6,  ppc_get_vector_addr( ASM_PROG_VECTOR));
	MTIVOR( 7,  ppc_get_vector_addr( ASM_FLOAT_VECTOR));
	MTIVOR( 8,  ppc_get_vector_addr( ASM_SYS_VECTOR));
	MTIVOR( 9,  0); /* APU unavailable */
	MTIVOR( 10, ppc_get_vector_addr( ASM_BOOKE_DEC_VECTOR));
	MTIVOR( 11, ppc_get_vector_addr( ASM_BOOKE_FIT_VECTOR));
	MTIVOR( 12, ppc_get_vector_addr( ASM_BOOKE_WDOG_VECTOR));
	MTIVOR( 13, ppc_get_vector_addr( ASM_BOOKE_ITLBMISS_VECTOR));
	MTIVOR( 14, ppc_get_vector_addr( ASM_BOOKE_DTLBMISS_VECTOR));
	MTIVOR( 15, ppc_get_vector_addr( ASM_TRACE_VECTOR));
	MTIVOR( 32, ppc_get_vector_addr( ASM_E200_SPE_UNAVAILABLE_VECTOR));
	MTIVOR( 33, ppc_get_vector_addr( ASM_E200_SPE_DATA_VECTOR));
	MTIVOR( 34, ppc_get_vector_addr( ASM_E200_SPE_ROUND_VECTOR));
}
