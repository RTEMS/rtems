/*  This file contains the termios TTY driver for the
 *  Motorola MC68360 SCC ports.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdio.h>
#include <termios.h>
#include <bsp.h>
#include <libcpu/io.h>
#include <rtems/libio.h>
#include <bsp/pci.h>
#include <bsp/irq.h>
#include <libchip/serial.h>
#include "m68360.h"
#include <libchip/sersupp.h>
#include <stdlib.h>
#include <rtems/bspIo.h>
#include <string.h>

#if 0
#define DEBUG_360
#endif

#if 1   /* XXX */
int EP1A_READ_LENGTH_GREATER_THAN_1 = 0;

#define MC68360_LENGTH_SIZE 400
int mc68360_length_array[ MC68360_LENGTH_SIZE ];
int mc68360_length_count=0;

void mc68360_Show_length_array(void) {
  int i;
  for (i=0; i<MC68360_LENGTH_SIZE; i++)
    printf(" %d", mc68360_length_array[i] );
  printf("\n\n");
}
#endif


M68360_t    M68360_chips = NULL;

#define SYNC     eieio
#define mc68360_scc_Is_422( _minor ) (Console_Port_Tbl[minor]->sDeviceName[7] == '4' )


void mc68360_scc_nullFunc(void) {}

uint8_t scc_read8(
  const char       *name,
  volatile uint8_t *address
)
{
  uint8_t value;

#ifdef DEBUG_360
  printk( "RD8 %s 0x%08x ", name, address );
#endif
  value = *address;
#ifdef DEBUG_360
  printk( "0x%02x\n", value );
#endif

  return value;
}

void scc_write8(
  const char       *name,
  volatile uint8_t *address,
  uint8_t           value
)
{
#ifdef DEBUG_360
  printk( "WR8 %s 0x%08x 0x%02x\n", name, address, value );
#endif
  *address = value;
}


uint16_t scc_read16(
  const char        *name,
  volatile uint16_t *address
)
{
  uint16_t value;

#ifdef DEBUG_360
  printk( "RD16 %s 0x%08x ", name, address );
#endif
  value = *address;
#ifdef DEBUG_360
  printk( "0x%04x\n", value );
#endif

  return value;
}

void scc_write16(
  const char        *name,
  volatile uint16_t *address,
  uint16_t           value
)
{
#ifdef DEBUG_360
  printk( "WR16 %s 0x%08x 0x%04x\n", name, address, value );
#endif
  *address = value;
}


uint32_t scc_read32(
  const char        *name,
  volatile uint32_t *address
)
{
  uint32_t value;

#ifdef DEBUG_360
  printk( "RD32 %s 0x%08x ", name, address );
#endif
  value = *address;
#ifdef DEBUG_360
  printk( "0x%08x\n", value );
#endif

  return value;
}

void scc_write32(
  const char        *name,
  volatile uint32_t *address,
  uint32_t           value
)
{
#ifdef DEBUG_360
  printk( "WR32 %s 0x%08x 0x%08x\n", name, address, value );
#endif
  *address = value;
}

void mc68360_sccShow_Regs(int minor){
  M68360_serial_ports_t  ptr;
  ptr   = Console_Port_Tbl[minor]->pDeviceParams;

  printk( "scce 0x%08x", &ptr->pSCCR->scce );
  printk( " 0x%04x\n", ptr->pSCCR->scce );

}

#define TX_BUFFER_ADDRESS( _ptr ) \
  ((char *)ptr->txBuf - (char *)ptr->chip->board_data->baseaddr)
#define RX_BUFFER_ADDRESS( _ptr ) \
  ((char *)ptr->rxBuf - (char *)ptr->chip->board_data->baseaddr)


/**************************************************************************
 * Function: mc68360_sccBRGC                                             *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This function is called to compute the divisor register values for  *
 *    a given baud rate.                                                  *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    int baud  - Baud rate (in bps).                                     *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    int  - baud rate generator configuration.                           *
 *                                                                        *
 **************************************************************************/
