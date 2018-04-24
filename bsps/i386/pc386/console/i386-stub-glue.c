/*
 * Copyright (c) 2016.
 * Chris Johns <chrisj@rtems.org>
 *
 * This software is Copyright (C) 1998 by T.sqware - all rights limited
 * It is provided in to the public domain "as is", can be freely modified
 * as far as this copyight notice is kept unchanged, but does not imply
 * an endorsement by T.sqware of the product in which it is included.
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <libchip/serial.h>

#include "../../../shared/dev/serial/legacy-console.h"

int  putDebugChar(int ch);     /* write a single character      */
int  getDebugChar(void);       /* read and return a single char */

/* Check is any characters received are a ^C */
int i386_gdb_uart_ctrl_c_check(void);

/* Raw interrupt handler. */
void i386_gdb_uart_isr(void);

/* assign an exception handler */
void exceptionHandler(int, void (*handler)(void));

/* User supplied remote debug option. */
extern int remote_debug;

/* Current uart and port used by the gdb stub */
static int          uart_current;
static int          uart_vector;
static console_tbl* port_current;

/*
 * Interrupt service routine for all, it does it check whether ^C is received
 * if yes it will flip TF bit before returning.
 *
 * Note: it should be installed as raw interrupt handler.
 *
 * Warning: I do not like the use of the global data, I am not
 *          sure if this is SMP safe.
 */
int i386_gdb_uart_isr_regsav[4] RTEMS_UNUSED;
__asm__ (".p2align 4");
__asm__ (".text");
__asm__ (".globl i386_gdb_uart_isr");
__asm__ ("i386_gdb_uart_isr:");
__asm__ ("    pusha");                                       /* Push all */
__asm__ ("    call  i386_gdb_uart_ctrl_c_check");            /* Look for ^C */
__asm__ ("    movl  %eax, i386_gdb_uart_isr_regsav");        /* Save eax */
__asm__ ("    popa");                                        /* Pop all */
__asm__ ("    xchgl %eax, i386_gdb_uart_isr_regsav");        /* Exchange eax */
__asm__ ("    cmpl  $0, %eax");                              /* 1 == ^C */
__asm__ ("    je    i386_gdb_uart_isr_1");                   /* branch if 0 */
__asm__ ("    movl  %ebx, i386_gdb_uart_isr_regsav + 4");    /* Save ebx */
__asm__ ("    movl  %edx, i386_gdb_uart_isr_regsav + 8");    /* Save edx */
__asm__ ("    popl  %ebx");                                  /* Pop eip */
__asm__ ("    popl  %edx");                                  /* Pop cs */
__asm__ ("    popl  %eax");                                  /* Pop flags */
__asm__ ("    orl   $0x100, %eax");                          /* Modify it */
__asm__ ("    pushl %eax");                                  /* Push it back */
__asm__ ("    pushl %edx");                                  /* Push cs */
__asm__ ("    pushl %ebx");                                  /* Push eip */
__asm__ ("    movl  i386_gdb_uart_isr_regsav + 4, %ebx");    /* Restore ebx */
__asm__ ("    movl  i386_gdb_uart_isr_regsav + 8, %edx");    /* Restore edx */
__asm__ ("i386_gdb_uart_isr_1:");
__asm__ ("    movl  i386_gdb_uart_isr_regsav, %eax");        /* Restore eax */
__asm__ ("    iret");                                        /* Done */

static int gdb_hello_index;
static const char *const gdb_hello = "+";

static inline uint8_t BSP_i8259a_irq_in_service_reg(uint32_t ioport)
{
  uint8_t isr;
  outport_byte(ioport, PIC_OCW3_SEL | PIC_OCW3_RR | PIC_OCW3_RIS);
  inport_byte(ioport, isr);
  outport_byte(ioport, PIC_OCW3_SEL | PIC_OCW3_RR);
  return isr;
}

static inline void BSP_irq_ack_at_i8259a(const int irqLine)
{
  uint8_t slave_isr = 0;
  if (irqLine >= 8) {
   outport_byte(PIC_SLAVE_COMMAND_IO_PORT, PIC_EOI);
   slave_isr = BSP_i8259a_irq_in_service_reg(PIC_SLAVE_COMMAND_IO_PORT);
  }

  /*
   * Only issue the EOI to the master if there are no more interrupts in
   * service for the slave. i8259a data sheet page 18, The Special Fully Nested
   * Mode, b.
   */
  if (slave_isr == 0)
    outport_byte(PIC_MASTER_COMMAND_IO_PORT, PIC_EOI);
}

