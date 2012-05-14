/*
 * Termios console serial driver.
 *
 * Based on SCI driver by Ralf Corsepius and John M. Mills
 *
 * Author: Radzislaw Galler <rgaller@et.put.poznan.pl>
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <bsp.h>
#include <stdlib.h>

#include <libchip/serial.h>
#include <libchip/sersupp.h>

#include <rtems/libio.h>
#include <rtems/iosupp.h>

#include <rtems/score/sh_io.h>
#include <rtems/score/ispsh7045.h>
#include <rtems/score/iosh7045.h>

#include <sh/sh7_sci.h>
#include <sh/sh7_pfc.h>
#include <sh/sci_termios.h>


/*
 * Some handy macros
 */
#define SH_SCI_REG_DATA(_data, _minor, _register) \
 (write8(_data, Console_Port_Tbl[_minor]->ulCtrlPort1 + (_register)))

#define SH_SCI_REG_FLAG(_flag, _minor, _register) \
 (write8(read8(Console_Port_Tbl[_minor]->ulCtrlPort1 + (_register)) | (_flag), \
         Console_Port_Tbl[_minor]->ulCtrlPort1 + (_register)))

#define SH_SCI_REG_MASK(_flag, _minor, _register) \
 (write8(read8(Console_Port_Tbl[_minor]->ulCtrlPort1 + (_register)) & ~(_flag),\
         Console_Port_Tbl[_minor]->ulCtrlPort1 + (_register)))

/*
 * NOTE: Some SH variants have 3 sci devices
 */

#define SCI_MINOR_DEVICES       2


/*
 * Automatically generated function imported from scitab.rel
 */
extern int _sci_get_brparms(
  tcflag_t      cflag,
  unsigned char *smr,
  unsigned char *brr
);

/*
 * Translate termios flags into SCI settings
 */
int sh_sci_set_attributes(
  int minor,
  const struct termios *t
)
{
    uint8_t  	smr;
    uint8_t  	brr;
    int a;

    tcflag_t c_cflag = t->c_cflag;

    if ( c_cflag & CBAUD ) {
        if ( _sci_get_brparms( c_cflag, &smr, &brr ) != 0 )
            return -1 ;
    }

    if ( c_cflag & CSIZE ) {
        if ( c_cflag & CS8 )
            smr &= ~SCI_SEVEN_BIT_DATA;
        else if ( c_cflag & CS7 )
            smr |= SCI_SEVEN_BIT_DATA;
        else
            return -1 ;
    }

    if ( c_cflag & CSTOPB )
        smr |= SCI_STOP_BITS_2;
    else
        smr &= ~SCI_STOP_BITS_2;

    if ( c_cflag & PARENB )
        smr |= SCI_PARITY_ON ;
    else
        smr &= ~SCI_PARITY_ON ;

    if ( c_cflag & PARODD )
        smr |= SCI_ODD_PARITY ;
    else
        smr &= ~SCI_ODD_PARITY;

    SH_SCI_REG_MASK((SCI_RE | SCI_TE), minor, SCI_SCR);

    SH_SCI_REG_DATA(smr, minor, SCI_SMR);
    SH_SCI_REG_DATA(brr, minor, SCI_BRR);

    for (a=0; a < 10000L; a++) { /* Delay one bit */
        __asm__ volatile ("nop");
    }

    SH_SCI_REG_FLAG((SCI_RE | SCI_TE), minor, SCI_SCR);

    return 0;
}

/*
 * Receive-data-full ISR
 *
 * The same routine for all interrupt sources of the same type.
 */
rtems_isr sh_sci_rx_isr(rtems_vector_number vector)
{
    int minor;

    for (minor = 0; minor < Console_Port_Count; minor++) {
        if (Console_Port_Tbl[minor]->ulIntVector == vector) {
            char   temp8;

            /*
             * FIXME: error handling should be added
             */
            temp8 = read8(Console_Port_Tbl[minor]->ulCtrlPort1 + SCI_RDR);

            rtems_termios_enqueue_raw_characters(
                Console_Port_Data[minor].termios_data, &temp8, 1);

            SH_SCI_REG_MASK(SCI_RDRF, minor, SCI_SSR);
            break;
        }
    }
}