static int
mc68360_sccBRGC(int baud, int m360_clock_rate)
{
   int data;
#if 0
   int divisor;
   int div16;

   div16 = 0;
   divisor = ((m360_clock_rate / 16) + (baud / 2)) / baud;
   if (divisor > 4096)
   {
      div16   = 1;
      divisor = (divisor + 8) / 16;
   }
   return(M360_BRG_EN | M360_BRG_EXTC_BRGCLK |
          ((divisor - 1) << 1) | div16);
#endif

  /*
   * configure baud rate generator for 16x bit rate, where.....
   * b    =   desired baud rate
   * clk  =   system clock (33mhz)
   * d    =   clock dividor value
   *
   * for b > 300  :   d = clk/(b*16)
   * for b<= 300  :   d = (clk/ (b*16*16))-1)
   */

  SYNC();
  if( baud > 300 ) data = 33333333 / (baud * 16 );
  else             data = (33333333 / (baud * 16 * 16) ) - 1;
  data *= 2;
  data &= 0x00001ffe ;

  /* really data = 0x010000 | data | ((baud>300)? 0 : 1 ) ; */
  data |= ((baud>300)? 0 : 1 ) ;
  data |= 0x010000 ;

  return data;
}


/**************************************************************************
 * Function: sccInterruptHandler                                          *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This is the interrupt service routine for the console UART.  It     *
 *    handles both receive and transmit interrupts.  The bulk of the      *
 *    work is done by termios.                                            *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    chip  - structure of chip specific information                      *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 **************************************************************************/
void mc68360_sccInterruptHandler( M68360_t chip )
{
  volatile m360_t    *m360;
  int                port;
  uint16_t           status;
  uint16_t           length;
  int                i;
  char               data;
  int                clear_isr;


#ifdef DEBUG_360
  printk("mc68360_sccInterruptHandler\n");
#endif
  for (port=0; port<4; port++) {

      clear_isr = FALSE;
      m360  = chip->m360;

      /*
       * XXX - Can we add something here to check if this is our interrupt.
       * XXX - We need a parameter here so that we know which 360 instead of
       *       looping through them all!
       */

      /*
       * Handle a RX interrupt.
       */
      if ( scc_read16("scce", &chip->port[port].pSCCR->scce) & 0x1)
      {
        clear_isr = TRUE;
        scc_write16("scce", &chip->port[port].pSCCR->scce, 0x1 );
        status =scc_read16( "sccRxBd->status", &chip->port[port].sccRxBd->status);
        while ((status & M360_BD_EMPTY) == 0)
        {
           length= scc_read16("sccRxBd->length",&chip->port[port].sccRxBd->length);
if (length > 1)
  EP1A_READ_LENGTH_GREATER_THAN_1 = length;

           for (i=0;i<length;i++) {
             data= chip->port[port].rxBuf[i];
             rtems_termios_enqueue_raw_characters(
               Console_Port_Data[ chip->port[port].minor ].termios_data,
               &data,
               1);
           }
           scc_write16( "sccRxBd->status", &chip->port[port].sccRxBd->status,
                        M360_BD_EMPTY | M360_BD_WRAP | M360_BD_INTERRUPT );
           status =scc_read16( "sccRxBd->status", &chip->port[port].sccRxBd->status);
        }
      }

      /*
       * Handle a TX interrupt.
       */
      if (scc_read16("scce", &chip->port[port].pSCCR->scce) & 0x2)
      {
        clear_isr = TRUE;
        scc_write16("scce", &chip->port[port].pSCCR->scce, 0x2);
        status = scc_read16("sccTxBd->status", &chip->port[port].sccTxBd->status);
        if ((status & M360_BD_EMPTY) == 0)
        {
           scc_write16("sccTxBd->status",&chip->port[port].sccTxBd->status,0);
#if 1
           rtems_termios_dequeue_characters(
             Console_Port_Data[chip->port[port].minor].termios_data,
             chip->port[port].sccTxBd->length);
#else
           mc68360_scc_write_support_int(chip->port[port].minor,"*****", 5);
#endif
        }
      }

      /*
       * Clear SCC interrupt-in-service bit.
       */
      if ( clear_isr )
        scc_write32( "cisr", &m360->cisr, (0x80000000 >> chip->port[port].channel) );
  }
}

/*
 *  mc68360_scc_open
 *
 *  This function opens a port for communication.
 *
 *  Default state is 9600 baud, 8 bits, No parity, and 1 stop bit.
 */