int i386_gdb_uart_ctrl_c_check(void)
{
  BSP_irq_ack_at_i8259a(uart_vector);
  if (port_current) {
    int c = 0;
    while (c >= 0) {
      c = port_current->pDeviceFns->deviceRead(uart_current);
      if (c == 3) {
        gdb_hello_index = 0;
        return 1;
      } else if (gdb_hello[gdb_hello_index] == (char) c) {
        ++gdb_hello_index;
        if (gdb_hello[gdb_hello_index] == '\0') {
          gdb_hello_index = 0;
          return 1;
        }
      } else {
        gdb_hello_index = 0;
      }
    }
  }
  return 0;
}

static void
nop(const rtems_raw_irq_connect_data* notused)
{
}

static int
isOn(const rtems_raw_irq_connect_data* notused)
{
  return 1;
}

int i386_stub_glue_uart(void)
{
  if (port_current == NULL)
    return -1;
  return uart_current;
}

/*
 * Initialize glue code linking i386-stub with the rest of
 * the system
 */
void
i386_stub_glue_init(int uart)
{
  rtems_device_minor_number minor = (rtems_device_minor_number) uart;

  port_current = console_find_console_entry(NULL, 0, &minor);

  if (port_current == NULL) {
    printk("GDB: invalid minor number for UART\n");
    return;
  }

  uart_current = uart;

  /* Intialise the UART, assuming polled drivers */
  port_current->pDeviceFns->deviceInitialize(uart);
}

static void BSP_uart_on(const rtems_raw_irq_connect_data* used)
{
  bsp_interrupt_vector_enable(used->idtIndex - BSP_IRQ_VECTOR_BASE);
}

static void BSP_uart_off(const rtems_raw_irq_connect_data* used)
{
  bsp_interrupt_vector_disable(used->idtIndex - BSP_IRQ_VECTOR_BASE);
}

/*
 * In order to have a possibility to break into
 * running program, one has to call this function
 */
void i386_stub_glue_init_breakin(void)
{
  rtems_raw_irq_connect_data uart_raw_irq_data;

  if (port_current == NULL) {
    printk("GDB: no port initialised\n");
    return;
  }

  if ((port_current->ulIntVector == 0) || (port_current->ulIntVector > 16)) {
    printk("GDB: no UART interrupt support\n");
  }
  else {
    uart_vector = port_current->ulIntVector;
    uart_raw_irq_data.idtIndex = port_current->ulIntVector + BSP_IRQ_VECTOR_BASE;

    if (!i386_get_current_idt_entry(&uart_raw_irq_data)) {
      printk("GBD: cannot get idt entry\n");
      rtems_fatal_error_occurred(1);
    }

    if (!i386_delete_idt_entry(&uart_raw_irq_data)) {
      printk("GDB: cannot delete idt entry\n");
      rtems_fatal_error_occurred(1);
    }

    uart_raw_irq_data.on  = BSP_uart_on;
    uart_raw_irq_data.off = BSP_uart_off;

    /* Install ISR  */
    uart_raw_irq_data.idtIndex = port_current->ulIntVector + BSP_IRQ_VECTOR_BASE;
    uart_raw_irq_data.hdl = i386_gdb_uart_isr;

    if (!i386_set_idt_entry (&uart_raw_irq_data)) {
      printk("GDB: raw exception handler connection failed\n");
      rtems_fatal_error_occurred(1);
    }

    /* Enable interrupts, this is a bit of a hack because we
     * have to know the device but there is no other call. */
    (*port_current->setRegister)(port_current->ulCtrlPort1, 1, 0x01);
  }
}

int
putDebugChar(int ch)
{
  if (port_current != NULL) {
    port_current->pDeviceFns->deviceWritePolled(uart_current, ch);
  }
  return 1;
}

int getDebugChar(void)
{
  int c = -1;

  if (port_current != NULL) {
    while (c < 0)
      c = port_current->pDeviceFns->deviceRead(uart_current);
  }

  return c;
}

void exceptionHandler(int vector, void (*handler)(void))
{
  rtems_raw_irq_connect_data excep_raw_irq_data;

  excep_raw_irq_data.idtIndex = vector;

  if(!i386_get_current_idt_entry(&excep_raw_irq_data))
    {
      printk("GDB: cannot get idt entry\n");
      rtems_fatal_error_occurred(1);
    }

  if(!i386_delete_idt_entry(&excep_raw_irq_data))
    {
      printk("GDB: cannot delete idt entry\n");
      rtems_fatal_error_occurred(1);
    }

  excep_raw_irq_data.on = nop;
  excep_raw_irq_data.off = nop;
  excep_raw_irq_data.isOn = isOn;
  excep_raw_irq_data.hdl = handler;

  if (!i386_set_idt_entry (&excep_raw_irq_data)) {
      printk("GDB: raw exception handler connection failed\n");
      rtems_fatal_error_occurred(1);
    }
  return;
}
