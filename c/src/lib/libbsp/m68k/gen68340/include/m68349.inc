/*----------------------------------------------------------------------------
 * file name: M68349.INC 				P. CADIC CNET/DSM/TAM/CAT
 *
 * MC68349 BCC  Board Support Package
 *
 * date: 31/07/97
 *
 * Description:	EQUATES FOR 68349 DEVICES
 *
 * Modifications:
 * 	- adapted for GNU CC by G.Montel 26/05/98
 *----------------------------------------------------------------------------*/

| -- SIM equates --                   

.equ	BASE_REG, 0x3FF00
.equ	BASE_SIM, 0xEFFFF000		| pour correction du bug 68349 sur IACK

.equ	SIM_MCR, 0x000    		| module configuration register
.equ	SIM_IDR, 0x002    		| processor identification register
.equ	SIM_SYNCR, 0x004    		| clock synthesizer control register
.equ	SIM_AVR, 0x006    		| autovector register
.equ	SIM_RSR, 0x007    		| reset status register

| -- Port A
.equ	SIM_PORTA, 0x011    		| port A data
.equ	SIM_DDRA, 0x013    		| port A direction data
.equ	SIM_PPRA1, 0x015    		| Port A pin assignement 1
.equ	SIM_PPRA2, 0x017    		| Port A pin assignement 2

| -- Port B
.equ	SIM_PORTB, 0x019    		| port B data
.equ	SIM_PORTB1, 0x01B    		| port B data auxiliary
.equ	SIM_DDRB, 0x01D    		| port B direction data
.equ	SIM_PPRB, 0x01F    		| Port B pin assignement

.equ	SIM_SWIV, 0x020    | SW interrupt vector
.equ	SIM_SYPCR, 0x021    | System protection control register
.equ	SIM_PICR, 0x022    | Periodic interrupt control register
.equ	SIM_PITR, 0x024    | Periodic interrupt timing register
.equ	SIM_SWSR, 0x027    | Sofware service

| -- Chip select
.equ	SIM_MASKH0, 0x040    | mask register CS0
.equ	SIM_MASKL0, 0x042    | mask register CS0
.equ	SIM_ADDRH0, 0x044    | base address CS0
.equ	SIM_ADDRL0, 0x046    | base address CS0

.equ	SIM_MASKH1, 0x048    | mask register CS1
.equ	SIM_MASKL1, 0x04A    | mask register CS1
.equ	SIM_ADDRH1, 0x04C    | base address CS1
.equ	SIM_ADDRL1, 0x04E    | base address CS1

.equ	SIM_MASKH2, 0x050    | mask register CS2
.equ	SIM_MASKL2, 0x052    | mask register CS2
.equ	SIM_ADDRH2, 0x054    | base address CS2
.equ	SIM_ADDRL2, 0x056    | base address CS2

.equ	SIM_MASKH3, 0x058    | mask register CS3
.equ	SIM_MASKL3, 0x05A    | mask register CS3
.equ	SIM_ADDRH3, 0x05C    | base address CS3
.equ	SIM_ADDRL3, 0x05E    | base address CS3

| -- TIMERS  equates --

| __ TIMER 0

.equ	TIM_MCR0, 0x600    | Module configuration register
.equ	TIM_IR0, 0x604    | interrupt register
.equ	TIM_CR0, 0x606    | controle register
.equ	TIM_SR0, 0x608    | Status/prescaler register
.equ	TIM_CNTR0, 0x60A    | counter register
.equ	TIM_PREL10, 0x60C    | Preload register 1
.equ	TIM_PREL20, 0x60E    | Preload register 2
.equ	TIM_COM0, 0x610    | Compare register

| __ TIMER 1

.equ	TIM_MCR1, 0x640    | Module configuration register
.equ	TIM_IR1, 0x644    | interrupt register
.equ	TIM_CR1, 0x646    | controle register
.equ	TIM_SR1, 0x648    | Status/prescaler register
.equ	TIM_CNTR1, 0x64A    | counter register
.equ	TIM_PREL11, 0x64C    | Preload register 1
.equ	TIM_PREL21, 0x64E    | Preload register 2
.equ	TIM_COM1, 0x650    | Compare register

