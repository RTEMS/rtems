/*----------------------------------------------------------------------------
* file name: M68340.INC 				JC RAHUEL CNET/DSM/TAM/CAT
* 
* MC68340 BCC  Board Support Package
*
* date: 1/12/1993					
*
* Copyright 1989, Ready Systems FRANCE
*
* Supports: VRTX32 and RTscope
*
* Related Board: MOTOROLA BCC M68340
*
* Description:	EQUATES FOR 68340 DEVICES
*
* Changes:
*	- Geoffroy Montel (g_montel@yahoo.com) :
*	  changed EQU syntax for GNU as
*
*----------------------------------------------------------------------------*/

/************************************************
 * ATTENTION: must match defs. in C header file *
 ************************************************/

/* -- SIM equates --    system integration module */
.equ BASE_REG, 0x3FF00	
.equ BASE_SIM, 0xEFFFF000
.equ SIM_MCR, 0x000	/* module configuration register */	
.equ SIM_SYNCR, 0x004	/* clock synthesizer control register */	
.equ SIM_AVR, 0x006	/* autovector register */	
.equ SIM_RSR, 0x007	/* reset status register */	

/* -- Port A -- */
.equ SIM_PORTA, 0x011	/* port A data */	
.equ SIM_DDRA, 0x013	/* port A direction data */	
.equ SIM_PPRA1, 0x015	/* Port A pin assignement 1 */	
.equ SIM_PPRA2, 0x017	/* Port A pin assignement 2 */ 	

/* -- Port B -- */
.equ SIM_PORTB, 0x019	/* port B data */	
.equ SIM_PORTB1, 0x01B	/* port B data auxiliary */	
.equ SIM_DDRB, 0x01D	/* port B direction data */	
.equ SIM_PPRB, 0x01F	/* Port B pin assignement */
.equ SIM_SWIV, 0x020	/* SW interrupt vector */	
.equ SIM_SYPCR, 0x021	/* System protection control register */	
.equ SIM_PICR, 0x022	/* Periodic interrupt control register */	
.equ SIM_PITR, 0x024	/* Periodic interrupt timing register */	
.equ SIM_SWSR, 0x027	/* Sofware service */	

/* -- Chip select -- */
.equ SIM_MASKH0, 0x040	/* mask register CS0 */	
.equ SIM_MASKL0, 0x042	/* mask register CS0 */	
.equ SIM_ADDRH0, 0x044	/* base address CS0 */	
.equ SIM_ADDRL0, 0x046	/* base address CS0 */	
.equ SIM_MASKH1, 0x048	/* mask register CS1 */	
.equ SIM_MASKL1, 0x04A	/* mask register CS1 */	
.equ SIM_ADDRH1, 0x04C	/* base address CS1 */	
.equ SIM_ADDRL1, 0x04E	/* base address CS1 */	
.equ SIM_MASKH2, 0x050	/* mask register CS2 */	
.equ SIM_MASKL2, 0x052	/* mask register CS2 */	
.equ SIM_ADDRH2, 0x054	/* base address CS2 */	
.equ SIM_ADDRL2, 0x056	/* base address CS2 */	
.equ SIM_MASKH3, 0x058	/* mask register CS3 */	
.equ SIM_MASKL3, 0x05A	/* mask register CS3 */	
.equ SIM_ADDRH3, 0x05C	/* base address CS3 */	
.equ SIM_ADDRL3, 0x05E	/* base address CS3 */	

/* -- TIMERS  equates -- */

/* __ TIMER 0 */
.equ TIM_MCR0, 0x600	/* Module configuration register */
.equ TIM_IR0, 0x604	/* interrupt register */
.equ TIM_CR0, 0x606	/* controle register */
.equ TIM_SR0, 0x608	/* Status/prescaler register */
.equ TIM_CNTR0, 0x60A	/* counter register */
.equ TIM_PREL10, 0x60C	/* Preload register 1 */
.equ TIM_PREL20, 0x60E	/* Preload register 2 */
.equ TIM_COM0, 0x610	/* Compare register */

/* __ TIMER 1 */