/*
 * Transmit-data-empty ISR
 *
 * The same routine for all interrupt sources of the same type.
 */
rtems_isr sh_sci_tx_isr(rtems_vector_number vector)
{
    int minor;

    for (minor = 0; minor < Console_Port_Count; minor++) {
        if (Console_Port_Tbl[minor]->ulDataPort == vector) {
            /*
             * FIXME: Error handling should be added
             */

            /*
             * Mask end-of-transmission interrupt
             */
            SH_SCI_REG_MASK(SCI_TIE, minor, SCI_SCR);

            if (rtems_termios_dequeue_characters(
                   Console_Port_Data[minor].termios_data, 1)) {
                /*
                 * More characters to be received - interrupt must be enabled
                 */
                SH_SCI_REG_FLAG(SCI_TIE, minor, SCI_SCR);
            }
            break;
        }
    }
}


/*
 * Initialization of serial port
 */
void sh_sci_init(int minor)
{
    uint16_t   temp16;

    /*
     * set PFC registers to enable I/O pins
     */
    if ((minor == 0)) {
        temp16 = read16(PFC_PACRL2);         /* disable SCK0, DMA, IRQ */
        temp16 &= ~(PA2MD1 | PA2MD0);
        temp16 |= (PA_TXD0 | PA_RXD0);       /* enable pins for Tx0, Rx0 */
        write16(temp16, PFC_PACRL2);

    } else if (minor == 1) {
        temp16 = read16(PFC_PACRL2);          /* disable SCK1, DMA, IRQ */
        temp16 &= ~(PA5MD1 | PA5MD0);
        temp16 |= (PA_TXD1 | PA_RXD1);        /* enable pins for Tx1, Rx1 */
        write16(temp16, PFC_PACRL2);
    }

    /*
     * Non-default hardware setup occurs in sh_sci_first_open
     */
}

/*
 * Initialization of interrupts
 *
 * Interrupts can be started only after opening a device, so interrupt
 * flags are set up in sh_sci_first_open function
 */