| -- U.A.R.T.  equates --

.equ	UA_MCRH, 0x700    | module configuration register
.equ	UA_MCRL, 0x701    | module configuration register
.equ	UA_ILR, 0x704    | Interrupt level
.equ	UA_IVR, 0x705    | Interrupt vector

.equ	UA_MR1A, 0x710    | Mode register 1 A
.equ	UA_MR2A, 0x720    | Mode register 2 A
.equ	UA_CSRA, 0x711    | Clock_select regiter A
.equ	UA_SRA, 0x711    | status register A
.equ	UA_CRA, 0x712    | command register A
.equ	UA_RBA, 0x713    | receive buffer A
.equ	UA_TBA, 0x713    | transmit buffer A

.equ	UA_IPCR, 0x714    | input port change register
.equ	UA_ACR, 0x714    | auxiliary control register
.equ	UA_ISR, 0x715    | interrupt status register
.equ	UA_IER, 0x715    | interrupt enable register

.equ	UA_MR1B, 0x718    | Mode register 1 B
.equ	UA_MR2B, 0x721    | Mode register 2 B
.equ	UA_CSRB, 0x719    | Clock_select regiter B
.equ	UA_SRB, 0x719    | status register B
.equ	UA_CRB, 0x71A    | command register A
.equ	UA_RBB, 0x71B    | receive buffer A
.equ	UA_TBB, 0x71B    | transmit buffer A

.equ	UA_IP, 0x71D    | Input port register
.equ	UA_OPCR, 0x71D    | output port control register
.equ	UA_OPS, 0x71E    | output port bit set
.equ	UA_OPR, 0x71F    | output port bit reset
.equ	TX_A_EN, 0x01     | Tx A irq enable
.equ	TX_B_EN, 0x10     | Tx B irq enable
.equ	TX_A_DIS, 0xFE     | Tx A irq enable
.equ	TX_B_DIS, 0xEF     | Tx B irq enable
.equ	TX_AB_DIS, 0x22


| -- DMA equates
.equ	DMA_MCR0, 0x780    | module configuration register
.equ	DMA_IR0, 0x784    | Interrupt register
.equ	DMA_CCR0, 0x788    | Channel control register
.equ	DMA_CSR0, 0x78A    | Channel status register
.equ	DMA_FCR0, 0x78B    | Function code register
.equ	DMA_SARH0, 0x78C    | Source adresse register
.equ	DMA_SARL0, 0x78E    | Source adresse register
.equ	DMA_DARH0, 0x790    | destination adresse register
.equ	DMA_DARL0, 0x792    | destination adresse register
.equ	DMA_BTCH0, 0x794    | byte transfer register
.equ	DMA_BTCL0, 0x796    | byte transfer register

.equ	DMA_MCR1, 0x7A0    | module configuration register
.equ	DMA_IR1, 0x7A4    | Interrupt register
.equ	DMA_CCR1, 0x7A8    | Channel control register
.equ	DMA_CSR1, 0x7AA    | Channel status register
.equ	DMA_FCR1, 0x7AB    | Function code register
.equ	DMA_SARH1, 0x7AC    | Source adresse register
.equ	DMA_SARL1, 0x7AE    | Source adresse register
.equ	DMA_DARH1, 0x7B0    | destination adresse register
.equ	DMA_DARL1, 0x7B2    | destination adresse register
.equ	DMA_BTCH1, 0x7B4    | byte transfer register
.equ	DMA_BTCL1, 0x7B6    | byte transfer register

| -- cache equates
.equ	CACHE_MCR, 0xFC0	| cache config reg. (long)

| -- quad data memory module (QDMM) equates
.equ	QDMM_MCR, 0xC00		| QDMM config reg (long)
.equ	QDMM_QBAR0, 0xC10	| QDMM base 0 (long)
.equ	QDMM_QBAR1, 0xC14	| QDMM base 1 (long)
.equ	QDMM_QBAR2, 0xC18	| QDMM base 2 (long)
.equ	QDMM_QBAR3, 0xC1C	| QDMM base 3 (long)