.equ TIM_MCR1, 0x640	/* Module configuration register */
.equ TIM_IR1, 0x644	/* interrupt register */
.equ TIM_CR1, 0x646	/* controle register */
.equ TIM_SR1, 0x648	/* Status/prescaler register */
.equ TIM_CNTR1, 0x64A	/* counter register */	
.equ TIM_PREL11, 0x64C	/* Preload register 1 */	
.equ TIM_PREL21, 0x64E	/* Preload register 2 */	
.equ TIM_COM1, 0x650	/* Compare register */	

/* -- U.A.R.T.  equates -- */

.equ UA_MCRH, 0x700	/* module configuration register */	
.equ UA_MCRL, 0x701	/* module configuration register */	
.equ UA_ILR, 0x704	/* Interrupt level */	
.equ UA_IVR, 0x705	/* Interrupt vector */	
.equ UA_MR1A, 0x710	/* Mode register 1 A */ 
.equ UA_MR2A, 0x720	/* Mode register 2 A*/ 	
.equ UA_CSRA, 0x711	/* Clock_select register A */	
.equ UA_SRA, 0x711	/* status register A */	
.equ UA_CRA, 0x712	/* command register A */	
.equ UA_RBA, 0x713	/* receive buffer A */	
.equ UA_TBA, 0x713	/* transmit buffer A */	
.equ UA_IPCR, 0x714	/* input port change register */	
.equ UA_ACR, 0x714	/* auxiliary control register */	
.equ UA_ISR, 0x715	/* interrupt status register */	
.equ UA_IER, 0x715	/* interrupt enable register */	
.equ UA_MR1B, 0x718	/* Mode register 1 B */
.equ UA_MR2B, 0x721	/* Mode register  2 B */
.equ UA_CSRB, 0x719	/* Clock_select register B */
.equ UA_SRB, 0x719	/* status register B */
.equ UA_CRB, 0x71A	/* command register A */
.equ UA_RBB, 0x71B	/* receive buffer A */
.equ UA_TBB, 0x71B	/* transmit buffer A */
.equ UA_IP, 0x71D	/* Input port register */
.equ UA_OPCR, 0x71D	/* output port control register */	
.equ UA_OPS, 0x71E	/* output port bit set */	
.equ UA_OPR, 0x71F	/* output port bit reset */
.equ TX_A_EN, 0x01	/* Tx A irq enable */
.equ TX_B_EN, 0x10	/* Tx B irq enable */
.equ TX_A_DIS, 0xFE	/* Tx A irq enable */
.equ TX_B_DIS, 0xEF	/* Tx B irq enable */
.equ TX_AB_DIS, 0x22
	
/* -- DMA equates -- */
.equ DMA_MCR0, 0x780	/* module configuration register */	
.equ DMA_IR0, 0x784	/* Interrupt register */	
.equ DMA_CCR0, 0x788	/* Channel control register */	
.equ DMA_CSR0, 0x78A	/* Channel status register */	
.equ DMA_FCR0, 0x78B	/* Function code register */	
.equ DMA_SARH0, 0x78C	/* Source adresse register */	
.equ DMA_SARL0, 0x78E	/* Source adresse register */	
.equ DMA_DARH0, 0x790	/* destination adresse register */	
.equ DMA_DARL0, 0x792	/* destination adresse register */	
.equ DMA_BTCH0, 0x794	/* byte transfer register */	
.equ DMA_BTCL0, 0x796	/* byte transfer register */	
.equ DMA_MCR1, 0x7A0	/* module configuration register */
.equ DMA_IR1, 0x7A4	/* Interrupt register */	
.equ DMA_CCR1, 0x7A8	/* Channel control register */	
.equ DMA_CSR1, 0x7AA	/* Channel status register */	
.equ DMA_FCR1, 0x7AB	/* Function code register */	
.equ DMA_SARH1, 0x7AC	/* Source adresse register */	
.equ DMA_SARL1, 0x7AE	/* Source adresse register */	
.equ DMA_DARH1, 0x7B0	/* destination adresse register */	
.equ DMA_DARL1, 0x7B2	/* destination adresse register */	
.equ DMA_BTCH1, 0x7B4	/* byte transfer register */	
.equ DMA_BTCL1, 0x7B6	/* byte transfer register */	
