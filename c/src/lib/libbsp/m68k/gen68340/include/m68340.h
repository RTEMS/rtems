/**********************************************************************
 *  MC68340 C Header File                                             *
 *                                                                    *
 *  Developed by         : Motorola                                   *
 *                         High Performance Embedded Systems Division *
 *                         Austin, TX                                 *
 *  Rectified by	 : Geoffroy Montel
 *			   g_montel@yahoo.com			      *
 *                                                                    *
 **********************************************************************/

typedef volatile unsigned char *  portb;  /* 8-bit port  */
typedef volatile unsigned short * portw;  /* 16-bit port */
typedef volatile unsigned int *   portl;  /* 32-bit port */

#define MBASE 0xEFFFF000             /* Module Base Address          */
			             /* not EFFFF000 due to a 68349
					hardware incompatibility     */

#define MBAR   (*(portb) 0x0003FF00) /* Module Base Addr Reg         */
#define MBAR1  (*(portb) 0x0003FF00) /* Module Base Addr Reg 1 (MSW) */
#define MBAR2  (*(portb) 0x0003FF02) /* Module Base Addr Reg 2 (LSW) */

/* System Integration Module */

#define SIMMCR    (*(portw) (MBASE+0x0000)) /* SIM Module Config Reg    */
#define SIMSYNCR  (*(portw) (MBASE+0x0004)) /* SIM Clock Synth Cont Reg */
#define SIMAVR    (*(portb) (MBASE+0x0006)) /* SIM Autovector Reg       */
#define SIMRSR    (*(portb) (MBASE+0x0007)) /* SIM Reset Status Reg     */
#define SIMPORTA  (*(portb) (MBASE+0x0011)) /* SIM Port A Data Reg      */
#define SIMDDRA   (*(portb) (MBASE+0x0013)) /* SIM Port A Data Dir Reg  */
#define SIMPPRA1  (*(portb) (MBASE+0x0015)) /* SIM Port A Pin Asm 1 Reg */
#define SIMPPRA2  (*(portb) (MBASE+0x0017)) /* SIM Port A Pin Asm 2 Reg */
#define SIMPORTB  (*(portb) (MBASE+0x0019)) /* SIM Port B Data Reg      */
#define SIMPORTB1 (*(portb) (MBASE+0x001B)) /* SIM Port B Data Reg      */
#define SIMDDRB   (*(portb) (MBASE+0x001D)) /* SIM Port B Data Dir Reg  */
#define SIMPPARB  (*(portb) (MBASE+0x001F)) /* SIM Port B Pin Asm Reg   */
#define SIMSWIV   (*(portb) (MBASE+0x0020)) /* SIM SW Interrupt Vector  */
#define SIMSYPCR  (*(portb) (MBASE+0x0021)) /* SIM System Prot Cont Reg */
#define SIMPICR   (*(portw) (MBASE+0x0022)) /* SIM Period Intr Cont Reg */
#define SIMPITR   (*(portw) (MBASE+0x0024)) /* SIM Period Intr Tmg Reg  */
#define SIMSWSR   (*(portb) (MBASE+0x0027)) /* SIM Software Service Reg */

