/* -*- asm -*- */
#ifndef __PPC_OFFS_H
#define __PPC_OFFS_H

/*
 * Offsets for various Contexts
 */
	.set	GP_1, 0
	.set	GP_2, (GP_1 + 4)
	.set	GP_13, (GP_2 + 4)
	.set	GP_14, (GP_13 + 4)

	.set	GP_15, (GP_14 + 4)
	.set	GP_16, (GP_15 + 4)
	.set	GP_17, (GP_16 + 4)
	.set	GP_18, (GP_17 + 4)

	.set	GP_19, (GP_18 + 4)
	.set	GP_20, (GP_19 + 4)
	.set	GP_21, (GP_20 + 4)
	.set	GP_22, (GP_21 + 4)

	.set	GP_23, (GP_22 + 4)
	.set	GP_24, (GP_23 + 4)
	.set	GP_25, (GP_24 + 4)
	.set	GP_26, (GP_25 + 4)

	.set	GP_27, (GP_26 + 4)
	.set	GP_28, (GP_27 + 4)
	.set	GP_29, (GP_28 + 4)
	.set	GP_30, (GP_29 + 4)

	.set	GP_31, (GP_30 + 4)
	.set	GP_CR, (GP_31 + 4)
	.set	GP_PC, (GP_CR + 4)
	.set	GP_MSR, (GP_PC + 4)

#if (PPC_HAS_DOUBLE == 1)
	.set	FP_0, 0
	.set	FP_1, (FP_0 + 8)
	.set	FP_2, (FP_1 + 8)
	.set	FP_3, (FP_2 + 8)
	.set	FP_4, (FP_3 + 8)
	.set	FP_5, (FP_4 + 8)
	.set	FP_6, (FP_5 + 8)
	.set	FP_7, (FP_6 + 8)
	.set	FP_8, (FP_7 + 8)
	.set	FP_9, (FP_8 + 8)
	.set	FP_10, (FP_9 + 8)
	.set	FP_11, (FP_10 + 8)
	.set	FP_12, (FP_11 + 8)
	.set	FP_13, (FP_12 + 8)
	.set	FP_14, (FP_13 + 8)
	.set	FP_15, (FP_14 + 8)
	.set	FP_16, (FP_15 + 8)
	.set	FP_17, (FP_16 + 8)
	.set	FP_18, (FP_17 + 8)
	.set	FP_19, (FP_18 + 8)
	.set	FP_20, (FP_19 + 8)
	.set	FP_21, (FP_20 + 8)
	.set	FP_22, (FP_21 + 8)
	.set	FP_23, (FP_22 + 8)
	.set	FP_24, (FP_23 + 8)
	.set	FP_25, (FP_24 + 8)
	.set	FP_26, (FP_25 + 8)
	.set	FP_27, (FP_26 + 8)
	.set	FP_28, (FP_27 + 8)
	.set	FP_29, (FP_28 + 8)
	.set	FP_30, (FP_29 + 8)
	.set	FP_31, (FP_30 + 8)
	.set	FP_FPSCR, (FP_31 + 8)
#else
	.set	FP_0, 0
	.set	FP_1, (FP_0 + 4)
	.set	FP_2, (FP_1 + 4)
	.set	FP_3, (FP_2 + 4)
	.set	FP_4, (FP_3 + 4)
	.set	FP_5, (FP_4 + 4)
	.set	FP_6, (FP_5 + 4)
	.set	FP_7, (FP_6 + 4)
	.set	FP_8, (FP_7 + 4)
	.set	FP_9, (FP_8 + 4)
	.set	FP_10, (FP_9 + 4)
	.set	FP_11, (FP_10 + 4)
	.set	FP_12, (FP_11 + 4)
	.set	FP_13, (FP_12 + 4)
	.set	FP_14, (FP_13 + 4)
	.set	FP_15, (FP_14 + 4)
	.set	FP_16, (FP_15 + 4)
	.set	FP_17, (FP_16 + 4)
	.set	FP_18, (FP_17 + 4)
	.set	FP_19, (FP_18 + 4)
	.set	FP_20, (FP_19 + 4)
	.set	FP_21, (FP_20 + 4)
	.set	FP_22, (FP_21 + 4)
	.set	FP_23, (FP_22 + 4)
	.set	FP_24, (FP_23 + 4)
	.set	FP_25, (FP_24 + 4)
	.set	FP_26, (FP_25 + 4)
	.set	FP_27, (FP_26 + 4)
	.set	FP_28, (FP_27 + 4)
	.set	FP_29, (FP_28 + 4)
	.set	FP_30, (FP_29 + 4)
	.set	FP_31, (FP_30 + 4)
	.set	FP_FPSCR, (FP_31 + 4)
#endif
	
	.set	IP_LINK, 0
#if (PPC_ABI == PPC_ABI_POWEROPEN || PPC_ABI == PPC_ABI_GCC27)
	.set	IP_0, (IP_LINK + 56)
#else
	.set	IP_0, (IP_LINK + 8)
#endif
	.set	IP_2, (IP_0 + 4)

	.set	IP_3, (IP_2 + 4)
	.set	IP_4, (IP_3 + 4)
	.set	IP_5, (IP_4 + 4)
	.set	IP_6, (IP_5 + 4)
	
	.set	IP_7, (IP_6 + 4)
	.set	IP_8, (IP_7 + 4)
	.set	IP_9, (IP_8 + 4)
	.set	IP_10, (IP_9 + 4)
	
	.set	IP_11, (IP_10 + 4)
	.set	IP_12, (IP_11 + 4)
	.set	IP_13, (IP_12 + 4)
	.set	IP_28, (IP_13 + 4)

	.set	IP_29, (IP_28 + 4)
	.set	IP_30, (IP_29 + 4)
	.set	IP_31, (IP_30 + 4)
	.set	IP_CR, (IP_31 + 4)
	
	.set	IP_CTR, (IP_CR + 4)
	.set	IP_XER, (IP_CTR + 4)
	.set	IP_LR, (IP_XER + 4)
	.set	IP_PC, (IP_LR + 4)
	
	.set	IP_MSR, (IP_PC + 4)
	.set	IP_END, (IP_MSR + 16)
	
	/* _CPU_IRQ_info offsets */

	/* These must be in this order */
	.set	Nest_level, 0
	.set	Disable_level, 4
	.set	Vector_table, 8
	.set	Stack, 12
#if (PPC_ABI == PPC_ABI_POWEROPEN)
	.set	Dispatch_r2, 16
	.set	Switch_necessary, 20
#else
	.set	Default_r2, 16
#if (PPC_ABI != PPC_ABI_GCC27)
	.set	Default_r13, 20
	.set	Switch_necessary, 24
#else
	.set	Switch_necessary, 20
#endif
#endif
	.set	Signal, Switch_necessary + 4
        .set    msr_initial, Signal + 4
	
#endif  /* __PPC_OFFS_H */