int mc68360_scc_open(
  int      major,
  int      minor,
  void    * arg
)
{
  M68360_serial_ports_t  ptr;
  volatile m360_t       *m360;
  uint32_t               data;

#ifdef DEBUG_360
  printk("mc68360_scc_open %d\n", minor);
#endif


  ptr   = Console_Port_Tbl[minor]->pDeviceParams;
  m360  = ptr->chip->m360;

  /*
   * Enable the receiver and the transmitter.
   */

  SYNC();
  data = scc_read32( "pSCCR->gsmr_l", &ptr->pSCCR->gsmr_l);
  scc_write32( "pSCCR->gsmr_l", &ptr->pSCCR->gsmr_l,
    (data | M360_GSMR_ENR | M360_GSMR_ENT) );

  data  = PMCQ1_Read_EPLD(ptr->chip->board_data->baseaddr, PMCQ1_INT_MASK );
  data &= (~PMCQ1_INT_MASK_QUICC);
  PMCQ1_Write_EPLD(ptr->chip->board_data->baseaddr, PMCQ1_INT_MASK, data );

  data = PMCQ1_Read_EPLD(ptr->chip->board_data->baseaddr, PMCQ1_INT_STATUS );
  data &= (~PMCQ1_INT_STATUS_QUICC);
  PMCQ1_Write_EPLD(ptr->chip->board_data->baseaddr, PMCQ1_INT_STATUS, data );

  return RTEMS_SUCCESSFUL;
}

uint32_t mc68360_scc_calculate_pbdat( M68360_t chip )
{
  uint32_t               i;
  uint32_t               pbdat_data;
  int                    minor;
  uint32_t               type422data[4] = {
    0x00440,  0x00880,  0x10100,  0x20200
  };

  pbdat_data = 0x3;
  for (i=0; i<4; i++) {
    minor = chip->port[i].minor;
    if mc68360_scc_Is_422( minor )
      pbdat_data |= type422data[i];
  }

  return pbdat_data;
}

/*
 *  mc68360_scc_initialize_interrupts
 *
 *  This routine initializes the console's receive and transmit
 *  ring buffers and loads the appropriate vectors to handle the interrupts.
 */