#define SIMCS0AM  (*(portl) (MBASE+0x0040)) /* SIM Chp Sel 0 Addr Msk   */
#define SIMCS0AM1 (*(portw) (MBASE+0x0040)) /* SIM Chp Sel 0 Addr Msk 1 */
#define SIMCS0AM2 (*(portw) (MBASE+0x0042)) /* SIM Chp Sel 0 Addr Msk 2 */
#define SIMCS0BA  (*(portl) (MBASE+0x0044)) /* SIM Chp Sel 0 Base Addr  */
#define SIMCS0BA1 (*(portw) (MBASE+0x0044)) /* SIM Chp Sel 0 Bas Addr 1 */
#define SIMCS0BA2 (*(portw) (MBASE+0x0046)) /* SIM Chp Sel 0 Bas Addr 2 */
#define SIMCS1AM  (*(portl) (MBASE+0x0048)) /* SIM Chp Sel 1 Adress Msk */
#define SIMCS1AM1 (*(portw) (MBASE+0x0048)) /* SIM Chp Sel 1 Addr Msk 1 */
#define SIMCS1AM2 (*(portw) (MBASE+0x004A)) /* SIM Chp Sel 1 Addr Msk 2 */
#define SIMCS1BA  (*(portl) (MBASE+0x004C)) /* SIM Chp Sel 1 Base Addr  */
#define SIMCS1BA1 (*(portw) (MBASE+0x004C)) /* SIM Chp Sel 1 Bas Addr 1 */
#define SIMCS1BA2 (*(portw) (MBASE+0x004E)) /* SIM Chp Sel 1 Bas Addr 2 */
#define SIMCS2AM  (*(portl) (MBASE+0x0050)) /* SIM Chp Sel 2 Addr Msk   */
#define SIMCS2AM1 (*(portw) (MBASE+0x0050)) /* SIM Chp Sel 2 Addr Msk 1 */
#define SIMCS2AM2 (*(portw) (MBASE+0x0052)) /* SIM Chp Sel 2 Addr Msk 2 */
#define SIMCS2BA  (*(portl) (MBASE+0x0054)) /* SIM Chp Sel 2 Base Addr  */
#define SIMCS2BA1 (*(portw) (MBASE+0x0054)) /* SIM Chp Sel 2 Bas Addr 1 */
#define SIMCS2BA2 (*(portw) (MBASE+0x0056)) /* SIM Chp Sel 2 Bas Addr 2 */
#define SIMCS3AM  (*(portl) (MBASE+0x0058)) /* SIM Chp Sel 3 Addr Msk   */
#define SIMCS3AM1 (*(portw) (MBASE+0x0058)) /* SIM Chp Sel 3 Addr Msk 1 */
#define SIMCS3AM2 (*(portw) (MBASE+0x005A)) /* SIM Chp Sel 3 Addr Msk 2 */
#define SIMCS3BA  (*(portl) (MBASE+0x005C)) /* SIM Chp Sel 3 Base Addr  */
#define SIMCS3BA1 (*(portw) (MBASE+0x005C)) /* SIM Chp Sel 3 Bas Addr 1 */
#define SIMCS3BA2 (*(portw) (MBASE+0x005E)) /* SIM Chp Sel 3 Bas Addr 2 */

/* Dynamic Memory Access (DMA) Module */

#define DMAMCR1   (*(portw) (MBASE+0x0780)) /* DMA Module Config Reg 1  */
#define DMAINTR1  (*(portw) (MBASE+0x0784)) /* DMA Interrupt Reg 1      */
#define DMACCR1   (*(portw) (MBASE+0x0788)) /* DMA Channel Cont Reg 1   */
#define DMACSR1   (*(portb) (MBASE+0x078A)) /* DMA Channel Status Reg 1 */
#define DMAFCR1   (*(portb) (MBASE+0x078B)) /* DMA Function Code Reg 1  */
#define DMASAR1   (*(portl) (MBASE+0x078C)) /* DMA DMA Src Addr Reg 1   */
#define DMADAR1   (*(portl) (MBASE+0x0790)) /* DMA Dest Addr Reg 1      */
#define DMABTC1   (*(portb) (MBASE+0x079l)) /* DMA Byte Trans Cnt Reg 1 */

#define DMAMCR2   (*(portw) (MBASE+0x07A0)) /* DMA Module Config Reg 2  */
#define DMAINTR2  (*(portw) (MBASE+0x07A4)) /* DMA Interrupt Reg 2      */
#define DMACCR2   (*(portw) (MBASE+0x07A8)) /* DMA Channel Cont Reg 2   */
#define DMACSR2   (*(portb) (MBASE+0x07AA)) /* DMA Channel Status Reg 2 */
#define DMAFCR2   (*(portb) (MBASE+0x07AB)) /* DMA Function Code Reg 1  */
#define DMASAR2   (*(portl) (MBASE+0x07AC)) /* DMA Source  Addr Reg 2   */
#define DMADAR2   (*(portl) (MBASE+0x07B0)) /* DMA Dest Addr Reg 2      */
#define DMABTC2   (*(portb) (MBASE+0x07B4)) /* DMA Byte Trans Cnt Reg 2 */

/* Dual Serial Module */