|-----------------------------------------------------
| AST68349 internal registers
|-----------------------------------------------------
.equ	EPLD_SPACE, 3				| "reserved user" space
.equ	CPU_SPACE, 7				| "CPU" space

||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
| GLUE EPLD
||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

.equ	GLUE_EPLD, 0xB0000000

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| configuration of /CS0 :
|
|		  7   6   5   4   3   2   1   0
| 		+---+---+---+---+---+---+---+---+
| 		|ena|val|wid| ws|b31|b30|b29|b28|
| 		+---+---+---+---+---+---+---+---+
|
| b[31..28] : base address for decoding /CS[3..0]
|  the decoding is as follow :
|
|  +----------+------------+------+
|	| a[31..28] | a[27..26]	| /CS  |
|	+-----------+-----------+------+
|	| b[31..28] |    00		| /CS0 |	each /CS decodes 64 Mbytes
|	| b[31..28] |    01		| /CS1 |
|	| b[31..28] |    10		| /CS2 |
|	| b[31..28] |    11		| /CS3 |
|	+-----------------------+------+
|
| after /RESET, /CS0 is validated for every cycle, until programmed
|
| ws : number of wait-states : 0 => 0 ws
|                              1 => external /dsackx
| wid : width of chip-select : 0 => 16 bits
|                              1 => 32 bits
| ena : enable chip-select   : 0 => disabled
|                              1 => enabled
|
| val : automatic validation. set after reset
| cleared when /CS0 is configured
|
.equ	REG_CS0, 0


|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| configuration of /CS1 to /CS3:
|
|		  7   6   5   4   3   2   1   0
| 		+---+---+---+---+---+---+---+---+
| 		|ena| x |wid| ws| x | x | x | x |
| 		+---+---+---+---+---+---+---+---+
|
| ws : number of wait-states : 0 => 0 ws
|                              1 => external /dsackx
| wid : width of chip-select : 0 => 16 bits
|                              1 => 32 bits
| ena : enable chip-select   : 0 => disabled
|                              1 => enabled
.equ	REG_CS1, 1
.equ	REG_CS2, 2
.equ	REG_CS3, 3

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| I2C register
|
|		  7   6   5   4   3   2   1   0
| 		+---+---+---+---+---+---+---+----+
| 		| x | x | x | x | x | x |clk|data|
| 		+---+---+---+---+---+---+---+----+
| bidirecionnal pin, open drain output.
| set bit to 1 to read external state of pin
|
.equ	REG_I2C, 4

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| PDCS register
|
|		  7   6   5   4   3   2   1   0
| 		+---+---+---+---+---+---+---+---+
| 		|s12|s11|s14|pd5|pd4|pd3|pd2|pd1|
| 		+---+---+---+---+---+---+---+---+
| pd[5..1] : value read on the DRAM module
| S12, S11 and S14 : "user reserved" configuration switch
|
.equ	REG_PDCS, 5

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| timer1 register
|
|		  7   6   5   4   3   2   1   0
| 		+---+---+---+---+---+---+---+---+
| 		|ena| x | x | x | x | x | d1| d0|
| 		+---+---+---+---+---+---+---+---+
|
| the timer clock is the 1000Hz clock of the ASTECC platform
| the timer is reloaded on each write to the register, or if the input
| TIN1 is set to 0.
| on overflow, the open drain output TOUT1 is set to 0
| the timer must be disabled to return TOUT1 to the inactive state
|
.equ	REG_TIMER1, 6

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| timer2 register
|
|		  7   6   5   4   3   2   1   0
| 		+---+---+---+---+---+---+---+---+
| 		|ena| x | x | x | x | x | d1| d0|
| 		+---+---+---+---+---+---+---+---+
| the timer clock is the 1000Hz clock of the ASTECC platform
| the timer is reloaded on each write to the register, or if the input
| TIN2 is set to 0.
| on overflow, the open drain output TOUT2 is set to 0
| the timer must be disabled to return TOUT2 to the inactive state
|
.equ	REG_TIMER2, 7

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| baudrate generator register
|
|		  7   6   5   4   3   2   1   0
| 		+---+---+---+---+---+---+---+---+
| 		| x | x | x | x | x | d2| d1| d0|
| 		+---+---+---+---+---+---+---+---+
|
| d[2..0] : divider of a 3.6864 Mhz clock
|
| d[2..0]    :     0      1       2       3      4      5      6      7
| divides by :     2      4       6       8     10     12     14     16
| SCLK (Mhz) :  1.8432  0.9216  0.6144  0.4608   x    0.3072   x    0.2304
| baudrate   :  115200   57600  38400    28800   x     19200   x     14400
|
.equ	REG_BAUDRATE, 8

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| IO register
|
|		  7   6   5   4   3   2   1   0
| 		+---+---+---+---+---+---+---+---+
| 		| x | x | x |io4|io3|io2|io1|io0|
| 		+---+---+---+---+---+---+---+---+
|
| io[4..0] : data written to port
|
| maximum current load is about 5 mA per pin
|
.equ	REG_IO, 9

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| IO port
|
|		  7   6   5   4   3   2   1   0
| 		+---+---+---+---+---+---+---+---+
| 		| x | x | x |io4|io3|io2|io1|io0|
| 		+---+---+---+---+---+---+---+---+
|
| io[4..0] : data read from port
|
.equ	REG_IO_PORT, 10

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| IO direction register
|
|		  7   6   5   4   3   2   1   0
| 		+---+---+---+---+---+---+---+---+
| 		| x | x | x | x | x |dr2|dr1|dr0|
| 		+---+---+---+---+---+---+---+---+
|
| dr0 : 0 => io port 0 is configured as input (default after /RESET)
|       1 => io port 0 is configured as output
|
| dr1 : 0 => io port 1 is configured as input (default after /RESET)
|       1 => io port 1 is configured as output
|
| dr2 : 0 => io ports 2 to 4 are configured as input (default after /RESET)
|       1 => io ports 2 to 4 are configured as output
|
.equ	REG_DIR_IO, 11