void mc68360_scc_initialize_interrupts(int minor)
{
  M68360_serial_ports_t  ptr;
  volatile m360_t       *m360;
  uint32_t               data;
  uint32_t               buffers_start;
  uint32_t               tmp_u32;

#ifdef DEBUG_360
  printk("mc68360_scc_initialize_interrupts: minor %d\n", minor );
  printk("Console_Port_Tbl[minor]->pDeviceParams 0x%08x\n",
    Console_Port_Tbl[minor]->pDeviceParams );
#endif

  ptr   = Console_Port_Tbl[minor]->pDeviceParams;
  m360  = ptr->chip->m360;

#ifdef DEBUG_360
  printk("m360 0x%08x baseaddr 0x%08x\n",
     m360, ptr->chip->board_data->baseaddr);
#endif

  buffers_start = ptr->chip->board_data->baseaddr + 0x00200000 +
            ( (M68360_RX_BUF_SIZE + M68360_TX_BUF_SIZE) * (ptr->channel-1));
  ptr->rxBuf = (uint8_t *) buffers_start;
  ptr->txBuf = (uint8_t *)(buffers_start + M68360_RX_BUF_SIZE);
#ifdef DEBUG_360
  printk("rxBuf 0x%08x  txBuf 0x%08x\n", ptr->rxBuf, ptr->txBuf );
#endif
  /*
   * Set Channel Drive Enable bits in EPLD
   */
  data = PMCQ1_Read_EPLD(ptr->chip->board_data->baseaddr, PMCQ1_DRIVER_ENABLE );
  SYNC();
  data = data & ~(PMCQ1_DRIVER_ENABLE_3 | PMCQ1_DRIVER_ENABLE_2 |
                  PMCQ1_DRIVER_ENABLE_1 | PMCQ1_DRIVER_ENABLE_0);
  PMCQ1_Write_EPLD(ptr->chip->board_data->baseaddr, PMCQ1_DRIVER_ENABLE, data);
  data = PMCQ1_Read_EPLD(ptr->chip->board_data->baseaddr, PMCQ1_DRIVER_ENABLE );
  SYNC();

  /*
   * Disable the receiver and the transmitter.
   */

  SYNC();
  tmp_u32 = scc_read32( "gsmr_l", &ptr->pSCCR->gsmr_l );
  tmp_u32 &= (~(M360_GSMR_ENR | M360_GSMR_ENT ) ) ;
  scc_write32( "gsmr_l", &ptr->pSCCR->gsmr_l, tmp_u32 );

  /*
   * Disable Interrupt Error and Interrupt Breakpoint
   * Set SAID to 4 XXX - Shouldn't it be 7 for slave mode
   * Set SAISM to 7
   */
  SYNC();
  scc_write16( "sdcr", &m360->sdcr, 0x0740 );

  /*
   * Clear status -- reserved interrupt, SDMA channel error, SDMA breakpoint
   */
  scc_write8( "sdsr", &m360->sdsr, 0x07 );
  SYNC();

  /*
   * Initialize timer information in RISC Controller Configuration Register
   */
  scc_write16( "rccr", &m360->rccr, 0x8100 );
  SYNC();

  /*
   * XXX
   */
  scc_write16( "papar", &m360->papar, 0xffff );
  scc_write16( "padir", &m360->padir, 0x5500 ); /* From Memo    */
  scc_write16( "paodr", &m360->paodr, 0x0000 );
  SYNC();

  /*
   * XXX
   */

#if 0
  scc_write32( "pbpar", &m360->pbpar, 0x00000000 );
  scc_write32( "pbdir", &m360->pbdir, 0x0003ffff );
  scc_write32( "pbdat", &m360->pbdat, 0x0000003f );
  SYNC();
#else
  data = mc68360_scc_calculate_pbdat( ptr->chip );
  scc_write32( "pbpar", &m360->pbpar, 0x00000000 );
  scc_write32( "pbdat", &m360->pbdat, data );
  SYNC();
  scc_write32( "pbdir", &m360->pbdir, 0x0003fc3 );
  SYNC();
#endif


  /*
   * XXX
   */
  scc_write16( "pcpar", &m360->pcpar, 0x0000 );
  scc_write16( "pcdir", &m360->pcdir, 0x0000 );
  scc_write16( "pcso", &m360->pcso, 0x0000 );
  SYNC();

  /*
   * configure baud rate generator for 16x bit rate, where.....
   * b    =   desired baud rate
   * clk  =   system clock (33mhz)
   * d    =   clock dividor value
   *
   * for b > 300  :   d = clk/(b*16)
   * for b<= 300  :   d = (clk/ (b*16*16))-1)
   */
  SYNC();
  if( ptr->baud > 300 ) data = 33333333 / (ptr->baud * 16 );
  else                  data = (33333333 / (ptr->baud * 16 * 16) ) - 1;
  data *= 2 ;
  data &= 0x00001ffe ;

  /* really data = 0x010000 | data | ((baud>300)? 0 : 1 ) ; */
  data |= ((ptr->baud>300)? 0 : 1 ) ;
  data |= 0x010000 ;

  scc_write32( "pBRGC", ptr->pBRGC, data );

  data =  (((ptr->channel-1)*8) | (ptr->channel-1)) ;
  data = data << ((ptr->channel-1)*8) ;
  data |= scc_read32( "sicr", &m360->sicr );
  scc_write32( "sicr", &m360->sicr, data );

  /*
   * initialise SCC parameter ram
   */
  SYNC();
  scc_write16( "pSCCB->rbase", &ptr->pSCCB->rbase,
               (char *)(ptr->sccRxBd) - (char *)m360 );
  scc_write16( "pSCCB->tbase", &ptr->pSCCB->tbase,
               (char *)(ptr->sccTxBd) - (char *)m360 );

  scc_write8( "pSCCB->rfcr", &ptr->pSCCB->rfcr, 0x15 ); /* 0x15 0x18 */
  scc_write8( "pSCCB->tfcr", &ptr->pSCCB->tfcr, 0x15 ); /* 0x15 0x18 */

  scc_write16( "pSCCB->mrblr", &ptr->pSCCB->mrblr, M68360_RX_BUF_SIZE );

  /*
   * initialise tx and rx scc parameters
   */
  SYNC();
  data  = M360_CR_INIT_TX_RX_PARAMS | 0x01;
  data |= (M360_CR_CH_NUM * (ptr->channel-1) );
  scc_write16( "CR", &m360->cr, data );

  /*
   * initialise uart specific parameter RAM
   */
  SYNC();
  scc_write16( "pSCCB->un.uart.max_idl", &ptr->pSCCB->un.uart.max_idl, 15000 );
  scc_write16( "pSCCB->un.uart.brkcr", &ptr->pSCCB->un.uart.brkcr, 0x0001 );
  scc_write16( "pSCCB->un.uart.parec", &ptr->pSCCB->un.uart.parec, 0x0000 );

  scc_write16( "pSCCB->un,uart.frmec", &ptr->pSCCB->un.uart.frmec, 0x0000 );

  scc_write16( "pSCCB->un.uart.nosec", &ptr->pSCCB->un.uart.nosec, 0x0000 );
  scc_write16( "pSCCB->un.uart.brkec", &ptr->pSCCB->un.uart.brkec, 0x0000 );
  scc_write16( "pSCCB->un.uart.uaddr0", &ptr->pSCCB->un.uart.uaddr[0], 0x0000 );
  scc_write16( "pSCCB->un.uart.uaddr1", &ptr->pSCCB->un.uart.uaddr[1], 0x0000 );
  scc_write16( "pSCCB->un.uart.toseq", &ptr->pSCCB->un.uart.toseq, 0x0000 );
  scc_write16( "pSCCB->un.uart.char0",
               &ptr->pSCCB->un.uart.character[0], 0x0039 );
  scc_write16( "pSCCB->un.uart.char1",
               &ptr->pSCCB->un.uart.character[1], 0x8000 );
  scc_write16( "pSCCB->un.uart.char2",
               &ptr->pSCCB->un.uart.character[2], 0x8000 );
  scc_write16( "pSCCB->un.uart.char3",
               &ptr->pSCCB->un.uart.character[3], 0x8000 );
  scc_write16( "pSCCB->un.uart.char4",
               &ptr->pSCCB->un.uart.character[4], 0x8000 );
  scc_write16( "pSCCB->un.uart.char5",
               &ptr->pSCCB->un.uart.character[5], 0x8000 );
  scc_write16( "pSCCB->un.uart.char6",
               &ptr->pSCCB->un.uart.character[6], 0x8000 );
  scc_write16( "pSCCB->un.uart.char7",
               &ptr->pSCCB->un.uart.character[7], 0x8000 );

  scc_write16( "pSCCB->un.uart.rccm", &ptr->pSCCB->un.uart.rccm, 0xc0ff );

  /*
   * setup buffer descriptor stuff
   */
  SYNC();
  scc_write16( "sccRxBd->status", &ptr->sccRxBd->status, 0x0000 );
  SYNC();
  scc_write16( "sccRxBd->length", &ptr->sccRxBd->length, 0x0000 );
  scc_write16( "sccRxBd->status", &ptr->sccRxBd->status,
               M360_BD_EMPTY | M360_BD_WRAP | M360_BD_INTERRUPT );
  /* XXX Radstone Example writes RX buffer ptr as two u16's */
  scc_write32( "sccRxBd->buffer", &ptr->sccRxBd->buffer,
               RX_BUFFER_ADDRESS( ptr ) );

  SYNC();
  scc_write16( "sccTxBd->status", &ptr->sccTxBd->status, 0x0000 );
  SYNC();
  scc_write16( "sccTxBd->length", &ptr->sccTxBd->length, 0x0000 );
  /* XXX Radstone Example writes TX buffer ptr as two u16's */
  scc_write32( "sccTxBd->buffer", &ptr->sccTxBd->buffer,
               TX_BUFFER_ADDRESS( ptr ) );

  /*
   * clear previous events and set interrupt priorities
   */
  scc_write16( "pSCCR->scce", &ptr->pSCCR->scce, 0x1bef ); /* From memo   */
  SYNC();
  SYNC();
  scc_write32( "cicr", &m360->cicr, 0x001b9f40 );
  SYNC();

  /* scc_write32( "cicr", &m360->cicr, scc_read32( "cicr", &m360->cicr ) ); */

  scc_write16( "pSCCR->sccm", &ptr->pSCCR->sccm, M360_SCCE_TX | M360_SCCE_RX );

  data = scc_read32("cimr", &m360->cimr);
  data |= (0x80000000 >> ptr->channel);
  scc_write32( "cimr", &m360->cimr, data );
  SYNC();
  scc_write32( "cipr", &m360->cipr, scc_read32( "cipr", &m360->cipr ) );

  scc_write32( "pSCCR->gsmr_h", &ptr->pSCCR->gsmr_h, M360_GSMR_RFW );
  scc_write32( "pSCCR->gsmr_l", &ptr->pSCCR->gsmr_l,
        (M360_GSMR_TDCR_16X | M360_GSMR_RDCR_16X | M360_GSMR_MODE_UART) );

  scc_write16( "pSCCR->dsr", &ptr->pSCCR->dsr, 0x7e7e );
  SYNC();

  scc_write16( "pSCCR->psmr", &ptr->pSCCR->psmr,
               (M360_PSMR_CL8 | M360_PSMR_UM_NORMAL | M360_PSMR_TPM_ODD) );
  SYNC();

#if 0          /* XXX - ??? */
  /*
   * Enable the receiver and the transmitter.
   */

  SYNC();
  data = scc_read32( "pSCCR->gsmr_l", &ptr->pSCCR->gsmr_l);
  scc_write32( "pSCCR->gsmr_l", &ptr->pSCCR->gsmr_l,
    (data | M360_GSMR_ENR | M360_GSMR_ENT) );

  data  = PMCQ1_Read_EPLD(ptr->chip->board_data->baseaddr, PMCQ1_INT_MASK );
  data &= (~PMCQ1_INT_MASK_QUICC);
  PMCQ1_Write_EPLD(ptr->chip->board_data->baseaddr, PMCQ1_INT_MASK, data );

  data = PMCQ1_Read_EPLD(ptr->chip->board_data->baseaddr, PMCQ1_INT_STATUS );
  data &= (~PMCQ1_INT_STATUS_QUICC);
  PMCQ1_Write_EPLD(ptr->chip->board_data->baseaddr, PMCQ1_INT_STATUS, data );
#endif
}