#define DUMCRH    (*(portb) (MBASE+0x0700)) /* DUART Module Config Reg  */
#define DUMCRL    (*(portb) (MBASE+0x0701)) /* DUART Module Config Reg  */
#define DUILR     (*(portb) (MBASE+0x0704)) /* DUART Interrupt Level    */
#define DUIVR     (*(portb) (MBASE+0x0705)) /* DUART Interrupt Vector   */
#define DUMR1A    (*(portb) (MBASE+0x0710)) /* DUART Mode Reg 1A        */
#define DUSRA     (*(portb) (MBASE+0x0711)) /* DUART Status Reg A       */
#define DUCSRA    (*(portb) (MBASE+0x0711)) /* DUART Clock Sel Reg A    */
#define DUCRA     (*(portb) (MBASE+0x0712)) /* DUART Command Reg A      */
#define DURBA     (*(portb) (MBASE+0x0713)) /* DUART Receiver Buffer A  */
#define DUTBA     (*(portb) (MBASE+0x0713)) /* DUART Transmitter Buff A */
#define DUIPCR    (*(portb) (MBASE+0x0714)) /* DUART Input Port Chg Reg */
#define DUACR     (*(portb) (MBASE+0x0714)) /* DUART Auxiliary Cont Reg */
#define DUISR     (*(portb) (MBASE+0x0715)) /* DUART Interrupt Stat Reg */
#define DUIER     (*(portb) (MBASE+0x0715)) /* DUART Interrupt Enb Reg  */

#define DUMR1B    (*(portb) (MBASE+0x0718)) /* DUART Mode Reg 1B        */
#define DUSRB     (*(portb) (MBASE+0x0719)) /* DUART Status Reg B       */
#define DUCSRB    (*(portb) (MBASE+0x0719)) /* DUART Clock Sel Reg B    */
#define DUCRB     (*(portb) (MBASE+0x071A)) /* DUART Command Reg B      */
#define DURBB     (*(portb) (MBASE+0x071B)) /* DUART Receiver Buffer B  */
#define DUTBB     (*(portb) (MBASE+0x071B)) /* DUART Transmitter Buff B */
#define DUIP      (*(portb) (MBASE+0x071D)) /* DUART Input Port Reg     */
#define DUOPCR    (*(portb) (MBASE+0x071D)) /* DUART Outp Port Cnt Reg  */
#define DUOPBS    (*(portb) (MBASE+0x071E)) /* DUART Outp Port Bit Set  */
#define DUOPBR    (*(portb) (MBASE+0x071F)) /* DUART Outp Port Bit Rst  */
#define DUMR2A    (*(portb) (MBASE+0x0720)) /* DUART Mode Reg 2A        */
#define DUMR2B    (*(portb) (MBASE+0x0721)) /* DUART Mode Reg 2B        */

/* Dual Timer Module */

#define TMCR1    (*(portw) (MBASE+0x0600)) /* Timer Module Config Reg 1 */
#define TIR1     (*(portw) (MBASE+0x0604)) /* Timer Interrupt Reg 1     */
#define TCR1     (*(portw) (MBASE+0x0606)) /* Timer Control Reg 1       */
#define TSR1     (*(portw) (MBASE+0x0608)) /* Timer Status Reg 1        */
#define TCNTR1   (*(portw) (MBASE+0x060A)) /* Timer Counter Reg 1       */
#define WPREL11  (*(portw) (MBASE+0x060C)) /* Timer Preload 1 Reg 1     */
#define WPREL21  (*(portw) (MBASE+0x060E)) /* Timer Preload 2 Reg 1     */
#define TCOM1    (*(portw) (MBASE+0x0610)) /* Timer Compare Reg 1       */

#define TMCR2    (*(portw) (MBASE+0x0640)) /* Timer Module Config Reg 2 */
#define TIR2     (*(portw) (MBASE+0x0644)) /* Timer Interrupt Reg 2     */
#define TCR2     (*(portw) (MBASE+0x0646)) /* Timer Control Reg 2       */
#define TSR2     (*(portw) (MBASE+0x0648)) /* Timer Status Reg 2        */
#define TCNTR2   (*(portw) (MBASE+0x064A)) /* Timer Counter Reg 2       */
#define WPREL12  (*(portw) (MBASE+0x064C)) /* Timer Preload 1 Reg 2     */
#define WPREL22  (*(portw) (MBASE+0x064E)) /* Timer Preload 2 Reg 2     */
#define TCOM2    (*(portw) (MBASE+0x0650)) /* Timer Compare Reg 2       */
