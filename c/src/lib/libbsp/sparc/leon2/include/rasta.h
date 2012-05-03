#ifndef __RASTA_H__
#define __RASTA_H__

#include <bsp.h>

#include <grcan.h>
#include <b1553brm_rasta.h>
#include <grspw.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int rasta_register(void);

/* Address of PCI bus on RASTA local AMBA bus */
#define RASTA_PCI_BASE             0xe0000000

/* Address of SRAM on RASTA local AMBA bus */
#define RASTA_LOCAL_SRAM           0x40000000

#define UART0_IRQNO  2
#define UART1_IRQNO  3
#define GRCAN_IRQNO  7
#define SPW0_IRQNO   10
#define SPW1_IRQNO   11
#define SPW2_IRQNO   12
#define BRM_IRQNO    13

#define GRCAN_IRQ   (3<<GRCAN_IRQNO)
#define SPW0_IRQ    (1<<SPW0_IRQNO)
#define SPW1_IRQ    (1<<SPW1_IRQNO)
#define SPW2_IRQ    (1<<SPW2_IRQNO)
#define SPW_IRQ     (7<<SPW0_IRQNO)
#define BRM_IRQ     (1<<BRM_IRQNO)
#define UART0_IRQ   (1<<UART0_IRQNO)
#define UART1_IRQ   (1<<UART1_IRQNO)

/*
 *  The following defines the bits in the UART Control Registers.
 *
 */
#define LEON_REG_UART_CONTROL_RTD  0x000000FF /* RX/TX data */

/*
 *  The following defines the bits in the LEON UART Status Registers.
 */
#define LEON_REG_UART_STATUS_DR   0x00000001 /* Data Ready */
#define LEON_REG_UART_STATUS_TSE  0x00000002 /* TX Send Register Empty */
#define LEON_REG_UART_STATUS_THE  0x00000004 /* TX Hold Register Empty */
#define LEON_REG_UART_STATUS_BR   0x00000008 /* Break Error */
#define LEON_REG_UART_STATUS_OE   0x00000010 /* RX Overrun Error */
#define LEON_REG_UART_STATUS_PE   0x00000020 /* RX Parity Error */
#define LEON_REG_UART_STATUS_FE   0x00000040 /* RX Framing Error */
#define LEON_REG_UART_STATUS_ERR  0x00000078 /* Error Mask */


/*
 *  The following defines the bits in the LEON UART Status Registers.
 */
#define LEON_REG_UART_CTRL_RE     0x00000001 /* Receiver enable */
#define LEON_REG_UART_CTRL_TE     0x00000002 /* Transmitter enable */
#define LEON_REG_UART_CTRL_RI     0x00000004 /* Receiver interrupt enable */
#define LEON_REG_UART_CTRL_TI     0x00000008 /* Transmitter interrupt enable */
#define LEON_REG_UART_CTRL_PS     0x00000010 /* Parity select */
#define LEON_REG_UART_CTRL_PE     0x00000020 /* Parity enable */
#define LEON_REG_UART_CTRL_FL     0x00000040 /* Flow control enable */
#define LEON_REG_UART_CTRL_LB     0x00000080 /* Loop Back enable */

#define UART_SET_SCALER 0
#define UART_SET_CTRL   1
#define UART_GET_STAT   2
#define UART_CLR_STAT   3

struct uart_reg {
    volatile unsigned int data;              /* 0x00 */
    volatile unsigned int status;            /* 0x04 */
    volatile unsigned int ctrl;              /* 0x08 */
    volatile unsigned int scaler;            /* 0x0C */
};


void uart_register(unsigned int baseaddr);
rtems_device_driver uart_initialize(rtems_device_major_number  major, rtems_device_minor_number  minor, void *arg);
rtems_device_driver uart_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
rtems_device_driver uart_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
rtems_device_driver uart_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
rtems_device_driver uart_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
rtems_device_driver uart_control(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);


struct gpio_reg {
    volatile unsigned int in_data;          /* 0x00 */
    volatile unsigned int out_data;         /* 0x04 */
    volatile unsigned int dir;              /* 0x08 */
    volatile unsigned int imask;            /* 0x0C */
    volatile unsigned int ipol;             /* 0x10 */
    volatile unsigned int iedge;            /* 0x14 */
};

extern struct gpio_reg *gpio0, *gpio1;

#ifdef __cplusplus
}
#endif

#endif