/*
 *  mc68360_scc_write_support_int
 *
 *  Console Termios output entry point when using interrupt driven output.
 */

int mc68360_scc_write_support_int(
  int         minor,
  const char *buf,
  int         len
)
{
  rtems_interrupt_level  Irql;
  M68360_serial_ports_t  ptr;

#if 1
  mc68360_length_array[ mc68360_length_count ] = len;
  mc68360_length_count++;
  if ( mc68360_length_count >= MC68360_LENGTH_SIZE )
    mc68360_length_count=0;
#endif

  ptr   = Console_Port_Tbl[minor]->pDeviceParams;

  /*
   *  We are using interrupt driven output and termios only sends us
   *  one character at a time.
   */

  if ( !len )
    return 0;

  /*
   *
   */
#ifdef DEBUG_360
  printk("mc68360_scc_write_support_int: char 0x%x length %d\n",
       (unsigned int)*buf, len );
#endif
  /*
   *  We must copy the data from the global memory space to MC68360 space
   */

  rtems_interrupt_disable(Irql);

  scc_write16( "sccTxBd->status", &ptr->sccTxBd->status, 0 );
  memcpy((void *) ptr->txBuf, buf, len);
  scc_write32( "sccTxBd->buffer", &ptr->sccTxBd->buffer,
               TX_BUFFER_ADDRESS(ptr->txBuf) );
  scc_write16( "sccTxBd->length", &ptr->sccTxBd->length, len );
  scc_write16( "sccTxBd->status", &ptr->sccTxBd->status,
               (M360_BD_READY | M360_BD_WRAP | M360_BD_INTERRUPT) );

  rtems_interrupt_enable(Irql);

  return len;
}