void sh_sci_initialize_interrupts(int minor)
{
    rtems_isr_entry old_isr;
    rtems_status_code status;

    sh_sci_init(minor);
    /*
     * Disable IRQ of SCIx
     */
    status = sh_set_irq_priority( Console_Port_Tbl[minor]->ulIntVector, 0);

    if (status != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred(status);

    SH_SCI_REG_MASK(SCI_RIE, minor, SCI_SCR);

    /*
     * Catch apropriate vectors
     */
    status = rtems_interrupt_catch(
        sh_sci_rx_isr,
        Console_Port_Tbl[minor]->ulIntVector,
        &old_isr);

    if (status != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred(status);

    status = rtems_interrupt_catch(
        sh_sci_tx_isr,
        Console_Port_Tbl[minor]->ulDataPort,
        &old_isr);

    if (status != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred(status);

    /*
     * Enable IRQ of SCIx
     */
    SH_SCI_REG_FLAG(SCI_RIE, minor, SCI_SCR);

    status = sh_set_irq_priority(
        Console_Port_Tbl[minor]->ulIntVector,
        Console_Port_Tbl[minor]->ulCtrlPort2);

    if (status != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred(status);
}


/*
 *  Open entry point
 *   Sets up port and pins for selected sci.
 */

int sh_sci_first_open(
  int major,
  int minor,
  void *arg
)
{
    char   temp8;
    unsigned int a ;

    /*
     * check for valid minor number
     */
    if (( minor > ( SCI_MINOR_DEVICES -1 )) || ( minor < 0 )) {
        return RTEMS_INVALID_NUMBER;
    }

    /*
     * set up SCI registers
     */
    /* Clear SCR - disable Tx and Rx */
    SH_SCI_REG_DATA(0x00, minor, SCI_SCR);

    /* set SMR and BRR - baudrate and format */
    sh_sci_set_attributes(minor, Console_Port_Tbl[minor]->pDeviceParams);

    for (a=0; a < 10000L; a++) {                      /* Delay */
        __asm__ volatile ("nop");
    }

    write8((SCI_RE | SCI_TE),              /* enable async. Tx and Rx */
           Console_Port_Tbl[minor]->ulCtrlPort1 + SCI_SCR);

    /*
     * clear error flags
     */
    temp8 = read8(Console_Port_Tbl[minor]->ulCtrlPort1 + SCI_SSR);
    while(temp8 & (SCI_RDRF | SCI_ORER | SCI_FER | SCI_PER)) {
        /* flush input */
        temp8 = read8(Console_Port_Tbl[minor]->ulCtrlPort1 + SCI_RDR);

        /* clear some flags */
        SH_SCI_REG_FLAG((SCI_RDRF|SCI_ORER|SCI_FER|SCI_PER), minor, SCI_SSR);

        /* check if everything is OK */
        temp8 = read8(Console_Port_Tbl[minor]->ulCtrlPort1 + SCI_SSR);
    }

    /* Clear RDRF flag */
    SH_SCI_REG_DATA(0x00, minor, SCI_TDR); /* force output */

    /* Clear the TDRE bit */
    SH_SCI_REG_FLAG(SCI_TDRE, minor, SCI_SSR);

    /*
     * Interrupt setup
     */
    if (Console_Port_Tbl[minor]->pDeviceFns->deviceOutputUsesInterrupts) {
        SH_SCI_REG_FLAG(SCI_RIE, minor, SCI_SCR);
    }

    return RTEMS_SUCCESSFUL ;
}

/*
 *  Close entry point
 */

int sh_sci_last_close(
  int major,
  int minor,
  void *arg
)
{
    /* FIXME: Incomplete */

    /* Shutdown interrupts if necessary */
    if (Console_Port_Tbl[minor]->pDeviceFns->deviceOutputUsesInterrupts)
    {
        SH_SCI_REG_MASK((SCI_TIE | SCI_RIE), minor, SCI_SCR);
    }
    return RTEMS_SUCCESSFUL ;
}

/*
 * Interrupt aware write routine
 */
ssize_t sh_sci_write_support_int(
  int            minor,
  const char    *buf,
  size_t         len
)
{
    if (!len)
        return 0;
    /*
     * Put data into TDR and clear transmission-end-flag
     */
    SH_SCI_REG_DATA(*buf, minor, SCI_TDR);
    SH_SCI_REG_MASK(SCI_TDRE, minor, SCI_SSR);
    /*
     * Enable interrupt
     */
    SH_SCI_REG_FLAG(SCI_TIE, minor, SCI_SCR);

    return 1;
}

/*
 * Polled write method
 */
ssize_t sh_sci_write_support_polled(
  int            minor,
  const char    *buf,
  size_t         len
)
{
    int count = 0;

    while(count < len) {
        sh_sci_write_polled(minor, buf[count]);
        count++;
    }
    /*
     * Return number of bytes written
     */
    return count;
}

/*
 * Polled write of one character at a time
 */
void sh_sci_write_polled(
  int     minor,
  char    c
)
{
    /*
     * Wait for end of previous character
     */
    while(!(read8(Console_Port_Tbl[minor]->ulCtrlPort1 + SCI_SSR) & SCI_TDRE));
    /*
     * Send the character
     */
    SH_SCI_REG_DATA(c, minor, SCI_TDR);

    /*
     * Clear TDRE flag
     */
    SH_SCI_REG_MASK(SCI_TDRE, minor, SCI_SSR);
}

/*
 * Non-blocking read
 */
int sh_sci_inbyte_nonblocking_polled(int minor)
{
    char   inbyte;

    /*
     * Check if input buffer is full
     */
    if (read8(Console_Port_Tbl[minor]->ulCtrlPort1 + SCI_SSR) & SCI_RDRF) {
        inbyte = read8(Console_Port_Tbl[minor]->ulCtrlPort1 + SCI_RDR);
        SH_SCI_REG_MASK(SCI_RDRF, minor, SCI_SSR);

        /*
         * Check for errors
         */
        if (read8(Console_Port_Tbl[minor]->ulCtrlPort1 + SCI_SSR) &
           (SCI_ORER | SCI_FER | SCI_PER)) {
            SH_SCI_REG_MASK((SCI_ORER | SCI_FER | SCI_PER), minor, SCI_SSR);
            return -1;
        }
        return (int)inbyte;
    }
    return -1;
}