||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
| DRAM EPLD
||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

.equ	DRAM_EPLD, 0xA0000000

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| number of wait-state for DRAM
|
|		  7   6   5   4   3   2   1   0
| 		+---+---+---+---+---+---+---+---+
| 		| x | x | x | x | x | x |ws1|ws0|
| 		+---+---+---+---+---+---+---+---+
|
| ws[1..0]    : 0   1   2   3
| wait states : 0   1   2   3
|
.equ	REG_WS, 0

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| configuration of refresh for DRAM
|
|		  7   6   5   4   3   2   1   0
| 		+---+---+---+---+---+---+---+---+
| 		|ena| x | x | x | x | x |rf1|rf0|
| 		+---+---+---+---+---+---+---+---+
|
|	rf[1..0] :  0   1    2    3
|	refresh  : 5µs 10µs 15µs 20µs
|
| ena == 0 : refresh disabled
| ena == 1 : refresh enabled
|
.equ	REG_REFRESH, 1

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| configuration of DRAM module size
|
|		  7   6   5   4   3   2   1   0
| 		+---+---+---+---+---+---+---+---+
| 		| x | x | x | x | x |sz2|sz1|sz0|
| 		+---+---+---+---+---+---+---+---+
|
|  sz[2..0]     : 0   1   2   3   4    5   6   7
|  size (Mbytes): 4   8   16  32  64  128  0   0
|
.equ	REG_CONFIG, 2

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| bus width of /CS0 during reset		bw[1..0]  : 0   1   2       3
|                                   bus width : 32  16  8   ext. /dsackx
|
| state of CS_SWITCH  : sel == 0 => CPU chip_selects  (/CS[3..0])
|                     : sel == 1 => EPLD chip_selects (/CS[3..0])
|
|		  7   6   5   4   3   2   1   0
| 		+---+---+---+---+---+---+---+---+
| 		|bw1|bw0| x | x | x | x | x |sel|
| 		+---+---+---+---+---+---+---+---+
|
.equ	REG_BUSWIDTH, 3