/*
 *  mc68360_scc_write_polled
 *
 *  This routine polls out the requested character.
 */

void mc68360_scc_write_polled(
  int   minor,
  char  cChar
)
{
#ifdef DEBUG_360
  printk("mc68360_scc_write_polled: %c\n", cChar);
#endif
}

/*
 *  mc68681_set_attributes
 *
 *  This function sets the DUART channel to reflect the requested termios
 *  port settings.
 */

int mc68360_scc_set_attributes(
  int minor,
  const struct termios *t
)
{
   int                    baud;
   volatile m360_t        *m360;
   M68360_serial_ports_t  ptr;
   uint16_t               value;

#ifdef DEBUG_360
printk("mc68360_scc_set_attributes\n");
#endif

   ptr   = Console_Port_Tbl[minor]->pDeviceParams;
   m360  = ptr->chip->m360;

   switch (t->c_cflag & CBAUD)
   {
      case B50:      baud = 50;      break;
      case B75:      baud = 75;      break;
      case B110:     baud = 110;     break;
      case B134:     baud = 134;     break;
      case B150:     baud = 150;     break;
      case B200:     baud = 200;     break;
      case B300:     baud = 300;     break;
      case B600:     baud = 600;     break;
      case B1200:    baud = 1200;    break;
      case B1800:    baud = 1800;    break;
      case B2400:    baud = 2400;    break;
      case B4800:    baud = 4800;    break;
      case B9600:    baud = 9600;    break;
      case B19200:   baud = 19200;   break;
      case B38400:   baud = 38400;   break;
      case B57600:   baud = 57600;   break;
      case B115200:  baud = 115200;  break;
      case B230400:  baud = 230400;  break;
      case B460800:  baud = 460800;  break;
      default:       baud = -1;      break;
   }

   if (baud > 0)
   {
      scc_write32(
        "pBRGC",
        ptr->pBRGC,
        mc68360_sccBRGC(baud, ptr->chip->m360_clock_rate)
      );
   }

  /* Initial value of PSMR should be 0 */
  value = M360_PSMR_UM_NORMAL;

  /* set the number of data bits, 8 is most common */
  if (t->c_cflag & CSIZE)                     /* was it specified? */
  {
    switch (t->c_cflag & CSIZE) {
      case CS5: value |= M360_PSMR_CL5; break;
      case CS6: value |= M360_PSMR_CL6; break;
      case CS7: value |= M360_PSMR_CL7; break;
      case CS8: value |= M360_PSMR_CL8; break;
    }
  } else {
    value |= M360_PSMR_CL8;         /* default to 8 data bits */
  }

  /* the number of stop bits */
  if (t->c_cflag & CSTOPB)
    value |= M360_PSMR_SL_2;   /* Two stop bits */
  else
    value |= M360_PSMR_SL_1;   /* One stop bit  */

  /* Set Parity M360_PSMR_PEN bit should be clear on no parity so
   * do nothing in that case
   */
  if (t->c_cflag & PARENB)                /* enable parity detection? */
  {
    value |= M360_PSMR_PEN;
    if (t->c_cflag & PARODD){
      value |= M360_PSMR_RPM_ODD;        /* select odd parity */
      value |= M360_PSMR_TPM_ODD;
    } else {
      value |= M360_PSMR_RPM_EVEN;       /* select even parity */
      value |= M360_PSMR_TPM_EVEN;
    }
  }

  SYNC();
  scc_write16( "pSCCR->psmr", &ptr->pSCCR->psmr, value );
  SYNC();

  return 0;
}

