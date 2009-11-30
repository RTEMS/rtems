/*
 * cpm.h
 *
 * This include file contains definitions pertaining
 * to the Communications Processor Module (CPM) on the MPC8xx.
 *
 * Copyright (c) 1999, National Research Council of Canada
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _MPC8260_CPM_H
#define _MPC8260_CPM_H

#ifdef __cplusplus
extern "C" {
#endif


#define M8xx_BRG_1	(1U << 0)
#define M8xx_BRG_2	(1U << 1)
#define M8xx_BRG_3	(1U << 2)
#define M8xx_BRG_4	(1U << 3)
#define M8xx_BRG_5	(1U << 4)
#define M8xx_BRG_6	(1U << 5)
#define M8xx_BRG_7	(1U << 6)
#define M8xx_BRG_8	(1U << 7)


#define M8260_SCC_BRGS	(M8xx_BRG_1 | M8xx_BRG_2 | M8xx_BRG_3 | M8xx_BRG_4)
#define M8260_FCC_BRGS  (M8xx_BRG_5 | M8xx_BRG_6 | M8xx_BRG_7 | M8xx_BRG_8)
#define M8260_SMC1_BRGS	(M8xx_BRG_1|M8xx_BRG_7)
#define M8260_SMC2_BRGS	(M8xx_BRG_2|M8xx_BRG_8)


#define M8xx_CLK_1	(1U << 0)
#define M8xx_CLK_2	(1U << 1)
#define M8xx_CLK_3	(1U << 2)
#define M8xx_CLK_4	(1U << 3)
#define M8xx_CLK_5	(1U << 4)
#define M8xx_CLK_6	(1U << 5)
#define M8xx_CLK_7	(1U << 6)
#define M8xx_CLK_8	(1U << 7)
#define M8xx_CLK_9	(1U << 8)
#define M8xx_CLK_10	(1U << 9)
#define M8xx_CLK_11	(1U << 10)
#define M8xx_CLK_12	(1U << 11)
#define M8xx_CLK_13	(1U << 12)
#define M8xx_CLK_14	(1U << 13)
#define M8xx_CLK_15	(1U << 14)
#define M8xx_CLK_16	(1U << 15)
#define M8xx_CLK_17	(1U << 16)
#define M8xx_CLK_18	(1U << 17)
#define M8xx_CLK_19	(1U << 18)
#define M8xx_CLK_20	(1U << 19)

#define M8260_BRG1_CLKS (M8xx_CLK_3  | M8xx_CLK_5  )
#define M8260_BRG2_CLKS (M8xx_CLK_3  | M8xx_CLK_5  )
#define M8260_BRG3_CLKS (M8xx_CLK_9  | M8xx_CLK_15 )
#define M8260_BRG4_CLKS (M8xx_CLK_9  | M8xx_CLK_15 )
#define M8260_BRG5_CLKS (M8xx_CLK_3  | M8xx_CLK_5  )
#define M8260_BRG6_CLKS (M8xx_CLK_3  | M8xx_CLK_5  )
#define M8260_BRG7_CLKS (M8xx_CLK_9  | M8xx_CLK_15 )
#define M8260_BRG8_CLKS (M8xx_CLK_9  | M8xx_CLK_15 )

#define M8260_SCC1_CLKS	(M8xx_CLK_3  | M8xx_CLK_4  | M8xx_CLK_11 | M8xx_CLK_12)
#define M8260_SCC2_CLKS	(M8xx_CLK_3  | M8xx_CLK_4  | M8xx_CLK_11 | M8xx_CLK_12)
#define M8260_SCC3_CLKS	(M8xx_CLK_5  | M8xx_CLK_6  | M8xx_CLK_7  | M8xx_CLK_8 )
#define M8260_SCC4_CLKS	(M8xx_CLK_5  | M8xx_CLK_6  | M8xx_CLK_7  | M8xx_CLK_8 )

#define M8260_FCC1_CLKS	(M8xx_CLK_9  | M8xx_CLK_10 | M8xx_CLK_11 | M8xx_CLK_12)
#define M8260_FCC2_CLKS	(M8xx_CLK_13 | M8xx_CLK_14 | M8xx_CLK_15 | M8xx_CLK_16)
#define M8260_FCC3_CLKS	(M8xx_CLK_13 | M8xx_CLK_14 | M8xx_CLK_15 | M8xx_CLK_16)

#define M8260_TDM_RXA1	(M8xx_CLK_1  | M8xx_CLK_19 )
#define M8260_TDM_RXB1	(M8xx_CLK_3  | M8xx_CLK_9  )
#define M8260_TDM_RXC1	(M8xx_CLK_5  | M8xx_CLK_13 )
#define M8260_TDM_RXD1	(M8xx_CLK_7  | M8xx_CLK_15 )
#define M8260_TDM_TXA1	(M8xx_CLK_2  | M8xx_CLK_20 )
#define M8260_TDM_TXB1	(M8xx_CLK_4  | M8xx_CLK_10 )
#define M8260_TDM_TXC1	(M8xx_CLK_6  | M8xx_CLK_14 )
#define M8260_TDM_TXD1	(M8xx_CLK_8  | M8xx_CLK_16 )

#define M8260_TDM_RXA2	(M8xx_CLK_13 | M8xx_CLK_5  )
#define M8260_TDM_RXB2	(M8xx_CLK_15 | M8xx_CLK_17 )
#define M8260_TDM_RXC2	(M8xx_CLK_3  | M8xx_CLK_17 )
#define M8260_TDM_RXD2	(M8xx_CLK_1  | M8xx_CLK_19 )
#define M8260_TDM_TXA2	(M8xx_CLK_14 | M8xx_CLK_6  )
#define M8260_TDM_TXB2	(M8xx_CLK_16 | M8xx_CLK_18 )
#define M8260_TDM_TXC2	(M8xx_CLK_4  | M8xx_CLK_18 )
#define M8260_TDM_TXD2	(M8xx_CLK_2  | M8xx_CLK_20 )



/* Functions */

void m8xx_cp_execute_cmd( uint32_t   command );
void *m8xx_dpram_allocate( unsigned int byte_count );

#define m8xx_bd_allocate(count)		\
		m8xx_dpram_allocate( (count) * sizeof(m8260BufferDescriptor_t) )
#define m8xx_RISC_timer_table_allocate(count)	\
		m8xx_dpram_allocate( (count) * 4 )



int m8xx_get_brg_cd (int baud);
int m8xx_get_brg(unsigned brgmask, int baud);
void m8xx_free_brg(int brg_num);


int m8xx_get_clk( unsigned clkmask );
void m8xx_free_clk( int clk_num );


#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