/*
 *  mc68360_scc_close
 *
 *  This function shuts down the requested port.
 */

int mc68360_scc_close(
  int      major,
  int      minor,
  void    *arg
)
{
  return(RTEMS_SUCCESSFUL);
}

/*
 *  mc68360_scc_inbyte_nonblocking_polled
 *
 *  Console Termios polling input entry point.
 */

int mc68360_scc_inbyte_nonblocking_polled(
  int minor
)
{
    return -1;
}

/*
 *  mc68360_scc_write_support_polled
 *
 *  Console Termios output entry point when using polled output.
 *
 */

ssize_t mc68360_scc_write_support_polled(
  int         minor,
  const char *buf,
  size_t      len
)
{
  printk("mc68360_scc_write_support_polled: minor %d char %c len %d\n",
         minor, buf, len );
  return len;
}

/*
 *  mc68360_scc_init
 *
 *  This function initializes the DUART to a quiecsent state.
 */

void mc68360_scc_init(int minor)
{
#ifdef DEBUG_360
  printk("mc68360_scc_init\n");
#endif
}

int mc68360_scc_create_chip( PPMCQ1BoardData BoardData, uint8_t int_vector )
{
  M68360_t   chip;
  int        i;

#ifdef DEBUG_360
  printk("mc68360_scc_create_chip\n");
#endif

  /*
   * Create console structure for this card
   * XXX - Note Does this need to be moved up to if a QUICC is fitted
   *       section?
   */
  if ((chip = malloc(sizeof(struct _m68360_per_chip))) == NULL)
  {
    printk("Error Unable to allocate memory for _m68360_per_chip\n");
    return RTEMS_IO_ERROR;
  }

  chip->next                    = M68360_chips;
  chip->m360                    = (void *)BoardData->baseaddr;
  chip->m360_interrupt          = int_vector;
  chip->m360_clock_rate         = 25000000;
  chip->board_data              = BoardData;
  M68360_chips                  = chip;

  for (i=1; i<=4; i++) {
    chip->port[i-1].channel     = i;
    chip->port[i-1].chip        = chip;
    chip->port[i-1].baud        = 9600;

    switch( i ) {
      case 1:
        chip->port[i-1].pBRGC = &chip->m360->brgc1;
        chip->port[i-1].pSCCB = (m360SCCparms_t *) &chip->m360->scc1p;
        chip->port[i-1].pSCCR = &chip->m360->scc1;
        M360SetupMemory( chip );            /* Do this first time through */
        break;
      case 2:
        chip->port[i-1].pBRGC = &chip->m360->brgc2;
        chip->port[i-1].pSCCB = &chip->m360->scc2p;
        chip->port[i-1].pSCCR = &chip->m360->scc2;
        break;
      case 3:
        chip->port[i-1].pBRGC = &chip->m360->brgc3;
        chip->port[i-1].pSCCB = &chip->m360->scc3p;
        chip->port[i-1].pSCCR = &chip->m360->scc3;
        break;
      case 4:
        chip->port[i-1].pBRGC = &chip->m360->brgc4;
        chip->port[i-1].pSCCB = &chip->m360->scc4p;
        chip->port[i-1].pSCCR = &chip->m360->scc4;
        break;
      default:
        printk("Invalid mc68360 channel %d\n", i);
        return RTEMS_IO_ERROR;
    }

    /*
     * Allocate buffer descriptors.
     */

    chip->port[i-1].sccRxBd = M360AllocateBufferDescriptors(chip, 1);
    chip->port[i-1].sccTxBd = M360AllocateBufferDescriptors(chip, 1);
  }

  rsPMCQ1QuiccIntConnect(
    chip->board_data->busNo,
    chip->board_data->slotNo,
    chip->board_data->funcNo,
    &mc68360_sccInterruptHandler,
    chip
  );

  return RTEMS_SUCCESSFUL;
}

console_fns mc68360_scc_fns = {
  libchip_serial_default_probe,        /* deviceProbe */
  mc68360_scc_open,                    /* deviceFirstOpen */
  NULL,                                /* deviceLastClose */
  NULL,                                /* deviceRead */
  mc68360_scc_write_support_int,       /* deviceWrite */
  mc68360_scc_initialize_interrupts,   /* deviceInitialize */
  mc68360_scc_write_polled,            /* deviceWritePolled */
  mc68360_scc_set_attributes,          /* deviceSetAttributes */
  TRUE                                 /* deviceOutputUsesInterrupts */
};

console_fns mc68360_scc_polled = {
  libchip_serial_default_probe,             /* deviceProbe */
  mc68360_scc_open,                         /* deviceFirstOpen */
  mc68360_scc_close,                        /* deviceLastClose */
  mc68360_scc_inbyte_nonblocking_polled,    /* deviceRead */
  mc68360_scc_write_support_polled,         /* deviceWrite */
  mc68360_scc_init,                         /* deviceInitialize */
  mc68360_scc_write_polled,                 /* deviceWritePolled */
  mc68360_scc_set_attributes,               /* deviceSetAttributes */
  FALSE                                     /* deviceOutputUsesInterrupts */
};

